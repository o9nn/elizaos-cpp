#include "elizaos/otaku.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <tuple>
#include <unordered_set>

namespace elizaos {
namespace {

template <typename T>
OperationResult<T> fail(ResultCode code, std::string message) {
    OperationResult<T> out;
    out.code = code;
    out.message = std::move(message);
    return out;
}

template <typename T>
OperationResult<T> pass(T value, std::string evidence = {}) {
    OperationResult<T> out;
    out.code = ResultCode::OK;
    out.value = std::move(value);
    out.evidence = std::move(evidence);
    return out;
}

bool positive(double v) { return std::isfinite(v) && v > 0.0; }
bool nonnegative(double v) { return std::isfinite(v) && v >= 0.0; }
bool text(const std::string& v) { return !v.empty() && v.size() <= 256; }

bool supported(ChainId chain) {
    switch (chain) {
        case ChainId::ETHEREUM_MAINNET: case ChainId::OPTIMISM: case ChainId::BSC:
        case ChainId::POLYGON: case ChainId::ARBITRUM: case ChainId::BASE:
        case ChainId::AVALANCHE: case ChainId::SOLANA: return true;
    }
    return false;
}

bool validDex(DexProtocol p) {
    switch (p) {
        case DexProtocol::UNISWAP_V2: case DexProtocol::UNISWAP_V3:
        case DexProtocol::SUSHISWAP: case DexProtocol::CURVE: case DexProtocol::BALANCER:
        case DexProtocol::ONEINCH: case DexProtocol::PARASWAP: case DexProtocol::COWSWAP:
            return true;
    }
    return false;
}

bool validBridge(BridgeProtocol p) {
    switch (p) {
        case BridgeProtocol::ACROSS: case BridgeProtocol::STARGATE: case BridgeProtocol::HOP:
        case BridgeProtocol::MULTICHAIN: case BridgeProtocol::CBRIDGE:
        case BridgeProtocol::WORMHOLE: case BridgeProtocol::LAYERZERO: return true;
    }
    return false;
}

bool validStrategy(YieldStrategy s) {
    switch (s) {
        case YieldStrategy::LIQUIDITY_PROVISION: case YieldStrategy::LENDING:
        case YieldStrategy::STAKING: case YieldStrategy::YIELD_AGGREGATOR:
        case YieldStrategy::LEVERAGED_FARMING: return true;
    }
    return false;
}

bool address(const std::string& value, ChainId chain) {
    return chain == ChainId::SOLANA ? otaku_utils::isValidSolanaAddress(value)
                                    : otaku_utils::isValidEthereumAddress(value);
}

bool current(std::chrono::system_clock::time_point deadline) {
    return deadline != std::chrono::system_clock::time_point{} &&
           deadline > std::chrono::system_clock::now();
}

bool swapQuoteValid(const SwapQuote& q, std::string& why) {
    if (!text(q.fromToken) || !text(q.toToken) || q.fromToken == q.toToken ||
        !positive(q.inputAmount) || !positive(q.expectedOutput) || !positive(q.minimumOutput) ||
        q.minimumOutput > q.expectedOutput || !nonnegative(q.priceImpact) ||
        !nonnegative(q.estimatedGas) || !supported(q.chainId) || !validDex(q.protocol)) {
        why = "malformed swap quote"; return false;
    }
    if (!current(q.validUntil)) { why = "expired swap quote"; return false; }
    if (q.quoteId.empty() || q.evidence.empty() || !q.evidenceVerified) {
        why = "swap quote lacks verified evidence"; return false;
    }
    return true;
}

bool bridgeQuoteValid(const BridgeQuote& q, std::string& why) {
    if (!supported(q.sourceChain) || !supported(q.destChain) || q.sourceChain == q.destChain ||
        !text(q.token) || !positive(q.inputAmount) || !positive(q.outputAmount) ||
        q.outputAmount > q.inputAmount || !nonnegative(q.bridgeFee) ||
        q.bridgeFee > q.inputAmount || !positive(q.estimatedTime) || !validBridge(q.protocol)) {
        why = "malformed bridge quote"; return false;
    }
    if (!current(q.validUntil)) { why = "expired bridge quote"; return false; }
    if (q.quoteId.empty() || q.evidence.empty() || !q.evidenceVerified) {
        why = "bridge quote lacks verified evidence"; return false;
    }
    return true;
}

bool receiptValid(const TransactionReceipt& r, std::string& why) {
    if (!supported(r.chainId) || !nonnegative(r.gasUsed) || !nonnegative(r.effectiveGasPrice)) {
        why = "malformed receipt"; return false;
    }
    if (r.status == TxStatus::FAILED) return true;
    if (r.status == TxStatus::SIMULATED) {
        if (r.txHash.empty()) { why = "simulation lacks stable id"; return false; }
        return true;
    }
    if (r.txHash.empty() || r.evidence.empty() || !r.evidenceVerified) {
        why = "receipt lacks verified evidence"; return false;
    }
    if (r.status == TxStatus::CONFIRMED &&
        (r.blockNumber == 0 || r.timestamp == std::chrono::system_clock::time_point{})) {
        why = "confirmed receipt lacks block evidence"; return false;
    }
    return true;
}

bool transition(TxStatus from, TxStatus to) {
    return from == to || (from == TxStatus::PENDING &&
        (to == TxStatus::CONFIRMED || to == TxStatus::FAILED || to == TxStatus::CANCELLED));
}

TransactionReceipt failedReceipt(ChainId chain, std::string why) {
    TransactionReceipt r;
    r.chainId = chain;
    r.status = TxStatus::FAILED;
    r.timestamp = std::chrono::system_clock::now();
    r.error = std::move(why);
    return r;
}

std::string number(double value) {
    std::ostringstream out;
    out << std::setprecision(17) << value;
    return out.str();
}

std::vector<std::pair<std::string, double>> targetPairs(
    const std::vector<PortfolioManager::RebalanceTarget>& targets) {
    std::vector<std::pair<std::string, double>> out;
    for (const auto& target : targets) out.emplace_back(target.asset, target.targetPercentage);
    return out;
}

} // namespace

using TimeSeries = std::vector<std::pair<std::chrono::system_clock::time_point, double>>;
using Balances = std::unordered_map<std::string, double>;
#define UNSUPPORTED_RESULT(type, message) return fail<type>(ResultCode::UNSUPPORTED, message)
OperationResult<GasEstimate> MarketDataAdapter::getGasEstimate(ChainId, const std::string&) { UNSUPPORTED_RESULT(GasEstimate, "gas estimate unsupported"); }
OperationResult<TimeSeries> MarketDataAdapter::getGasHistory(ChainId) { UNSUPPORTED_RESULT(TimeSeries, "gas history unsupported"); }
OperationResult<Balances> MarketDataAdapter::getBalances(const std::string&, ChainId) { UNSUPPORTED_RESULT(Balances, "balances unsupported"); }
OperationResult<PortfolioSummary> MarketDataAdapter::getPortfolio(const std::string&) { UNSUPPORTED_RESULT(PortfolioSummary, "portfolio unsupported"); }
OperationResult<TimeSeries> MarketDataAdapter::getPortfolioHistory(const std::string&, int) { UNSUPPORTED_RESULT(TimeSeries, "portfolio history unsupported"); }
OperationResult<SwapQuote> MarketDataAdapter::getSwapQuote(ChainId, const std::string&, const std::string&, double, const MEVProtectionOptions&) { UNSUPPORTED_RESULT(SwapQuote, "swap quote unsupported"); }
OperationResult<std::vector<SwapQuote>> MarketDataAdapter::getMultiRouteQuotes(ChainId, const std::string&, const std::string&, double) { UNSUPPORTED_RESULT(std::vector<SwapQuote>, "multi-route quotes unsupported"); }
OperationResult<BridgeQuote> MarketDataAdapter::getBridgeQuote(ChainId, ChainId, const std::string&, double) { UNSUPPORTED_RESULT(BridgeQuote, "bridge quote unsupported"); }
OperationResult<std::vector<BridgeQuote>> MarketDataAdapter::getMultiBridgeQuotes(ChainId, ChainId, const std::string&, double) { UNSUPPORTED_RESULT(std::vector<BridgeQuote>, "multi-bridge quotes unsupported"); }
OperationResult<std::vector<SwapQuote>> MarketDataAdapter::getRebalanceQuotes(const std::string&, const std::vector<std::pair<std::string, double>>&) { UNSUPPORTED_RESULT(std::vector<SwapQuote>, "rebalance quotes unsupported"); }
OperationResult<std::vector<YieldPosition>> MarketDataAdapter::findBestYields(const std::string&, YieldStrategy, double) { UNSUPPORTED_RESULT(std::vector<YieldPosition>, "yield discovery unsupported"); }
OperationResult<std::vector<std::string>> MarketDataAdapter::getSupportedProtocols(ChainId) { UNSUPPORTED_RESULT(std::vector<std::string>, "protocol discovery unsupported"); }
OperationResult<std::vector<YieldPosition>> MarketDataAdapter::getYieldPositions(const std::string&, ChainId) { UNSUPPORTED_RESULT(std::vector<YieldPosition>, "yield positions unsupported"); }
OperationResult<double> MarketDataAdapter::getProtocolRiskScore(const std::string&) { UNSUPPORTED_RESULT(double, "risk score unsupported"); }
OperationResult<bool> MarketDataAdapter::isProtocolAudited(const std::string&) { UNSUPPORTED_RESULT(bool, "audit status unsupported"); }
OperationResult<double> MarketDataAdapter::getImpermanentLossRisk(const std::string&) { UNSUPPORTED_RESULT(double, "IL risk unsupported"); }
OperationResult<std::vector<NFTInfo>> MarketDataAdapter::getOwnedNFTs(const std::string&, ChainId) { UNSUPPORTED_RESULT(std::vector<NFTInfo>, "NFT ownership unsupported"); }
OperationResult<NFTInfo> MarketDataAdapter::getNFTDetails(const std::string&, const std::string&, ChainId) { UNSUPPORTED_RESULT(NFTInfo, "NFT details unsupported"); }
OperationResult<double> MarketDataAdapter::getNFTFloorPrice(const std::string&, ChainId) { UNSUPPORTED_RESULT(double, "NFT floor unsupported"); }
OperationResult<std::vector<NFTInfo>> MarketDataAdapter::getTrendingCollections(ChainId) { UNSUPPORTED_RESULT(std::vector<NFTInfo>, "trending collections unsupported"); }
OperationResult<double> MarketDataAdapter::getCollectionVolume24h(const std::string&) { UNSUPPORTED_RESULT(double, "collection volume unsupported"); }
OperationResult<double> MarketDataAdapter::getPoolApr(const std::string&, DexProtocol) { UNSUPPORTED_RESULT(double, "pool APR unsupported"); }
OperationResult<double> MarketDataAdapter::getTokenPrice(const std::string&, ChainId) { UNSUPPORTED_RESULT(double, "token price unsupported"); }
OperationResult<std::vector<std::string>> MarketDataAdapter::getTrendingTokens(ChainId) { UNSUPPORTED_RESULT(std::vector<std::string>, "trending tokens unsupported"); }
OperationResult<std::vector<TokenInfo>> MarketDataAdapter::searchTokens(const std::string&, ChainId) { UNSUPPORTED_RESULT(std::vector<TokenInfo>, "token search unsupported"); }
OperationResult<double> MarketDataAdapter::get24hPriceChange(const std::string&, ChainId) { UNSUPPORTED_RESULT(double, "price change unsupported"); }
OperationResult<double> MarketDataAdapter::get24hVolume(const std::string&, ChainId) { UNSUPPORTED_RESULT(double, "volume unsupported"); }
OperationResult<BlockchainExecutionResult> BlockchainExecutionAdapter::execute(const BlockchainExecutionRequest&) { UNSUPPORTED_RESULT(BlockchainExecutionResult, "execution unsupported"); }
OperationResult<TransactionReceipt> BlockchainExecutionAdapter::getTransactionReceipt(const std::string&, ChainId) { UNSUPPORTED_RESULT(TransactionReceipt, "receipt lookup unsupported"); }
OperationResult<TxStatus> BlockchainExecutionAdapter::getBridgeStatus(const std::string&, ChainId) { UNSUPPORTED_RESULT(TxStatus, "bridge status unsupported"); }
OperationResult<TransactionSimulation> BlockchainExecutionAdapter::simulate(const std::string&, const std::string&, const std::string&, double, ChainId) { UNSUPPORTED_RESULT(TransactionSimulation, "simulation unsupported"); }
OperationResult<bool> BlockchainExecutionAdapter::verifyContractSafety(const std::string&, ChainId) { UNSUPPORTED_RESULT(bool, "contract verification unsupported"); }
OperationResult<std::vector<std::string>> BlockchainExecutionAdapter::getContractWarnings(const std::string&, ChainId) { UNSUPPORTED_RESULT(std::vector<std::string>, "contract warnings unsupported"); }
#undef UNSUPPORTED_RESULT

GasOptimizer::GasOptimizer(std::shared_ptr<MarketDataAdapter> adapter) : marketData_(std::move(adapter)) {}
void GasOptimizer::setMarketDataAdapter(std::shared_ptr<MarketDataAdapter> adapter) {
    std::lock_guard<std::mutex> lock(gasMutex_); marketData_ = std::move(adapter); currentEstimates_.clear(); gasPriceHistory_.clear();
}
OperationResult<GasEstimate> GasOptimizer::estimateGasResult(ChainId chain, const std::string& data) {
    if (!supported(chain) || data.size() > 1000000) return fail<GasEstimate>(ResultCode::INVALID_ARGUMENT, "invalid gas request");
    std::shared_ptr<MarketDataAdapter> adapter;
    { std::lock_guard<std::mutex> lock(gasMutex_); adapter = marketData_; }
    if (!adapter) return fail<GasEstimate>(ResultCode::UNSUPPORTED, "no market adapter");
    auto result = adapter->getGasEstimate(chain, data);
    if (!result) return result;
    const auto& v = result.value;
    if (!v.available || v.chainId != chain || v.evidence.empty() || !nonnegative(v.estimatedGas) ||
        !nonnegative(v.baseFee) || !nonnegative(v.priorityFee) || !nonnegative(v.maxFee) ||
        !nonnegative(v.totalCostWei) || !nonnegative(v.totalCostUsd))
        return fail<GasEstimate>(ResultCode::ADAPTER_ERROR, "unverified gas estimate");
    { std::lock_guard<std::mutex> lock(gasMutex_); currentEstimates_[chain] = v; gasPriceHistory_[chain].emplace_back(std::chrono::system_clock::now(), v.maxFee); }
    return result;
}
GasEstimate GasOptimizer::estimateGas(ChainId c, const std::string& d) { auto r = estimateGasResult(c, d); if (r) return r.value; GasEstimate v; v.chainId = c; return v; }
OperationResult<double> GasOptimizer::getOptimalGasPriceResult(ChainId c) { auto r = estimateGasResult(c, {}); return r ? pass(r.value.maxFee, r.value.evidence) : fail<double>(r.code, r.message); }
double GasOptimizer::getOptimalGasPrice(ChainId c) { auto r = getOptimalGasPriceResult(c); return r ? r.value : 0.0; }
double GasOptimizer::getBaseFee(ChainId c) { auto r = estimateGasResult(c, {}); return r ? r.value.baseFee : 0.0; }
double GasOptimizer::getPriorityFee(ChainId c) { auto r = estimateGasResult(c, {}); return r ? r.value.priorityFee : 0.0; }
OperationResult<double> GasOptimizer::updateGasPricesResult(ChainId c) { return getOptimalGasPriceResult(c); }
void GasOptimizer::updateGasPrices(ChainId c) { (void)updateGasPricesResult(c); }
std::vector<std::pair<std::chrono::system_clock::time_point, double>> GasOptimizer::getGasHistory(ChainId c) {
    std::shared_ptr<MarketDataAdapter> adapter; { std::lock_guard<std::mutex> lock(gasMutex_); adapter = marketData_; }
    if (!adapter) return {};
    auto r = adapter->getGasHistory(c);
    if (!r) return {};
    for (const auto& item : r.value) if (!nonnegative(item.second)) return {};
    std::sort(r.value.begin(), r.value.end(), [](const auto& a, const auto& b) { return a.first < b.first; }); return r.value;
}
double GasOptimizer::getHistoricalGasPrice(ChainId c, int ago) { auto h = getGasHistory(c); return ago >= 0 && static_cast<std::size_t>(ago) < h.size() ? h[h.size() - 1U - static_cast<std::size_t>(ago)].second : 0.0; }
bool GasOptimizer::shouldWaitForLowerGas(ChainId c, double t) { auto r = getOptimalGasPriceResult(c); return nonnegative(t) && r && r.value > t; }
std::chrono::system_clock::time_point GasOptimizer::predictLowGasTime(ChainId) { return {}; }

PortfolioManager::PortfolioManager(std::shared_ptr<MarketDataAdapter> m, std::shared_ptr<BlockchainExecutionAdapter> b) : marketData_(std::move(m)), blockchain_(std::move(b)) {}
void PortfolioManager::setAdapters(std::shared_ptr<MarketDataAdapter> m, std::shared_ptr<BlockchainExecutionAdapter> b) { std::lock_guard<std::mutex> lock(portfolioMutex_); marketData_ = std::move(m); blockchain_ = std::move(b); }
OperationResult<PortfolioSummary> PortfolioManager::getPortfolioSummaryResult(const std::string& wallet) {
    if (!otaku_utils::isValidEthereumAddress(wallet) && !otaku_utils::isValidSolanaAddress(wallet)) return fail<PortfolioSummary>(ResultCode::INVALID_ARGUMENT, "invalid wallet");
    std::shared_ptr<MarketDataAdapter> adapter; { std::lock_guard<std::mutex> lock(portfolioMutex_); adapter = marketData_; }
    if (!adapter) return fail<PortfolioSummary>(ResultCode::UNSUPPORTED, "no market adapter");
    auto r = adapter->getPortfolio(wallet);
    if (!r) return r;
    if (!r.value.available || r.value.evidence.empty() || !nonnegative(r.value.totalValueUsd) || !std::isfinite(r.value.totalPnL) || !std::isfinite(r.value.pnlPercentage24h)) return fail<PortfolioSummary>(ResultCode::ADAPTER_ERROR, "unverified portfolio");
    for (const auto& item : r.value.tokenBalances) if (!text(item.first) || !nonnegative(item.second)) return fail<PortfolioSummary>(ResultCode::ADAPTER_ERROR, "malformed balance");
    return r;
}
PortfolioSummary PortfolioManager::getPortfolioSummary(const std::string& w) { auto r = getPortfolioSummaryResult(w); return r ? r.value : PortfolioSummary{}; }
double PortfolioManager::getTotalValue(const std::string& w) { auto r = getPortfolioSummaryResult(w); return r ? r.value.totalValueUsd : 0.0; }
std::unordered_map<std::string, double> PortfolioManager::getAssetAllocation(const std::string& w) { auto r = getPortfolioSummaryResult(w); if (!r) return {}; double total = 0; for (const auto& p : r.value.tokenBalances) total += p.second; if (!positive(total)) return {}; std::unordered_map<std::string, double> out; for (const auto& p : r.value.tokenBalances) out[p.first] = p.second * 100.0 / total; return out; }
OperationResult<std::vector<SwapQuote>> PortfolioManager::calculateRebalanceTradesResult(const std::string& w, const std::vector<RebalanceTarget>& targets) {
    if (targets.empty()) return fail<std::vector<SwapQuote>>(ResultCode::INVALID_ARGUMENT, "empty targets");
    double total = 0;
    std::unordered_set<std::string> names;
    for (const auto& t : targets) { if (!text(t.asset) || !nonnegative(t.targetPercentage) || t.targetPercentage > 100 || !names.insert(t.asset).second) return fail<std::vector<SwapQuote>>(ResultCode::INVALID_ARGUMENT, "invalid targets"); total += t.targetPercentage; }
    if (std::abs(total - 100.0) > 1e-9) return fail<std::vector<SwapQuote>>(ResultCode::INVALID_ARGUMENT, "targets must total 100");
    std::shared_ptr<MarketDataAdapter> adapter; { std::lock_guard<std::mutex> lock(portfolioMutex_); adapter = marketData_; } if (!adapter) return fail<std::vector<SwapQuote>>(ResultCode::UNSUPPORTED, "no market adapter");
    auto r = adapter->getRebalanceQuotes(w, targetPairs(targets)); if (!r) return r; for (const auto& q : r.value) { std::string why; if (!swapQuoteValid(q, why)) return fail<std::vector<SwapQuote>>(ResultCode::ADAPTER_ERROR, why); } return r;
}
std::vector<SwapQuote> PortfolioManager::calculateRebalanceTrades(const std::string& w, const std::vector<RebalanceTarget>& t) { auto r = calculateRebalanceTradesResult(w, t); return r ? r.value : std::vector<SwapQuote>{}; }
OperationResult<std::vector<TransactionReceipt>> PortfolioManager::executeRebalanceResult(const std::string& w, const std::vector<RebalanceTarget>& t, ChainId c) {
    auto quotes = calculateRebalanceTradesResult(w, t); if (!quotes) return fail<std::vector<TransactionReceipt>>(quotes.code, quotes.message);
    std::shared_ptr<BlockchainExecutionAdapter> adapter; { std::lock_guard<std::mutex> lock(portfolioMutex_); adapter = blockchain_; } if (!adapter) return fail<std::vector<TransactionReceipt>>(ResultCode::UNSUPPORTED, "no execution adapter");
    std::vector<TransactionReceipt> out; for (const auto& q : quotes.value) { BlockchainExecutionRequest req; req.operation = BlockchainOperation::REBALANCE; req.chainId = c; req.from = w; req.asset = q.fromToken; req.secondaryAsset = q.toToken; req.amount = q.inputAmount; req.quoteId = q.quoteId; auto e = adapter->execute(req); if (!e) return fail<std::vector<TransactionReceipt>>(e.code, e.message); std::string why; if (!receiptValid(e.value.receipt, why)) return fail<std::vector<TransactionReceipt>>(ResultCode::ADAPTER_ERROR, why); out.push_back(e.value.receipt); } return pass(std::move(out));
}
bool PortfolioManager::executeRebalance(const std::string& w, const std::vector<RebalanceTarget>& t) { auto r = executeRebalanceResult(w, t); return r && !r.value.empty(); }
double PortfolioManager::getPnL(const std::string& w, int days) { auto r = days > 0 ? getPortfolioSummaryResult(w) : fail<PortfolioSummary>(ResultCode::INVALID_ARGUMENT, "invalid days"); return r ? r.value.totalPnL : 0.0; }
std::vector<std::pair<std::chrono::system_clock::time_point, double>> PortfolioManager::getValueHistory(const std::string& w, int days) { std::shared_ptr<MarketDataAdapter> a; { std::lock_guard<std::mutex> lock(portfolioMutex_); a = marketData_; } if (!a || days <= 0) return {}; auto r = a->getPortfolioHistory(w, days); if (!r) return {}; for (const auto& p : r.value) if (!nonnegative(p.second)) return {}; std::sort(r.value.begin(), r.value.end()); return r.value; }
double PortfolioManager::calculateVolatility(const std::string& w) { auto h = getValueHistory(w, 30); if (h.size() < 2) return 0; std::vector<double> returns; for (std::size_t i = 1; i < h.size(); ++i) { if (!positive(h[i-1].second)) return 0; returns.push_back((h[i].second-h[i-1].second)/h[i-1].second); } double mean = std::accumulate(returns.begin(), returns.end(), 0.0)/returns.size(), sum = 0; for (double v : returns) sum += (v-mean)*(v-mean); return std::sqrt(sum/returns.size())*std::sqrt(365.0); }
double PortfolioManager::calculateSharpeRatio(const std::string& w) { auto h = getValueHistory(w,365); double v = calculateVolatility(w); return h.size() >= 2 && positive(h.front().second) && positive(v) ? ((h.back().second-h.front().second)/h.front().second)/v : 0.0; }
std::vector<std::string> PortfolioManager::getHighRiskPositions(const std::string& w) { auto a = getAssetAllocation(w); std::vector<std::string> out; for (const auto& p : a) if (p.second > 30) out.push_back(p.first+" ("+number(p.second)+"%)"); std::sort(out.begin(),out.end()); return out; }

YieldManager::YieldManager(std::shared_ptr<MarketDataAdapter> m, std::shared_ptr<BlockchainExecutionAdapter> b) : marketData_(std::move(m)), blockchain_(std::move(b)) {}
void YieldManager::setAdapters(std::shared_ptr<MarketDataAdapter> m, std::shared_ptr<BlockchainExecutionAdapter> b) { std::lock_guard<std::mutex> lock(yieldMutex_); marketData_=std::move(m); blockchain_=std::move(b); localPositions_.clear(); }
void YieldManager::setContext(const std::string& w, ChainId c) { std::lock_guard<std::mutex> lock(yieldMutex_); walletAddress_=w; chain_=c; }
OperationResult<std::vector<YieldPosition>> YieldManager::findBestYieldsResult(const std::string& a, YieldStrategy s, double min) { if (!text(a)||!validStrategy(s)||!nonnegative(min)) return fail<std::vector<YieldPosition>>(ResultCode::INVALID_ARGUMENT,"invalid yield query"); std::shared_ptr<MarketDataAdapter> m; {std::lock_guard<std::mutex> lock(yieldMutex_);m=marketData_;} if(!m)return fail<std::vector<YieldPosition>>(ResultCode::UNSUPPORTED,"no market adapter"); auto r=m->findBestYields(a,s,min); if(!r)return r; for(const auto& p:r.value)if(!text(p.protocol)||!text(p.asset)||!nonnegative(p.apy))return fail<std::vector<YieldPosition>>(ResultCode::ADAPTER_ERROR,"malformed yield"); std::sort(r.value.begin(),r.value.end(),[](const auto& x,const auto& y){return std::tie(y.apy,x.protocol)<std::tie(x.apy,y.protocol);}); return r; }
std::vector<YieldPosition> YieldManager::findBestYields(const std::string&a,YieldStrategy s,double m){auto r=findBestYieldsResult(a,s,m);return r?r.value:std::vector<YieldPosition>{};}
std::vector<std::string> YieldManager::getSupportedProtocols(ChainId c){std::shared_ptr<MarketDataAdapter>m;{std::lock_guard<std::mutex>lock(yieldMutex_);m=marketData_;}if(!m||!supported(c))return{};auto r=m->getSupportedProtocols(c);if(!r)return{};std::sort(r.value.begin(),r.value.end());r.value.erase(std::unique(r.value.begin(),r.value.end()),r.value.end());return r.value;}
OperationResult<TransactionReceipt> YieldManager::depositToYieldResult(const std::string& protocol,const std::string& assetName,double amount){if(!text(protocol)||!text(assetName)||!positive(amount))return fail<TransactionReceipt>(ResultCode::INVALID_ARGUMENT,"invalid yield deposit");std::shared_ptr<BlockchainExecutionAdapter>b;std::string w;ChainId c;{std::lock_guard<std::mutex>lock(yieldMutex_);b=blockchain_;w=walletAddress_;c=chain_;}if(w.empty())return fail<TransactionReceipt>(ResultCode::NOT_CONNECTED,"wallet not connected");if(!b)return fail<TransactionReceipt>(ResultCode::UNSUPPORTED,"no execution adapter");BlockchainExecutionRequest q;q.operation=BlockchainOperation::YIELD_DEPOSIT;q.chainId=c;q.from=w;q.asset=assetName;q.amount=amount;q.metadata["protocol"]=protocol;auto r=b->execute(q);if(!r)return fail<TransactionReceipt>(r.code,r.message);std::string why;if(!receiptValid(r.value.receipt,why))return fail<TransactionReceipt>(ResultCode::ADAPTER_ERROR,why);if((r.value.receipt.status==TxStatus::CONFIRMED||r.value.receipt.status==TxStatus::SIMULATED)&&!r.value.yieldPosition)return fail<TransactionReceipt>(ResultCode::ADAPTER_ERROR,"missing yield position evidence");if(r.value.yieldPosition){auto p=*r.value.yieldPosition;if(p.positionId.empty()||p.owner!=w||p.protocol!=protocol||p.asset!=assetName||!positive(p.depositedAmount)||p.receiptTxHash!=r.value.receipt.txHash||p.status!=r.value.receipt.status||p.evidence.empty())return fail<TransactionReceipt>(ResultCode::ADAPTER_ERROR,"inconsistent yield position");std::lock_guard<std::mutex>lock(yieldMutex_);localPositions_[p.positionId]=p;}return pass(r.value.receipt,r.evidence);}
bool YieldManager::depositToYield(const std::string&p,const std::string&a,double v){return depositToYieldResult(p,a,v).ok();}
OperationResult<TransactionReceipt> YieldManager::executePositionOperation(BlockchainOperation op,const std::string&id,double amount){if(id.empty()||(op==BlockchainOperation::YIELD_WITHDRAW&&!positive(amount)))return fail<TransactionReceipt>(ResultCode::INVALID_ARGUMENT,"invalid yield operation");std::shared_ptr<BlockchainExecutionAdapter>b;YieldPosition p;{std::lock_guard<std::mutex>lock(yieldMutex_);auto it=localPositions_.find(id);if(it==localPositions_.end()||it->second.owner!=walletAddress_)return fail<TransactionReceipt>(ResultCode::NOT_FOUND,"owned yield position not found");if(op==BlockchainOperation::YIELD_WITHDRAW&&amount>it->second.depositedAmount)return fail<TransactionReceipt>(ResultCode::INVALID_ARGUMENT,"withdraw exceeds position");p=it->second;b=blockchain_;}if(!b)return fail<TransactionReceipt>(ResultCode::UNSUPPORTED,"no execution adapter");BlockchainExecutionRequest q;q.operation=op;q.chainId=p.chainId;q.from=p.owner;q.asset=p.asset;q.amount=amount;q.referenceId=id;q.metadata["protocol"]=p.protocol;auto r=b->execute(q);if(!r)return fail<TransactionReceipt>(r.code,r.message);std::string why;if(!receiptValid(r.value.receipt,why))return fail<TransactionReceipt>(ResultCode::ADAPTER_ERROR,why);if(r.value.receipt.status==TxStatus::CONFIRMED||r.value.receipt.status==TxStatus::SIMULATED){std::lock_guard<std::mutex>lock(yieldMutex_);auto it=localPositions_.find(id);if(it==localPositions_.end())return fail<TransactionReceipt>(ResultCode::STATE_CONFLICT,"position changed");if(op==BlockchainOperation::YIELD_WITHDRAW){it->second.depositedAmount-=amount;if(it->second.depositedAmount<=1e-12)localPositions_.erase(it);}else if(op==BlockchainOperation::YIELD_CLAIM)it->second.earnedRewards=0;else if(op==BlockchainOperation::YIELD_COMPOUND){it->second.depositedAmount+=it->second.earnedRewards;it->second.earnedRewards=0;}}return pass(r.value.receipt,r.evidence);}
OperationResult<TransactionReceipt> YieldManager::withdrawFromYieldResult(const std::string&i,double a){return executePositionOperation(BlockchainOperation::YIELD_WITHDRAW,i,a);}bool YieldManager::withdrawFromYield(const std::string&i,double a){return withdrawFromYieldResult(i,a).ok();}
OperationResult<TransactionReceipt> YieldManager::claimRewardsResult(const std::string&i){return executePositionOperation(BlockchainOperation::YIELD_CLAIM,i,0);}bool YieldManager::claimRewards(const std::string&i){return claimRewardsResult(i).ok();}
OperationResult<TransactionReceipt> YieldManager::compoundRewardsResult(const std::string&i){return executePositionOperation(BlockchainOperation::YIELD_COMPOUND,i,0);}bool YieldManager::compoundRewards(const std::string&i){return compoundRewardsResult(i).ok();}
std::vector<YieldPosition> YieldManager::getActivePositions(const std::string&w){std::vector<YieldPosition>out;std::shared_ptr<MarketDataAdapter>m;ChainId c;{std::lock_guard<std::mutex>lock(yieldMutex_);m=marketData_;c=chain_;for(const auto& item:localPositions_)if(item.second.owner==w)out.push_back(item.second);}if(m&&address(w,c)){auto r=m->getYieldPositions(w,c);if(r)for(const auto&p:r.value)if(p.owner==w&&!p.positionId.empty()&&nonnegative(p.depositedAmount)&&nonnegative(p.earnedRewards))out.push_back(p);}std::sort(out.begin(),out.end(),[](const auto&a,const auto&b){return a.positionId<b.positionId;});out.erase(std::unique(out.begin(),out.end(),[](const auto&a,const auto&b){return a.positionId==b.positionId;}),out.end());return out;}
double YieldManager::getTotalYieldEarned(const std::string&w){auto p=getActivePositions(w);return std::accumulate(p.begin(),p.end(),0.0,[](double t,const auto&v){return t+v.earnedRewards;});}
double YieldManager::getProtocolRiskScore(const std::string&p){std::shared_ptr<MarketDataAdapter>m;{std::lock_guard<std::mutex>lock(yieldMutex_);m=marketData_;}if(!m||!text(p))return 0;auto r=m->getProtocolRiskScore(p);return r&&nonnegative(r.value)&&r.value<=1?r.value:0;}
bool YieldManager::isProtocolAudited(const std::string&p){std::shared_ptr<MarketDataAdapter>m;{std::lock_guard<std::mutex>lock(yieldMutex_);m=marketData_;}if(!m)return false;auto r=m->isProtocolAudited(p);return r&&r.value;}
double YieldManager::getImpermanentLossRisk(const std::string&p){std::shared_ptr<MarketDataAdapter>m;{std::lock_guard<std::mutex>lock(yieldMutex_);m=marketData_;}if(!m||!otaku_utils::isValidEthereumAddress(p))return 0;auto r=m->getImpermanentLossRisk(p);return r&&nonnegative(r.value)&&r.value<=1?r.value:0;}

NFTManager::NFTManager(std::shared_ptr<MarketDataAdapter>m,std::shared_ptr<BlockchainExecutionAdapter>b):marketData_(std::move(m)),blockchain_(std::move(b)){}
void NFTManager::setAdapters(std::shared_ptr<MarketDataAdapter>m,std::shared_ptr<BlockchainExecutionAdapter>b){std::lock_guard<std::mutex>lock(nftMutex_);marketData_=std::move(m);blockchain_=std::move(b);localNfts_.clear();transferredNfts_.clear();}
void NFTManager::setContext(const std::string&w,ChainId c){std::lock_guard<std::mutex>lock(nftMutex_);walletAddress_=w;chain_=c;}
std::vector<NFTInfo> NFTManager::getOwnedNFTs(const std::string&w,ChainId c){if(!address(w,c))return{};std::shared_ptr<MarketDataAdapter>m;std::unordered_map<std::string,NFTInfo>local;std::unordered_map<std::string,bool>transferred;{std::lock_guard<std::mutex>lock(nftMutex_);m=marketData_;local=localNfts_;transferred=transferredNfts_;}std::vector<NFTInfo>out;if(m){auto r=m->getOwnedNFTs(w,c);if(r)out=r.value;}out.erase(std::remove_if(out.begin(),out.end(),[&](const auto&n){const std::string key=n.contractAddress+":"+n.tokenId;return n.owner!=w||n.chainId!=c||n.tokenId.empty()||!address(n.contractAddress,c)||transferred.count(key)>0;}),out.end());for(const auto&item:local)if(item.second.owner==w&&item.second.chainId==c)out.push_back(item.second);std::sort(out.begin(),out.end(),[](const auto&a,const auto&b){return std::tie(a.contractAddress,a.tokenId)<std::tie(b.contractAddress,b.tokenId);});out.erase(std::unique(out.begin(),out.end(),[](const auto&a,const auto&b){return a.contractAddress==b.contractAddress&&a.tokenId==b.tokenId;}),out.end());return out;}
NFTInfo NFTManager::getNFTDetails(const std::string&c,const std::string&i,ChainId chain){std::shared_ptr<MarketDataAdapter>m;{std::lock_guard<std::mutex>lock(nftMutex_);m=marketData_;}if(!m||!address(c,chain)||i.empty())return{};auto r=m->getNFTDetails(c,i,chain);return r&&r.value.contractAddress==c&&r.value.tokenId==i?r.value:NFTInfo{};}
double NFTManager::getNFTFloorPrice(const std::string&c,ChainId chain){std::shared_ptr<MarketDataAdapter>m;{std::lock_guard<std::mutex>lock(nftMutex_);m=marketData_;}if(!m||!address(c,chain))return 0;auto r=m->getNFTFloorPrice(c,chain);return r&&nonnegative(r.value)?r.value:0;}
OperationResult<TransactionReceipt> NFTManager::executeNftOperation(BlockchainOperation op,const std::string&contract,const std::string&id,const std::string&to,double amount,ChainId c){if(!address(contract,c)||id.empty()||(op==BlockchainOperation::NFT_TRANSFER&&!address(to,c))||((op==BlockchainOperation::NFT_LIST||op==BlockchainOperation::NFT_BUY)&&!positive(amount)))return fail<TransactionReceipt>(ResultCode::INVALID_ARGUMENT,"invalid NFT operation");std::shared_ptr<BlockchainExecutionAdapter>b;std::string w;{std::lock_guard<std::mutex>lock(nftMutex_);b=blockchain_;w=walletAddress_;}if(w.empty())return fail<TransactionReceipt>(ResultCode::NOT_CONNECTED,"wallet not connected");if(op==BlockchainOperation::NFT_TRANSFER||op==BlockchainOperation::NFT_LIST){auto owned=getOwnedNFTs(w,c);auto found=std::find_if(owned.begin(),owned.end(),[&](const auto&n){return n.contractAddress==contract&&n.tokenId==id;});if(found==owned.end())return fail<TransactionReceipt>(ResultCode::NOT_FOUND,"owned NFT not found");}if(!b)return fail<TransactionReceipt>(ResultCode::UNSUPPORTED,"no execution adapter");BlockchainExecutionRequest q;q.operation=op;q.chainId=c;q.from=w;q.to=to;q.asset=contract;q.referenceId=id;q.amount=amount;auto r=b->execute(q);if(!r)return fail<TransactionReceipt>(r.code,r.message);std::string why;if(!receiptValid(r.value.receipt,why)||r.value.receipt.from!=w||r.value.receipt.chainId!=c)return fail<TransactionReceipt>(ResultCode::ADAPTER_ERROR,why.empty()?"NFT receipt mismatch":why);if(r.value.receipt.status==TxStatus::CONFIRMED||r.value.receipt.status==TxStatus::SIMULATED){const std::string key=contract+":"+id;std::lock_guard<std::mutex>lock(nftMutex_);if(op==BlockchainOperation::NFT_TRANSFER){transferredNfts_[key]=true;localNfts_.erase(key);}else if(op==BlockchainOperation::NFT_BUY){if(!r.value.nft||r.value.nft->contractAddress!=contract||r.value.nft->tokenId!=id||r.value.nft->owner!=w||r.value.nft->chainId!=c||r.value.nft->receiptTxHash!=r.value.receipt.txHash||r.value.nft->status!=r.value.receipt.status||r.value.nft->evidence.empty())return fail<TransactionReceipt>(ResultCode::ADAPTER_ERROR,"inconsistent NFT purchase evidence");localNfts_[key]=*r.value.nft;transferredNfts_.erase(key);}}return pass(r.value.receipt,r.evidence);}
OperationResult<TransactionReceipt>NFTManager::transferNFTResult(const std::string&c,const std::string&i,const std::string&t,ChainId ch){return executeNftOperation(BlockchainOperation::NFT_TRANSFER,c,i,t,0,ch);}bool NFTManager::transferNFT(const std::string&c,const std::string&i,const std::string&t,ChainId ch){return transferNFTResult(c,i,t,ch).ok();}
OperationResult<TransactionReceipt>NFTManager::listNFTForSaleResult(const std::string&c,const std::string&i,double p,const std::string&m){if(!text(m))return fail<TransactionReceipt>(ResultCode::INVALID_ARGUMENT,"invalid marketplace");return executeNftOperation(BlockchainOperation::NFT_LIST,c,i,m,p,chain_);}bool NFTManager::listNFTForSale(const std::string&c,const std::string&i,double p,const std::string&m){return listNFTForSaleResult(c,i,p,m).ok();}
OperationResult<TransactionReceipt>NFTManager::buyNFTResult(const std::string&c,const std::string&i,double p){return executeNftOperation(BlockchainOperation::NFT_BUY,c,i,{},p,chain_);}bool NFTManager::buyNFT(const std::string&c,const std::string&i,double p){return buyNFTResult(c,i,p).ok();}
std::vector<NFTInfo>NFTManager::getTrendingCollections(ChainId c){std::shared_ptr<MarketDataAdapter>m;{std::lock_guard<std::mutex>lock(nftMutex_);m=marketData_;}if(!m)return{};auto r=m->getTrendingCollections(c);return r?r.value:std::vector<NFTInfo>{};}
double NFTManager::getCollectionVolume24h(const std::string&c){std::shared_ptr<MarketDataAdapter>m;{std::lock_guard<std::mutex>lock(nftMutex_);m=marketData_;}if(!m)return 0;auto r=m->getCollectionVolume24h(c);return r&&nonnegative(r.value)?r.value:0;}

TransactionSimulator::TransactionSimulator(std::shared_ptr<BlockchainExecutionAdapter>b):blockchain_(std::move(b)){}
void TransactionSimulator::setBlockchainAdapter(std::shared_ptr<BlockchainExecutionAdapter>b){std::lock_guard<std::mutex>lock(simMutex_);blockchain_=std::move(b);}
TransactionSimulator::SimulationResult TransactionSimulator::simulateTransaction(const std::string&f,const std::string&t,const std::string&d,double v,ChainId c){TransactionSimulation bad;auto reject=[&](std::string why){bad.errorMessage=std::move(why);return bad;};if(!address(f,c)||!address(t,c)||!nonnegative(v)||d.size()>1000000)return reject("invalid simulation request");std::shared_ptr<BlockchainExecutionAdapter>b;{std::lock_guard<std::mutex>lock(simMutex_);b=blockchain_;}if(!b)return reject("no simulation adapter");auto r=b->simulate(f,t,d,v,c);if(!r)return reject(r.message);if(!r.value.evidenceVerified||r.value.evidence.empty()||!nonnegative(r.value.gasUsed)||!nonnegative(r.value.estimatedPriceImpact))return reject("unverified simulation");return r.value;}
TransactionSimulator::SimulationResult TransactionSimulator::simulateSwap(const SwapQuote&q){TransactionSimulation r;std::string why;if(!swapQuoteValid(q,why))r.errorMessage=why;else r.errorMessage="swap simulation requires calldata";return r;}
TransactionSimulator::SimulationResult TransactionSimulator::simulateBridge(const BridgeQuote&q){TransactionSimulation r;std::string why;if(!bridgeQuoteValid(q,why))r.errorMessage=why;else r.errorMessage="bridge simulation requires calldata";return r;}
bool TransactionSimulator::detectPotentialScam(const std::string&a,ChainId c){return !getContractWarnings(a,c).empty();}
bool TransactionSimulator::verifyContractSafety(const std::string&a,ChainId c){std::shared_ptr<BlockchainExecutionAdapter>b;{std::lock_guard<std::mutex>lock(simMutex_);b=blockchain_;}if(!b||!address(a,c))return false;auto r=b->verifyContractSafety(a,c);return r&&!r.evidence.empty()&&r.value;}
std::vector<std::string>TransactionSimulator::getContractWarnings(const std::string&a,ChainId c){if(!address(a,c))return{"invalid contract address"};std::shared_ptr<BlockchainExecutionAdapter>b;{std::lock_guard<std::mutex>lock(simMutex_);b=blockchain_;}if(!b)return{};auto r=b->getContractWarnings(a,c);return r?r.value:std::vector<std::string>{};}

OtakuAgent::OtakuAgent(const std::string&id):OtakuAgent(id,{},{}){}
OtakuAgent::OtakuAgent(const std::string&id,std::shared_ptr<MarketDataAdapter>m,std::shared_ptr<BlockchainExecutionAdapter>b):agentId_(id),logger_(std::make_shared<AgentLogger>()),marketData_(std::move(m)),blockchain_(std::move(b)){initializeSubManagers();}
void OtakuAgent::initializeSubManagers(){gasOptimizer_=std::make_unique<GasOptimizer>(marketData_);portfolioManager_=std::make_unique<PortfolioManager>(marketData_,blockchain_);yieldManager_=std::make_unique<YieldManager>(marketData_,blockchain_);nftManager_=std::make_unique<NFTManager>(marketData_,blockchain_);txSimulator_=std::make_unique<TransactionSimulator>(blockchain_);}
void OtakuAgent::setAdapters(std::shared_ptr<MarketDataAdapter>m,std::shared_ptr<BlockchainExecutionAdapter>b){{std::lock_guard<std::mutex>lock(agentMutex_);marketData_=m;blockchain_=b;}gasOptimizer_->setMarketDataAdapter(m);portfolioManager_->setAdapters(m,b);yieldManager_->setAdapters(m,b);nftManager_->setAdapters(m,b);txSimulator_->setBlockchainAdapter(b);}
bool OtakuAgent::connectWallet(const std::string&w){ChainId c;{std::lock_guard<std::mutex>lock(agentMutex_);c=currentChain_;if(!address(w,c))return false;walletAddress_=w;}yieldManager_->setContext(w,c);nftManager_->setContext(w,c);logStatus("Wallet connected");return true;}
bool OtakuAgent::disconnectWallet(){{std::lock_guard<std::mutex>lock(agentMutex_);if(walletAddress_.empty())return false;walletAddress_.clear();}auto c=getCurrentChain();yieldManager_->setContext({},c);nftManager_->setContext({},c);logStatus("Wallet disconnected");return true;}
std::string OtakuAgent::getWalletAddress()const{std::lock_guard<std::mutex>lock(agentMutex_);return walletAddress_;}bool OtakuAgent::isWalletConnected()const{std::lock_guard<std::mutex>lock(agentMutex_);return!walletAddress_.empty();}
OperationResult<std::unordered_map<std::string,double>>OtakuAgent::getAllBalancesResult(){std::shared_ptr<MarketDataAdapter>m;std::string w;ChainId c;{std::lock_guard<std::mutex>lock(agentMutex_);m=marketData_;w=walletAddress_;c=currentChain_;}if(w.empty())return fail<std::unordered_map<std::string,double>>(ResultCode::NOT_CONNECTED,"wallet not connected");if(!m)return fail<std::unordered_map<std::string,double>>(ResultCode::UNSUPPORTED,"no market adapter");auto r=m->getBalances(w,c);if(!r)return r;for(const auto&p:r.value)if(!text(p.first)||!nonnegative(p.second))return fail<std::unordered_map<std::string,double>>(ResultCode::ADAPTER_ERROR,"malformed balance");return r;}
std::unordered_map<std::string,double>OtakuAgent::getAllBalances(){auto r=getAllBalancesResult();return r?r.value:std::unordered_map<std::string,double>{};}
OperationResult<std::string>OtakuAgent::getBalanceResult(const std::string&t){if(!text(t))return fail<std::string>(ResultCode::INVALID_ARGUMENT,"invalid token");auto r=getAllBalancesResult();if(!r)return fail<std::string>(r.code,r.message);auto i=r.value.find(t);return i==r.value.end()?fail<std::string>(ResultCode::NOT_FOUND,"balance unavailable"):pass(number(i->second),r.evidence);}
std::string OtakuAgent::getBalance(const std::string&t){auto r=getBalanceResult(t);return r?r.value:std::string{};}
bool OtakuAgent::switchChain(ChainId c){if(!supported(c))return false;std::string w;{std::lock_guard<std::mutex>lock(agentMutex_);if(!walletAddress_.empty()&&!address(walletAddress_,c))return false;currentChain_=c;w=walletAddress_;}yieldManager_->setContext(w,c);nftManager_->setContext(w,c);logStatus("Chain switched");return true;}
ChainId OtakuAgent::getCurrentChain()const{std::lock_guard<std::mutex>lock(agentMutex_);return currentChain_;}
std::vector<ChainId>OtakuAgent::getSupportedChains()const{return{ChainId::ETHEREUM_MAINNET,ChainId::OPTIMISM,ChainId::BSC,ChainId::POLYGON,ChainId::ARBITRUM,ChainId::BASE,ChainId::AVALANCHE,ChainId::SOLANA};}
OperationResult<BlockchainExecutionResult>OtakuAgent::executeRequest(const BlockchainExecutionRequest&q){std::shared_ptr<BlockchainExecutionAdapter>b;{std::lock_guard<std::mutex>lock(agentMutex_);b=blockchain_;}if(!b)return fail<BlockchainExecutionResult>(ResultCode::UNSUPPORTED,"no execution adapter");auto r=b->execute(q);if(!r)return r;std::string why;if(!receiptValid(r.value.receipt,why)||r.value.receipt.chainId!=q.chainId||(!q.from.empty()&&r.value.receipt.from!=q.from))return fail<BlockchainExecutionResult>(ResultCode::ADAPTER_ERROR,why.empty()?"receipt mismatch":why);return r;}
OperationResult<TransactionReceipt>OtakuAgent::updateRecordedReceipt(const TransactionReceipt&r){std::string why;if(!receiptValid(r,why))return fail<TransactionReceipt>(ResultCode::ADAPTER_ERROR,why);std::lock_guard<std::mutex>lock(agentMutex_);auto it=transactionIndex_.find(r.txHash);if(it==transactionIndex_.end()){transactionIndex_[r.txHash]=transactionHistory_.size();transactionHistory_.push_back(r);}else{auto&old=transactionHistory_[it->second];if(!transition(old.status,r.status))return fail<TransactionReceipt>(ResultCode::STATE_CONFLICT,"invalid receipt transition");old=r;}return pass(r,r.evidence);}
OperationResult<TransactionReceipt>OtakuAgent::recordExecutionResult(const OperationResult<BlockchainExecutionResult>&r,const BlockchainExecutionRequest&q){if(!r)return fail<TransactionReceipt>(r.code,r.message);if(r.value.receipt.from!=q.from||r.value.receipt.chainId!=q.chainId)return fail<TransactionReceipt>(ResultCode::ADAPTER_ERROR,"receipt mismatch");return updateRecordedReceipt(r.value.receipt);}
OperationResult<SwapQuote>OtakuAgent::getSwapQuoteResult(const std::string&f,const std::string&t,double a,const MEVProtectionOptions&o){if(!text(f)||!text(t)||f==t||!positive(a)||!nonnegative(o.maxSlippage)||o.maxSlippage>100||(o.deadline&&(!std::isfinite(*o.deadline)||*o.deadline<=std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count())))return fail<SwapQuote>(ResultCode::INVALID_ARGUMENT,"invalid swap request");std::shared_ptr<MarketDataAdapter>m;ChainId c;{std::lock_guard<std::mutex>lock(agentMutex_);m=marketData_;c=currentChain_;}if(!m)return fail<SwapQuote>(ResultCode::UNSUPPORTED,"no market adapter");auto r=m->getSwapQuote(c,f,t,a,o);if(!r)return r;std::string why;if(!swapQuoteValid(r.value,why)||r.value.chainId!=c||r.value.fromToken!=f||r.value.toToken!=t||r.value.inputAmount!=a)return fail<SwapQuote>(why.find("expired")!=std::string::npos?ResultCode::EXPIRED:ResultCode::ADAPTER_ERROR,why);return r;}
SwapQuote OtakuAgent::getSwapQuote(const std::string&f,const std::string&t,double a,const MEVProtectionOptions&o){auto r=getSwapQuoteResult(f,t,a,o);return r?r.value:SwapQuote{};}
OperationResult<std::vector<SwapQuote>>OtakuAgent::getMultiRouteQuotesResult(const std::string&f,const std::string&t,double a){if(!text(f)||!text(t)||f==t||!positive(a))return fail<std::vector<SwapQuote>>(ResultCode::INVALID_ARGUMENT,"invalid quote request");std::shared_ptr<MarketDataAdapter>m;ChainId c;{std::lock_guard<std::mutex>lock(agentMutex_);m=marketData_;c=currentChain_;}if(!m)return fail<std::vector<SwapQuote>>(ResultCode::UNSUPPORTED,"no market adapter");auto r=m->getMultiRouteQuotes(c,f,t,a);if(!r)return r;for(const auto&q:r.value){std::string why;if(!swapQuoteValid(q,why))return fail<std::vector<SwapQuote>>(ResultCode::ADAPTER_ERROR,why);}std::sort(r.value.begin(),r.value.end(),[](const auto&x,const auto&y){return x.expectedOutput>y.expectedOutput;});return r;}
std::vector<SwapQuote>OtakuAgent::getMultiRouteQuotes(const std::string&f,const std::string&t,double a){auto r=getMultiRouteQuotesResult(f,t,a);return r?r.value:std::vector<SwapQuote>{};}
OperationResult<TransactionReceipt>OtakuAgent::executeSwapWithQuoteResult(const SwapQuote&q){std::string why;if(!swapQuoteValid(q,why))return fail<TransactionReceipt>(why.find("expired")!=std::string::npos?ResultCode::EXPIRED:ResultCode::INVALID_ARGUMENT,why);std::string w=getWalletAddress();if(w.empty())return fail<TransactionReceipt>(ResultCode::NOT_CONNECTED,"wallet not connected");BlockchainExecutionRequest e;e.operation=BlockchainOperation::SWAP;e.chainId=q.chainId;e.from=w;e.asset=q.fromToken;e.secondaryAsset=q.toToken;e.amount=q.inputAmount;e.quoteId=q.quoteId;return recordExecutionResult(executeRequest(e),e);}
TransactionReceipt OtakuAgent::executeSwapWithQuote(const SwapQuote&q){auto r=executeSwapWithQuoteResult(q);return r?r.value:failedReceipt(getCurrentChain(),r.message);}
OperationResult<TransactionReceipt>OtakuAgent::executeSwapResult(const std::string&f,const std::string&t,double a){MEVProtectionOptions o;{std::lock_guard<std::mutex>lock(agentMutex_);o.maxSlippage=slippageTolerance_;o.useCowSwap=mevProtectionEnabled_&&preferredDex_==DexProtocol::COWSWAP;}auto q=getSwapQuoteResult(f,t,a,o);return q?executeSwapWithQuoteResult(q.value):fail<TransactionReceipt>(q.code,q.message);}
bool OtakuAgent::executeSwap(const std::string&f,const std::string&t,float a){return executeSwapResult(f,t,a).ok();}
OperationResult<BridgeQuote>OtakuAgent::getBridgeQuoteResult(ChainId s,ChainId d,const std::string&t,double a){if(!supported(s)||!supported(d)||s==d||!text(t)||!positive(a))return fail<BridgeQuote>(ResultCode::INVALID_ARGUMENT,"invalid bridge request");std::shared_ptr<MarketDataAdapter>m;{std::lock_guard<std::mutex>lock(agentMutex_);m=marketData_;}if(!m)return fail<BridgeQuote>(ResultCode::UNSUPPORTED,"no market adapter");auto r=m->getBridgeQuote(s,d,t,a);if(!r)return r;std::string why;if(!bridgeQuoteValid(r.value,why)||r.value.sourceChain!=s||r.value.destChain!=d||r.value.token!=t||r.value.inputAmount!=a)return fail<BridgeQuote>(why.find("expired")!=std::string::npos?ResultCode::EXPIRED:ResultCode::ADAPTER_ERROR,why);return r;}
BridgeQuote OtakuAgent::getBridgeQuote(ChainId s,ChainId d,const std::string&t,double a){auto r=getBridgeQuoteResult(s,d,t,a);return r?r.value:BridgeQuote{};}
OperationResult<std::vector<BridgeQuote>>OtakuAgent::getMultiBridgeQuotesResult(ChainId s,ChainId d,const std::string&t,double a){if(!supported(s)||!supported(d)||s==d||!text(t)||!positive(a))return fail<std::vector<BridgeQuote>>(ResultCode::INVALID_ARGUMENT,"invalid bridge request");std::shared_ptr<MarketDataAdapter>m;{std::lock_guard<std::mutex>lock(agentMutex_);m=marketData_;}if(!m)return fail<std::vector<BridgeQuote>>(ResultCode::UNSUPPORTED,"no market adapter");auto r=m->getMultiBridgeQuotes(s,d,t,a);if(!r)return r;for(const auto&q:r.value){std::string why;if(!bridgeQuoteValid(q,why))return fail<std::vector<BridgeQuote>>(ResultCode::ADAPTER_ERROR,why);}std::sort(r.value.begin(),r.value.end(),[](const auto&x,const auto&y){return x.outputAmount>y.outputAmount;});return r;}
std::vector<BridgeQuote>OtakuAgent::getMultiBridgeQuotes(ChainId s,ChainId d,const std::string&t,double a){auto r=getMultiBridgeQuotesResult(s,d,t,a);return r?r.value:std::vector<BridgeQuote>{};}
OperationResult<TransactionReceipt>OtakuAgent::executeBridgeWithQuoteResult(const BridgeQuote&q){std::string why;if(!bridgeQuoteValid(q,why))return fail<TransactionReceipt>(why.find("expired")!=std::string::npos?ResultCode::EXPIRED:ResultCode::INVALID_ARGUMENT,why);std::string w=getWalletAddress();if(w.empty()||!address(w,q.sourceChain))return fail<TransactionReceipt>(ResultCode::NOT_CONNECTED,"wallet unavailable on source chain");BlockchainExecutionRequest e;e.operation=BlockchainOperation::BRIDGE;e.chainId=q.sourceChain;e.from=w;e.asset=q.token;e.amount=q.inputAmount;e.quoteId=q.quoteId;e.metadata["destination_chain"]=std::to_string(static_cast<int>(q.destChain));return recordExecutionResult(executeRequest(e),e);}
TransactionReceipt OtakuAgent::executeBridgeWithQuote(const BridgeQuote&q){auto r=executeBridgeWithQuoteResult(q);return r?r.value:failedReceipt(q.sourceChain,r.message);}
OperationResult<TransactionReceipt>OtakuAgent::executeBridgeResult(const std::string&f,const std::string&t,double a){auto s=stringToChainId(f),d=stringToChainId(t);if(!s||!d)return fail<TransactionReceipt>(ResultCode::INVALID_ARGUMENT,"unknown chain");auto q=getBridgeQuoteResult(*s,*d,otaku_utils::getNativeCurrency(*s),a);return q?executeBridgeWithQuoteResult(q.value):fail<TransactionReceipt>(q.code,q.message);}
bool OtakuAgent::executeBridge(const std::string&f,const std::string&t,float a){return executeBridgeResult(f,t,a).ok();}
TxStatus OtakuAgent::getBridgeStatus(const std::string&h){std::shared_ptr<BlockchainExecutionAdapter>b;ChainId c;{std::lock_guard<std::mutex>lock(agentMutex_);b=blockchain_;c=currentChain_;}if(!b||h.empty())return TxStatus::FAILED;auto r=b->getBridgeStatus(h,c);return r?r.value:TxStatus::FAILED;}
OperationResult<TransactionReceipt>OtakuAgent::executeTransferResult(const std::string&to,const std::string&token,double amount){std::string w=getWalletAddress();ChainId c=getCurrentChain();if(w.empty())return fail<TransactionReceipt>(ResultCode::NOT_CONNECTED,"wallet not connected");if(!address(to,c)||!text(token)||!positive(amount))return fail<TransactionReceipt>(ResultCode::INVALID_ARGUMENT,"invalid transfer");BlockchainExecutionRequest q;q.operation=BlockchainOperation::TRANSFER;q.chainId=c;q.from=w;q.to=to;q.asset=token;q.amount=amount;return recordExecutionResult(executeRequest(q),q);}
bool OtakuAgent::executeTransfer(const std::string&t,const std::string&a,float v){return executeTransferResult(t,a,v).ok();}
OperationResult<TransactionReceipt>OtakuAgent::transferTokenResult(const std::string&t,const std::string&a,double v){return executeTransferResult(t,a,v);}TransactionReceipt OtakuAgent::transferToken(const std::string&t,const std::string&a,double v){auto r=transferTokenResult(t,a,v);return r?r.value:failedReceipt(getCurrentChain(),r.message);}
OperationResult<TransactionReceipt>OtakuAgent::batchTransferResult(const std::vector<std::pair<std::string,double>>&rs,const std::string&t){std::string w=getWalletAddress();ChainId c=getCurrentChain();if(w.empty())return fail<TransactionReceipt>(ResultCode::NOT_CONNECTED,"wallet not connected");if(rs.empty()||!text(t))return fail<TransactionReceipt>(ResultCode::INVALID_ARGUMENT,"invalid batch");double total=0;for(const auto&r:rs){if(!address(r.first,c)||!positive(r.second))return fail<TransactionReceipt>(ResultCode::INVALID_ARGUMENT,"invalid recipient");total+=r.second;if(!std::isfinite(total))return fail<TransactionReceipt>(ResultCode::INVALID_ARGUMENT,"invalid total");}BlockchainExecutionRequest q;q.operation=BlockchainOperation::BATCH_TRANSFER;q.chainId=c;q.from=w;q.asset=t;q.amount=total;q.metadata["recipient_count"]=std::to_string(rs.size());return recordExecutionResult(executeRequest(q),q);}
TransactionReceipt OtakuAgent::batchTransfer(const std::vector<std::pair<std::string,double>>&r,const std::string&t){auto x=batchTransferResult(r,t);return x?x.value:failedReceipt(getCurrentChain(),x.message);}
OperationResult<LiquidityPosition>OtakuAgent::addLiquidityResult(const std::string&t0,const std::string&t1,double a0,double a1,DexProtocol p){std::string w=getWalletAddress();ChainId c=getCurrentChain();if(w.empty())return fail<LiquidityPosition>(ResultCode::NOT_CONNECTED,"wallet not connected");if(!text(t0)||!text(t1)||t0==t1||!positive(a0)||!positive(a1)||!validDex(p))return fail<LiquidityPosition>(ResultCode::INVALID_ARGUMENT,"invalid liquidity request");BlockchainExecutionRequest q;q.operation=BlockchainOperation::ADD_LIQUIDITY;q.chainId=c;q.from=w;q.asset=t0;q.secondaryAsset=t1;q.amount=a0;q.secondaryAmount=a1;q.metadata["protocol"]=std::to_string(static_cast<int>(p));auto r=executeRequest(q);if(!r)return fail<LiquidityPosition>(r.code,r.message);auto rr=recordExecutionResult(r,q);if(!rr)return fail<LiquidityPosition>(rr.code,rr.message);if((rr.value.status==TxStatus::CONFIRMED||rr.value.status==TxStatus::SIMULATED)&&!r.value.liquidityPosition)return fail<LiquidityPosition>(ResultCode::ADAPTER_ERROR,"missing position evidence");if(!r.value.liquidityPosition)return fail<LiquidityPosition>(ResultCode::ADAPTER_ERROR,"position unavailable");auto pos=*r.value.liquidityPosition;if(pos.positionId.empty()||pos.owner!=w||pos.chainId!=c||pos.token0!=t0||pos.token1!=t1||!positive(pos.amount0)||!positive(pos.amount1)||pos.receiptTxHash!=rr.value.txHash||pos.status!=rr.value.status||pos.evidence.empty())return fail<LiquidityPosition>(ResultCode::ADAPTER_ERROR,"inconsistent liquidity position");{std::lock_guard<std::mutex>lock(agentMutex_);liquidityPositions_.push_back(pos);std::sort(liquidityPositions_.begin(),liquidityPositions_.end(),[](const auto&a,const auto&b){return a.positionId<b.positionId;});}return pass(pos,pos.evidence);}
LiquidityPosition OtakuAgent::addLiquidity(const std::string&a,const std::string&b,double x,double y,DexProtocol p){auto r=addLiquidityResult(a,b,x,y,p);return r?r.value:LiquidityPosition{};}
OperationResult<TransactionReceipt>OtakuAgent::removeLiquidityResult(const std::string&id,double pct){if(id.empty()||!positive(pct)||pct>100)return fail<TransactionReceipt>(ResultCode::INVALID_ARGUMENT,"invalid removal percentage");LiquidityPosition pos;{std::lock_guard<std::mutex>lock(agentMutex_);auto it=std::find_if(liquidityPositions_.begin(),liquidityPositions_.end(),[&](const auto&p){return p.positionId==id&&p.owner==walletAddress_;});if(it==liquidityPositions_.end())return fail<TransactionReceipt>(ResultCode::NOT_FOUND,"owned position not found");pos=*it;}BlockchainExecutionRequest q;q.operation=BlockchainOperation::REMOVE_LIQUIDITY;q.chainId=pos.chainId;q.from=pos.owner;q.referenceId=id;q.amount=pct;auto r=recordExecutionResult(executeRequest(q),q);if(!r)return r;if(r.value.status==TxStatus::CONFIRMED||r.value.status==TxStatus::SIMULATED){std::lock_guard<std::mutex>lock(agentMutex_);auto it=std::find_if(liquidityPositions_.begin(),liquidityPositions_.end(),[&](const auto&p){return p.positionId==id;});if(it==liquidityPositions_.end())return fail<TransactionReceipt>(ResultCode::STATE_CONFLICT,"position changed");if(pct==100)liquidityPositions_.erase(it);else{double f=1-pct/100;it->amount0*=f;it->amount1*=f;it->liquidityTokens*=f;it->currentValue*=f;}}return r;}
bool OtakuAgent::removeLiquidity(const std::string&i,double p){return removeLiquidityResult(i,p).ok();}
std::vector<LiquidityPosition>OtakuAgent::getLiquidityPositions(){std::lock_guard<std::mutex>lock(agentMutex_);return liquidityPositions_;}
double OtakuAgent::getPoolApr(const std::string&p,DexProtocol d){std::shared_ptr<MarketDataAdapter>m;{std::lock_guard<std::mutex>lock(agentMutex_);m=marketData_;}if(!m||!otaku_utils::isValidEthereumAddress(p)||!validDex(d))return 0;auto r=m->getPoolApr(p,d);return r&&nonnegative(r.value)?r.value:0;}
OperationResult<double>OtakuAgent::getTokenPriceResult(const std::string&t){if(!text(t))return fail<double>(ResultCode::INVALID_ARGUMENT,"invalid token");std::shared_ptr<MarketDataAdapter>m;ChainId c;{std::lock_guard<std::mutex>lock(agentMutex_);m=marketData_;c=currentChain_;}if(!m)return fail<double>(ResultCode::UNSUPPORTED,"no market adapter");auto r=m->getTokenPrice(t,c);if(!r)return r;return positive(r.value)&&!r.evidence.empty()?r:fail<double>(ResultCode::ADAPTER_ERROR,"unverified price");}
float OtakuAgent::getTokenPrice(const std::string&t){auto r=getTokenPriceResult(t);return r?static_cast<float>(r.value):0.0f;}
OperationResult<double>OtakuAgent::getTokenPriceInTokenResult(const std::string&t,const std::string&q){auto a=getTokenPriceResult(t),b=getTokenPriceResult(q);if(!a)return a;if(!b)return b;return pass(a.value/b.value,a.evidence+";"+b.evidence);}
double OtakuAgent::getTokenPriceInToken(const std::string&t,const std::string&q){auto r=getTokenPriceInTokenResult(t,q);return r?r.value:0;}
std::vector<std::string>OtakuAgent::getTrendingTokens(){std::shared_ptr<MarketDataAdapter>m;ChainId c;{std::lock_guard<std::mutex>lock(agentMutex_);m=marketData_;c=currentChain_;}if(!m)return{};auto r=m->getTrendingTokens(c);return r?r.value:std::vector<std::string>{};}
std::vector<TokenInfo>OtakuAgent::searchTokens(const std::string&q){std::shared_ptr<MarketDataAdapter>m;ChainId c;{std::lock_guard<std::mutex>lock(agentMutex_);m=marketData_;c=currentChain_;}if(!m||q.empty())return{};auto r=m->searchTokens(q,c);return r?r.value:std::vector<TokenInfo>{};}
double OtakuAgent::get24hPriceChange(const std::string&t){std::shared_ptr<MarketDataAdapter>m;ChainId c;{std::lock_guard<std::mutex>lock(agentMutex_);m=marketData_;c=currentChain_;}if(!m)return 0;auto r=m->get24hPriceChange(t,c);return r&&std::isfinite(r.value)?r.value:0;}
double OtakuAgent::get24hVolume(const std::string&t){std::shared_ptr<MarketDataAdapter>m;ChainId c;{std::lock_guard<std::mutex>lock(agentMutex_);m=marketData_;c=currentChain_;}if(!m)return 0;auto r=m->get24hVolume(t,c);return r&&nonnegative(r.value)?r.value:0;}
OperationResult<PortfolioSummary>OtakuAgent::getPortfolioResult(){auto w=getWalletAddress();return w.empty()?fail<PortfolioSummary>(ResultCode::NOT_CONNECTED,"wallet not connected"):portfolioManager_->getPortfolioSummaryResult(w);}PortfolioSummary OtakuAgent::getPortfolio(){auto r=getPortfolioResult();return r?r.value:PortfolioSummary{};}
OperationResult<std::vector<TransactionReceipt>>OtakuAgent::rebalancePortfolioResult(const std::vector<PortfolioManager::RebalanceTarget>&t){auto w=getWalletAddress();if(w.empty())return fail<std::vector<TransactionReceipt>>(ResultCode::NOT_CONNECTED,"wallet not connected");return portfolioManager_->executeRebalanceResult(w,t,getCurrentChain());}bool OtakuAgent::rebalancePortfolio(const std::vector<PortfolioManager::RebalanceTarget>&t){return rebalancePortfolioResult(t).ok();}
std::vector<YieldPosition>OtakuAgent::getYieldPositions(){auto w=getWalletAddress();return w.empty()?std::vector<YieldPosition>{}:yieldManager_->getActivePositions(w);}
OperationResult<TransactionReceipt>OtakuAgent::depositToYieldResult(const std::string&p,const std::string&a,double v){auto r=yieldManager_->depositToYieldResult(p,a,v);if(r)updateRecordedReceipt(r.value);return r;}bool OtakuAgent::depositToYield(const std::string&p,const std::string&a,double v){return depositToYieldResult(p,a,v).ok();}
OperationResult<TransactionReceipt>OtakuAgent::withdrawFromYieldResult(const std::string&i,double v){auto r=yieldManager_->withdrawFromYieldResult(i,v);if(r)updateRecordedReceipt(r.value);return r;}bool OtakuAgent::withdrawFromYield(const std::string&i,double v){return withdrawFromYieldResult(i,v).ok();}
OperationResult<std::vector<TransactionReceipt>>OtakuAgent::harvestAllRewardsResult(){auto p=getYieldPositions();if(p.empty())return fail<std::vector<TransactionReceipt>>(ResultCode::NOT_FOUND,"no yield positions");std::vector<TransactionReceipt>out;for(const auto&v:p){auto r=yieldManager_->claimRewardsResult(v.positionId);if(!r)return fail<std::vector<TransactionReceipt>>(r.code,r.message);updateRecordedReceipt(r.value);out.push_back(r.value);}return pass(std::move(out));}bool OtakuAgent::harvestAllRewards(){return harvestAllRewardsResult().ok();}
std::vector<NFTInfo>OtakuAgent::getOwnedNFTs(){auto w=getWalletAddress();return w.empty()?std::vector<NFTInfo>{}:nftManager_->getOwnedNFTs(w,getCurrentChain());}
OperationResult<TransactionReceipt>OtakuAgent::transferNFTResult(const std::string&c,const std::string&i,const std::string&t){auto r=nftManager_->transferNFTResult(c,i,t,getCurrentChain());if(r)updateRecordedReceipt(r.value);return r;}bool OtakuAgent::transferNFT(const std::string&c,const std::string&i,const std::string&t){return transferNFTResult(c,i,t).ok();}
OperationResult<TransactionReceipt>OtakuAgent::buyNFTResult(const std::string&c,const std::string&i,double p){auto r=nftManager_->buyNFTResult(c,i,p);if(r)updateRecordedReceipt(r.value);return r;}
bool OtakuAgent::buyNFT(const std::string&c,const std::string&i,double p){return buyNFTResult(c,i,p).ok();}
OperationResult<TransactionReceipt>OtakuAgent::getTransactionReceiptResult(const std::string&h){if(h.empty())return fail<TransactionReceipt>(ResultCode::INVALID_ARGUMENT,"empty transaction hash");std::shared_ptr<BlockchainExecutionAdapter>b;ChainId c;{std::lock_guard<std::mutex>lock(agentMutex_);b=blockchain_;c=currentChain_;auto it=transactionIndex_.find(h);if(!b&&it!=transactionIndex_.end())return pass(transactionHistory_[it->second]);}if(!b)return fail<TransactionReceipt>(ResultCode::UNSUPPORTED,"no receipt adapter");auto r=b->getTransactionReceipt(h,c);if(!r)return r;if(r.value.txHash!=h)return fail<TransactionReceipt>(ResultCode::ADAPTER_ERROR,"receipt hash mismatch");return updateRecordedReceipt(r.value);}
TransactionReceipt OtakuAgent::getTransactionReceipt(const std::string&h){auto r=getTransactionReceiptResult(h);return r?r.value:failedReceipt(getCurrentChain(),r.message);}
std::vector<TransactionReceipt>OtakuAgent::getTransactionHistory(int limit){if(limit<=0)return{};std::vector<TransactionReceipt>history;{std::lock_guard<std::mutex>lock(agentMutex_);history=transactionHistory_;}std::stable_sort(history.begin(),history.end(),[](const auto&a,const auto&b){if(a.timestamp!=b.timestamp)return a.timestamp<b.timestamp;return a.txHash<b.txHash;});if(history.size()>static_cast<std::size_t>(limit))history.resize(static_cast<std::size_t>(limit));return history;}
OperationResult<TransactionReceipt>OtakuAgent::cancelTransactionResult(const std::string&h){TransactionReceipt existing;{std::lock_guard<std::mutex>lock(agentMutex_);auto it=transactionIndex_.find(h);if(it==transactionIndex_.end())return fail<TransactionReceipt>(ResultCode::NOT_FOUND,"transaction not found");existing=transactionHistory_[it->second];}if(existing.status!=TxStatus::PENDING)return fail<TransactionReceipt>(ResultCode::STATE_CONFLICT,"only pending transactions can be cancelled");BlockchainExecutionRequest q;q.operation=BlockchainOperation::CANCEL_TRANSACTION;q.chainId=existing.chainId;q.from=existing.from;q.referenceId=h;return recordExecutionResult(executeRequest(q),q);}
bool OtakuAgent::cancelTransaction(const std::string&h){return cancelTransactionResult(h).ok();}
OperationResult<TransactionReceipt>OtakuAgent::speedUpTransactionResult(const std::string&h,double gas){if(!positive(gas))return fail<TransactionReceipt>(ResultCode::INVALID_ARGUMENT,"invalid gas increase");TransactionReceipt existing;{std::lock_guard<std::mutex>lock(agentMutex_);auto it=transactionIndex_.find(h);if(it==transactionIndex_.end())return fail<TransactionReceipt>(ResultCode::NOT_FOUND,"transaction not found");existing=transactionHistory_[it->second];}if(existing.status!=TxStatus::PENDING)return fail<TransactionReceipt>(ResultCode::STATE_CONFLICT,"only pending transactions can be replaced");BlockchainExecutionRequest q;q.operation=BlockchainOperation::SPEED_UP_TRANSACTION;q.chainId=existing.chainId;q.from=existing.from;q.referenceId=h;q.amount=gas;auto r=executeRequest(q);if(!r)return fail<TransactionReceipt>(r.code,r.message);if(r.value.receipt.txHash==h)return fail<TransactionReceipt>(ResultCode::ADAPTER_ERROR,"replacement must have a new hash");return updateRecordedReceipt(r.value.receipt);}
bool OtakuAgent::speedUpTransaction(const std::string&h,double g){return speedUpTransactionResult(h,g).ok();}
OperationResult<GasEstimate>OtakuAgent::getGasEstimateResult(const std::string&d){return gasOptimizer_->estimateGasResult(getCurrentChain(),d);}
GasEstimate OtakuAgent::getGasEstimate(const std::string&d){return gasOptimizer_->estimateGas(getCurrentChain(),d);}
bool OtakuAgent::setGasStrategy(const std::string&s){if(s!="fast"&&s!="standard"&&s!="slow")return false;std::lock_guard<std::mutex>lock(agentMutex_);gasStrategy_=s;return true;}
double OtakuAgent::getOptimalGasPrice(){return gasOptimizer_->getOptimalGasPrice(getCurrentChain());}
TransactionSimulator::SimulationResult OtakuAgent::simulateTransaction(const std::string&data){auto w=getWalletAddress();TransactionSimulation bad;if(w.empty()){bad.errorMessage="wallet not connected";return bad;}return txSimulator_->simulateTransaction(w,w,data,0,getCurrentChain());}
OperationResult<bool>OtakuAgent::verifyContractSafetyResult(const std::string&a){std::shared_ptr<BlockchainExecutionAdapter>b;ChainId c;{std::lock_guard<std::mutex>lock(agentMutex_);b=blockchain_;c=currentChain_;}if(!address(a,c))return fail<bool>(ResultCode::INVALID_ARGUMENT,"invalid contract");if(!b)return fail<bool>(ResultCode::UNSUPPORTED,"no verification adapter");auto r=b->verifyContractSafety(a,c);if(!r)return r;if(r.evidence.empty())return fail<bool>(ResultCode::ADAPTER_ERROR,"missing verification evidence");return r;}
bool OtakuAgent::verifyContractSafety(const std::string&a){auto r=verifyContractSafetyResult(a);return r&&r.value;}
std::vector<std::string>OtakuAgent::getContractWarnings(const std::string&a){return txSimulator_->getContractWarnings(a,getCurrentChain());}
bool OtakuAgent::trySetSlippageTolerance(double p){if(!nonnegative(p)||p>100)return false;std::lock_guard<std::mutex>lock(agentMutex_);slippageTolerance_=p;return true;}
void OtakuAgent::setSlippageTolerance(double p){(void)trySetSlippageTolerance(p);}
void OtakuAgent::setMEVProtection(bool e){std::lock_guard<std::mutex>lock(agentMutex_);mevProtectionEnabled_=e;}
void OtakuAgent::setPreferredDex(DexProtocol d){if(!validDex(d))return;std::lock_guard<std::mutex>lock(agentMutex_);preferredDex_=d;}
void OtakuAgent::setPreferredBridge(BridgeProtocol b){if(!validBridge(b))return;std::lock_guard<std::mutex>lock(agentMutex_);preferredBridge_=b;}
std::string OtakuAgent::getAgentId()const{std::lock_guard<std::mutex>lock(agentMutex_);return agentId_;}
std::string OtakuAgent::getStatus()const{std::lock_guard<std::mutex>lock(agentMutex_);return "Agent: "+agentId_+" | Chain: "+chainIdToString(currentChain_)+" | Wallet: "+(walletAddress_.empty()?"Not connected":"Connected");}
void OtakuAgent::setStatusCallback(std::function<void(const std::string&)>cb){std::lock_guard<std::mutex>lock(agentMutex_);statusCallback_=std::move(cb);}
void OtakuAgent::logStatus(const std::string&s){std::function<void(const std::string&)>cb;{std::lock_guard<std::mutex>lock(agentMutex_);cb=statusCallback_;}if(cb)cb(s);}
bool OtakuAgent::validateWalletConnection()const{std::lock_guard<std::mutex>lock(agentMutex_);return!walletAddress_.empty();}
std::string OtakuAgent::chainIdToString(ChainId c)const{return otaku_utils::getChainName(c);}
std::optional<ChainId>OtakuAgent::stringToChainId(const std::string&value)const{std::string s=value;std::transform(s.begin(),s.end(),s.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});if(s=="ethereum"||s=="eth")return ChainId::ETHEREUM_MAINNET;if(s=="optimism"||s=="op")return ChainId::OPTIMISM;if(s=="bsc"||s=="binance")return ChainId::BSC;if(s=="polygon"||s=="matic")return ChainId::POLYGON;if(s=="arbitrum"||s=="arb")return ChainId::ARBITRUM;if(s=="base")return ChainId::BASE;if(s=="avalanche"||s=="avax")return ChainId::AVALANCHE;if(s=="solana"||s=="sol")return ChainId::SOLANA;return std::nullopt;}

