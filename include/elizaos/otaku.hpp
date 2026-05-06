#pragma once

#include "elizaos/core.hpp"
#include "elizaos/agentlogger.hpp"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <optional>
#include <chrono>
#include <functional>
#include <mutex>

namespace elizaos {

/**
 * @brief Otaku AI Agent - Advanced DeFi-focused AI agent
 *
 * Features:
 * - CDP wallet integration (Coinbase Developer Platform)
 * - Multi-chain DeFi operations (Ethereum, Base, Polygon, Arbitrum, Optimism)
 * - Token swaps, transfers, bridging, NFT operations
 * - Liquidity provision and yield farming
 * - Gas optimization and transaction simulation
 * - Real-time market data and analytics
 * - Portfolio management and rebalancing
 * - MEV protection strategies
 */

// Supported blockchain networks
enum class ChainId {
    ETHEREUM_MAINNET = 1,
    OPTIMISM = 10,
    BSC = 56,
    POLYGON = 137,
    ARBITRUM = 42161,
    BASE = 8453,
    AVALANCHE = 43114,
    SOLANA = 0  // Solana uses different addressing
};

// Token standard types
enum class TokenStandard {
    ERC20,
    ERC721,      // NFT
    ERC1155,     // Multi-token
    NATIVE       // Native chain token (ETH, MATIC, etc.)
};

// Transaction status
enum class TxStatus {
    PENDING,
    CONFIRMED,
    FAILED,
    CANCELLED,
    SIMULATED
};

// DEX/Protocol types for swap routing
enum class DexProtocol {
    UNISWAP_V2,
    UNISWAP_V3,
    SUSHISWAP,
    CURVE,
    BALANCER,
    ONEINCH,
    PARASWAP,
    COWSWAP
};

// Bridge protocols
enum class BridgeProtocol {
    ACROSS,
    STARGATE,
    HOP,
    MULTICHAIN,
    CBRIDGE,
    WORMHOLE,
    LAYERZERO
};

// Yield strategy types
enum class YieldStrategy {
    LIQUIDITY_PROVISION,
    LENDING,
    STAKING,
    YIELD_AGGREGATOR,
    LEVERAGED_FARMING
};

// Token information structure
struct TokenInfo {
    std::string symbol;
    std::string name;
    std::string contractAddress;
    ChainId chainId;
    TokenStandard standard;
    int decimals;
    std::optional<std::string> logoUrl;
};

// Transaction receipt
struct TransactionReceipt {
    std::string txHash;
    TxStatus status;
    ChainId chainId;
    std::string from;
    std::string to;
    double gasUsed;
    double effectiveGasPrice;
    uint64_t blockNumber;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> logs;
};

// Swap quote/route
struct SwapQuote {
    std::string fromToken;
    std::string toToken;
    double inputAmount;
    double expectedOutput;
    double minimumOutput;  // With slippage
    double priceImpact;    // Percentage
    double estimatedGas;
    DexProtocol protocol;
    std::vector<std::string> route;  // Intermediate tokens
    std::chrono::system_clock::time_point validUntil;
};

// Bridge quote
struct BridgeQuote {
    ChainId sourceChain;
    ChainId destChain;
    std::string token;
    double inputAmount;
    double outputAmount;
    double bridgeFee;
    double estimatedTime;  // In seconds
    BridgeProtocol protocol;
    std::chrono::system_clock::time_point validUntil;
};

// Liquidity position
struct LiquidityPosition {
    std::string positionId;
    std::string poolAddress;
    std::string token0;
    std::string token1;
    double amount0;
    double amount1;
    double liquidityTokens;
    double currentValue;
    double unrealizedPnL;
    double apr;
    ChainId chainId;
    DexProtocol protocol;
};

// Yield position
struct YieldPosition {
    std::string positionId;
    std::string protocol;
    std::string asset;
    double depositedAmount;
    double currentValue;
    double earnedRewards;
    double apy;
    YieldStrategy strategy;
    ChainId chainId;
    std::chrono::system_clock::time_point depositedAt;
};

// NFT information
struct NFTInfo {
    std::string contractAddress;
    std::string tokenId;
    std::string name;
    std::string description;
    std::optional<std::string> imageUrl;
    std::optional<std::string> animationUrl;
    std::unordered_map<std::string, std::string> attributes;
    ChainId chainId;
    TokenStandard standard;
};

// Portfolio summary
struct PortfolioSummary {
    double totalValueUsd;
    double totalPnL;
    double pnlPercentage24h;
    std::unordered_map<std::string, double> tokenBalances;
    std::unordered_map<ChainId, double> chainDistribution;
    std::vector<LiquidityPosition> liquidityPositions;
    std::vector<YieldPosition> yieldPositions;
    std::vector<NFTInfo> nfts;
    std::chrono::system_clock::time_point lastUpdated;
};

// Gas estimate
struct GasEstimate {
    double estimatedGas;
    double baseFee;
    double priorityFee;
    double maxFee;
    double totalCostWei;
    double totalCostUsd;
    ChainId chainId;
};

// MEV protection options
struct MEVProtectionOptions {
    bool useFlashbots = false;
    bool useCowSwap = false;
    double maxSlippage = 0.5;  // Percentage
    std::optional<double> deadline;  // Unix timestamp
    bool partialFill = false;
};

/**
 * @brief Gas Optimizer - Optimizes transaction gas costs
 */
class GasOptimizer {
public:
    GasOptimizer();
    ~GasOptimizer() = default;

