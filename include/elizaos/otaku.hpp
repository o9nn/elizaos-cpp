#pragma once

#include "elizaos/agentlogger.hpp"
#include "elizaos/elizaos.hpp"

#include <chrono>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace elizaos {

enum class ChainId {
    ETHEREUM_MAINNET = 1,
    OPTIMISM = 10,
    BSC = 56,
    POLYGON = 137,
    ARBITRUM = 42161,
    BASE = 8453,
    AVALANCHE = 43114,
    SOLANA = 0
};

enum class TokenStandard { ERC20, ERC721, ERC1155, NATIVE };
enum class TxStatus { PENDING, CONFIRMED, FAILED, CANCELLED, SIMULATED };
enum class DexProtocol {
    UNISWAP_V2, UNISWAP_V3, SUSHISWAP, CURVE, BALANCER, ONEINCH, PARASWAP, COWSWAP
};
enum class BridgeProtocol { ACROSS, STARGATE, HOP, MULTICHAIN, CBRIDGE, WORMHOLE, LAYERZERO };
enum class YieldStrategy {
    LIQUIDITY_PROVISION, LENDING, STAKING, YIELD_AGGREGATOR, LEVERAGED_FARMING
};

enum class ResultCode {
    OK,
    UNSUPPORTED,
    INVALID_ARGUMENT,
    NOT_CONNECTED,
    NOT_FOUND,
    EXPIRED,
    ADAPTER_ERROR,
    STATE_CONFLICT
};

template <typename T>
struct OperationResult {
    ResultCode code = ResultCode::UNSUPPORTED;
    T value{};
    std::string message;
    std::string evidence;

    bool ok() const noexcept { return code == ResultCode::OK; }
    explicit operator bool() const noexcept { return ok(); }
};

struct TokenInfo {
    std::string symbol;
    std::string name;
    std::string contractAddress;
    ChainId chainId = ChainId::ETHEREUM_MAINNET;
    TokenStandard standard = TokenStandard::ERC20;
    int decimals = 0;
    std::optional<std::string> logoUrl;
};

struct TransactionReceipt {
    std::string txHash;
    TxStatus status = TxStatus::FAILED;
    ChainId chainId = ChainId::ETHEREUM_MAINNET;
    std::string from;
    std::string to;
    double gasUsed = 0.0;
    double effectiveGasPrice = 0.0;
    std::uint64_t blockNumber = 0;
    std::chrono::system_clock::time_point timestamp{};
    std::vector<std::string> logs;
    std::string error;
    std::string evidence;
    bool evidenceVerified = false;
};

struct SwapQuote {
    std::string fromToken;
    std::string toToken;
    double inputAmount = 0.0;
    double expectedOutput = 0.0;
    double minimumOutput = 0.0;
    double priceImpact = 0.0;
    double estimatedGas = 0.0;
    DexProtocol protocol = DexProtocol::UNISWAP_V3;
    std::vector<std::string> route;
    std::chrono::system_clock::time_point validUntil{};
    ChainId chainId = ChainId::ETHEREUM_MAINNET;
    std::string quoteId;
    std::string evidence;
    bool evidenceVerified = false;
};

struct BridgeQuote {
    ChainId sourceChain = ChainId::ETHEREUM_MAINNET;
    ChainId destChain = ChainId::ETHEREUM_MAINNET;
    std::string token;
    double inputAmount = 0.0;
    double outputAmount = 0.0;
    double bridgeFee = 0.0;
    double estimatedTime = 0.0;
    BridgeProtocol protocol = BridgeProtocol::ACROSS;
    std::chrono::system_clock::time_point validUntil{};
    std::string quoteId;
    std::string evidence;
    bool evidenceVerified = false;
};

