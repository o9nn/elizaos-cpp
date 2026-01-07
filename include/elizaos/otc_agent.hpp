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
 * @brief OTC Agent - Advanced Multi-chain OTC Trading Desk
 *
 * Features:
 * - Multi-chain support (Base, BSC, Solana, Ethereum, Arbitrum)
 * - AI-powered negotiation with Eliza integration
 * - Smart contract escrow system
 * - Orderbook management and matching
 * - Atomic swaps and settlement
 * - Counterparty risk management
 * - Trade analytics and reporting
 * - Market making capabilities
 * - Liquidity aggregation
 */

// Supported chains for OTC trading
enum class OTCChain {
    ETHEREUM,
    BASE,
    BSC,
    SOLANA,
    ARBITRUM,
    POLYGON
};

// Offer side
enum class OfferSide {
    BUY,   // Looking to buy tokens
    SELL   // Looking to sell tokens
};

// Offer status
enum class OfferStatus {
    PENDING,      // Awaiting counterparty
    ACTIVE,       // Available for matching
    NEGOTIATING,  // In negotiation
    MATCHED,      // Matched with counterparty
    SETTLING,     // Settlement in progress
    COMPLETED,    // Trade completed
    CANCELLED,    // Cancelled by maker
    EXPIRED,      // Time expired
    DISPUTED      // Under dispute resolution
};

// Settlement type
enum class SettlementType {
    INSTANT,       // Immediate atomic swap
    ESCROW,        // Escrow-based settlement
    SCHEDULED,     // Scheduled settlement
    CROSS_CHAIN    // Cross-chain settlement
};

// Token information
struct OTCToken {
    std::string symbol;
    std::string contractAddress;
    OTCChain chain;
    int decimals;
    std::string name;
};

// OTC Offer structure
struct OTCOffer {
    std::string offerId;
    std::string makerId;        // Offer creator
    std::string takerId;        // Counterparty (if matched)
    OTCToken baseToken;         // Token being traded
    OTCToken quoteToken;        // Token used for payment
    double baseAmount;          // Amount of base token
    double quoteAmount;         // Total quote amount (baseAmount * price)
    double pricePerUnit;        // Price per unit
    double minAmount;           // Minimum trade size
    double maxAmount;           // Maximum trade size
    OfferSide side;
    OfferStatus status;
    SettlementType settlementType;
    OTCChain chain;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point expiresAt;
    std::optional<std::chrono::system_clock::time_point> matchedAt;
    std::optional<std::chrono::system_clock::time_point> settledAt;
    std::string escrowAddress;
    std::string notes;
    bool partialFillAllowed;
    double filledAmount;
};

// Trade execution record
struct TradeExecution {
    std::string executionId;
    std::string offerId;
    std::string makerId;
    std::string takerId;
    OTCToken baseToken;
    OTCToken quoteToken;
    double baseAmount;
    double quoteAmount;
    double pricePerUnit;
    std::string makerTxHash;
    std::string takerTxHash;
    std::string escrowTxHash;
    std::string settlementTxHash;
    OfferStatus status;
    std::chrono::system_clock::time_point executedAt;
    double feeAmount;
    std::string feeToken;
};

// Negotiation message
struct NegotiationMessage {
    std::string messageId;
    std::string offerId;
    std::string senderId;
    std::string recipientId;
    std::string message;
    std::optional<double> proposedPrice;
    std::optional<double> proposedAmount;
    std::chrono::system_clock::time_point timestamp;
    bool isCounterOffer;
};

// Counterparty profile
struct CounterpartyProfile {
    std::string counterpartyId;
    int totalTrades;
    int successfulTrades;
    double totalVolume;
    double averageTradeSize;
    double reputationScore;        // 0.0 to 5.0
    std::vector<std::string> verifications;
    std::chrono::system_clock::time_point memberSince;
    bool isVerified;
    bool isWhitelisted;
    bool isBlacklisted;
};

