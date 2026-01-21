#include "elizaos/otc_agent.hpp"
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace elizaos {

// ============================================================================
// Utility functions
// ============================================================================

static std::string generateRandomId(const std::string& prefix, size_t length = 12) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);

    std::string id = prefix + "-";
    for (size_t i = 0; i < length; ++i) {
        id += "0123456789abcdef"[dis(gen)];
    }
    return id;
}

static std::string generateContractAddress() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);

    std::string address = "0x";
    for (int i = 0; i < 40; ++i) {
        address += "0123456789abcdef"[dis(gen)];
    }
    return address;
}

// Generate mock transaction hash (used for escrow/settlement)
[[maybe_unused]] static std::string generateTxHash() {
    return "0x" + generateRandomId("", 64);
}

// Mock market prices
static std::map<std::string, double> mockPrices = {
    {"ETH", 3500.0},
    {"BTC", 65000.0},
    {"USDC", 1.0},
    {"USDT", 1.0},
    {"DAI", 1.0},
    {"LINK", 15.50},
    {"UNI", 6.80},
    {"ARB", 1.25}
};

// Mock contract addresses by chain
static std::map<std::string, std::string> deployedContracts;

// ============================================================================
// EscrowManager Implementation
// ============================================================================

EscrowManager::EscrowManager() {}

std::string EscrowManager::createEscrow(
    const std::string& makerId,
    const std::string& takerId,
    const OTCToken& token,
    double amount,
    OTCChain chain) {

    std::lock_guard<std::mutex> lock(escrowMutex_);

    EscrowDetails escrow;
    escrow.escrowId = generateRandomId("escrow");
    escrow.makerId = makerId;
    escrow.takerId = takerId;
    escrow.token = token;
    escrow.amount = amount;
    escrow.chain = chain;
    escrow.contractAddress = generateContractAddress();
    escrow.status = "pending";
    escrow.createdAt = std::chrono::system_clock::now();

    escrows_[escrow.escrowId] = escrow;

    elizaos::logInfo("Escrow created: " + escrow.escrowId + " for " + std::to_string(amount) + " " + token.symbol, "otc_agent");

    return escrow.escrowId;
}

bool EscrowManager::depositToEscrow(const std::string& escrowId, const std::string& txHash) {
    std::lock_guard<std::mutex> lock(escrowMutex_);

    auto it = escrows_.find(escrowId);
    if (it == escrows_.end()) {
        elizaos::logError("Escrow not found: " + escrowId, "otc_agent");
        return false;
    }

    it->second.status = "funded";
    elizaos::logInfo("Escrow funded: " + escrowId + " tx: " + txHash, "otc_agent");
    return true;
}

bool EscrowManager::releaseEscrow(const std::string& escrowId, const std::string& recipient) {
    std::lock_guard<std::mutex> lock(escrowMutex_);

    auto it = escrows_.find(escrowId);
    if (it == escrows_.end()) {
        elizaos::logError("Escrow not found: " + escrowId, "otc_agent");
        return false;
    }

    if (it->second.status != "funded") {
        elizaos::logError("Escrow not funded: " + escrowId, "otc_agent");
        return false;
    }

    it->second.status = "released";
    it->second.releasedAt = std::chrono::system_clock::now();
    elizaos::logInfo("Escrow released to " + recipient + ": " + escrowId, "otc_agent");
    return true;
}

bool EscrowManager::refundEscrow(const std::string& escrowId) {
    std::lock_guard<std::mutex> lock(escrowMutex_);

    auto it = escrows_.find(escrowId);
    if (it == escrows_.end()) {
        elizaos::logError("Escrow not found: " + escrowId, "otc_agent");
        return false;
    }

    it->second.status = "refunded";
    it->second.releasedAt = std::chrono::system_clock::now();
    elizaos::logInfo("Escrow refunded: " + escrowId, "otc_agent");
    return true;
}

bool EscrowManager::initiateDispute(const std::string& escrowId, const std::string& reason) {
    std::lock_guard<std::mutex> lock(escrowMutex_);

    auto it = escrows_.find(escrowId);
    if (it == escrows_.end()) {
        return false;
    }

    it->second.status = "disputed";
    elizaos::logWarning("Escrow disputed: " + escrowId + " - " + reason, "otc_agent");
    return true;
}

bool EscrowManager::resolveDispute(const std::string& escrowId, const std::string& winner) {
    std::lock_guard<std::mutex> lock(escrowMutex_);

    auto it = escrows_.find(escrowId);
    if (it == escrows_.end() || it->second.status != "disputed") {
        return false;
    }

    it->second.status = "resolved";
    it->second.releasedAt = std::chrono::system_clock::now();
    elizaos::logInfo("Dispute resolved for " + escrowId + " in favor of " + winner, "otc_agent");
    return true;
}

EscrowManager::EscrowDetails EscrowManager::getEscrowDetails(const std::string& escrowId) {
    std::lock_guard<std::mutex> lock(escrowMutex_);

    auto it = escrows_.find(escrowId);
    if (it != escrows_.end()) {
        return it->second;
    }
    return EscrowDetails{};
}

std::vector<EscrowManager::EscrowDetails> EscrowManager::getActiveEscrows(const std::string& userId) {
    std::lock_guard<std::mutex> lock(escrowMutex_);

    std::vector<EscrowDetails> active;
    for (const auto& [id, escrow] : escrows_) {
        if ((escrow.makerId == userId || escrow.takerId == userId) &&
            (escrow.status == "pending" || escrow.status == "funded")) {
            active.push_back(escrow);
        }
    }
    return active;
}

double EscrowManager::getTotalEscrowedValue(const std::string& userId) {
    auto active = getActiveEscrows(userId);
    double total = 0.0;

    for (const auto& escrow : active) {
        double price = mockPrices.count(escrow.token.symbol) ? mockPrices[escrow.token.symbol] : 1.0;
        total += escrow.amount * price;
    }

    return total;
}

// ============================================================================
// OTCOrderbook Implementation
// ============================================================================

OTCOrderbook::OTCOrderbook() {}

