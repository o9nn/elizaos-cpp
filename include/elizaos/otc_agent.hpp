#pragma once

#include "elizaos/agentlogger.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace elizaos {

enum class OTCChain { ETHEREUM, BASE, BSC, SOLANA, ARBITRUM, POLYGON };
enum class OfferSide { BUY, SELL };
enum class OfferStatus {
    PENDING, ACTIVE, NEGOTIATING, MATCHED, SETTLING, COMPLETED,
    CANCELLED, EXPIRED, DISPUTED
};
enum class SettlementType { INSTANT, ESCROW, SCHEDULED, CROSS_CHAIN };

enum class OTCOperationStatus {
    UNAVAILABLE, REJECTED, PROPOSED, ACCEPTED, SUBMITTED, VERIFIED,
    FAILED, CANCELLED
};

const char* otcOperationStatusName(OTCOperationStatus status) noexcept;

struct OTCAdapterEvidence {
    std::string provider;
    std::string reference;
    std::chrono::system_clock::time_point observedAt{};
    bool verified = false;
};

template <typename T>
struct OTCResult {
    OTCOperationStatus status = OTCOperationStatus::UNAVAILABLE;
    T value{};
    std::string message;
    OTCAdapterEvidence evidence;

    bool verified() const noexcept { return status == OTCOperationStatus::VERIFIED; }
    bool accepted() const noexcept {
        return status == OTCOperationStatus::PROPOSED ||
               status == OTCOperationStatus::ACCEPTED ||
               status == OTCOperationStatus::SUBMITTED ||
               status == OTCOperationStatus::VERIFIED ||
               status == OTCOperationStatus::CANCELLED;
    }
    explicit operator bool() const noexcept { return accepted(); }
};

struct OTCToken {
    std::string symbol;
    std::string contractAddress;
    OTCChain chain = OTCChain::ETHEREUM;
    int decimals = 0;
    std::string name;
};

struct OTCOffer {
    std::string offerId;
    std::string makerId;
    std::string takerId;
    OTCToken baseToken;
    OTCToken quoteToken;
    double baseAmount = 0.0;
    double quoteAmount = 0.0;
    double pricePerUnit = 0.0;
    double minAmount = 0.0;
    double maxAmount = 0.0;
    OfferSide side = OfferSide::BUY;
    OfferStatus status = OfferStatus::PENDING;
    SettlementType settlementType = SettlementType::INSTANT;
    OTCChain chain = OTCChain::ETHEREUM;
    std::chrono::system_clock::time_point createdAt{};
    std::chrono::system_clock::time_point expiresAt{};
    std::optional<std::chrono::system_clock::time_point> matchedAt;
    std::optional<std::chrono::system_clock::time_point> settledAt;
    std::string escrowAddress;
    std::string notes;
    bool partialFillAllowed = false;
    double filledAmount = 0.0;
};

struct TradeExecution {
    std::string executionId;
    std::string offerId;
    std::string makerId;
    std::string takerId;
    OTCToken baseToken;
    OTCToken quoteToken;
    double baseAmount = 0.0;
    double quoteAmount = 0.0;
    double pricePerUnit = 0.0;
    OfferSide side = OfferSide::BUY;
    std::string makerTxHash;
    std::string takerTxHash;
    std::string escrowTxHash;
    std::string settlementTxHash;
    OfferStatus status = OfferStatus::PENDING;
    std::chrono::system_clock::time_point executedAt{};
    double feeAmount = 0.0;
    std::string feeToken;
    OTCOperationStatus lifecycleStatus = OTCOperationStatus::UNAVAILABLE;
    OTCAdapterEvidence settlementEvidence;
    bool settlementVerified = false;
};

struct NegotiationMessage {
    std::string messageId;
    std::string offerId;
    std::string senderId;
    std::string recipientId;
    std::string message;
    std::optional<double> proposedPrice;
    std::optional<double> proposedAmount;
    std::chrono::system_clock::time_point timestamp{};
    bool isCounterOffer = false;
    OTCOperationStatus deliveryStatus = OTCOperationStatus::PROPOSED;
    OTCAdapterEvidence deliveryEvidence;
    std::string idempotencyKey;
};