    // Gas estimation
    GasEstimate estimateGas(ChainId chain, const std::string& txData);
    double getOptimalGasPrice(ChainId chain);
    double getBaseFee(ChainId chain);
    double getPriorityFee(ChainId chain);

    // Gas price tracking
    void updateGasPrices(ChainId chain);
    double getHistoricalGasPrice(ChainId chain, int blocksAgo);
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> getGasHistory(ChainId chain);

    // Gas saving strategies
    bool shouldWaitForLowerGas(ChainId chain, double threshold);
    std::chrono::system_clock::time_point predictLowGasTime(ChainId chain);

private:
    std::unordered_map<ChainId, std::vector<double>> gasPriceHistory_;
    std::unordered_map<ChainId, double> currentBaseFees_;
    std::mutex gasMutex_;
};

/**
 * @brief Multi-chain Portfolio Manager
 */
class PortfolioManager {
public:
    PortfolioManager();
    ~PortfolioManager() = default;

    // Portfolio tracking
    PortfolioSummary getPortfolioSummary(const std::string& walletAddress);
    double getTotalValue(const std::string& walletAddress);
    std::unordered_map<std::string, double> getAssetAllocation(const std::string& walletAddress);

    // Rebalancing
    struct RebalanceTarget {
        std::string asset;
        double targetPercentage;
    };
    std::vector<SwapQuote> calculateRebalanceTrades(
        const std::string& walletAddress,
        const std::vector<RebalanceTarget>& targets);
    bool executeRebalance(
        const std::string& walletAddress,
        const std::vector<RebalanceTarget>& targets);

    // Performance tracking
    double getPnL(const std::string& walletAddress, int days = 30);
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> getValueHistory(
        const std::string& walletAddress, int days = 30);

    // Risk analysis
    double calculateVolatility(const std::string& walletAddress);
    double calculateSharpeRatio(const std::string& walletAddress);
    std::vector<std::string> getHighRiskPositions(const std::string& walletAddress);

private:
    std::unordered_map<std::string, PortfolioSummary> portfolioCache_;
    std::mutex portfolioMutex_;
};

/**
 * @brief Yield Farming Manager
 */
class YieldManager {
public:
    YieldManager();
    ~YieldManager() = default;

    // Yield discovery
    std::vector<YieldPosition> findBestYields(
        const std::string& asset,
        YieldStrategy strategy,
        double minApy = 0.0);
    std::vector<std::string> getSupportedProtocols(ChainId chain);

    // Position management
    bool depositToYield(
        const std::string& protocol,
        const std::string& asset,
        double amount);
    bool withdrawFromYield(
        const std::string& positionId,
        double amount);
    bool claimRewards(const std::string& positionId);
    bool compoundRewards(const std::string& positionId);

    // Yield tracking
    std::vector<YieldPosition> getActivePositions(const std::string& walletAddress);
    double getTotalYieldEarned(const std::string& walletAddress);

    // Risk assessment
    double getProtocolRiskScore(const std::string& protocol);
    bool isProtocolAudited(const std::string& protocol);
    double getImpermanentLossRisk(const std::string& poolAddress);

private:
    std::unordered_map<std::string, double> protocolRiskScores_;
    std::mutex yieldMutex_;
};

/**
 * @brief NFT Manager
 */
class NFTManager {
public:
    NFTManager();
    ~NFTManager() = default;

