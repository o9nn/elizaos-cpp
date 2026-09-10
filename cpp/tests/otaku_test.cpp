#include <gtest/gtest.h>

#include "elizaos/otaku.hpp"

#include <atomic>
#include <chrono>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace elizaos;
using namespace std::chrono_literals;

namespace {

const std::string kWallet = "0x1111111111111111111111111111111111111111";
const std::string kOther = "0x2222222222222222222222222222222222222222";
const std::string kContract = "0x3333333333333333333333333333333333333333";

template <typename T>
OperationResult<T> ok(T value, const std::string& evidence = "fake-evidence") {
    OperationResult<T> result;
    result.code = ResultCode::OK;
    result.value = std::move(value);
    result.evidence = evidence;
    return result;
}

template <typename T>
OperationResult<T> adapterFailure(const std::string& message = "fake adapter failed") {
    OperationResult<T> result;
    result.code = ResultCode::ADAPTER_ERROR;
    result.message = message;
    return result;
}

class FakeMarketData final : public MarketDataAdapter {
public:
    bool failRequests = false;
    bool expireQuotes = false;
    bool malformedPrice = false;

    OperationResult<GasEstimate> getGasEstimate(ChainId chain, const std::string&) override {
        if (failRequests) return adapterFailure<GasEstimate>();
        GasEstimate estimate;
        estimate.chainId = chain;
        estimate.estimatedGas = 21000.0;
        estimate.baseFee = 2.0;
        estimate.priorityFee = 1.0;
        estimate.maxFee = 3.0;
        estimate.totalCostWei = 63000000000000.0;
        estimate.totalCostUsd = 0.19;
        estimate.available = true;
        estimate.evidence = "fixture:block-100";
        return ok(estimate);
    }

    OperationResult<std::unordered_map<std::string, double>> getBalances(
        const std::string&, ChainId) override {
        return ok(std::unordered_map<std::string, double>{{"ETH", 2.0}, {"USDC", 500.0}});
    }

    OperationResult<PortfolioSummary> getPortfolio(const std::string&) override {
        PortfolioSummary summary;
        summary.totalValueUsd = 5500.0;
        summary.totalPnL = 125.0;
        summary.pnlPercentage24h = 1.25;
        summary.tokenBalances = {{"ETH", 2.0}, {"USDC", 500.0}};
        summary.chainDistribution = {{ChainId::ETHEREUM_MAINNET, 5500.0}};
        summary.lastUpdated = std::chrono::system_clock::time_point{100s};
        summary.available = true;
        summary.evidence = "fixture:portfolio-1";
        return ok(summary);
    }

    OperationResult<std::vector<std::pair<std::chrono::system_clock::time_point, double>>>
    getPortfolioHistory(const std::string&, int) override {
        return ok(std::vector<std::pair<std::chrono::system_clock::time_point, double>>{
            {std::chrono::system_clock::time_point{1s}, 100.0},
            {std::chrono::system_clock::time_point{2s}, 110.0},
            {std::chrono::system_clock::time_point{3s}, 105.0}});
    }

    OperationResult<SwapQuote> getSwapQuote(
        ChainId chain, const std::string& from, const std::string& to, double amount,
        const MEVProtectionOptions& options) override {
        if (failRequests) return adapterFailure<SwapQuote>();
        SwapQuote quote;
        quote.fromToken = from;
        quote.toToken = to;
        quote.inputAmount = amount;
        quote.expectedOutput = amount * 2.0;
        quote.minimumOutput = quote.expectedOutput * (1.0 - options.maxSlippage / 100.0);
        quote.priceImpact = 0.1;
        quote.estimatedGas = 90000.0;
        quote.protocol = options.useCowSwap ? DexProtocol::COWSWAP : DexProtocol::UNISWAP_V3;
        quote.chainId = chain;
        quote.quoteId = "swap-quote-1";
        quote.evidence = "fixture:swap-quote";
        quote.evidenceVerified = true;
        quote.validUntil = std::chrono::system_clock::now() + (expireQuotes ? -1s : 60s);
        return ok(quote);
    }