struct CounterpartyProfile {
    std::string counterpartyId;
    int totalTrades = 0;
    int successfulTrades = 0;
    double totalVolume = 0.0;
    double averageTradeSize = 0.0;
    double reputationScore = 0.0;
    std::vector<std::string> verifications;
    std::chrono::system_clock::time_point memberSince{};
    bool isVerified = false;
    bool isWhitelisted = false;
    bool isBlacklisted = false;
};

struct OTCMarketStats {
    std::string tokenPair;
    double bestBid = 0.0;
    double bestAsk = 0.0;
    double spread = 0.0;
    double volume24h = 0.0;
    int trades24h = 0;
    double avgTradeSize = 0.0;
    double priceChange24h = 0.0;
    double currentPrice = 0.0;
    double volatility = 0.0;
    std::chrono::system_clock::time_point asOf{};
    std::string source;
};

struct RiskAssessment {
    std::string offerId;
    double counterpartyRisk = 1.0;
    double priceRisk = 1.0;
    double liquidityRisk = 1.0;
    double volatilityRisk = 1.0;
    double overallRisk = 1.0;
    std::vector<std::string> warnings;
    bool recommended = false;
    bool marketDataVerified = false;
    OTCAdapterEvidence marketEvidence;
};

struct MarketDataRequest {
    OTCToken baseToken;
    OTCToken quoteToken;
    std::chrono::seconds maximumAge{300};
};

class OTCMarketDataAdapter {
public:
    virtual ~OTCMarketDataAdapter() = default;
    virtual OTCResult<OTCMarketStats> getMarketData(const MarketDataRequest& request) = 0;
};

struct NegotiationTransportRequest {
    std::string negotiationId;
    std::string offerId;
    std::string senderId;
    std::string recipientId;
    std::string message;
    std::optional<double> proposedPrice;
    std::optional<double> proposedAmount;
    std::string idempotencyKey;
};

struct NegotiationTransportReceipt {
    std::string deliveryId;
    std::string idempotencyKey;
};

class NegotiationTransport {
public:
    virtual ~NegotiationTransport() = default;
    virtual OTCResult<NegotiationTransportReceipt> send(
        const NegotiationTransportRequest& request) = 0;
};

enum class ComplianceAction { ACCEPT_OFFER, SUBMIT_SETTLEMENT, DEPLOY_CONTRACT };

struct ComplianceRequest {
    ComplianceAction action = ComplianceAction::ACCEPT_OFFER;
    std::string requestId;
    std::string offerId;
    std::string makerId;
    std::string takerId;
    OTCToken baseToken;
    OTCToken quoteToken;
    double baseAmount = 0.0;
    double quoteAmount = 0.0;
    OTCChain chain = OTCChain::ETHEREUM;
};

struct ComplianceDecision {
    bool approved = false;
    std::string decisionId;
    std::chrono::system_clock::time_point validUntil{};
};

class ComplianceAdapter {
public:
    virtual ~ComplianceAdapter() = default;
    virtual OTCResult<ComplianceDecision> check(const ComplianceRequest& request) = 0;
};

enum class SettlementAction {
    EXECUTE_OFFER, CREATE_ESCROW, DEPOSIT_ESCROW, RELEASE_ESCROW,
    REFUND_ESCROW, DISPUTE_ESCROW, RESOLVE_DISPUTE, DEPLOY_CONTRACT
};

struct SettlementRequest {
    SettlementAction action = SettlementAction::EXECUTE_OFFER;
    std::string idempotencyKey;
    std::string offerId;
    std::string escrowId;
    std::string makerId;
    std::string takerId;
    std::string recipientId;
    std::string externalReference;
    OTCToken baseToken;
    OTCToken quoteToken;
    double baseAmount = 0.0;
    double expectedPrice = 0.0;
    double maximumSlippagePercent = 0.0;
    SettlementType settlementType = SettlementType::INSTANT;
    OTCChain chain = OTCChain::ETHEREUM;
};

struct SettlementReceipt {
    std::string transactionId;
    std::string adapterReference;
    std::string contractAddress;
    double executedBaseAmount = 0.0;
    double executedQuoteAmount = 0.0;
    double executionPrice = 0.0;
    double feeAmount = 0.0;
    std::string feeToken;
    std::chrono::system_clock::time_point settledAt{};
};