struct LiquidityPosition {
    std::string positionId;
    std::string poolAddress;
    std::string token0;
    std::string token1;
    double amount0 = 0.0;
    double amount1 = 0.0;
    double liquidityTokens = 0.0;
    double currentValue = 0.0;
    double unrealizedPnL = 0.0;
    double apr = 0.0;
    ChainId chainId = ChainId::ETHEREUM_MAINNET;
    DexProtocol protocol = DexProtocol::UNISWAP_V3;
    std::string owner;
    std::string receiptTxHash;
    TxStatus status = TxStatus::FAILED;
    std::string evidence;
};

struct YieldPosition {
    std::string positionId;
    std::string protocol;
    std::string asset;
    double depositedAmount = 0.0;
    double currentValue = 0.0;
    double earnedRewards = 0.0;
    double apy = 0.0;
    YieldStrategy strategy = YieldStrategy::LIQUIDITY_PROVISION;
    ChainId chainId = ChainId::ETHEREUM_MAINNET;
    std::chrono::system_clock::time_point depositedAt{};
    std::string owner;
    std::string receiptTxHash;
    TxStatus status = TxStatus::FAILED;
    std::string evidence;
};

struct NFTInfo {
    std::string contractAddress;
    std::string tokenId;
    std::string name;
    std::string description;
    std::optional<std::string> imageUrl;
    std::optional<std::string> animationUrl;
    std::unordered_map<std::string, std::string> attributes;
    ChainId chainId = ChainId::ETHEREUM_MAINNET;
    TokenStandard standard = TokenStandard::ERC721;
    std::string owner;
    std::string receiptTxHash;
    TxStatus status = TxStatus::FAILED;
    std::string evidence;
};

struct PortfolioSummary {
    double totalValueUsd = 0.0;
    double totalPnL = 0.0;
    double pnlPercentage24h = 0.0;
    std::unordered_map<std::string, double> tokenBalances;
    std::unordered_map<ChainId, double> chainDistribution;
    std::vector<LiquidityPosition> liquidityPositions;
    std::vector<YieldPosition> yieldPositions;
    std::vector<NFTInfo> nfts;
    std::chrono::system_clock::time_point lastUpdated{};
    bool available = false;
    std::string evidence;
};

struct GasEstimate {
    double estimatedGas = 0.0;
    double baseFee = 0.0;
    double priorityFee = 0.0;
    double maxFee = 0.0;
    double totalCostWei = 0.0;
    double totalCostUsd = 0.0;
    ChainId chainId = ChainId::ETHEREUM_MAINNET;
    bool available = false;
    std::string evidence;
};

struct MEVProtectionOptions {
    bool useFlashbots = false;
    bool useCowSwap = false;
    double maxSlippage = 0.5;
    std::optional<double> deadline;
    bool partialFill = false;
};

struct TransactionSimulation {
    bool success = false;
    std::string errorMessage;
    double gasUsed = 0.0;
    std::vector<std::string> stateChanges;
    double estimatedPriceImpact = 0.0;
    std::vector<std::string> warnings;
    std::string evidence;
    bool evidenceVerified = false;
};

enum class BlockchainOperation {
    SWAP,
    BRIDGE,
    TRANSFER,
    BATCH_TRANSFER,
    ADD_LIQUIDITY,
    REMOVE_LIQUIDITY,
    REBALANCE,
    YIELD_DEPOSIT,
    YIELD_WITHDRAW,
    YIELD_CLAIM,
    YIELD_COMPOUND,
    NFT_TRANSFER,
    NFT_LIST,
    NFT_BUY,
    CANCEL_TRANSACTION,
    SPEED_UP_TRANSACTION
};

struct BlockchainExecutionRequest {
    BlockchainOperation operation = BlockchainOperation::TRANSFER;
    ChainId chainId = ChainId::ETHEREUM_MAINNET;
    std::string from;
    std::string to;
    std::string asset;
    std::string secondaryAsset;
    double amount = 0.0;
    double secondaryAmount = 0.0;
    std::string referenceId;
    std::string quoteId;
    std::unordered_map<std::string, std::string> metadata;
};