// Market stats
struct OTCMarketStats {
    std::string tokenPair;
    double bestBid;
    double bestAsk;
    double spread;
    double volume24h;
    int trades24h;
    double avgTradeSize;
    double priceChange24h;
};

// Risk assessment
struct RiskAssessment {
    std::string offerId;
    double counterpartyRisk;       // 0.0 to 1.0
    double priceRisk;              // Deviation from market
    double liquidityRisk;          // Settlement risk
    double volatilityRisk;         // Token volatility
    double overallRisk;            // Combined risk score
    std::vector<std::string> warnings;
    bool recommended;
};

/**
 * @brief Escrow Manager for secure settlements
 */
class EscrowManager {
public:
    EscrowManager();
    ~EscrowManager() = default;

    // Escrow operations
    std::string createEscrow(
        const std::string& makerId,
        const std::string& takerId,
        const OTCToken& token,
        double amount,
        OTCChain chain);
    bool depositToEscrow(const std::string& escrowId, const std::string& txHash);
    bool releaseEscrow(const std::string& escrowId, const std::string& recipient);
    bool refundEscrow(const std::string& escrowId);
    bool initiateDispute(const std::string& escrowId, const std::string& reason);
    bool resolveDispute(const std::string& escrowId, const std::string& winner);

    // Escrow queries
    struct EscrowDetails {
        std::string escrowId;
        std::string makerId;
        std::string takerId;
        OTCToken token;
        double amount;
        OTCChain chain;
        std::string contractAddress;
        std::string status;
        std::chrono::system_clock::time_point createdAt;
        std::optional<std::chrono::system_clock::time_point> releasedAt;
    };

    EscrowDetails getEscrowDetails(const std::string& escrowId);
    std::vector<EscrowDetails> getActiveEscrows(const std::string& userId);
    double getTotalEscrowedValue(const std::string& userId);

private:
    std::unordered_map<std::string, EscrowDetails> escrows_;
    std::mutex escrowMutex_;
};

/**
 * @brief Orderbook for OTC offers
 */
class OTCOrderbook {
public:
    OTCOrderbook();
    ~OTCOrderbook() = default;

    // Orderbook operations
    void addOffer(const OTCOffer& offer);
    void removeOffer(const std::string& offerId);
    void updateOffer(const OTCOffer& offer);

    // Matching
    std::vector<OTCOffer> findMatchingOffers(
        const std::string& baseToken,
        const std::string& quoteToken,
        OfferSide side,
        double amount,
        double maxPrice = 0.0);

    std::optional<OTCOffer> getBestOffer(
        const std::string& baseToken,
        const std::string& quoteToken,
        OfferSide side);

    // Queries
    std::vector<OTCOffer> getOffersByMaker(const std::string& makerId);
    std::vector<OTCOffer> getOffersByToken(const std::string& tokenSymbol);
    std::vector<OTCOffer> getActiveOffers();
    OTCMarketStats getMarketStats(const std::string& baseToken, const std::string& quoteToken);
    double getSpread(const std::string& baseToken, const std::string& quoteToken);

private:
    std::unordered_map<std::string, OTCOffer> offers_;
    std::unordered_map<std::string, std::vector<std::string>> tokenOffers_; // token -> offer IDs
    mutable std::mutex orderbookMutex_;
};

/**
 * @brief Risk Manager for counterparty and trade risk
 */
class OTCRiskManager {
public:
    OTCRiskManager();
    ~OTCRiskManager() = default;

    // Risk assessment
    RiskAssessment assessOffer(const OTCOffer& offer, const std::string& userId);
    double getCounterpartyRisk(const std::string& counterpartyId);
    double getPriceRisk(const OTCOffer& offer);

    // Counterparty management
    void updateCounterpartyProfile(const std::string& counterpartyId, const TradeExecution& trade);
    CounterpartyProfile getCounterpartyProfile(const std::string& counterpartyId);
    bool isCounterpartyTrusted(const std::string& counterpartyId);
    void blacklistCounterparty(const std::string& counterpartyId, const std::string& reason);
    void whitelistCounterparty(const std::string& counterpartyId);