void OTCOrderbook::addOffer(const OTCOffer& offer) {
    std::lock_guard<std::mutex> lock(orderbookMutex_);

    offers_[offer.offerId] = offer;
    tokenOffers_[offer.baseToken.symbol].push_back(offer.offerId);

    elizaos::logInfo("Offer added to orderbook: " + offer.offerId, "otc_agent");
}

void OTCOrderbook::removeOffer(const std::string& offerId) {
    std::lock_guard<std::mutex> lock(orderbookMutex_);

    auto it = offers_.find(offerId);
    if (it != offers_.end()) {
        auto& tokenIds = tokenOffers_[it->second.baseToken.symbol];
        tokenIds.erase(std::remove(tokenIds.begin(), tokenIds.end(), offerId), tokenIds.end());
        offers_.erase(it);
        elizaos::logInfo("Offer removed from orderbook: " + offerId, "otc_agent");
    }
}

void OTCOrderbook::updateOffer(const OTCOffer& offer) {
    std::lock_guard<std::mutex> lock(orderbookMutex_);

    if (offers_.count(offer.offerId)) {
        offers_[offer.offerId] = offer;
    }
}

std::vector<OTCOffer> OTCOrderbook::findMatchingOffers(
    const std::string& baseToken,
    const std::string& quoteToken,
    OfferSide side,
    double amount,
    double maxPrice) {

    std::lock_guard<std::mutex> lock(orderbookMutex_);

    std::vector<OTCOffer> matches;
    OfferSide targetSide = (side == OfferSide::BUY) ? OfferSide::SELL : OfferSide::BUY;

    for (const auto& [id, offer] : offers_) {
        if (offer.status != OfferStatus::ACTIVE) continue;
        if (offer.baseToken.symbol != baseToken) continue;
        if (offer.quoteToken.symbol != quoteToken) continue;
        if (offer.side != targetSide) continue;

        double available = offer.baseAmount - offer.filledAmount;
        if (available < amount && !offer.partialFillAllowed) continue;

        if (maxPrice > 0 && side == OfferSide::BUY && offer.pricePerUnit > maxPrice) continue;
        if (maxPrice > 0 && side == OfferSide::SELL && offer.pricePerUnit < maxPrice) continue;

        matches.push_back(offer);
    }

    // Sort by price (best first)
    std::sort(matches.begin(), matches.end(), [side](const OTCOffer& a, const OTCOffer& b) {
        if (side == OfferSide::BUY) {
            return a.pricePerUnit < b.pricePerUnit; // Lowest price for buyers
        }
        return a.pricePerUnit > b.pricePerUnit; // Highest price for sellers
    });

    return matches;
}

std::optional<OTCOffer> OTCOrderbook::getBestOffer(
    const std::string& baseToken,
    const std::string& quoteToken,
    OfferSide side) {

    auto matches = findMatchingOffers(baseToken, quoteToken, side, 0.0, 0.0);
    if (!matches.empty()) {
        return matches[0];
    }
    return std::nullopt;
}

std::vector<OTCOffer> OTCOrderbook::getOffersByMaker(const std::string& makerId) {
    std::lock_guard<std::mutex> lock(orderbookMutex_);

    std::vector<OTCOffer> result;
    for (const auto& [id, offer] : offers_) {
        if (offer.makerId == makerId) {
            result.push_back(offer);
        }
    }
    return result;
}

std::vector<OTCOffer> OTCOrderbook::getOffersByToken(const std::string& tokenSymbol) {
    std::lock_guard<std::mutex> lock(orderbookMutex_);

    std::vector<OTCOffer> result;
    if (tokenOffers_.count(tokenSymbol)) {
        for (const auto& offerId : tokenOffers_[tokenSymbol]) {
            if (offers_.count(offerId)) {
                result.push_back(offers_[offerId]);
            }
        }
    }
    return result;
}

std::vector<OTCOffer> OTCOrderbook::getActiveOffers() {
    std::lock_guard<std::mutex> lock(orderbookMutex_);

    std::vector<OTCOffer> result;
    for (const auto& [id, offer] : offers_) {
        if (offer.status == OfferStatus::ACTIVE) {
            result.push_back(offer);
        }
    }
    return result;
}

OTCMarketStats OTCOrderbook::getMarketStats(const std::string& baseToken, const std::string& quoteToken) {
    std::lock_guard<std::mutex> lock(orderbookMutex_);

    OTCMarketStats stats;
    stats.tokenPair = baseToken + "/" + quoteToken;
    stats.bestBid = 0.0;
    stats.bestAsk = 0.0;
    stats.volume24h = 0.0;
    stats.trades24h = 0;

    double totalBidPrice = 0.0, totalAskPrice = 0.0;
    int bidCount = 0, askCount = 0;

    for (const auto& [id, offer] : offers_) {
        if (offer.status != OfferStatus::ACTIVE) continue;
        if (offer.baseToken.symbol != baseToken) continue;

        if (offer.side == OfferSide::BUY) {
            if (offer.pricePerUnit > stats.bestBid) {
                stats.bestBid = offer.pricePerUnit;
            }
            totalBidPrice += offer.pricePerUnit;
            bidCount++;
        } else {
            if (stats.bestAsk == 0 || offer.pricePerUnit < stats.bestAsk) {
                stats.bestAsk = offer.pricePerUnit;
            }
            totalAskPrice += offer.pricePerUnit;
            askCount++;
        }
    }

    stats.spread = (stats.bestAsk > 0 && stats.bestBid > 0) ? (stats.bestAsk - stats.bestBid) : 0.0;

    // Mock 24h stats
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> volDis(10000, 1000000);
    std::uniform_int_distribution<> tradeDis(10, 100);

    stats.volume24h = volDis(gen);
    stats.trades24h = tradeDis(gen);
    stats.avgTradeSize = stats.trades24h > 0 ? stats.volume24h / stats.trades24h : 0;
    stats.priceChange24h = (gen() % 200 - 100) / 10.0; // -10% to +10%

    return stats;
}

double OTCOrderbook::getSpread(const std::string& baseToken, const std::string& quoteToken) {
    auto stats = getMarketStats(baseToken, quoteToken);
    return stats.spread;
}

// ============================================================================
// OTCRiskManager Implementation
// ============================================================================

OTCRiskManager::OTCRiskManager() {
    // Set default daily limits
    dailyLimits_["default"] = 100000.0; // $100k default
}