struct BlockchainExecutionResult {
    TransactionReceipt receipt;
    std::optional<LiquidityPosition> liquidityPosition;
    std::optional<YieldPosition> yieldPosition;
    std::optional<NFTInfo> nft;
};

class MarketDataAdapter {
public:
    virtual ~MarketDataAdapter() = default;

    virtual OperationResult<GasEstimate> getGasEstimate(ChainId chain, const std::string& txData);
    virtual OperationResult<std::vector<std::pair<std::chrono::system_clock::time_point, double>>>
        getGasHistory(ChainId chain);
    virtual OperationResult<std::unordered_map<std::string, double>>
        getBalances(const std::string& walletAddress, ChainId chain);
    virtual OperationResult<PortfolioSummary> getPortfolio(const std::string& walletAddress);
    virtual OperationResult<std::vector<std::pair<std::chrono::system_clock::time_point, double>>>
        getPortfolioHistory(const std::string& walletAddress, int days);
    virtual OperationResult<SwapQuote> getSwapQuote(
        ChainId chain, const std::string& fromToken, const std::string& toToken,
        double amount, const MEVProtectionOptions& options);
    virtual OperationResult<std::vector<SwapQuote>> getMultiRouteQuotes(
        ChainId chain, const std::string& fromToken, const std::string& toToken, double amount);
    virtual OperationResult<BridgeQuote> getBridgeQuote(
        ChainId sourceChain, ChainId destChain, const std::string& token, double amount);
    virtual OperationResult<std::vector<BridgeQuote>> getMultiBridgeQuotes(
        ChainId sourceChain, ChainId destChain, const std::string& token, double amount);
    virtual OperationResult<std::vector<SwapQuote>> getRebalanceQuotes(
        const std::string& walletAddress,
        const std::vector<std::pair<std::string, double>>& targets);
    virtual OperationResult<std::vector<YieldPosition>> findBestYields(
        const std::string& asset, YieldStrategy strategy, double minApy);
    virtual OperationResult<std::vector<std::string>> getSupportedProtocols(ChainId chain);
    virtual OperationResult<std::vector<YieldPosition>> getYieldPositions(
        const std::string& walletAddress, ChainId chain);
    virtual OperationResult<double> getProtocolRiskScore(const std::string& protocol);
    virtual OperationResult<bool> isProtocolAudited(const std::string& protocol);
    virtual OperationResult<double> getImpermanentLossRisk(const std::string& poolAddress);
    virtual OperationResult<std::vector<NFTInfo>> getOwnedNFTs(
        const std::string& walletAddress, ChainId chain);
    virtual OperationResult<NFTInfo> getNFTDetails(
        const std::string& contractAddress, const std::string& tokenId, ChainId chain);
    virtual OperationResult<double> getNFTFloorPrice(
        const std::string& collectionAddress, ChainId chain);
    virtual OperationResult<std::vector<NFTInfo>> getTrendingCollections(ChainId chain);
    virtual OperationResult<double> getCollectionVolume24h(const std::string& collectionAddress);
    virtual OperationResult<double> getPoolApr(const std::string& poolAddress, DexProtocol protocol);
    virtual OperationResult<double> getTokenPrice(const std::string& token, ChainId chain);
    virtual OperationResult<std::vector<std::string>> getTrendingTokens(ChainId chain);
    virtual OperationResult<std::vector<TokenInfo>> searchTokens(const std::string& query, ChainId chain);
    virtual OperationResult<double> get24hPriceChange(const std::string& token, ChainId chain);
    virtual OperationResult<double> get24hVolume(const std::string& token, ChainId chain);
};

class BlockchainExecutionAdapter {
public:
    virtual ~BlockchainExecutionAdapter() = default;