namespace otaku_utils {
bool isValidEthereumAddress(const std::string&a){if(a.size()!=42||a.rfind("0x",0)!=0)return false;return std::all_of(a.begin()+2,a.end(),[](unsigned char c){return std::isxdigit(c)!=0;});}
bool isValidSolanaAddress(const std::string&a){if(a.size()<32||a.size()>44)return false;static const std::string chars="123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";return std::all_of(a.begin(),a.end(),[](char c){return chars.find(c)!=std::string::npos;});}
std::string checksumAddress(const std::string&a){if(!isValidEthereumAddress(a))return{};std::string out=a;std::transform(out.begin()+2,out.end(),out.begin()+2,[](unsigned char c){return static_cast<char>(std::tolower(c));});return out;}
double fromWei(const std::string&w,int decimals){if(decimals<0||decimals>308||w.empty())return 0;try{std::size_t used=0;double v=std::stod(w,&used);if(used!=w.size()||!nonnegative(v))return 0;return v/std::pow(10.0,decimals);}catch(...){return 0;}}
std::string toWei(double amount,int decimals){if(!nonnegative(amount)||decimals<0||decimals>308)return{};double value=amount*std::pow(10.0,decimals);if(!std::isfinite(value))return{};std::ostringstream out;out<<std::fixed<<std::setprecision(0)<<value;return out.str();}
std::string getChainName(ChainId c){switch(c){case ChainId::ETHEREUM_MAINNET:return"Ethereum Mainnet";case ChainId::OPTIMISM:return"Optimism";case ChainId::BSC:return"BNB Smart Chain";case ChainId::POLYGON:return"Polygon";case ChainId::ARBITRUM:return"Arbitrum One";case ChainId::BASE:return"Base";case ChainId::AVALANCHE:return"Avalanche C-Chain";case ChainId::SOLANA:return"Solana";}return"Unknown Chain";}
std::string getNativeCurrency(ChainId c){switch(c){case ChainId::ETHEREUM_MAINNET:case ChainId::OPTIMISM:case ChainId::ARBITRUM:case ChainId::BASE:return"ETH";case ChainId::BSC:return"BNB";case ChainId::POLYGON:return"MATIC";case ChainId::AVALANCHE:return"AVAX";case ChainId::SOLANA:return"SOL";}return{};}
std::string getBlockExplorerUrl(ChainId c,const std::string&h){if(h.empty())return{};switch(c){case ChainId::ETHEREUM_MAINNET:return"https://etherscan.io/tx/"+h;case ChainId::OPTIMISM:return"https://optimistic.etherscan.io/tx/"+h;case ChainId::BSC:return"https://bscscan.com/tx/"+h;case ChainId::POLYGON:return"https://polygonscan.com/tx/"+h;case ChainId::ARBITRUM:return"https://arbiscan.io/tx/"+h;case ChainId::BASE:return"https://basescan.org/tx/"+h;case ChainId::AVALANCHE:return"https://snowtrace.io/tx/"+h;case ChainId::SOLANA:return"https://solscan.io/tx/"+h;}return{};}
TokenInfo getTokenInfo(const std::string&a,ChainId c){TokenInfo out;out.chainId=c;if(address(a,c))out.contractAddress=a;return out;}
std::vector<TokenInfo>getCommonTokens(ChainId){return{};}
} // namespace otaku_utils
} // namespace elizaos