    // NFT queries
    std::vector<NFTInfo> getOwnedNFTs(const std::string& walletAddress, ChainId chain);
    NFTInfo getNFTDetails(const std::string& contractAddress, const std::string& tokenId, ChainId chain);
    double getNFTFloorPrice(const std::string& collectionAddress, ChainId chain);

    // NFT operations
    bool transferNFT(
        const std::string& contractAddress,
        const std::string& tokenId,
        const std::string& to,
        ChainId chain);
    bool listNFTForSale(
        const std::string& contractAddress,
        const std::string& tokenId,
        double price,
        const std::string& marketplace);
    bool buyNFT(
        const std::string& contractAddress,
        const std::string& tokenId,
        double maxPrice);

    // Collection analysis
    std::vector<NFTInfo> getTrendingCollections(ChainId chain);
    double getCollectionVolume24h(const std::string& collectionAddress);

private:
    std::mutex nftMutex_;
};

/**
 * @brief Transaction Simulator
 */
class TransactionSimulator {
public:
    TransactionSimulator();
    ~TransactionSimulator() = default;

    // Simulation
    struct SimulationResult {
        bool success;
        std::string errorMessage;
        double gasUsed;
        std::vector<std::string> stateChanges;
        double estimatedPriceImpact;
        std::vector<std::string> warnings;
    };

    SimulationResult simulateTransaction(
        const std::string& from,
        const std::string& to,
        const std::string& data,
        double value,
        ChainId chain);

    SimulationResult simulateSwap(const SwapQuote& quote);
    SimulationResult simulateBridge(const BridgeQuote& quote);

    // Risk analysis
    bool detectPotentialScam(const std::string& contractAddress, ChainId chain);
    bool verifyContractSafety(const std::string& contractAddress, ChainId chain);
    std::vector<std::string> getContractWarnings(const std::string& contractAddress, ChainId chain);

private:
    std::mutex simMutex_;
};

/**
 * @brief Main Otaku AI Agent - Advanced DeFi-focused AI agent
 */
class OtakuAgent {
public:
    OtakuAgent(const std::string& agentId);
    ~OtakuAgent() = default;

    // Wallet operations
    bool connectWallet(const std::string& walletAddress);
    bool disconnectWallet();
    std::string getWalletAddress() const { return walletAddress_; }
    bool isWalletConnected() const { return !walletAddress_.empty(); }
    std::string getBalance(const std::string& token);
    std::unordered_map<std::string, double> getAllBalances();

    // Multi-chain support
    bool switchChain(ChainId chainId);
    ChainId getCurrentChain() const { return currentChain_; }
    std::vector<ChainId> getSupportedChains() const;

    // DeFi operations
    bool executeSwap(const std::string& fromToken, const std::string& toToken, float amount);
    SwapQuote getSwapQuote(
        const std::string& fromToken,
        const std::string& toToken,
        double amount,
        const MEVProtectionOptions& mevOptions = {});
    std::vector<SwapQuote> getMultiRouteQuotes(
        const std::string& fromToken,
        const std::string& toToken,
        double amount);
    TransactionReceipt executeSwapWithQuote(const SwapQuote& quote);

    // Bridging
    bool executeBridge(const std::string& fromChain, const std::string& toChain, float amount);
    BridgeQuote getBridgeQuote(
        ChainId sourceChain,
        ChainId destChain,
        const std::string& token,
        double amount);
    std::vector<BridgeQuote> getMultiBridgeQuotes(
        ChainId sourceChain,
        ChainId destChain,
        const std::string& token,
        double amount);
    TransactionReceipt executeBridgeWithQuote(const BridgeQuote& quote);
    TxStatus getBridgeStatus(const std::string& bridgeTxHash);

    // Transfers
    bool executeTransfer(const std::string& to, const std::string& token, float amount);
    TransactionReceipt transferToken(
        const std::string& to,
        const std::string& token,
        double amount);
    TransactionReceipt batchTransfer(
        const std::vector<std::pair<std::string, double>>& recipients,
        const std::string& token);