    virtual OperationResult<BlockchainExecutionResult> execute(const BlockchainExecutionRequest& request);
    virtual OperationResult<TransactionReceipt> getTransactionReceipt(
        const std::string& txHash, ChainId chain);
    virtual OperationResult<TxStatus> getBridgeStatus(const std::string& txHash, ChainId sourceChain);
    virtual OperationResult<TransactionSimulation> simulate(
        const std::string& from, const std::string& to, const std::string& data,
        double value, ChainId chain);
    virtual OperationResult<bool> verifyContractSafety(
        const std::string& contractAddress, ChainId chain);
    virtual OperationResult<std::vector<std::string>> getContractWarnings(
        const std::string& contractAddress, ChainId chain);
};

class GasOptimizer {
public:
    explicit GasOptimizer(std::shared_ptr<MarketDataAdapter> marketData = {});
    ~GasOptimizer() = default;

    void setMarketDataAdapter(std::shared_ptr<MarketDataAdapter> marketData);
    OperationResult<GasEstimate> estimateGasResult(ChainId chain, const std::string& txData);
    GasEstimate estimateGas(ChainId chain, const std::string& txData);
    OperationResult<double> getOptimalGasPriceResult(ChainId chain);
    double getOptimalGasPrice(ChainId chain);
    double getBaseFee(ChainId chain);
    double getPriorityFee(ChainId chain);
    OperationResult<double> updateGasPricesResult(ChainId chain);
    void updateGasPrices(ChainId chain);
    double getHistoricalGasPrice(ChainId chain, int blocksAgo);
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> getGasHistory(ChainId chain);
    bool shouldWaitForLowerGas(ChainId chain, double threshold);
    std::chrono::system_clock::time_point predictLowGasTime(ChainId chain);

private:
    std::shared_ptr<MarketDataAdapter> marketData_;
    std::unordered_map<ChainId, std::vector<std::pair<std::chrono::system_clock::time_point, double>>>
        gasPriceHistory_;
    std::unordered_map<ChainId, GasEstimate> currentEstimates_;
    mutable std::mutex gasMutex_;
};

class PortfolioManager {
public:
    struct RebalanceTarget {
        std::string asset;
        double targetPercentage = 0.0;
    };

    explicit PortfolioManager(
        std::shared_ptr<MarketDataAdapter> marketData = {},
        std::shared_ptr<BlockchainExecutionAdapter> blockchain = {});
    ~PortfolioManager() = default;

    void setAdapters(std::shared_ptr<MarketDataAdapter> marketData,
                     std::shared_ptr<BlockchainExecutionAdapter> blockchain);
    OperationResult<PortfolioSummary> getPortfolioSummaryResult(const std::string& walletAddress);
    PortfolioSummary getPortfolioSummary(const std::string& walletAddress);
    double getTotalValue(const std::string& walletAddress);
    std::unordered_map<std::string, double> getAssetAllocation(const std::string& walletAddress);
    OperationResult<std::vector<SwapQuote>> calculateRebalanceTradesResult(
        const std::string& walletAddress, const std::vector<RebalanceTarget>& targets);
    std::vector<SwapQuote> calculateRebalanceTrades(
        const std::string& walletAddress, const std::vector<RebalanceTarget>& targets);
    OperationResult<std::vector<TransactionReceipt>> executeRebalanceResult(
        const std::string& walletAddress, const std::vector<RebalanceTarget>& targets,
        ChainId chain = ChainId::ETHEREUM_MAINNET);
    bool executeRebalance(const std::string& walletAddress,
                          const std::vector<RebalanceTarget>& targets);
    double getPnL(const std::string& walletAddress, int days = 30);
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> getValueHistory(
        const std::string& walletAddress, int days = 30);
    double calculateVolatility(const std::string& walletAddress);
    double calculateSharpeRatio(const std::string& walletAddress);
    std::vector<std::string> getHighRiskPositions(const std::string& walletAddress);

private:
    std::shared_ptr<MarketDataAdapter> marketData_;
    std::shared_ptr<BlockchainExecutionAdapter> blockchain_;
    mutable std::mutex portfolioMutex_;
};

class YieldManager {
public:
    explicit YieldManager(
        std::shared_ptr<MarketDataAdapter> marketData = {},
        std::shared_ptr<BlockchainExecutionAdapter> blockchain = {});
    ~YieldManager() = default;