class SettlementAdapter {
public:
    virtual ~SettlementAdapter() = default;
    virtual OTCResult<SettlementReceipt> submit(const SettlementRequest& request) = 0;
    virtual OTCResult<SettlementReceipt> verify(const SettlementRequest& request) = 0;
};

class EscrowManager {
public:
    struct EscrowDetails {
        std::string escrowId;
        std::string makerId;
        std::string takerId;
        OTCToken token;
        double amount = 0.0;
        OTCChain chain = OTCChain::ETHEREUM;
        std::string contractAddress;
        std::string status;
        OTCOperationStatus operationStatus = OTCOperationStatus::UNAVAILABLE;
        std::chrono::system_clock::time_point createdAt{};
        std::optional<std::chrono::system_clock::time_point> releasedAt;
        std::string adapterReference;
        OTCAdapterEvidence evidence;
    };

    explicit EscrowManager(std::shared_ptr<SettlementAdapter> settlementAdapter = nullptr,
                           std::shared_ptr<OTCMarketDataAdapter> marketDataAdapter = nullptr);
    ~EscrowManager() = default;
    OTCResult<EscrowDetails> createEscrowResult(const std::string& makerId,
                                                 const std::string& takerId,
                                                 const OTCToken& token,
                                                 double amount,
                                                 OTCChain chain,
                                                 const std::string& idempotencyKey = "");
    OTCResult<EscrowDetails> depositToEscrowResult(const std::string& escrowId,
                                                   const std::string& transactionId,
                                                   const std::string& idempotencyKey = "");
    OTCResult<EscrowDetails> releaseEscrowResult(const std::string& escrowId,
                                                 const std::string& recipient,
                                                 const std::string& idempotencyKey = "");
    OTCResult<EscrowDetails> refundEscrowResult(const std::string& escrowId,
                                                const std::string& idempotencyKey = "");
    OTCResult<EscrowDetails> initiateDisputeResult(const std::string& escrowId,
                                                   const std::string& reason,
                                                   const std::string& idempotencyKey = "");
    OTCResult<EscrowDetails> resolveDisputeResult(const std::string& escrowId,
                                                  const std::string& winner,
                                                  const std::string& idempotencyKey = "");
    OTCResult<double> getTotalEscrowedValueResult(const std::string& userId);
    std::string createEscrow(const std::string& makerId, const std::string& takerId,
                             const OTCToken& token, double amount, OTCChain chain);
    bool depositToEscrow(const std::string& escrowId, const std::string& txHash);
    bool releaseEscrow(const std::string& escrowId, const std::string& recipient);
    bool refundEscrow(const std::string& escrowId);
    bool initiateDispute(const std::string& escrowId, const std::string& reason);
    bool resolveDispute(const std::string& escrowId, const std::string& winner);
    EscrowDetails getEscrowDetails(const std::string& escrowId);
    std::vector<EscrowDetails> getActiveEscrows(const std::string& userId);
    double getTotalEscrowedValue(const std::string& userId);
    void setSettlementAdapter(std::shared_ptr<SettlementAdapter> adapter);
    void setOTCMarketDataAdapter(std::shared_ptr<OTCMarketDataAdapter> adapter);

private:
    OTCResult<EscrowDetails> performEscrowAction(SettlementAction action,
                                                 const std::string& escrowId,
                                                 const std::string& value,
                                                 const std::string& idempotencyKey);
    std::unordered_map<std::string, EscrowDetails> escrows_;
    std::unordered_map<std::string, OTCResult<EscrowDetails>> replayResults_;
    std::shared_ptr<SettlementAdapter> settlementAdapter_;
    std::shared_ptr<OTCMarketDataAdapter> marketDataAdapter_;
    mutable std::mutex escrowMutex_;
};