    OperationResult<std::vector<SwapQuote>> getMultiRouteQuotes(
        ChainId chain, const std::string& from, const std::string& to, double amount) override {
        MEVProtectionOptions options;
        auto first = getSwapQuote(chain, from, to, amount, options);
        if (!first) return adapterFailure<std::vector<SwapQuote>>();
        auto second = first.value;
        first.value.expectedOutput = amount * 1.9;
        first.value.minimumOutput = amount * 1.8;
        second.quoteId = "swap-quote-2";
        second.expectedOutput = amount * 2.1;
        second.minimumOutput = amount * 2.0;
        return ok(std::vector<SwapQuote>{first.value, second});
    }

    OperationResult<BridgeQuote> getBridgeQuote(
        ChainId source, ChainId destination, const std::string& token, double amount) override {
        if (failRequests) return adapterFailure<BridgeQuote>();
        BridgeQuote quote;
        quote.sourceChain = source;
        quote.destChain = destination;
        quote.token = token;
        quote.inputAmount = amount;
        quote.bridgeFee = amount * 0.01;
        quote.outputAmount = amount - quote.bridgeFee;
        quote.estimatedTime = 120.0;
        quote.protocol = BridgeProtocol::ACROSS;
        quote.validUntil = std::chrono::system_clock::now() + (expireQuotes ? -1s : 60s);
        quote.quoteId = "bridge-quote-1";
        quote.evidence = "fixture:bridge-quote";
        quote.evidenceVerified = true;
        return ok(quote);
    }

    OperationResult<std::vector<BridgeQuote>> getMultiBridgeQuotes(
        ChainId source, ChainId destination, const std::string& token, double amount) override {
        auto first = getBridgeQuote(source, destination, token, amount);
        if (!first) return adapterFailure<std::vector<BridgeQuote>>();
        auto second = first.value;
        first.value.outputAmount = amount * 0.97;
        second.quoteId = "bridge-quote-2";
        second.outputAmount = amount * 0.99;
        return ok(std::vector<BridgeQuote>{first.value, second});
    }

    OperationResult<std::vector<SwapQuote>> getRebalanceQuotes(
        const std::string&, const std::vector<std::pair<std::string, double>>&) override {
        MEVProtectionOptions options;
        auto quote = getSwapQuote(ChainId::ETHEREUM_MAINNET, "USDC", "ETH", 100.0, options);
        return quote ? ok(std::vector<SwapQuote>{quote.value})
                     : adapterFailure<std::vector<SwapQuote>>();
    }

    OperationResult<double> getTokenPrice(const std::string& token, ChainId) override {
        if (failRequests) return adapterFailure<double>();
        if (malformedPrice) return ok(std::numeric_limits<double>::quiet_NaN());
        if (token == "ETH") return ok(2500.0, "fixture:price-eth");
        if (token == "USDC") return ok(1.0, "fixture:price-usdc");
        return adapterFailure<double>("unknown token");
    }

    OperationResult<std::vector<std::string>> getTrendingTokens(ChainId) override {
        return ok(std::vector<std::string>{"ETH", "USDC"});
    }

    OperationResult<std::vector<TokenInfo>> searchTokens(
        const std::string& query, ChainId chain) override {
        TokenInfo token;
        token.symbol = query;
        token.name = query;
        token.contractAddress = kContract;
        token.chainId = chain;
        token.standard = TokenStandard::ERC20;
        token.decimals = 18;
        return ok(std::vector<TokenInfo>{token});
    }

    OperationResult<double> get24hPriceChange(const std::string&, ChainId) override {
        return ok(-2.5);
    }
    OperationResult<double> get24hVolume(const std::string&, ChainId) override {
        return ok(1000000.0);
    }
};

class FakeExecution final : public BlockchainExecutionAdapter {
public:
    TxStatus nextStatus = TxStatus::SIMULATED;
    bool failRequests = false;
    bool omitEvidence = false;
    std::atomic<unsigned> sequence{0};
    mutable std::mutex mutex;
    std::unordered_map<std::string, TransactionReceipt> receipts;