    void setAdapters(std::shared_ptr<MarketDataAdapter> marketData,
                     std::shared_ptr<BlockchainExecutionAdapter> blockchain);
    void setContext(const std::string& walletAddress, ChainId chain);
    OperationResult<std::vector<YieldPosition>> findBestYieldsResult(
        const std::string& asset, YieldStrategy strategy, double minApy = 0.0);
    std::vector<YieldPosition> findBestYields(
        const std::string& asset, YieldStrategy strategy, double minApy = 0.0);
    std::vector<std::string> getSupportedProtocols(ChainId chain);
    OperationResult<TransactionReceipt> depositToYieldResult(
        const std::string& protocol, const std::string& asset, double amount);
    bool depositToYield(const std::string& protocol, const std::string& asset, double amount);
    OperationResult<TransactionReceipt> withdrawFromYieldResult(
        const std::string& positionId, double amount);
    bool withdrawFromYield(const std::string& positionId, double amount);
    OperationResult<TransactionReceipt> claimRewardsResult(const std::string& positionId);
    bool claimRewards(const std::string& positionId);
    OperationResult<TransactionReceipt> compoundRewardsResult(const std::string& positionId);
    bool compoundRewards(const std::string& positionId);
    std::vector<YieldPosition> getActivePositions(const std::string& walletAddress);
    double getTotalYieldEarned(const std::string& walletAddress);
    double getProtocolRiskScore(const std::string& protocol);
    bool isProtocolAudited(const std::string& protocol);
    double getImpermanentLossRisk(const std::string& poolAddress);

private:
    OperationResult<TransactionReceipt> executePositionOperation(
        BlockchainOperation operation, const std::string& positionId, double amount);
    std::shared_ptr<MarketDataAdapter> marketData_;
    std::shared_ptr<BlockchainExecutionAdapter> blockchain_;
    std::string walletAddress_;
    ChainId chain_ = ChainId::ETHEREUM_MAINNET;
    std::unordered_map<std::string, YieldPosition> localPositions_;
    mutable std::mutex yieldMutex_;
};

class NFTManager {
public:
    explicit NFTManager(
        std::shared_ptr<MarketDataAdapter> marketData = {},
        std::shared_ptr<BlockchainExecutionAdapter> blockchain = {});
    ~NFTManager() = default;

    void setAdapters(std::shared_ptr<MarketDataAdapter> marketData,
                     std::shared_ptr<BlockchainExecutionAdapter> blockchain);
    void setContext(const std::string& walletAddress, ChainId chain);
    std::vector<NFTInfo> getOwnedNFTs(const std::string& walletAddress, ChainId chain);
    NFTInfo getNFTDetails(const std::string& contractAddress, const std::string& tokenId, ChainId chain);
    double getNFTFloorPrice(const std::string& collectionAddress, ChainId chain);
    OperationResult<TransactionReceipt> transferNFTResult(
        const std::string& contractAddress, const std::string& tokenId,
        const std::string& to, ChainId chain);
    bool transferNFT(const std::string& contractAddress, const std::string& tokenId,
                     const std::string& to, ChainId chain);
    OperationResult<TransactionReceipt> listNFTForSaleResult(
        const std::string& contractAddress, const std::string& tokenId,
        double price, const std::string& marketplace);
    bool listNFTForSale(const std::string& contractAddress, const std::string& tokenId,
                        double price, const std::string& marketplace);
    OperationResult<TransactionReceipt> buyNFTResult(
        const std::string& contractAddress, const std::string& tokenId, double maxPrice);
    bool buyNFT(const std::string& contractAddress, const std::string& tokenId, double maxPrice);
    std::vector<NFTInfo> getTrendingCollections(ChainId chain);
    double getCollectionVolume24h(const std::string& collectionAddress);

private:
    OperationResult<TransactionReceipt> executeNftOperation(
        BlockchainOperation operation, const std::string& contractAddress,
        const std::string& tokenId, const std::string& to, double amount, ChainId chain);
    std::shared_ptr<MarketDataAdapter> marketData_;
    std::shared_ptr<BlockchainExecutionAdapter> blockchain_;
    std::string walletAddress_;
    ChainId chain_ = ChainId::ETHEREUM_MAINNET;
    std::unordered_map<std::string, NFTInfo> localNfts_;
    std::unordered_map<std::string, bool> transferredNfts_;
    mutable std::mutex nftMutex_;
};