class OTCOrderbook {
public:
    OTCOrderbook() = default;
    ~OTCOrderbook() = default;
    OTCResult<OTCOffer> addOfferResult(const OTCOffer& offer);
    OTCResult<OTCOffer> removeOfferResult(const std::string& offerId);
    OTCResult<OTCOffer> updateOfferResult(
        const OTCOffer& offer, std::optional<OfferStatus> expectedStatus = std::nullopt);
    void addOffer(const OTCOffer& offer);
    void removeOffer(const std::string& offerId);
    void updateOffer(const OTCOffer& offer);
    std::vector<OTCOffer> findMatchingOffers(const std::string& baseToken,
                                             const std::string& quoteToken,
                                             OfferSide side, double amount,
                                             double maxPrice = 0.0);
    std::optional<OTCOffer> getBestOffer(const std::string& baseToken,
                                        const std::string& quoteToken,
                                        OfferSide side);
    std::optional<OTCOffer> findOffer(const std::string& offerId) const;
    std::vector<OTCOffer> getOffersByMaker(const std::string& makerId);
    std::vector<OTCOffer> getOffersByToken(const std::string& tokenSymbol);
    std::vector<OTCOffer> getActiveOffers();
    OTCMarketStats getMarketStats(const std::string& baseToken,
                                  const std::string& quoteToken);
    double getSpread(const std::string& baseToken, const std::string& quoteToken);

private:
    std::unordered_map<std::string, OTCOffer> offers_;
    mutable std::mutex orderbookMutex_;
};

class OTCRiskManager {
public:
    explicit OTCRiskManager(std::shared_ptr<OTCMarketDataAdapter> marketDataAdapter = nullptr);
    ~OTCRiskManager() = default;
    OTCResult<RiskAssessment> assessOfferResult(const OTCOffer& offer,
                                                const std::string& userId);
    RiskAssessment assessOffer(const OTCOffer& offer, const std::string& userId);
    OTCResult<double> getPriceRiskResult(const OTCOffer& offer);
    double getCounterpartyRisk(const std::string& counterpartyId);
    double getPriceRisk(const OTCOffer& offer);
    void updateCounterpartyProfile(const std::string& counterpartyId,
                                   const TradeExecution& trade);
    CounterpartyProfile getCounterpartyProfile(const std::string& counterpartyId);
    bool isCounterpartyTrusted(const std::string& counterpartyId);
    void blacklistCounterparty(const std::string& counterpartyId,
                               const std::string& reason);
    void whitelistCounterparty(const std::string& counterpartyId);
    double getDailyLimit(const std::string& userId);
    double getRemainingLimit(const std::string& userId);
    bool checkLimit(const std::string& userId, double amount);
    OTCResult<double> setDailyLimitResult(const std::string& userId, double amount);
    void recordVerifiedUsage(const std::string& userId, double amount);
    void setOTCMarketDataAdapter(std::shared_ptr<OTCMarketDataAdapter> adapter);

private:
    double counterpartyRiskUnlocked(const std::string& counterpartyId) const;
    double dailyLimitUnlocked(const std::string& userId) const;
    std::unordered_map<std::string, CounterpartyProfile> profiles_;
    std::unordered_map<std::string, double> dailyLimits_;
    std::unordered_map<std::string, double> dailyUsed_;
    std::shared_ptr<OTCMarketDataAdapter> marketDataAdapter_;
    mutable std::mutex riskMutex_;
};