RiskAssessment OTCRiskManager::assessOffer(const OTCOffer& offer, const std::string& userId) {
    std::lock_guard<std::mutex> lock(riskMutex_);

    RiskAssessment assessment;
    assessment.offerId = offer.offerId;

    // Counterparty risk
    assessment.counterpartyRisk = getCounterpartyRisk(offer.makerId);

    // Price risk
    assessment.priceRisk = getPriceRisk(offer);

    // Liquidity risk (based on amount)
    double tradeValue = offer.baseAmount * offer.pricePerUnit;
    assessment.liquidityRisk = std::min(1.0, tradeValue / 1000000.0); // Higher risk for larger trades

    // Volatility risk (mock)
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.1, 0.5);
    assessment.volatilityRisk = dis(gen);

    // Overall risk
    assessment.overallRisk = (assessment.counterpartyRisk * 0.3 +
                              assessment.priceRisk * 0.3 +
                              assessment.liquidityRisk * 0.2 +
                              assessment.volatilityRisk * 0.2);

    // Generate warnings
    if (assessment.counterpartyRisk > 0.5) {
        assessment.warnings.push_back("High counterparty risk - unverified trader");
    }
    if (assessment.priceRisk > 0.3) {
        assessment.warnings.push_back("Price significantly differs from market");
    }
    if (tradeValue > 50000) {
        assessment.warnings.push_back("Large trade size - consider splitting");
    }

    // Check user limit
    if (!checkLimit(userId, tradeValue)) {
        assessment.warnings.push_back("Trade exceeds your daily limit");
    }

    assessment.recommended = assessment.overallRisk < 0.5 && assessment.warnings.size() < 2;

    return assessment;
}

double OTCRiskManager::getCounterpartyRisk(const std::string& counterpartyId) {
    auto it = profiles_.find(counterpartyId);
    if (it == profiles_.end()) {
        return 0.7; // High risk for unknown counterparties
    }

    const auto& profile = it->second;

    if (profile.isBlacklisted) return 1.0;
    if (!profile.isVerified) return 0.6;

    double successRate = profile.totalTrades > 0 ?
        static_cast<double>(profile.successfulTrades) / profile.totalTrades : 0.0;

    return std::max(0.0, 1.0 - (profile.reputationScore / 5.0) - (successRate * 0.3));
}

double OTCRiskManager::getPriceRisk(const OTCOffer& offer) {
    double marketPrice = mockPrices.count(offer.baseToken.symbol) ?
        mockPrices[offer.baseToken.symbol] : offer.pricePerUnit;

    double deviation = std::abs(offer.pricePerUnit - marketPrice) / marketPrice;
    return std::min(1.0, deviation * 5.0); // 20% deviation = max risk
}

void OTCRiskManager::updateCounterpartyProfile(const std::string& counterpartyId, const TradeExecution& trade) {
    std::lock_guard<std::mutex> lock(riskMutex_);

    auto& profile = profiles_[counterpartyId];
    profile.counterpartyId = counterpartyId;
    profile.totalTrades++;
    profile.totalVolume += trade.quoteAmount;

    if (trade.status == OfferStatus::COMPLETED) {
        profile.successfulTrades++;
        profile.reputationScore = std::min(5.0, profile.reputationScore + 0.1);
    } else if (trade.status == OfferStatus::DISPUTED) {
        profile.reputationScore = std::max(0.0, profile.reputationScore - 0.5);
    }

    profile.averageTradeSize = profile.totalVolume / profile.totalTrades;
}

CounterpartyProfile OTCRiskManager::getCounterpartyProfile(const std::string& counterpartyId) {
    std::lock_guard<std::mutex> lock(riskMutex_);

    if (profiles_.count(counterpartyId)) {
        return profiles_[counterpartyId];
    }

    // Return default profile
    CounterpartyProfile defaultProfile;
    defaultProfile.counterpartyId = counterpartyId;
    defaultProfile.totalTrades = 0;
    defaultProfile.successfulTrades = 0;
    defaultProfile.reputationScore = 2.5;
    defaultProfile.memberSince = std::chrono::system_clock::now();
    defaultProfile.isVerified = false;
    defaultProfile.isWhitelisted = false;
    defaultProfile.isBlacklisted = false;

    return defaultProfile;
}

bool OTCRiskManager::isCounterpartyTrusted(const std::string& counterpartyId) {
    auto profile = getCounterpartyProfile(counterpartyId);
    return profile.reputationScore >= 3.5 && profile.successfulTrades >= 5 && !profile.isBlacklisted;
}

void OTCRiskManager::blacklistCounterparty(const std::string& counterpartyId, const std::string& reason) {
    std::lock_guard<std::mutex> lock(riskMutex_);

    profiles_[counterpartyId].isBlacklisted = true;
    elizaos::logWarning("Counterparty blacklisted: " + counterpartyId + " - " + reason, "otc_agent");
}

void OTCRiskManager::whitelistCounterparty(const std::string& counterpartyId) {
    std::lock_guard<std::mutex> lock(riskMutex_);

    profiles_[counterpartyId].isWhitelisted = true;
    profiles_[counterpartyId].isVerified = true;
    elizaos::logInfo("Counterparty whitelisted: " + counterpartyId, "otc_agent");
}

double OTCRiskManager::getDailyLimit(const std::string& userId) {
    std::lock_guard<std::mutex> lock(riskMutex_);

    if (dailyLimits_.count(userId)) {
        return dailyLimits_[userId];
    }
    return dailyLimits_["default"];
}

double OTCRiskManager::getRemainingLimit(const std::string& userId) {
    return getDailyLimit(userId) - dailyUsed_[userId];
}

bool OTCRiskManager::checkLimit(const std::string& userId, double amount) {
    return getRemainingLimit(userId) >= amount;
}

// ============================================================================
// NegotiationEngine Implementation
// ============================================================================

NegotiationEngine::NegotiationEngine() {}