class TransactionSimulator {
public:
    using SimulationResult = TransactionSimulation;

    explicit TransactionSimulator(std::shared_ptr<BlockchainExecutionAdapter> blockchain = {});
    ~TransactionSimulator() = default;

    void setBlockchainAdapter(std::shared_ptr<BlockchainExecutionAdapter> blockchain);
    SimulationResult simulateTransaction(const std::string& from, const std::string& to,
                                         const std::string& data, double value, ChainId chain);
    SimulationResult simulateSwap(const SwapQuote& quote);
    SimulationResult simulateBridge(const BridgeQuote& quote);
    bool detectPotentialScam(const std::string& contractAddress, ChainId chain);
    bool verifyContractSafety(const std::string& contractAddress, ChainId chain);
    std::vector<std::string> getContractWarnings(const std::string& contractAddress, ChainId chain);

private:
    std::shared_ptr<BlockchainExecutionAdapter> blockchain_;
    mutable std::mutex simMutex_;
};

class OtakuAgent {
public:
    explicit OtakuAgent(const std::string& agentId);
    OtakuAgent(const std::string& agentId,
               std::shared_ptr<MarketDataAdapter> marketData,
               std::shared_ptr<BlockchainExecutionAdapter> blockchain);
    ~OtakuAgent() = default;

    void setAdapters(std::shared_ptr<MarketDataAdapter> marketData,
                     std::shared_ptr<BlockchainExecutionAdapter> blockchain);

    bool connectWallet(const std::string& walletAddress);
    bool disconnectWallet();
    std::string getWalletAddress() const;
    bool isWalletConnected() const;
    OperationResult<std::string> getBalanceResult(const std::string& token);
    std::string getBalance(const std::string& token);
    OperationResult<std::unordered_map<std::string, double>> getAllBalancesResult();
    std::unordered_map<std::string, double> getAllBalances();

    bool switchChain(ChainId chainId);
    ChainId getCurrentChain() const;
    std::vector<ChainId> getSupportedChains() const;

    OperationResult<TransactionReceipt> executeSwapResult(
        const std::string& fromToken, const std::string& toToken, double amount);
    bool executeSwap(const std::string& fromToken, const std::string& toToken, float amount);
    OperationResult<SwapQuote> getSwapQuoteResult(
        const std::string& fromToken, const std::string& toToken, double amount,
        const MEVProtectionOptions& mevOptions = {});
    SwapQuote getSwapQuote(const std::string& fromToken, const std::string& toToken,
                           double amount, const MEVProtectionOptions& mevOptions = {});
    OperationResult<std::vector<SwapQuote>> getMultiRouteQuotesResult(
        const std::string& fromToken, const std::string& toToken, double amount);
    std::vector<SwapQuote> getMultiRouteQuotes(
        const std::string& fromToken, const std::string& toToken, double amount);
    OperationResult<TransactionReceipt> executeSwapWithQuoteResult(const SwapQuote& quote);
    TransactionReceipt executeSwapWithQuote(const SwapQuote& quote);