class NegotiationEngine {
public:
    struct NegotiationAdvice {
        bool shouldAccept = false;
        double suggestedPrice = 0.0;
        std::string reasoning;
        double confidence = 0.0;
    };
    explicit NegotiationEngine(std::shared_ptr<NegotiationTransport> transport = nullptr);
    ~NegotiationEngine() = default;
    OTCResult<std::string> startNegotiationResult(const std::string& offerId,
                                                   const std::string& initiatorId,
                                                   const std::string& counterpartyId = "",
                                                   const std::string& idempotencyKey = "");
    OTCResult<NegotiationMessage> sendMessageResult(
        const std::string& negotiationId, const std::string& senderId,
        const std::string& message, std::optional<double> proposedPrice = std::nullopt,
        std::optional<double> proposedAmount = std::nullopt,
        const std::string& idempotencyKey = "");
    OTCResult<NegotiationMessage> acceptCounterOfferResult(
        const std::string& negotiationId, const std::string& userId,
        const std::string& idempotencyKey = "");
    OTCResult<NegotiationMessage> rejectCounterOfferResult(
        const std::string& negotiationId, const std::string& userId,
        const std::string& idempotencyKey = "");
    OTCResult<NegotiationMessage> endNegotiationResult(
        const std::string& negotiationId, bool accepted,
        const std::string& idempotencyKey = "");
    OTCResult<NegotiationAdvice> getAIAdviceResult(
        const OTCOffer& offer, double marketPrice,
        const CounterpartyProfile& counterparty);
    std::string startNegotiation(const std::string& offerId,
                                 const std::string& initiatorId);
    bool sendMessage(const std::string& negotiationId, const std::string& senderId,
                     const std::string& message,
                     std::optional<double> proposedPrice = std::nullopt,
                     std::optional<double> proposedAmount = std::nullopt);
    bool acceptCounterOffer(const std::string& negotiationId,
                            const std::string& userId);
    bool rejectCounterOffer(const std::string& negotiationId,
                            const std::string& userId);
    bool endNegotiation(const std::string& negotiationId, bool accepted);
    NegotiationAdvice getAIAdvice(const OTCOffer& offer, double marketPrice,
                                  const CounterpartyProfile& counterparty);
    std::vector<NegotiationMessage> getNegotiationHistory(
        const std::string& negotiationId);
    std::string getNegotiationStatus(const std::string& negotiationId);
    OTCOperationStatus getNegotiationLifecycle(const std::string& negotiationId);
    std::string getNegotiationOfferId(const std::string& negotiationId);
    void setTransport(std::shared_ptr<NegotiationTransport> transport);

private:
    struct NegotiationSession {
        std::string offerId;
        std::string initiatorId;
        std::string counterpartyId;
        OTCOperationStatus status = OTCOperationStatus::PROPOSED;
        std::vector<NegotiationMessage> messages;
        std::unordered_map<std::string, OTCResult<NegotiationMessage>> replayResults;
    };
    OTCResult<NegotiationMessage> appendLifecycleMessage(
        const std::string& negotiationId, const std::string& senderId,
        const std::string& text, OTCOperationStatus finalStatus,
        const std::string& idempotencyKey, bool requireCounterOffer);
    std::unordered_map<std::string, NegotiationSession> negotiations_;
    std::unordered_map<std::string, OTCResult<std::string>> startReplayResults_;
    std::shared_ptr<NegotiationTransport> transport_;
    mutable std::mutex negotiationMutex_;
};

class OTCAnalytics {
public:
    struct TradingStats {
        int totalTrades = 0;
        double totalVolume = 0.0;
        double avgTradeSize = 0.0;
        double successRate = 0.0;
        double avgExecutionTime = 0.0;
        std::unordered_map<std::string, double> volumeByToken;
        std::unordered_map<OTCChain, double> volumeByChain;
    };
    OTCAnalytics() = default;
    ~OTCAnalytics() = default;
    OTCResult<TradeExecution> recordTradeResult(const TradeExecution& trade);
    void recordTrade(const TradeExecution& trade);
    TradingStats getUserStats(const std::string& userId, int days = 30);
    TradingStats getPlatformStats(int days = 30);
    std::vector<TradeExecution> getTradeHistory(const std::string& userId,
                                                int limit = 50);
    double getPnL(const std::string& userId, int days = 30);
    double getVWAP(const std::string& baseToken, const std::string& quoteToken,
                   int hours = 24);
    std::vector<std::pair<std::chrono::system_clock::time_point, double>>
    getPriceHistory(const std::string& baseToken, const std::string& quoteToken,
                    int days = 7);

private:
    std::vector<TradeExecution> tradeHistory_;
    mutable std::mutex analyticsMutex_;
};