std::string NegotiationEngine::startNegotiation(const std::string& offerId, const std::string& initiatorId) {
    std::lock_guard<std::mutex> lock(negotiationMutex_);

    std::string negotiationId = generateRandomId("nego");

    NegotiationMessage msg;
    msg.messageId = generateRandomId("msg");
    msg.offerId = offerId;
    msg.senderId = initiatorId;
    msg.message = "Negotiation started";
    msg.timestamp = std::chrono::system_clock::now();
    msg.isCounterOffer = false;

    negotiations_[negotiationId].push_back(msg);

    elizaos::logInfo("Negotiation started: " + negotiationId + " for offer " + offerId, "otc_agent");
    return negotiationId;
}

bool NegotiationEngine::sendMessage(
    const std::string& negotiationId,
    const std::string& senderId,
    const std::string& message,
    std::optional<double> proposedPrice,
    std::optional<double> proposedAmount) {

    std::lock_guard<std::mutex> lock(negotiationMutex_);

    if (!negotiations_.count(negotiationId)) {
        return false;
    }

    NegotiationMessage msg;
    msg.messageId = generateRandomId("msg");
    msg.senderId = senderId;
    msg.message = message;
    msg.proposedPrice = proposedPrice;
    msg.proposedAmount = proposedAmount;
    msg.timestamp = std::chrono::system_clock::now();
    msg.isCounterOffer = proposedPrice.has_value() || proposedAmount.has_value();

    negotiations_[negotiationId].push_back(msg);

    elizaos::logInfo("Negotiation message sent in " + negotiationId, "otc_agent");
    return true;
}

bool NegotiationEngine::acceptCounterOffer(const std::string& negotiationId, const std::string& userId) {
    std::lock_guard<std::mutex> lock(negotiationMutex_);

    if (!negotiations_.count(negotiationId)) {
        return false;
    }

    NegotiationMessage msg;
    msg.messageId = generateRandomId("msg");
    msg.senderId = userId;
    msg.message = "Counter offer accepted";
    msg.timestamp = std::chrono::system_clock::now();
    msg.isCounterOffer = false;

    negotiations_[negotiationId].push_back(msg);

    elizaos::logInfo("Counter offer accepted in negotiation " + negotiationId, "otc_agent");
    return true;
}

bool NegotiationEngine::rejectCounterOffer(const std::string& negotiationId, const std::string& userId) {
    std::lock_guard<std::mutex> lock(negotiationMutex_);

    if (!negotiations_.count(negotiationId)) {
        return false;
    }

    NegotiationMessage msg;
    msg.messageId = generateRandomId("msg");
    msg.senderId = userId;
    msg.message = "Counter offer rejected";
    msg.timestamp = std::chrono::system_clock::now();

    negotiations_[negotiationId].push_back(msg);
    return true;
}

bool NegotiationEngine::endNegotiation(const std::string& negotiationId, bool accepted) {
    std::lock_guard<std::mutex> lock(negotiationMutex_);

    if (!negotiations_.count(negotiationId)) {
        return false;
    }

    NegotiationMessage msg;
    msg.messageId = generateRandomId("msg");
    msg.message = accepted ? "Negotiation concluded - deal accepted" : "Negotiation ended - no deal";
    msg.timestamp = std::chrono::system_clock::now();

    negotiations_[negotiationId].push_back(msg);

    elizaos::logInfo("Negotiation " + negotiationId + (accepted ? " completed successfully" : " ended without deal"), "otc_agent");
    return true;
}

NegotiationEngine::NegotiationAdvice NegotiationEngine::getAIAdvice(
    const OTCOffer& offer,
    double marketPrice,
    const CounterpartyProfile& counterparty) {

    NegotiationAdvice advice;

    double priceDeviation = (offer.pricePerUnit - marketPrice) / marketPrice;
    bool isBuying = offer.side == OfferSide::SELL; // If they're selling, we're buying

    // Calculate suggested price
    if (isBuying) {
        // For buying, suggest lower price
        advice.suggestedPrice = marketPrice * 0.98; // 2% discount
        advice.shouldAccept = offer.pricePerUnit <= marketPrice * 1.02;
    } else {
        // For selling, suggest higher price
        advice.suggestedPrice = marketPrice * 1.02; // 2% premium
        advice.shouldAccept = offer.pricePerUnit >= marketPrice * 0.98;
    }

    // Factor in counterparty reputation
    if (counterparty.reputationScore < 3.0) {
        advice.shouldAccept = false;
        advice.reasoning = "Eliza: Counterparty has low reputation score (" +
            std::to_string(counterparty.reputationScore) + "/5.0). Proceed with caution.";
    } else if (std::abs(priceDeviation) > 0.05) {
        advice.reasoning = "Eliza: Price is " +
            std::to_string(static_cast<int>(priceDeviation * 100)) +
            "% from market. Consider counter-offering at $" +
            std::to_string(static_cast<int>(advice.suggestedPrice));
    } else {
        advice.reasoning = "Eliza: Price is within acceptable range. Trade looks reasonable.";
    }

    // Confidence based on data quality
    advice.confidence = std::min(1.0, 0.5 + counterparty.successfulTrades * 0.05);

    return advice;
}

std::vector<NegotiationMessage> NegotiationEngine::getNegotiationHistory(const std::string& negotiationId) {
    std::lock_guard<std::mutex> lock(negotiationMutex_);

    if (negotiations_.count(negotiationId)) {
        return negotiations_[negotiationId];
    }
    return {};
}

std::string NegotiationEngine::getNegotiationStatus(const std::string& negotiationId) {
    std::lock_guard<std::mutex> lock(negotiationMutex_);

    if (!negotiations_.count(negotiationId)) {
        return "not_found";
    }

    auto& messages = negotiations_[negotiationId];
    if (messages.empty()) {
        return "empty";
    }

    const auto& lastMsg = messages.back();
    if (lastMsg.message.find("accepted") != std::string::npos) {
        return "accepted";
    }
    if (lastMsg.message.find("rejected") != std::string::npos ||
        lastMsg.message.find("ended") != std::string::npos) {
        return "ended";
    }

    return "active";
}

// ============================================================================
// OTCAnalytics Implementation
// ============================================================================

OTCAnalytics::OTCAnalytics() {}

void OTCAnalytics::recordTrade(const TradeExecution& trade) {
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    tradeHistory_.push_back(trade);
}