    OperationResult<TransactionReceipt> executeBridgeResult(
        const std::string& fromChain, const std::string& toChain, double amount);
    bool executeBridge(const std::string& fromChain, const std::string& toChain, float amount);
    OperationResult<BridgeQuote> getBridgeQuoteResult(
        ChainId sourceChain, ChainId destChain, const std::string& token, double amount);
    BridgeQuote getBridgeQuote(ChainId sourceChain, ChainId destChain,
                               const std::string& token, double amount);
    OperationResult<std::vector<BridgeQuote>> getMultiBridgeQuotesResult(
        ChainId sourceChain, ChainId destChain, const std::string& token, double amount);
    std::vector<BridgeQuote> getMultiBridgeQuotes(
        ChainId sourceChain, ChainId destChain, const std::string& token, double amount);
    OperationResult<TransactionReceipt> executeBridgeWithQuoteResult(const BridgeQuote& quote);
    TransactionReceipt executeBridgeWithQuote(const BridgeQuote& quote);
    TxStatus getBridgeStatus(const std::string& bridgeTxHash);

    OperationResult<TransactionReceipt> executeTransferResult(
        const std::string& to, const std::string& token, double amount);
    bool executeTransfer(const std::string& to, const std::string& token, float amount);
    OperationResult<TransactionReceipt> transferTokenResult(
        const std::string& to, const std::string& token, double amount);
    TransactionReceipt transferToken(const std::string& to, const std::string& token, double amount);
    OperationResult<TransactionReceipt> batchTransferResult(
        const std::vector<std::pair<std::string, double>>& recipients, const std::string& token);
    TransactionReceipt batchTransfer(
        const std::vector<std::pair<std::string, double>>& recipients, const std::string& token);

    OperationResult<LiquidityPosition> addLiquidityResult(
        const std::string& token0, const std::string& token1, double amount0,
        double amount1, DexProtocol protocol);
    LiquidityPosition addLiquidity(const std::string& token0, const std::string& token1,
                                   double amount0, double amount1, DexProtocol protocol);
    OperationResult<TransactionReceipt> removeLiquidityResult(
        const std::string& positionId, double percentage = 100.0);
    bool removeLiquidity(const std::string& positionId, double percentage = 100.0);
    std::vector<LiquidityPosition> getLiquidityPositions();
    double getPoolApr(const std::string& poolAddress, DexProtocol protocol);

    OperationResult<double> getTokenPriceResult(const std::string& token);
    float getTokenPrice(const std::string& token);
    OperationResult<double> getTokenPriceInTokenResult(
        const std::string& token, const std::string& quoteToken);
    double getTokenPriceInToken(const std::string& token, const std::string& quoteToken);
    std::vector<std::string> getTrendingTokens();
    std::vector<TokenInfo> searchTokens(const std::string& query);
    double get24hPriceChange(const std::string& token);
    double get24hVolume(const std::string& token);

    OperationResult<PortfolioSummary> getPortfolioResult();
    PortfolioSummary getPortfolio();
    OperationResult<std::vector<TransactionReceipt>> rebalancePortfolioResult(
        const std::vector<PortfolioManager::RebalanceTarget>& targets);
    bool rebalancePortfolio(const std::vector<PortfolioManager::RebalanceTarget>& targets);

    std::vector<YieldPosition> getYieldPositions();
    OperationResult<TransactionReceipt> depositToYieldResult(
        const std::string& protocol, const std::string& asset, double amount);
    bool depositToYield(const std::string& protocol, const std::string& asset, double amount);
    OperationResult<TransactionReceipt> withdrawFromYieldResult(
        const std::string& positionId, double amount);
    bool withdrawFromYield(const std::string& positionId, double amount);
    OperationResult<std::vector<TransactionReceipt>> harvestAllRewardsResult();
    bool harvestAllRewards();

    std::vector<NFTInfo> getOwnedNFTs();
    OperationResult<TransactionReceipt> transferNFTResult(
        const std::string& contract, const std::string& tokenId, const std::string& to);
    bool transferNFT(const std::string& contract, const std::string& tokenId, const std::string& to);
    OperationResult<TransactionReceipt> buyNFTResult(
        const std::string& contract, const std::string& tokenId, double maxPrice);
    bool buyNFT(const std::string& contract, const std::string& tokenId, double maxPrice);