class OTCAgent {
public:
    explicit OTCAgent(const std::string& agentId);
    OTCAgent(const std::string& agentId,
             std::shared_ptr<OTCMarketDataAdapter> marketDataAdapter,
             std::shared_ptr<NegotiationTransport> negotiationTransport,
             std::shared_ptr<ComplianceAdapter> complianceAdapter,
             std::shared_ptr<SettlementAdapter> settlementAdapter);
    ~OTCAgent() = default;
    OTCResult<OTCOffer> createOfferResult(const std::string& token,
                                          double amount, double price);
    OTCResult<OTCOffer> createAdvancedOfferResult(
        const OTCToken& baseToken, const OTCToken& quoteToken, double amount,
        double pricePerUnit, OfferSide side,
        SettlementType settlement = SettlementType::ESCROW,
        int expiryHours = 24, bool partialFill = false);
    OTCResult<OTCOffer> addObservedOfferResult(const OTCOffer& offer);
    OTCResult<OTCOffer> acceptOfferResult(const std::string& offerId,
                                          const std::string& idempotencyKey = "");
    OTCResult<OTCOffer> acceptPartialOfferResult(
        const std::string& offerId, double amount,
        const std::string& idempotencyKey = "");
    OTCResult<OTCOffer> cancelOfferResult(const std::string& offerId);
    OTCResult<OTCOffer> updateOfferPriceResult(const std::string& offerId,
                                               double newPrice);
    bool createOffer(const std::string& token, float amount, float price);
    std::string createAdvancedOffer(const OTCToken& baseToken,
                                    const OTCToken& quoteToken, double amount,
                                    double pricePerUnit, OfferSide side,
                                    SettlementType settlement = SettlementType::ESCROW,
                                    int expiryHours = 24, bool partialFill = false);
    bool acceptOffer(const std::string& offerId);
    bool acceptPartialOffer(const std::string& offerId, double amount);
    bool cancelOffer(const std::string& offerId);
    bool updateOfferPrice(const std::string& offerId, double newPrice);
    std::vector<OTCOffer> getActiveOffers(const std::string& tokenSymbol = "");
    std::vector<OTCOffer> getMyOffers();
    OTCOffer getOffer(const std::string& offerId);
    OTCResult<OTCMarketStats> getMarketStatsResult(const std::string& baseToken,
                                                   const std::string& quoteToken);
    OTCMarketStats getMarketStats(const std::string& baseToken,
                                  const std::string& quoteToken);
    std::optional<OTCOffer> findBestOffer(const std::string& baseToken,
                                         const std::string& quoteToken,
                                         OfferSide side, double amount);
    std::vector<OTCOffer> findMatchingOffers(const std::string& baseToken,
                                             OfferSide side,
                                             double minAmount = 0.0,
                                             double maxPrice = 0.0);
    OTCResult<std::string> startNegotiationResult(
        const std::string& offerId, const std::string& idempotencyKey = "");
    OTCResult<NegotiationMessage> sendNegotiationMessageResult(
        const std::string& negotiationId, const std::string& message,
        std::optional<double> counterPrice = std::nullopt,
        const std::string& idempotencyKey = "");
    OTCResult<NegotiationMessage> acceptNegotiationResult(
        const std::string& negotiationId, const std::string& idempotencyKey = "");
    OTCResult<NegotiationMessage> rejectNegotiationResult(
        const std::string& negotiationId, const std::string& idempotencyKey = "");
    OTCResult<NegotiationEngine::NegotiationAdvice> getAIAdviceResult(
        const std::string& offerId);
    std::string startNegotiation(const std::string& offerId);
    bool sendNegotiationMessage(const std::string& negotiationId,
                                const std::string& message,
                                std::optional<double> counterPrice = std::nullopt);
    bool acceptNegotiation(const std::string& negotiationId);
    bool rejectNegotiation(const std::string& negotiationId);
    NegotiationEngine::NegotiationAdvice getAIAdvice(const std::string& offerId);
    OTCResult<SettlementReceipt> initiateSettlementResult(
        const std::string& offerId, const std::string& idempotencyKey = "");
    OTCResult<SettlementReceipt> confirmSettlementResult(
        const std::string& offerId, const std::string& transactionId,
        const std::string& idempotencyKey = "");
    bool initiateSettlement(const std::string& offerId);
    bool confirmSettlement(const std::string& offerId, const std::string& txHash);
    std::string getSettlementStatus(const std::string& offerId);
    OTCOperationStatus getSettlementLifecycle(const std::string& offerId);
    OTCResult<RiskAssessment> assessOfferResult(const std::string& offerId);
    RiskAssessment assessOffer(const std::string& offerId);
    CounterpartyProfile getCounterpartyProfile(const std::string& counterpartyId);
    bool isCounterpartyTrusted(const std::string& counterpartyId);
    OTCResult<std::string> deployContractResult(const std::string& chain,
                                                const std::string& idempotencyKey = "");
    bool deployContract(const std::string& chain);
    std::string getContractAddress(const std::string& chain);
    std::vector<OTCChain> getSupportedChains() const;
    bool switchChain(OTCChain chain);
    OTCAnalytics::TradingStats getMyStats(int days = 30);
    std::vector<TradeExecution> getTradeHistory(int limit = 50);
    double getPnL(int days = 30);
    OTCResult<double> setDefaultExpiryResult(int hours);
    OTCResult<double> setMaxSlippageResult(double percentage);
    OTCResult<double> setMinTradeSizeResult(double amount);
    void setDefaultSettlement(SettlementType type);
    void setDefaultExpiry(int hours);
    void setMaxSlippage(double percentage);
    void setMinTradeSize(double amount);
    void setOTCMarketDataAdapter(std::shared_ptr<OTCMarketDataAdapter> adapter);
    void setNegotiationTransport(std::shared_ptr<NegotiationTransport> transport);
    void setComplianceAdapter(std::shared_ptr<ComplianceAdapter> adapter);
    void setSettlementAdapter(std::shared_ptr<SettlementAdapter> adapter);
    std::string getAgentId() const;
    std::string getStatus() const;
    void setStatusCallback(std::function<void(const std::string&)> callback);

private:
    struct SettlementState {
        OTCOperationStatus status = OTCOperationStatus::UNAVAILABLE;
        SettlementRequest request;
        SettlementReceipt receipt;
        OTCAdapterEvidence evidence;
        OTCResult<SettlementReceipt> result;
    };
    struct CachedAcceptance {
        std::string fingerprint;
        OTCResult<OTCOffer> result;
    };
    OTCResult<OTCOffer> acceptAmountResult(const std::string& offerId,
                                           std::optional<double> amount,
                                           const std::string& idempotencyKey);
    OTCResult<ComplianceDecision> checkCompliance(const OTCOffer& offer,
                                                   double amount,
                                                   ComplianceAction action,
                                                   const std::string& requestId);
    void initializeSubManagers();
    void logStatus(const std::string& status);
    OTCChain stringToChain(const std::string& chain) const;
    std::string chainToString(OTCChain chain) const;
    std::string generateId(const std::string& prefix);