    // Liquidity provision
    LiquidityPosition addLiquidity(
        const std::string& token0,
        const std::string& token1,
        double amount0,
        double amount1,
        DexProtocol protocol);
    bool removeLiquidity(
        const std::string& positionId,
        double percentage = 100.0);
    std::vector<LiquidityPosition> getLiquidityPositions();
    double getPoolApr(const std::string& poolAddress, DexProtocol protocol);

    // Market data
    float getTokenPrice(const std::string& token);
    double getTokenPriceInToken(const std::string& token, const std::string& quoteToken);
    std::vector<std::string> getTrendingTokens();
    std::vector<TokenInfo> searchTokens(const std::string& query);
    double get24hPriceChange(const std::string& token);
    double get24hVolume(const std::string& token);

    // Portfolio management
    PortfolioSummary getPortfolio();
    bool rebalancePortfolio(const std::vector<PortfolioManager::RebalanceTarget>& targets);

    // Yield farming
    std::vector<YieldPosition> getYieldPositions();
    bool depositToYield(const std::string& protocol, const std::string& asset, double amount);
    bool withdrawFromYield(const std::string& positionId, double amount);
    bool harvestAllRewards();

    // NFT operations
    std::vector<NFTInfo> getOwnedNFTs();
    bool transferNFT(const std::string& contract, const std::string& tokenId, const std::string& to);
    bool buyNFT(const std::string& contract, const std::string& tokenId, double maxPrice);

    // Transaction management
    TransactionReceipt getTransactionReceipt(const std::string& txHash);
    std::vector<TransactionReceipt> getTransactionHistory(int limit = 50);
    bool cancelTransaction(const std::string& txHash);
    bool speedUpTransaction(const std::string& txHash, double additionalGas);

    // Gas optimization
    GasEstimate getGasEstimate(const std::string& txData);
    bool setGasStrategy(const std::string& strategy); // "fast", "standard", "slow"
    double getOptimalGasPrice();

    // Simulation and safety
    TransactionSimulator::SimulationResult simulateTransaction(const std::string& txData);
    bool verifyContractSafety(const std::string& contractAddress);
    std::vector<std::string> getContractWarnings(const std::string& contractAddress);

    // Agent configuration
    void setSlippageTolerance(double percentage);
    void setMEVProtection(bool enabled);
    void setPreferredDex(DexProtocol dex);
    void setPreferredBridge(BridgeProtocol bridge);

    // Status and monitoring
    std::string getAgentId() const { return agentId_; }
    std::string getStatus() const;
    void setStatusCallback(std::function<void(const std::string&)> callback);

private:
    std::string agentId_;
    std::string walletAddress_;
    ChainId currentChain_;
    std::shared_ptr<AgentLogger> logger_;

    // Sub-managers
    std::unique_ptr<GasOptimizer> gasOptimizer_;
    std::unique_ptr<PortfolioManager> portfolioManager_;
    std::unique_ptr<YieldManager> yieldManager_;
    std::unique_ptr<NFTManager> nftManager_;
    std::unique_ptr<TransactionSimulator> txSimulator_;

    // Configuration
    double slippageTolerance_;
    bool mevProtectionEnabled_;
    DexProtocol preferredDex_;
    BridgeProtocol preferredBridge_;
    std::string gasStrategy_;

    // Callbacks
    std::function<void(const std::string&)> statusCallback_;

    // Thread safety
    mutable std::mutex agentMutex_;

    // Internal helpers
    void initializeSubManagers();
    void logStatus(const std::string& status);
    bool validateWalletConnection();
    std::string chainIdToString(ChainId chain) const;
    ChainId stringToChainId(const std::string& chain) const;
};

// Utility functions
namespace otaku_utils {
    // Address utilities
    bool isValidEthereumAddress(const std::string& address);
    bool isValidSolanaAddress(const std::string& address);
    std::string checksumAddress(const std::string& address);

    // Amount conversions
    double fromWei(const std::string& weiAmount, int decimals = 18);
    std::string toWei(double amount, int decimals = 18);

    // Chain utilities
    std::string getChainName(ChainId chainId);
    std::string getNativeCurrency(ChainId chainId);
    std::string getBlockExplorerUrl(ChainId chainId, const std::string& txHash);

    // Token utilities
    TokenInfo getTokenInfo(const std::string& address, ChainId chainId);
    std::vector<TokenInfo> getCommonTokens(ChainId chainId);
}

} // namespace elizaos