    OperationResult<TransactionReceipt> getTransactionReceiptResult(const std::string& txHash);
    TransactionReceipt getTransactionReceipt(const std::string& txHash);
    std::vector<TransactionReceipt> getTransactionHistory(int limit = 50);
    OperationResult<TransactionReceipt> cancelTransactionResult(const std::string& txHash);
    bool cancelTransaction(const std::string& txHash);
    OperationResult<TransactionReceipt> speedUpTransactionResult(
        const std::string& txHash, double additionalGas);
    bool speedUpTransaction(const std::string& txHash, double additionalGas);

    OperationResult<GasEstimate> getGasEstimateResult(const std::string& txData);
    GasEstimate getGasEstimate(const std::string& txData);
    bool setGasStrategy(const std::string& strategy);
    double getOptimalGasPrice();

    TransactionSimulator::SimulationResult simulateTransaction(const std::string& txData);
    OperationResult<bool> verifyContractSafetyResult(const std::string& contractAddress);
    bool verifyContractSafety(const std::string& contractAddress);
    std::vector<std::string> getContractWarnings(const std::string& contractAddress);

    bool trySetSlippageTolerance(double percentage);
    void setSlippageTolerance(double percentage);
    void setMEVProtection(bool enabled);
    void setPreferredDex(DexProtocol dex);
    void setPreferredBridge(BridgeProtocol bridge);

    std::string getAgentId() const;
    std::string getStatus() const;
    void setStatusCallback(std::function<void(const std::string&)> callback);

private:
    std::string agentId_;
    std::string walletAddress_;
    ChainId currentChain_ = ChainId::ETHEREUM_MAINNET;
    std::shared_ptr<AgentLogger> logger_;
    std::shared_ptr<MarketDataAdapter> marketData_;
    std::shared_ptr<BlockchainExecutionAdapter> blockchain_;
    std::unique_ptr<GasOptimizer> gasOptimizer_;
    std::unique_ptr<PortfolioManager> portfolioManager_;
    std::unique_ptr<YieldManager> yieldManager_;
    std::unique_ptr<NFTManager> nftManager_;
    std::unique_ptr<TransactionSimulator> txSimulator_;
    double slippageTolerance_ = 0.5;
    bool mevProtectionEnabled_ = true;
    DexProtocol preferredDex_ = DexProtocol::UNISWAP_V3;
    BridgeProtocol preferredBridge_ = BridgeProtocol::ACROSS;
    std::string gasStrategy_ = "standard";
    std::function<void(const std::string&)> statusCallback_;
    std::vector<LiquidityPosition> liquidityPositions_;
    std::vector<TransactionReceipt> transactionHistory_;
    std::unordered_map<std::string, std::size_t> transactionIndex_;
    mutable std::mutex agentMutex_;

    void initializeSubManagers();
    void logStatus(const std::string& status);
    OperationResult<BlockchainExecutionResult> executeRequest(
        const BlockchainExecutionRequest& request);
    OperationResult<TransactionReceipt> recordExecutionResult(
        const OperationResult<BlockchainExecutionResult>& result,
        const BlockchainExecutionRequest& request);
    OperationResult<TransactionReceipt> updateRecordedReceipt(const TransactionReceipt& receipt);
    bool validateWalletConnection() const;
    std::string chainIdToString(ChainId chain) const;
    std::optional<ChainId> stringToChainId(const std::string& chain) const;
};

namespace otaku_utils {
bool isValidEthereumAddress(const std::string& address);
bool isValidSolanaAddress(const std::string& address);
std::string checksumAddress(const std::string& address);
double fromWei(const std::string& weiAmount, int decimals = 18);
std::string toWei(double amount, int decimals = 18);
std::string getChainName(ChainId chainId);
std::string getNativeCurrency(ChainId chainId);
std::string getBlockExplorerUrl(ChainId chainId, const std::string& txHash);
TokenInfo getTokenInfo(const std::string& address, ChainId chainId);
std::vector<TokenInfo> getCommonTokens(ChainId chainId);
}

} // namespace elizaos