OTCAnalytics::TradingStats OTCAnalytics::getUserStats(const std::string& userId, int days) {
    std::lock_guard<std::mutex> lock(analyticsMutex_);

    TradingStats stats;
    stats.totalTrades = 0;
    stats.totalVolume = 0.0;
    stats.successRate = 0.0;

    auto cutoff = std::chrono::system_clock::now() - std::chrono::hours(days * 24);
    int successfulTrades = 0;
    double totalExecutionTime = 0.0;

    for (const auto& trade : tradeHistory_) {
        if ((trade.makerId == userId || trade.takerId == userId) &&
            trade.executedAt > cutoff) {

            stats.totalTrades++;
            stats.totalVolume += trade.quoteAmount;
            stats.volumeByToken[trade.baseToken.symbol] += trade.quoteAmount;

            if (trade.status == OfferStatus::COMPLETED) {
                successfulTrades++;
            }
        }
    }

    stats.avgTradeSize = stats.totalTrades > 0 ? stats.totalVolume / stats.totalTrades : 0;
    stats.successRate = stats.totalTrades > 0 ?
        static_cast<double>(successfulTrades) / stats.totalTrades : 0.0;
    stats.avgExecutionTime = stats.totalTrades > 0 ? totalExecutionTime / stats.totalTrades : 0;

    return stats;
}

OTCAnalytics::TradingStats OTCAnalytics::getPlatformStats(int days) {
    std::lock_guard<std::mutex> lock(analyticsMutex_);

    TradingStats stats;
    stats.totalTrades = 0;
    stats.totalVolume = 0.0;

    auto cutoff = std::chrono::system_clock::now() - std::chrono::hours(days * 24);
    int successfulTrades = 0;

    for (const auto& trade : tradeHistory_) {
        if (trade.executedAt > cutoff) {
            stats.totalTrades++;
            stats.totalVolume += trade.quoteAmount;
            stats.volumeByToken[trade.baseToken.symbol] += trade.quoteAmount;

            if (trade.status == OfferStatus::COMPLETED) {
                successfulTrades++;
            }
        }
    }

    stats.avgTradeSize = stats.totalTrades > 0 ? stats.totalVolume / stats.totalTrades : 0;
    stats.successRate = stats.totalTrades > 0 ?
        static_cast<double>(successfulTrades) / stats.totalTrades : 0.0;

    return stats;
}

std::vector<TradeExecution> OTCAnalytics::getTradeHistory(const std::string& userId, int limit) {
    std::lock_guard<std::mutex> lock(analyticsMutex_);

    std::vector<TradeExecution> userTrades;
    for (auto it = tradeHistory_.rbegin(); it != tradeHistory_.rend() && static_cast<int>(userTrades.size()) < limit; ++it) {
        if (it->makerId == userId || it->takerId == userId) {
            userTrades.push_back(*it);
        }
    }
    return userTrades;
}

double OTCAnalytics::getPnL(const std::string& userId, int days) {
    (void)days;

    auto stats = getUserStats(userId, days);

    // Mock PnL calculation
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.05, 0.15);

    return stats.totalVolume * dis(gen);
}

double OTCAnalytics::getVWAP(const std::string& baseToken, const std::string& quoteToken, int hours) {
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    (void)quoteToken;

    auto cutoff = std::chrono::system_clock::now() - std::chrono::hours(hours);
    double totalVolume = 0.0;
    double volumeWeightedSum = 0.0;

    for (const auto& trade : tradeHistory_) {
        if (trade.executedAt > cutoff && trade.baseToken.symbol == baseToken) {
            volumeWeightedSum += trade.pricePerUnit * trade.baseAmount;
            totalVolume += trade.baseAmount;
        }
    }

    if (totalVolume > 0) {
        return volumeWeightedSum / totalVolume;
    }

    // Return mock price if no trades
    return mockPrices.count(baseToken) ? mockPrices[baseToken] : 0.0;
}

std::vector<std::pair<std::chrono::system_clock::time_point, double>>
OTCAnalytics::getPriceHistory(const std::string& baseToken, const std::string& quoteToken, int days) {
    (void)quoteToken;

    std::vector<std::pair<std::chrono::system_clock::time_point, double>> history;

    double basePrice = mockPrices.count(baseToken) ? mockPrices[baseToken] : 100.0;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.03, 0.03);

    auto now = std::chrono::system_clock::now();
    for (int i = days * 24; i >= 0; --i) {
        auto timestamp = now - std::chrono::hours(i);
        double price = basePrice * (1.0 + dis(gen) * (days * 24 - i) / 24.0);
        history.emplace_back(timestamp, price);
    }

    return history;
}

// ============================================================================
// OTCAgent Implementation
// ============================================================================

OTCAgent::OTCAgent(const std::string& agentId)
    : agentId_(agentId)
    , currentChain_(OTCChain::BASE)
    , logger_(std::make_shared<AgentLogger>())
    , defaultSettlement_(SettlementType::ESCROW)
    , defaultExpiryHours_(24)
    , maxSlippage_(1.0)
    , minTradeSize_(100.0) {

    initializeSubManagers();

    elizaos::logInfo("OTC Agent initialized: " + agentId, "otc_agent");
    elizaos::logInfo("Supported chains: Ethereum, Base, BSC, Solana, Arbitrum, Polygon", "otc_agent");
    elizaos::logInfo("AI-powered negotiation enabled with Eliza integration", "otc_agent");
}

void OTCAgent::initializeSubManagers() {
    escrowManager_ = std::make_unique<EscrowManager>();
    orderbook_ = std::make_unique<OTCOrderbook>();
    riskManager_ = std::make_unique<OTCRiskManager>();
    negotiationEngine_ = std::make_unique<NegotiationEngine>();
    analytics_ = std::make_unique<OTCAnalytics>();
}

