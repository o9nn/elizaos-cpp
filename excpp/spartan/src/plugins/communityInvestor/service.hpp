#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "clients.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "elizaos/core.hpp"
#include "reports.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



 // Import the seed

// Event types
/**
 * Represents different types of trading events that can occur.
 * @typedef {Object} TradingEvent
 * @property {string} type - The type of trading event.
 * @property {Position} [position] - The position associated with the event. (if type is 'position_opened' or 'position_closed')
 * @property {Transaction} [transaction] - The transaction associated with the event. (if type is 'transaction_added')
 * @property {TokenRecommendation} [recommendation] - The token recommendation associated with the event. (if type is 'recommendation_added')
 * @property {TokenPerformance} [performance] - The token performance associated with the event. (if type is 'token_performance_updated')
 */
using TradingEvent = std::variant<, { type: 'position_opened'>; position: Position }

/**
 * Unified Trading Service that centralizes all trading operations
 */
/**
 * CommunityInvestorService class representing a service for trading on the Solana blockchain.
 * @extends Service
 * @property {string} serviceType - The type of service, set to ServiceType.COMMUNITY_INVESTOR.
 * @property {string} capabilityDescription - Description of the agent's ability to trade on the Solana blockchain.
 * @method storeRecommenderMetrics - Store entity metrics and cache for 5 minutes.
 * @method storeRecommenderMetricsHistory - Store entity metrics history.
 */
class CommunityInvestorService {
public:
    CommunityInvestorService();
    static std::future<CommunityInvestorService> start(IAgentRuntime runtime);
    static std::future<void> stop(IAgentRuntime runtime);
    std::variant<Promise<Position, null>> processBuySignal(BuySignalMessage buySignal, Entity entity);
    std::future<bool> processSellSignal(UUID positionId, UUID _sellRecommenderId);
    std::variant<Promise<Position, null>> handleRecommendation(Entity entity, std::optional<std::any> recommendation);
    bool hasWallet(const std::string& chain);
    Promise<TokenMetadata getTokenOverview(const std::string& chain, const std::string& tokenAddress, auto forceRefresh);
    Promise< resolveTicker(const std::string& ticker, SupportedChain chain = SupportedChain.SOLANA, std::optional<Memory[] // Context might be used to disambiguate if multiple matches> contextMessages);
    ticker::toUpperCase();

    // Check context messages first if they contain a contract address for this ticker recently
    if (contextMessages) toUpperCase();
    std::future<double> getCurrentPrice(const std::string& chain, const std::string& tokenAddress);
    std::future<bool> shouldTradeToken(const std::string& chain, const std::string& tokenAddress);
    std::variant<Promise<ProcessedTokenData, null>> getProcessedTokenData(const std::string& chain, const std::string& tokenAddress);
    std::future<std::string> analyzeHolderDistribution(TokenTradeData tradeData);
    std::future<TokenPerformance> updateTokenPerformance(const std::string& chain, const std::string& tokenAddress);
    double calculateRiskScore(TokenPerformance token);
    std::future<void> updateRecommenderMetrics(UUID entityId, auto performance);
    double calculateTrustScore(RecommenderMetrics metrics, double newPerformance);
    std::variant<Promise<TokenPerformance, null>> getOrFetchTokenPerformance(const std::string& tokenAddress, const std::string& chain);
    bool validateToken(TokenPerformance token);
    std::variant<Promise<TokenRecommendation, null>> createTokenRecommendation(UUID entityId, TokenPerformance token, Conviction conviction = Conviction.MEDIUM, RecommendationType type = RecommendationType.BUY);
    bigint calculateBuyAmount(Entity entity, Conviction conviction, TokenPerformance token);
    std::variant<Promise<Position, null>> createPosition(UUID recommendationId, UUID entityId, const std::string& tokenAddress, const std::string& walletAddress, bigint amount, const std::string& price, bool isSimulation);
    std::future<bool> recordTransaction(UUID positionId, const std::string& tokenAddress, TransactionType type, bigint amount, double price, bool isSimulation);
    std::future<std::vector<Position>> getPositionsByRecommender(UUID entityId);
    std::future<std::vector<Position>> getPositionsByToken(const std::string& tokenAddress);
    std::future<std::vector<Transaction>> getTransactionsByPosition(UUID positionId);
    std::future<std::vector<Transaction>> getTransactionsByToken(const std::string& tokenAddress);
    std::variant<Promise<Position, null>> getPosition(UUID positionId);
    std::future<std::vector<TokenRecommendation>> getRecommendationsByRecommender(UUID entityId);
    std::future<bool> closePosition(UUID positionId);
    std::future<double> calculatePositionPerformance(Position position, const std::vector<Transaction>& transactions);
    std::future<void> storeTokenPerformance(TokenPerformance token);
    std::future<void> storePosition(Position position);
    std::future<void> storeTransaction(Transaction transaction);
    std::future<void> storeTokenRecommendation(TokenRecommendation recommendation);
    std::future<void> storeRecommenderMetrics(RecommenderMetrics metrics);
    std::future<void> storeRecommenderMetricsHistory(RecommenderMetricsHistory history);
    std::variant<Promise<RecommenderMetrics, null>> getRecommenderMetrics(UUID entityId);
    std::future<std::vector<RecommenderMetricsHistory>> getRecommenderMetricsHistory(UUID entityId);
    std::future<void> initializeRecommenderMetrics(UUID entityId, const std::string& platform);
    std::variant<Promise<TokenPerformance, null>> getTokenPerformance(const std::string& tokenAddress, const std::string& chain);
    std::future<std::vector<PositionWithBalance>> getOpenPositionsWithBalance();
    std::future<std::vector<Transaction>> getPositionsTransactions(const std::vector<UUID>& positionIds);
    std::future<std::string> getFormattedPortfolioReport(std::optional<UUID> entityId);
    std::future<void> initialize(IAgentRuntime runtime);
    std::variant<Promise<TokenAPIData, null>> getTokenAPIData(const std::string& address, SupportedChain chain);
    std::future<bool> isLikelyScamOrRug(TokenAPIData tokenData, double recommendationTimestamp);
    std::future<RecommendationMetric> evaluateRecommendationPerformance(Recommendation recommendation, TokenAPIData tokenData);
    double getRecencyWeight(double recommendationTimestamp);
    double getConvictionWeight();
    std::future<void> calculateUserTrustScore(UUID userId, IAgentRuntime runtime, std::optional<UUID // _worldId from task is no longer needed here for component lookup> _worldId);
    std::future<void> executeProcessTradeDecision(const std::any& options, Task task);
    void registerTaskWorkers(IAgentRuntime runtime);
    std::future<std::vector<LeaderboardEntry>> getLeaderboardData(IAgentRuntime runtime);
    void registerUser(UUID userId);
    std::future<void> loadUserRegistry();
    std::future<void> ensurePluginComponentContext();

private:
    BirdeyeClient birdeyeClient_;
    DexscreenerClient dexscreenerClient_;
    TradingConfig tradingConfig_;
    UUID componentWorldId_;
    UUID componentRoomId_;
};


} // namespace elizaos