    // Limits
    double getDailyLimit(const std::string& userId);
    double getRemainingLimit(const std::string& userId);
    bool checkLimit(const std::string& userId, double amount);

private:
    std::unordered_map<std::string, CounterpartyProfile> profiles_;
    std::unordered_map<std::string, double> dailyLimits_;
    std::unordered_map<std::string, double> dailyUsed_;
    std::mutex riskMutex_;
};

/**
 * @brief Negotiation Engine for AI-powered price negotiation
 */
class NegotiationEngine {
public:
    NegotiationEngine();
    ~NegotiationEngine() = default;

    // Negotiation
    std::string startNegotiation(const std::string& offerId, const std::string& initiatorId);
    bool sendMessage(
        const std::string& negotiationId,
        const std::string& senderId,
        const std::string& message,
        std::optional<double> proposedPrice = std::nullopt,
        std::optional<double> proposedAmount = std::nullopt);
    bool acceptCounterOffer(const std::string& negotiationId, const std::string& userId);
    bool rejectCounterOffer(const std::string& negotiationId, const std::string& userId);
    bool endNegotiation(const std::string& negotiationId, bool accepted);

    // AI assistance
    struct NegotiationAdvice {
        bool shouldAccept;
        double suggestedPrice;
        std::string reasoning;
        double confidence;
    };

    NegotiationAdvice getAIAdvice(
        const OTCOffer& offer,
        double marketPrice,
        const CounterpartyProfile& counterparty);

    // Query
    std::vector<NegotiationMessage> getNegotiationHistory(const std::string& negotiationId);
    std::string getNegotiationStatus(const std::string& negotiationId);

private:
    std::unordered_map<std::string, std::vector<NegotiationMessage>> negotiations_;
    std::mutex negotiationMutex_;
};

/**
 * @brief Analytics for trade performance
 */
class OTCAnalytics {
public:
    OTCAnalytics();
    ~OTCAnalytics() = default;

    // Recording
    void recordTrade(const TradeExecution& trade);

    // Analytics
    struct TradingStats {
        int totalTrades;
        double totalVolume;
        double avgTradeSize;
        double successRate;
        double avgExecutionTime;
        std::unordered_map<std::string, double> volumeByToken;
        std::unordered_map<OTCChain, double> volumeByChain;
    };

    TradingStats getUserStats(const std::string& userId, int days = 30);
    TradingStats getPlatformStats(int days = 30);
    std::vector<TradeExecution> getTradeHistory(const std::string& userId, int limit = 50);
    double getPnL(const std::string& userId, int days = 30);

    // Market data
    double getVWAP(const std::string& baseToken, const std::string& quoteToken, int hours = 24);
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> getPriceHistory(
        const std::string& baseToken,
        const std::string& quoteToken,
        int days = 7);

private:
    std::vector<TradeExecution> tradeHistory_;
    std::mutex analyticsMutex_;
};

/**
 * @brief Main OTC Agent - Advanced Multi-chain OTC Trading Desk
 */
class OTCAgent {
public:
    OTCAgent(const std::string& agentId);
    ~OTCAgent() = default;

    // Offer management
    bool createOffer(const std::string& token, float amount, float price);
    std::string createAdvancedOffer(
        const OTCToken& baseToken,
        const OTCToken& quoteToken,
        double amount,
        double pricePerUnit,
        OfferSide side,
        SettlementType settlement = SettlementType::ESCROW,
        int expiryHours = 24,
        bool partialFill = false);
    bool acceptOffer(const std::string& offerId);
    bool acceptPartialOffer(const std::string& offerId, double amount);
    bool cancelOffer(const std::string& offerId);
    bool updateOfferPrice(const std::string& offerId, double newPrice);