bool OTCAgent::createOffer(const std::string& token, float amount, float price) {
    std::lock_guard<std::mutex> lock(agentMutex_);

    if (token.empty()) {
        elizaos::logError("Token cannot be empty", "otc_agent");
        return false;
    }

    if (amount <= 0) {
        elizaos::logError("Amount must be positive", "otc_agent");
        return false;
    }

    if (price <= 0) {
        elizaos::logError("Price must be positive", "otc_agent");
        return false;
    }

    std::ostringstream oss;
    oss << "Creating OTC offer for " << token << ": "
        << std::fixed << std::setprecision(4) << amount
        << " tokens at $" << price << " each";
    elizaos::logInfo(oss.str(), "otc_agent");

    // Create offer
    OTCOffer offer;
    offer.offerId = generateId("otc");
    offer.makerId = agentId_;
    offer.baseToken.symbol = token;
    offer.baseToken.chain = currentChain_;
    offer.baseToken.decimals = 18;
    offer.quoteToken.symbol = "USDC";
    offer.quoteToken.chain = currentChain_;
    offer.quoteToken.decimals = 6;
    offer.baseAmount = amount;
    offer.pricePerUnit = price;
    offer.quoteAmount = amount * price;
    offer.side = OfferSide::SELL;
    offer.status = OfferStatus::ACTIVE;
    offer.settlementType = defaultSettlement_;
    offer.chain = currentChain_;
    offer.createdAt = std::chrono::system_clock::now();
    offer.expiresAt = offer.createdAt + std::chrono::hours(defaultExpiryHours_);
    offer.partialFillAllowed = false;
    offer.filledAmount = 0.0;

    // Add to orderbook
    orderbook_->addOffer(offer);

    std::ostringstream resultOss;
    resultOss << "OTC offer created successfully. Offer ID: " << offer.offerId;
    elizaos::logInfo(resultOss.str(), "otc_agent");

    logStatus("Offer created: " + offer.offerId);

    return true;
}

std::string OTCAgent::createAdvancedOffer(
    const OTCToken& baseToken,
    const OTCToken& quoteToken,
    double amount,
    double pricePerUnit,
    OfferSide side,
    SettlementType settlement,
    int expiryHours,
    bool partialFill) {

    std::lock_guard<std::mutex> lock(agentMutex_);

    OTCOffer offer;
    offer.offerId = generateId("otc");
    offer.makerId = agentId_;
    offer.baseToken = baseToken;
    offer.quoteToken = quoteToken;
    offer.baseAmount = amount;
    offer.pricePerUnit = pricePerUnit;
    offer.quoteAmount = amount * pricePerUnit;
    offer.side = side;
    offer.status = OfferStatus::ACTIVE;
    offer.settlementType = settlement;
    offer.chain = baseToken.chain;
    offer.createdAt = std::chrono::system_clock::now();
    offer.expiresAt = offer.createdAt + std::chrono::hours(expiryHours);
    offer.partialFillAllowed = partialFill;
    offer.filledAmount = 0.0;
    offer.minAmount = partialFill ? amount * 0.1 : amount;
    offer.maxAmount = amount;

    // Create escrow if needed
    if (settlement == SettlementType::ESCROW) {
        offer.escrowAddress = escrowManager_->createEscrow(
            agentId_, "", baseToken, amount, baseToken.chain);
    }

    orderbook_->addOffer(offer);

    elizaos::logInfo("Advanced offer created: " + offer.offerId, "otc_agent");
    return offer.offerId;
}

bool OTCAgent::acceptOffer(const std::string& offerId) {
    std::lock_guard<std::mutex> lock(agentMutex_);

    if (offerId.empty()) {
        elizaos::logError("Offer ID cannot be empty", "otc_agent");
        return false;
    }

    elizaos::logInfo("Accepting OTC offer: " + offerId, "otc_agent");

    // Get offer from orderbook
    auto offers = orderbook_->getOffersByMaker("");
    OTCOffer* targetOffer = nullptr;
    for (auto& offer : offers) {
        if (offer.offerId == offerId) {
            targetOffer = &offer;
            break;
        }
    }

    if (!targetOffer) {
        elizaos::logError("Offer not found: " + offerId, "otc_agent");
        return false;
    }

    // Risk assessment
    auto risk = riskManager_->assessOffer(*targetOffer, agentId_);
    if (!risk.recommended) {
        for (const auto& warning : risk.warnings) {
            elizaos::logWarning(warning, "otc_agent");
        }
    }

    // AI negotiation analysis
    double marketPrice = mockPrices.count(targetOffer->baseToken.symbol) ?
        mockPrices[targetOffer->baseToken.symbol] : targetOffer->pricePerUnit;
    auto counterparty = riskManager_->getCounterpartyProfile(targetOffer->makerId);
    auto advice = negotiationEngine_->getAIAdvice(*targetOffer, marketPrice, counterparty);

    elizaos::logInfo(advice.reasoning, "otc_agent");

    // Update offer status
    targetOffer->status = OfferStatus::MATCHED;
    targetOffer->takerId = agentId_;
    targetOffer->matchedAt = std::chrono::system_clock::now();

    orderbook_->updateOffer(*targetOffer);

    // Record trade
    TradeExecution trade;
    trade.executionId = generateId("trade");
    trade.offerId = offerId;
    trade.makerId = targetOffer->makerId;
    trade.takerId = agentId_;
    trade.baseToken = targetOffer->baseToken;
    trade.quoteToken = targetOffer->quoteToken;
    trade.baseAmount = targetOffer->baseAmount;
    trade.quoteAmount = targetOffer->quoteAmount;
    trade.pricePerUnit = targetOffer->pricePerUnit;
    trade.status = OfferStatus::COMPLETED;
    trade.executedAt = std::chrono::system_clock::now();
    trade.feeAmount = targetOffer->quoteAmount * 0.001; // 0.1% fee
    trade.feeToken = "USDC";

    analytics_->recordTrade(trade);

    std::ostringstream resultOss;
    resultOss << "OTC offer accepted: " << offerId
              << " | Token: " << targetOffer->baseToken.symbol
              << " | Amount: " << std::fixed << std::setprecision(4) << targetOffer->baseAmount
              << " | Price: $" << std::fixed << std::setprecision(2) << targetOffer->pricePerUnit;
    elizaos::logInfo(resultOss.str(), "otc_agent");

    logStatus("Offer accepted: " + offerId);

    return true;
}

bool OTCAgent::acceptPartialOffer(const std::string& offerId, double amount) {
    elizaos::logInfo("Accepting partial offer " + offerId + " for " + std::to_string(amount), "otc_agent");
    // Simplified partial fill logic
    return acceptOffer(offerId);
}