    std::string agentId_;
    OTCChain currentChain_ = OTCChain::BASE;
    std::shared_ptr<AgentLogger> logger_;
    std::unique_ptr<EscrowManager> escrowManager_;
    std::unique_ptr<OTCOrderbook> orderbook_;
    std::unique_ptr<OTCRiskManager> riskManager_;
    std::unique_ptr<NegotiationEngine> negotiationEngine_;
    std::unique_ptr<OTCAnalytics> analytics_;
    SettlementType defaultSettlement_ = SettlementType::ESCROW;
    int defaultExpiryHours_ = 24;
    double maxSlippage_ = 1.0;
    double minTradeSize_ = 100.0;
    std::shared_ptr<OTCMarketDataAdapter> marketDataAdapter_;
    std::shared_ptr<NegotiationTransport> negotiationTransport_;
    std::shared_ptr<ComplianceAdapter> complianceAdapter_;
    std::shared_ptr<SettlementAdapter> settlementAdapter_;
    std::function<void(const std::string&)> statusCallback_;
    std::unordered_map<std::string, double> pendingAcceptanceAmounts_;
    std::unordered_map<std::string, CachedAcceptance> acceptanceReplay_;
    std::unordered_map<std::string, SettlementState> settlements_;
    std::unordered_set<std::string> settlementInFlight_;
    std::unordered_map<std::string, std::string> deployedContracts_;
    mutable std::mutex agentMutex_;
};

namespace otc_utils {
double calculateSpread(double bid, double ask);
double calculatePriceImpact(double amount, double liquidity, double currentPrice);
double calculateFee(double amount, double feeRate);
std::string getChainName(OTCChain chain);
std::string getChainCurrency(OTCChain chain);
std::string getExplorerUrl(OTCChain chain, const std::string& txHash);
bool isValidAmount(double amount, const OTCToken& token);
bool isValidPrice(double price);
bool isValidAddress(const std::string& address, OTCChain chain);
}  // namespace otc_utils

}  // namespace elizaos