    // Orderbook
    std::vector<OTCOffer> getActiveOffers(const std::string& tokenSymbol = "");
    std::vector<OTCOffer> getMyOffers();
    OTCOffer getOffer(const std::string& offerId);
    OTCMarketStats getMarketStats(const std::string& baseToken, const std::string& quoteToken);

    // Matching
    std::optional<OTCOffer> findBestOffer(
        const std::string& baseToken,
        const std::string& quoteToken,
        OfferSide side,
        double amount);
    std::vector<OTCOffer> findMatchingOffers(
        const std::string& baseToken,
        OfferSide side,
        double minAmount = 0.0,
        double maxPrice = 0.0);

    // Negotiation
    std::string startNegotiation(const std::string& offerId);
    bool sendNegotiationMessage(
        const std::string& negotiationId,
        const std::string& message,
        std::optional<double> counterPrice = std::nullopt);
    bool acceptNegotiation(const std::string& negotiationId);
    bool rejectNegotiation(const std::string& negotiationId);
    NegotiationEngine::NegotiationAdvice getAIAdvice(const std::string& offerId);

    // Settlement
    bool initiateSettlement(const std::string& offerId);
    bool confirmSettlement(const std::string& offerId, const std::string& txHash);
    std::string getSettlementStatus(const std::string& offerId);

    // Risk management
    RiskAssessment assessOffer(const std::string& offerId);
    CounterpartyProfile getCounterpartyProfile(const std::string& counterpartyId);
    bool isCounterpartyTrusted(const std::string& counterpartyId);

    // Multi-chain support
    bool deployContract(const std::string& chain);
    std::string getContractAddress(const std::string& chain);
    std::vector<OTCChain> getSupportedChains() const;
    bool switchChain(OTCChain chain);

    // Analytics
    OTCAnalytics::TradingStats getMyStats(int days = 30);
    std::vector<TradeExecution> getTradeHistory(int limit = 50);
    double getPnL(int days = 30);

    // Configuration
    void setDefaultSettlement(SettlementType type);
    void setDefaultExpiry(int hours);
    void setMaxSlippage(double percentage);
    void setMinTradeSize(double amount);

    // Status
    std::string getAgentId() const { return agentId_; }
    std::string getStatus() const;
    void setStatusCallback(std::function<void(const std::string&)> callback);

private:
    std::string agentId_;
    OTCChain currentChain_;
    std::shared_ptr<AgentLogger> logger_;

    // Sub-managers
    std::unique_ptr<EscrowManager> escrowManager_;
    std::unique_ptr<OTCOrderbook> orderbook_;
    std::unique_ptr<OTCRiskManager> riskManager_;
    std::unique_ptr<NegotiationEngine> negotiationEngine_;
    std::unique_ptr<OTCAnalytics> analytics_;

    // Configuration
    SettlementType defaultSettlement_;
    int defaultExpiryHours_;
    double maxSlippage_;
    double minTradeSize_;

    // Callbacks
    std::function<void(const std::string&)> statusCallback_;

    // Thread safety
    mutable std::mutex agentMutex_;

    // Internal helpers
    void initializeSubManagers();
    void logStatus(const std::string& status);
    OTCChain stringToChain(const std::string& chain) const;
    std::string chainToString(OTCChain chain) const;
    std::string generateId(const std::string& prefix);
};

// Utility functions
namespace otc_utils {
    // Price utilities
    double calculateSpread(double bid, double ask);
    double calculatePriceImpact(double amount, double liquidity, double currentPrice);
    double calculateFee(double amount, double feeRate);

    // Chain utilities
    std::string getChainName(OTCChain chain);
    std::string getChainCurrency(OTCChain chain);
    std::string getExplorerUrl(OTCChain chain, const std::string& txHash);

    // Validation
    bool isValidAmount(double amount, const OTCToken& token);
    bool isValidPrice(double price);
    bool isValidAddress(const std::string& address, OTCChain chain);
}

} // namespace elizaos