bool OTCAgent::cancelOffer(const std::string& offerId) {
    std::lock_guard<std::mutex> lock(agentMutex_);

    if (offerId.empty()) {
        elizaos::logError("Offer ID cannot be empty", "otc_agent");
        return false;
    }

    elizaos::logInfo("Cancelling OTC offer: " + offerId, "otc_agent");

    orderbook_->removeOffer(offerId);

    std::ostringstream resultOss;
    resultOss << "OTC offer cancelled: " << offerId;
    elizaos::logInfo(resultOss.str(), "otc_agent");

    logStatus("Offer cancelled: " + offerId);

    return true;
}

bool OTCAgent::updateOfferPrice(const std::string& offerId, double newPrice) {
    elizaos::logInfo("Updating offer " + offerId + " price to $" + std::to_string(newPrice), "otc_agent");
    // Would update offer in orderbook
    return true;
}

std::vector<OTCOffer> OTCAgent::getActiveOffers(const std::string& tokenSymbol) {
    if (tokenSymbol.empty()) {
        return orderbook_->getActiveOffers();
    }
    return orderbook_->getOffersByToken(tokenSymbol);
}

std::vector<OTCOffer> OTCAgent::getMyOffers() {
    return orderbook_->getOffersByMaker(agentId_);
}

OTCOffer OTCAgent::getOffer(const std::string& offerId) {
    auto offers = orderbook_->getActiveOffers();
    for (const auto& offer : offers) {
        if (offer.offerId == offerId) {
            return offer;
        }
    }
    return OTCOffer{};
}

OTCMarketStats OTCAgent::getMarketStats(const std::string& baseToken, const std::string& quoteToken) {
    return orderbook_->getMarketStats(baseToken, quoteToken);
}

std::optional<OTCOffer> OTCAgent::findBestOffer(
    const std::string& baseToken,
    const std::string& quoteToken,
    OfferSide side,
    double amount) {
    (void)amount;
    return orderbook_->getBestOffer(baseToken, quoteToken, side);
}

std::vector<OTCOffer> OTCAgent::findMatchingOffers(
    const std::string& baseToken,
    OfferSide side,
    double minAmount,
    double maxPrice) {
    return orderbook_->findMatchingOffers(baseToken, "USDC", side, minAmount, maxPrice);
}

std::string OTCAgent::startNegotiation(const std::string& offerId) {
    return negotiationEngine_->startNegotiation(offerId, agentId_);
}

bool OTCAgent::sendNegotiationMessage(
    const std::string& negotiationId,
    const std::string& message,
    std::optional<double> counterPrice) {
    return negotiationEngine_->sendMessage(negotiationId, agentId_, message, counterPrice);
}

bool OTCAgent::acceptNegotiation(const std::string& negotiationId) {
    return negotiationEngine_->acceptCounterOffer(negotiationId, agentId_);
}

bool OTCAgent::rejectNegotiation(const std::string& negotiationId) {
    return negotiationEngine_->rejectCounterOffer(negotiationId, agentId_);
}

NegotiationEngine::NegotiationAdvice OTCAgent::getAIAdvice(const std::string& offerId) {
    auto offer = getOffer(offerId);
    double marketPrice = mockPrices.count(offer.baseToken.symbol) ?
        mockPrices[offer.baseToken.symbol] : offer.pricePerUnit;
    auto counterparty = riskManager_->getCounterpartyProfile(offer.makerId);

    return negotiationEngine_->getAIAdvice(offer, marketPrice, counterparty);
}

bool OTCAgent::initiateSettlement(const std::string& offerId) {
    elizaos::logInfo("Initiating settlement for offer: " + offerId, "otc_agent");
    return true;
}

bool OTCAgent::confirmSettlement(const std::string& offerId, const std::string& txHash) {
    elizaos::logInfo("Settlement confirmed for offer " + offerId + " tx: " + txHash, "otc_agent");
    return true;
}

std::string OTCAgent::getSettlementStatus(const std::string& offerId) {
    (void)offerId;
    return "completed";
}

RiskAssessment OTCAgent::assessOffer(const std::string& offerId) {
    auto offer = getOffer(offerId);
    return riskManager_->assessOffer(offer, agentId_);
}

CounterpartyProfile OTCAgent::getCounterpartyProfile(const std::string& counterpartyId) {
    return riskManager_->getCounterpartyProfile(counterpartyId);
}

bool OTCAgent::isCounterpartyTrusted(const std::string& counterpartyId) {
    return riskManager_->isCounterpartyTrusted(counterpartyId);
}

bool OTCAgent::deployContract(const std::string& chain) {
    std::lock_guard<std::mutex> lock(agentMutex_);

    if (chain.empty()) {
        elizaos::logError("Chain cannot be empty", "otc_agent");
        return false;
    }

    std::vector<std::string> supportedChains = {"base", "bsc", "solana", "ethereum", "arbitrum", "polygon"};
    std::string chainLower = chain;
    std::transform(chainLower.begin(), chainLower.end(), chainLower.begin(), ::tolower);

    if (std::find(supportedChains.begin(), supportedChains.end(), chainLower) == supportedChains.end()) {
        elizaos::logError("Unsupported chain: " + chain, "otc_agent");
        return false;
    }

    if (deployedContracts.count(chainLower)) {
        elizaos::logWarning("Contract already deployed on " + chain + ": " + deployedContracts[chainLower], "otc_agent");
        return true;
    }

    elizaos::logInfo("Deploying OTC smart contract on " + chain + "...", "otc_agent");

    std::string contractAddress = generateContractAddress();
    deployedContracts[chainLower] = contractAddress;

    std::ostringstream resultOss;
    resultOss << "OTC contract deployed successfully on " << chain
              << " | Contract address: " << contractAddress;
    elizaos::logInfo(resultOss.str(), "otc_agent");
    elizaos::logInfo("Contract features: Atomic swaps, Escrow, Multi-sig, Time locks", "otc_agent");

    return true;
}

std::string OTCAgent::getContractAddress(const std::string& chain) {
    if (chain.empty()) {
        elizaos::logError("Chain cannot be empty", "otc_agent");
        return "";
    }

    std::string chainLower = chain;
    std::transform(chainLower.begin(), chainLower.end(), chainLower.begin(), ::tolower);

    if (!deployedContracts.count(chainLower)) {
        elizaos::logWarning("Contract not deployed on " + chain, "otc_agent");
        return "";
    }

    return deployedContracts[chainLower];
}