    TransactionReceipt makeReceipt(const BlockchainExecutionRequest& request) {
        TransactionReceipt receipt;
        receipt.txHash = "local-" + std::to_string(sequence.fetch_add(1));
        receipt.status = nextStatus;
        receipt.chainId = request.chainId;
        receipt.from = request.from;
        receipt.to = request.to;
        receipt.gasUsed = 100.0;
        receipt.effectiveGasPrice = 2.0;
        receipt.timestamp = std::chrono::system_clock::time_point{100s};
        receipt.evidence = omitEvidence ? "" : "fixture:receipt";
        receipt.evidenceVerified = !omitEvidence;
        if (receipt.status == TxStatus::CONFIRMED) receipt.blockNumber = 100;
        return receipt;
    }

    OperationResult<BlockchainExecutionResult> execute(
        const BlockchainExecutionRequest& request) override {
        if (failRequests) return adapterFailure<BlockchainExecutionResult>();
        BlockchainExecutionResult result;
        result.receipt = makeReceipt(request);
        if (request.operation == BlockchainOperation::CANCEL_TRANSACTION) {
            result.receipt.txHash = request.referenceId;
            result.receipt.status = TxStatus::CANCELLED;
        }
        {
            std::lock_guard<std::mutex> lock(mutex);
            receipts[result.receipt.txHash] = result.receipt;
        }
        return ok(result);
    }

    OperationResult<TransactionReceipt> getTransactionReceipt(
        const std::string& hash, ChainId) override {
        std::lock_guard<std::mutex> lock(mutex);
        auto found = receipts.find(hash);
        if (found == receipts.end()) return adapterFailure<TransactionReceipt>("not found");
        return ok(found->second);
    }

    OperationResult<TxStatus> getBridgeStatus(const std::string& hash, ChainId) override {
        std::lock_guard<std::mutex> lock(mutex);
        auto found = receipts.find(hash);
        return found == receipts.end() ? adapterFailure<TxStatus>("not found")
                                      : ok(found->second.status);
    }

    OperationResult<TransactionSimulation> simulate(
        const std::string&, const std::string&, const std::string&, double, ChainId) override {
        TransactionSimulation simulation;
        simulation.success = true;
        simulation.gasUsed = 123.0;
        simulation.estimatedPriceImpact = 0.0;
        simulation.stateChanges = {"fixture-only"};
        simulation.evidence = "fixture:simulation";
        simulation.evidenceVerified = true;
        return ok(simulation);
    }

    OperationResult<bool> verifyContractSafety(const std::string&, ChainId) override {
        return ok(true, "fixture:verified-bytecode");
    }