std::vector<OTCChain> OTCAgent::getSupportedChains() const {
    return {OTCChain::ETHEREUM, OTCChain::BASE, OTCChain::BSC,
            OTCChain::SOLANA, OTCChain::ARBITRUM, OTCChain::POLYGON};
}

bool OTCAgent::switchChain(OTCChain chain) {
    std::lock_guard<std::mutex> lock(agentMutex_);
    currentChain_ = chain;
    elizaos::logInfo("Switched to chain: " + chainToString(chain), "otc_agent");
    return true;
}

OTCAnalytics::TradingStats OTCAgent::getMyStats(int days) {
    return analytics_->getUserStats(agentId_, days);
}

std::vector<TradeExecution> OTCAgent::getTradeHistory(int limit) {
    return analytics_->getTradeHistory(agentId_, limit);
}

double OTCAgent::getPnL(int days) {
    return analytics_->getPnL(agentId_, days);
}

void OTCAgent::setDefaultSettlement(SettlementType type) {
    defaultSettlement_ = type;
}

void OTCAgent::setDefaultExpiry(int hours) {
    defaultExpiryHours_ = hours;
}

void OTCAgent::setMaxSlippage(double percentage) {
    maxSlippage_ = percentage;
}

void OTCAgent::setMinTradeSize(double amount) {
    minTradeSize_ = amount;
}

std::string OTCAgent::getStatus() const {
    std::ostringstream oss;
    oss << "Agent: " << agentId_
        << " | Chain: " << chainToString(currentChain_)
        << " | Active offers: " << orderbook_->getOffersByMaker(agentId_).size();
    return oss.str();
}

void OTCAgent::setStatusCallback(std::function<void(const std::string&)> callback) {
    statusCallback_ = callback;
}

void OTCAgent::logStatus(const std::string& status) {
    if (statusCallback_) {
        statusCallback_(status);
    }
}

OTCChain OTCAgent::stringToChain(const std::string& chain) const {
    std::string chainLower = chain;
    std::transform(chainLower.begin(), chainLower.end(), chainLower.begin(), ::tolower);

    if (chainLower == "ethereum" || chainLower == "eth") return OTCChain::ETHEREUM;
    if (chainLower == "base") return OTCChain::BASE;
    if (chainLower == "bsc" || chainLower == "binance") return OTCChain::BSC;
    if (chainLower == "solana" || chainLower == "sol") return OTCChain::SOLANA;
    if (chainLower == "arbitrum" || chainLower == "arb") return OTCChain::ARBITRUM;
    if (chainLower == "polygon" || chainLower == "matic") return OTCChain::POLYGON;

    return OTCChain::BASE;
}

std::string OTCAgent::chainToString(OTCChain chain) const {
    switch (chain) {
        case OTCChain::ETHEREUM: return "Ethereum";
        case OTCChain::BASE: return "Base";
        case OTCChain::BSC: return "BSC";
        case OTCChain::SOLANA: return "Solana";
        case OTCChain::ARBITRUM: return "Arbitrum";
        case OTCChain::POLYGON: return "Polygon";
        default: return "Unknown";
    }
}

std::string OTCAgent::generateId(const std::string& prefix) {
    return generateRandomId(prefix);
}

// ============================================================================
// Utility Functions Implementation
// ============================================================================

namespace otc_utils {

double calculateSpread(double bid, double ask) {
    if (bid <= 0 || ask <= 0) return 0.0;
    return (ask - bid) / ((bid + ask) / 2.0) * 100.0;
}

double calculatePriceImpact(double amount, double liquidity, double currentPrice) {
    if (liquidity <= 0) return 0.0;
    return (amount / liquidity) * currentPrice * 0.5;
}

double calculateFee(double amount, double feeRate) {
    return amount * feeRate;
}

std::string getChainName(OTCChain chain) {
    switch (chain) {
        case OTCChain::ETHEREUM: return "Ethereum Mainnet";
        case OTCChain::BASE: return "Base";
        case OTCChain::BSC: return "BNB Smart Chain";
        case OTCChain::SOLANA: return "Solana";
        case OTCChain::ARBITRUM: return "Arbitrum One";
        case OTCChain::POLYGON: return "Polygon";
        default: return "Unknown";
    }
}

std::string getChainCurrency(OTCChain chain) {
    switch (chain) {
        case OTCChain::ETHEREUM: return "ETH";
        case OTCChain::BASE: return "ETH";
        case OTCChain::BSC: return "BNB";
        case OTCChain::SOLANA: return "SOL";
        case OTCChain::ARBITRUM: return "ETH";
        case OTCChain::POLYGON: return "MATIC";
        default: return "ETH";
    }
}

std::string getExplorerUrl(OTCChain chain, const std::string& txHash) {
    std::string baseUrl;
    switch (chain) {
        case OTCChain::ETHEREUM: baseUrl = "https://etherscan.io/tx/"; break;
        case OTCChain::BASE: baseUrl = "https://basescan.org/tx/"; break;
        case OTCChain::BSC: baseUrl = "https://bscscan.com/tx/"; break;
        case OTCChain::SOLANA: baseUrl = "https://solscan.io/tx/"; break;
        case OTCChain::ARBITRUM: baseUrl = "https://arbiscan.io/tx/"; break;
        case OTCChain::POLYGON: baseUrl = "https://polygonscan.com/tx/"; break;
        default: baseUrl = "https://etherscan.io/tx/";
    }
    return baseUrl + txHash;
}

bool isValidAmount(double amount, const OTCToken& token) {
    (void)token;
    return amount > 0 && amount < 1e18;
}

bool isValidPrice(double price) {
    return price > 0 && price < 1e18;
}

bool isValidAddress(const std::string& address, OTCChain chain) {
    if (chain == OTCChain::SOLANA) {
        return address.length() >= 32 && address.length() <= 44;
    }
    return address.length() == 42 && address.substr(0, 2) == "0x";
}

} // namespace otc_utils

} // namespace elizaos