    OperationResult<std::vector<std::string>> getContractWarnings(
        const std::string&, ChainId) override {
        return ok(std::vector<std::string>{});
    }
};

TEST(OtakuDefaults, NoAdapterNeverFabricatesCurrentClaimsOrExecution) {
    OtakuAgent agent("default");
    ASSERT_TRUE(agent.connectWallet(kWallet));
    EXPECT_EQ(agent.getBalance("ETH"), "");
    EXPECT_TRUE(agent.getAllBalances().empty());
    EXPECT_FLOAT_EQ(agent.getTokenPrice("ETH"), 0.0F);
    EXPECT_TRUE(agent.getTrendingTokens().empty());
    EXPECT_TRUE(agent.searchTokens("ETH").empty());
    EXPECT_FALSE(agent.getPortfolio().available);
    EXPECT_TRUE(agent.getTransactionHistory().empty());
    EXPECT_FALSE(agent.executeTransfer(kOther, "ETH", 1.0F));
    EXPECT_EQ(agent.transferToken(kOther, "ETH", 1.0).status, TxStatus::FAILED);
    EXPECT_EQ(agent.getGasEstimate("0x").available, false);
    EXPECT_DOUBLE_EQ(agent.getOptimalGasPrice(), 0.0);
    EXPECT_FALSE(agent.simulateTransaction("0x").success);
    EXPECT_FALSE(agent.verifyContractSafety(kContract));
}

TEST(OtakuValidation, RejectsMalformedValuesAddressesChainsAndDeadlines) {
    auto market = std::make_shared<FakeMarketData>();
    auto execution = std::make_shared<FakeExecution>();
    OtakuAgent agent("validation", market, execution);
    EXPECT_FALSE(agent.connectWallet("0x123"));
    ASSERT_TRUE(agent.connectWallet(kWallet));
    EXPECT_FALSE(agent.executeTransfer("not-an-address", "ETH", 1.0F));
    EXPECT_FALSE(agent.executeTransfer(kOther, "ETH", std::numeric_limits<float>::infinity()));
    EXPECT_FALSE(agent.trySetSlippageTolerance(std::numeric_limits<double>::quiet_NaN()));
    EXPECT_FALSE(agent.trySetSlippageTolerance(101.0));
    EXPECT_FALSE(agent.switchChain(static_cast<ChainId>(999999)));
    MEVProtectionOptions expired;
    expired.deadline = 1.0;
    EXPECT_EQ(agent.getSwapQuoteResult("ETH", "USDC", 1.0, expired).code,
              ResultCode::INVALID_ARGUMENT);
    EXPECT_EQ(agent.batchTransferResult({{kOther, -1.0}}, "ETH").code,
              ResultCode::INVALID_ARGUMENT);
}

TEST(OtakuQuotes, VerifiedQuotesSortAndExpiredQuotesAreRejected) {
    auto market = std::make_shared<FakeMarketData>();
    auto execution = std::make_shared<FakeExecution>();
    OtakuAgent agent("quotes", market, execution);
    ASSERT_TRUE(agent.connectWallet(kWallet));
    auto quotes = agent.getMultiRouteQuotesResult("ETH", "USDC", 1.0);
    ASSERT_TRUE(quotes);
    ASSERT_EQ(quotes.value.size(), 2U);
    EXPECT_GT(quotes.value[0].expectedOutput, quotes.value[1].expectedOutput);
    auto bridges = agent.getMultiBridgeQuotesResult(
        ChainId::ETHEREUM_MAINNET, ChainId::BASE, "ETH", 2.0);
    ASSERT_TRUE(bridges);
    EXPECT_GT(bridges.value[0].outputAmount, bridges.value[1].outputAmount);
    market->expireQuotes = true;
    EXPECT_EQ(agent.getSwapQuoteResult("ETH", "USDC", 1.0).code, ResultCode::EXPIRED);
    EXPECT_EQ(agent.getBridgeQuoteResult(
                  ChainId::ETHEREUM_MAINNET, ChainId::BASE, "ETH", 1.0).code,
              ResultCode::EXPIRED);
}

TEST(OtakuExecution, SimulationIsNeverPresentedAsConfirmed) {
    auto market = std::make_shared<FakeMarketData>();
    auto execution = std::make_shared<FakeExecution>();
    OtakuAgent agent("simulation", market, execution);
    ASSERT_TRUE(agent.connectWallet(kWallet));
    auto transfer = agent.transferTokenResult(kOther, "ETH", 1.0);
    ASSERT_TRUE(transfer);
    EXPECT_EQ(transfer.value.status, TxStatus::SIMULATED);
    EXPECT_EQ(transfer.value.blockNumber, 0U);
    ASSERT_EQ(agent.getTransactionHistory().size(), 1U);
    EXPECT_EQ(agent.getTransactionHistory()[0].status, TxStatus::SIMULATED);
    auto simulation = agent.simulateTransaction("0xdeadbeef");
    EXPECT_TRUE(simulation.success);
    EXPECT_TRUE(simulation.evidenceVerified);
}

TEST(OtakuExecution, ConfirmedRequiresEvidenceAndAdapterFailuresPropagate) {
    auto market = std::make_shared<FakeMarketData>();
    auto execution = std::make_shared<FakeExecution>();
    execution->nextStatus = TxStatus::CONFIRMED;
    execution->omitEvidence = true;
    OtakuAgent agent("evidence", market, execution);
    ASSERT_TRUE(agent.connectWallet(kWallet));
    EXPECT_EQ(agent.transferTokenResult(kOther, "ETH", 1.0).code, ResultCode::ADAPTER_ERROR);
    execution->omitEvidence = false;
    execution->failRequests = true;
    EXPECT_EQ(agent.transferTokenResult(kOther, "ETH", 1.0).code, ResultCode::ADAPTER_ERROR);
    market->failRequests = true;
    EXPECT_EQ(agent.getTokenPriceResult("ETH").code, ResultCode::ADAPTER_ERROR);
}

TEST(OtakuReceipts, PendingMayConfirmButTerminalReceiptCannotRegress) {
    auto market = std::make_shared<FakeMarketData>();
    auto execution = std::make_shared<FakeExecution>();
    execution->nextStatus = TxStatus::PENDING;
    OtakuAgent agent("receipts", market, execution);
    ASSERT_TRUE(agent.connectWallet(kWallet));
    auto pending = agent.transferTokenResult(kOther, "ETH", 1.0);
    ASSERT_TRUE(pending);
    ASSERT_EQ(pending.value.status, TxStatus::PENDING);

    TransactionReceipt confirmed = pending.value;
    confirmed.status = TxStatus::CONFIRMED;
    confirmed.blockNumber = 200;
    confirmed.timestamp = std::chrono::system_clock::time_point{200s};
    {
        std::lock_guard<std::mutex> lock(execution->mutex);
        execution->receipts[pending.value.txHash] = confirmed;
    }
    auto updated = agent.getTransactionReceiptResult(pending.value.txHash);
    ASSERT_TRUE(updated);
    EXPECT_EQ(updated.value.status, TxStatus::CONFIRMED);

    TransactionReceipt regressed = confirmed;
    regressed.status = TxStatus::PENDING;
    {
        std::lock_guard<std::mutex> lock(execution->mutex);
        execution->receipts[pending.value.txHash] = regressed;
    }
    EXPECT_EQ(agent.getTransactionReceiptResult(pending.value.txHash).code,
              ResultCode::STATE_CONFLICT);
    EXPECT_FALSE(agent.cancelTransaction(pending.value.txHash));
}

TEST(OtakuMarketData, GasPortfolioAndPricesRequireValidEvidence) {
    auto market = std::make_shared<FakeMarketData>();
    auto execution = std::make_shared<FakeExecution>();
    OtakuAgent agent("market", market, execution);
    ASSERT_TRUE(agent.connectWallet(kWallet));
    auto gas = agent.getGasEstimateResult("0xdead");
    ASSERT_TRUE(gas);
    EXPECT_TRUE(gas.value.available);
    EXPECT_EQ(gas.value.evidence, "fixture:block-100");
    auto portfolio = agent.getPortfolioResult();
    ASSERT_TRUE(portfolio);
    EXPECT_TRUE(portfolio.value.available);
    EXPECT_DOUBLE_EQ(agent.getTokenPriceInToken("ETH", "USDC"), 2500.0);
    market->malformedPrice = true;
    EXPECT_EQ(agent.getTokenPriceResult("ETH").code, ResultCode::ADAPTER_ERROR);
}

TEST(OtakuConcurrency, SharedStateAndCallbackAreSafeUnderReconnectAndCalls) {
    auto market = std::make_shared<FakeMarketData>();
    auto execution = std::make_shared<FakeExecution>();
    OtakuAgent agent("concurrent", market, execution);
    std::atomic<int> callbacks{0};
    agent.setStatusCallback([&](const std::string&) {
        ++callbacks;
        (void)agent.getStatus();
    });
    ASSERT_TRUE(agent.connectWallet(kWallet));
    std::vector<std::thread> threads;
    for (int thread = 0; thread < 8; ++thread) {
        threads.emplace_back([&, thread] {
            for (int i = 0; i < 100; ++i) {
                if ((thread + i) % 3 == 0) (void)agent.getTokenPrice("ETH");
                else if ((thread + i) % 3 == 1) (void)agent.getStatus();
                else (void)agent.transferTokenResult(kOther, "ETH", 1.0);
            }
        });
    }
    for (auto& thread : threads) thread.join();
    EXPECT_GT(agent.getTransactionHistory(1000).size(), 0U);
    EXPECT_GE(callbacks.load(), 1);
    EXPECT_TRUE(agent.disconnectWallet());
    EXPECT_TRUE(agent.connectWallet(kWallet));
}

} // namespace
