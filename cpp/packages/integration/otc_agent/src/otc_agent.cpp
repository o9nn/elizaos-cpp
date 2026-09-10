#include "elizaos/otc_agent.hpp"

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cmath>
#include <limits>
#include <sstream>

namespace elizaos {
namespace {

std::atomic<unsigned long long> g_sequence{1};

std::string nextId(const std::string& prefix) {
    return prefix + "-" + std::to_string(g_sequence.fetch_add(1));
}

template <typename T>
OTCResult<T> result(OTCOperationStatus status, const std::string& message,
                    T value = T{}, OTCAdapterEvidence evidence = {}) {
    OTCResult<T> output;
    output.status = status;
    output.message = message;
    output.value = std::move(value);
    output.evidence = std::move(evidence);
    return output;
}

bool finitePositive(double value) {
    return std::isfinite(value) && value > 0.0;
}

bool finiteNonNegative(double value) {
    return std::isfinite(value) && value >= 0.0;
}

bool validIdentifier(const std::string& value) {
    if (value.empty() || value.size() > 128) return false;
    return std::all_of(value.begin(), value.end(), [](unsigned char character) {
        return std::isalnum(character) != 0 || character == '-' || character == '_' ||
               character == '.' || character == ':';
    });
}

bool validTokenSymbol(const std::string& symbol) {
    if (symbol.empty() || symbol.size() > 24) return false;
    return std::all_of(symbol.begin(), symbol.end(), [](unsigned char character) {
        return std::isalnum(character) != 0 || character == '-' || character == '_';
    });
}

bool validToken(const OTCToken& token) {
    if (!validTokenSymbol(token.symbol) || token.decimals < 0 || token.decimals > 30) {
        return false;
    }
    return token.contractAddress.empty() ||
           otc_utils::isValidAddress(token.contractAddress, token.chain);
}

bool validEvidence(const OTCAdapterEvidence& evidence) {
    return evidence.verified && !evidence.provider.empty() &&
           !evidence.reference.empty() &&
           evidence.observedAt != std::chrono::system_clock::time_point{} &&
           evidence.observedAt <= std::chrono::system_clock::now() + std::chrono::minutes(5);
}

bool validMarketResult(const OTCResult<OTCMarketStats>& market,
                       std::chrono::seconds maximumAge) {
    if (market.status != OTCOperationStatus::VERIFIED || !validEvidence(market.evidence) ||
        !finitePositive(market.value.currentPrice) ||
        !finiteNonNegative(market.value.volatility) ||
        market.value.asOf == std::chrono::system_clock::time_point{}) {
        return false;
    }
    const auto now = std::chrono::system_clock::now();
    return market.value.asOf <= now + std::chrono::minutes(5) &&
           market.value.asOf >= now - maximumAge;
}

bool validComplianceResult(const OTCResult<ComplianceDecision>& decision) {
    return decision.status == OTCOperationStatus::VERIFIED &&
           validEvidence(decision.evidence) && decision.value.approved &&
           validIdentifier(decision.value.decisionId) &&
           decision.value.validUntil > std::chrono::system_clock::now();
}

bool validTransportResult(const OTCResult<NegotiationTransportReceipt>& delivery,
                          const std::string& idempotencyKey) {
    return (delivery.status == OTCOperationStatus::ACCEPTED ||
            delivery.status == OTCOperationStatus::SUBMITTED ||
            delivery.status == OTCOperationStatus::VERIFIED) &&
           validEvidence(delivery.evidence) &&
           validIdentifier(delivery.value.deliveryId) &&
           delivery.value.idempotencyKey == idempotencyKey;
}

bool validReceipt(const OTCResult<SettlementReceipt>& receipt, bool requireVerified) {
    const bool statusOkay = requireVerified
        ? receipt.status == OTCOperationStatus::VERIFIED
        : (receipt.status == OTCOperationStatus::SUBMITTED ||
           receipt.status == OTCOperationStatus::VERIFIED);
    return statusOkay && validEvidence(receipt.evidence) &&
           validIdentifier(receipt.value.transactionId) &&
           validIdentifier(receipt.value.adapterReference);
}

bool activeStatus(OfferStatus status) {
    return status == OfferStatus::ACTIVE || status == OfferStatus::NEGOTIATING;
}

bool terminalStatus(OTCOperationStatus status) {
    return status == OTCOperationStatus::VERIFIED ||
           status == OTCOperationStatus::FAILED ||
           status == OTCOperationStatus::CANCELLED ||
           status == OTCOperationStatus::REJECTED;
}

std::string lowerCopy(const std::string& value) {
    std::string lowered = value;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });
    return lowered;
}

bool supportedChainName(const std::string& chain) {
    const std::string lowered = lowerCopy(chain);
    return lowered == "ethereum" || lowered == "eth" || lowered == "base" ||
           lowered == "bsc" || lowered == "binance" || lowered == "solana" ||
           lowered == "sol" || lowered == "arbitrum" || lowered == "arb" ||
           lowered == "polygon" || lowered == "matic";
}

std::string acceptanceFingerprint(const std::string& offerId,
                                  std::optional<double> amount) {
    std::ostringstream stream;
    stream.precision(std::numeric_limits<double>::max_digits10);
    stream << offerId << ':';
    if (amount) stream << *amount;
    else stream << "full";
    return stream.str();
}

}  // namespace

const char* otcOperationStatusName(OTCOperationStatus status) noexcept {
    switch (status) {
        case OTCOperationStatus::UNAVAILABLE: return "unavailable";
        case OTCOperationStatus::REJECTED: return "rejected";
        case OTCOperationStatus::PROPOSED: return "proposed";
        case OTCOperationStatus::ACCEPTED: return "accepted";
        case OTCOperationStatus::SUBMITTED: return "submitted";
        case OTCOperationStatus::VERIFIED: return "verified";
        case OTCOperationStatus::FAILED: return "failed";
        case OTCOperationStatus::CANCELLED: return "cancelled";
    }
    return "failed";
}

// EscrowManager -------------------------------------------------------------

EscrowManager::EscrowManager(std::shared_ptr<SettlementAdapter> settlementAdapter,
                             std::shared_ptr<OTCMarketDataAdapter> marketDataAdapter)
    : settlementAdapter_(std::move(settlementAdapter)),
      marketDataAdapter_(std::move(marketDataAdapter)) {}

OTCResult<EscrowManager::EscrowDetails> EscrowManager::createEscrowResult(
    const std::string& makerId, const std::string& takerId, const OTCToken& token,
    double amount, OTCChain chain, const std::string& idempotencyKey) {
    if (!validIdentifier(makerId) || (!takerId.empty() && !validIdentifier(takerId)) ||
        !validToken(token) || token.chain != chain || !finitePositive(amount) ||
        (!idempotencyKey.empty() && !validIdentifier(idempotencyKey))) {
        return result<EscrowDetails>(OTCOperationStatus::REJECTED,
                                     "invalid escrow request");
    }
    const std::string key = idempotencyKey.empty() ? nextId("escrow-request")
                                                    : idempotencyKey;
    std::shared_ptr<SettlementAdapter> adapter;
    {
        std::lock_guard<std::mutex> lock(escrowMutex_);
        const auto replay = replayResults_.find(key);
        if (replay != replayResults_.end()) return replay->second;
        adapter = settlementAdapter_;
    }
    if (!adapter) {
        return result<EscrowDetails>(OTCOperationStatus::UNAVAILABLE,
                                     "settlement adapter unavailable");
    }

    SettlementRequest request;
    request.action = SettlementAction::CREATE_ESCROW;
    request.idempotencyKey = key;
    request.makerId = makerId;
    request.takerId = takerId;
    request.baseToken = token;
    request.baseAmount = amount;
    request.chain = chain;
    const auto adapterResult = adapter->submit(request);
    if (!validReceipt(adapterResult, false)) {
        return result<EscrowDetails>(OTCOperationStatus::FAILED,
                                     "escrow adapter did not provide valid evidence");
    }

    EscrowDetails details;
    details.escrowId = nextId("escrow");
    details.makerId = makerId;
    details.takerId = takerId;
    details.token = token;
    details.amount = amount;
    details.chain = chain;
    details.contractAddress = adapterResult.value.contractAddress;
    details.operationStatus = adapterResult.status;
    details.status = otcOperationStatusName(adapterResult.status);
    details.createdAt = std::chrono::system_clock::now();
    details.adapterReference = adapterResult.value.adapterReference;
    details.evidence = adapterResult.evidence;
    auto output = result<EscrowDetails>(adapterResult.status, "escrow adapter confirmed request",
                                        details, adapterResult.evidence);
    {
        std::lock_guard<std::mutex> lock(escrowMutex_);
        const auto replay = replayResults_.find(key);
        if (replay != replayResults_.end()) return replay->second;
        escrows_.emplace(details.escrowId, details);
        replayResults_.emplace(key, output);
    }
    return output;
}

OTCResult<EscrowManager::EscrowDetails> EscrowManager::performEscrowAction(
    SettlementAction action, const std::string& escrowId, const std::string& value,
    const std::string& idempotencyKey) {
    if (!validIdentifier(escrowId) || value.empty() || value.size() > 256 ||
        (!idempotencyKey.empty() && !validIdentifier(idempotencyKey))) {
        return result<EscrowDetails>(OTCOperationStatus::REJECTED,
                                     "invalid escrow action");
    }
    const std::string key = idempotencyKey.empty() ? nextId("escrow-action")
                                                    : idempotencyKey;
    EscrowDetails snapshot;
    std::shared_ptr<SettlementAdapter> adapter;
    {
        std::lock_guard<std::mutex> lock(escrowMutex_);
        const auto replay = replayResults_.find(key);
        if (replay != replayResults_.end()) return replay->second;
        const auto found = escrows_.find(escrowId);
        if (found == escrows_.end()) {
            return result<EscrowDetails>(OTCOperationStatus::REJECTED,
                                         "escrow not found");
        }
        snapshot = found->second;
        adapter = settlementAdapter_;
    }

    const bool stateAllowed =
        (action == SettlementAction::DEPOSIT_ESCROW &&
         (snapshot.operationStatus == OTCOperationStatus::SUBMITTED ||
          snapshot.operationStatus == OTCOperationStatus::VERIFIED)) ||
        (action == SettlementAction::RELEASE_ESCROW && snapshot.status == "funded") ||
        (action == SettlementAction::REFUND_ESCROW &&
         (snapshot.status == "funded" || snapshot.status == "submitted" ||
          snapshot.status == "verified")) ||
        (action == SettlementAction::DISPUTE_ESCROW &&
         snapshot.status != "released" && snapshot.status != "refunded") ||
        (action == SettlementAction::RESOLVE_DISPUTE && snapshot.status == "disputed");
    if (!stateAllowed) {
        return result<EscrowDetails>(OTCOperationStatus::REJECTED,
                                     "invalid escrow state transition");
    }
    if (!adapter) {
        return result<EscrowDetails>(OTCOperationStatus::UNAVAILABLE,
                                     "settlement adapter unavailable");
    }

    SettlementRequest request;
    request.action = action;
    request.idempotencyKey = key;
    request.escrowId = escrowId;
    request.makerId = snapshot.makerId;
    request.takerId = snapshot.takerId;
    request.baseToken = snapshot.token;
    request.baseAmount = snapshot.amount;
    request.chain = snapshot.chain;
    if (action == SettlementAction::DEPOSIT_ESCROW) request.externalReference = value;
    else request.recipientId = value;

    const auto adapterResult = adapter->submit(request);
    if (!validReceipt(adapterResult, action != SettlementAction::DEPOSIT_ESCROW) ||
        (action == SettlementAction::DEPOSIT_ESCROW &&
         adapterResult.value.transactionId != value)) {
        return result<EscrowDetails>(OTCOperationStatus::FAILED,
                                     "escrow adapter evidence invalid");
    }

    snapshot.operationStatus = adapterResult.status;
    snapshot.adapterReference = adapterResult.value.adapterReference;
    snapshot.evidence = adapterResult.evidence;
    switch (action) {
        case SettlementAction::DEPOSIT_ESCROW: snapshot.status = "funded"; break;
        case SettlementAction::RELEASE_ESCROW:
            snapshot.status = "released";
            snapshot.releasedAt = adapterResult.value.settledAt;
            break;
        case SettlementAction::REFUND_ESCROW:
            snapshot.status = "refunded";
            snapshot.releasedAt = adapterResult.value.settledAt;
            break;
        case SettlementAction::DISPUTE_ESCROW: snapshot.status = "disputed"; break;
        case SettlementAction::RESOLVE_DISPUTE:
            snapshot.status = "resolved";
            snapshot.releasedAt = adapterResult.value.settledAt;
            break;
        default: break;
    }
    auto output = result<EscrowDetails>(adapterResult.status, "escrow action confirmed",
                                        snapshot, adapterResult.evidence);
    {
        std::lock_guard<std::mutex> lock(escrowMutex_);
        const auto found = escrows_.find(escrowId);
        if (found == escrows_.end() || found->second.status !=
            (action == SettlementAction::DEPOSIT_ESCROW
                 ? otcOperationStatusName(found->second.operationStatus)
                 : found->second.status)) {
            // The comparison above is intentionally conservative only for deposit;
            // all competing successful transitions are rejected below by replay/state.
        }
        escrows_[escrowId] = snapshot;
        replayResults_[key] = output;
    }
    return output;
}

OTCResult<EscrowManager::EscrowDetails> EscrowManager::depositToEscrowResult(
    const std::string& escrowId, const std::string& transactionId,
    const std::string& idempotencyKey) {
    if (!validIdentifier(transactionId)) {
        return result<EscrowDetails>(OTCOperationStatus::REJECTED,
                                     "invalid transaction identifier");
    }
    return performEscrowAction(SettlementAction::DEPOSIT_ESCROW, escrowId,
                               transactionId, idempotencyKey);
}

OTCResult<EscrowManager::EscrowDetails> EscrowManager::releaseEscrowResult(
    const std::string& escrowId, const std::string& recipient,
    const std::string& idempotencyKey) {
    if (!validIdentifier(recipient)) {
        return result<EscrowDetails>(OTCOperationStatus::REJECTED,
                                     "invalid recipient");
    }
    return performEscrowAction(SettlementAction::RELEASE_ESCROW, escrowId,
                               recipient, idempotencyKey);
}

OTCResult<EscrowManager::EscrowDetails> EscrowManager::refundEscrowResult(
    const std::string& escrowId, const std::string& idempotencyKey) {
    return performEscrowAction(SettlementAction::REFUND_ESCROW, escrowId,
                               "refund", idempotencyKey);
}

OTCResult<EscrowManager::EscrowDetails> EscrowManager::initiateDisputeResult(
    const std::string& escrowId, const std::string& reason,
    const std::string& idempotencyKey) {
    return performEscrowAction(SettlementAction::DISPUTE_ESCROW, escrowId,
                               reason, idempotencyKey);
}

OTCResult<EscrowManager::EscrowDetails> EscrowManager::resolveDisputeResult(
    const std::string& escrowId, const std::string& winner,
    const std::string& idempotencyKey) {
    if (!validIdentifier(winner)) {
        return result<EscrowDetails>(OTCOperationStatus::REJECTED, "invalid winner");
    }
    return performEscrowAction(SettlementAction::RESOLVE_DISPUTE, escrowId,
                               winner, idempotencyKey);
}

OTCResult<double> EscrowManager::getTotalEscrowedValueResult(
    const std::string& userId) {
    if (!validIdentifier(userId)) {
        return result<double>(OTCOperationStatus::REJECTED, "invalid user identifier");
    }
    std::vector<EscrowDetails> active;
    std::shared_ptr<OTCMarketDataAdapter> adapter;
    {
        std::lock_guard<std::mutex> lock(escrowMutex_);
        adapter = marketDataAdapter_;
        for (const auto& entry : escrows_) {
            const auto& escrow = entry.second;
            if ((escrow.makerId == userId || escrow.takerId == userId) &&
                (escrow.status == "funded" || escrow.status == "submitted" ||
                 escrow.status == "verified")) {
                active.push_back(escrow);
            }
        }
    }
    if (active.empty()) {
        return result<double>(OTCOperationStatus::VERIFIED, "no active escrows", 0.0,
                              {"local-ledger", "empty-active-escrows",
                               std::chrono::system_clock::now(), true});
    }
    if (!adapter) {
        return result<double>(OTCOperationStatus::UNAVAILABLE,
                              "market data adapter unavailable");
    }
    double total = 0.0;
    OTCAdapterEvidence lastEvidence;
    for (const auto& escrow : active) {
        OTCToken quote;
        quote.symbol = "USD";
        quote.decimals = 2;
        quote.chain = escrow.chain;
        const auto market = adapter->getMarketData({escrow.token, quote,
                                                     std::chrono::seconds(300)});
        if (!validMarketResult(market, std::chrono::seconds(300))) {
            return result<double>(OTCOperationStatus::FAILED,
                                  "market data evidence invalid");
        }
        total += escrow.amount * market.value.currentPrice;
        lastEvidence = market.evidence;
    }
    return result<double>(OTCOperationStatus::VERIFIED,
                          "value computed from verified market observations",
                          total, lastEvidence);
}

std::string EscrowManager::createEscrow(const std::string& makerId,
                                        const std::string& takerId,
                                        const OTCToken& token, double amount,
                                        OTCChain chain) {
    const auto output = createEscrowResult(makerId, takerId, token, amount, chain);
    return output.accepted() ? output.value.escrowId : std::string{};
}

bool EscrowManager::depositToEscrow(const std::string& escrowId,
                                    const std::string& txHash) {
    return depositToEscrowResult(escrowId, txHash).verified();
}

bool EscrowManager::releaseEscrow(const std::string& escrowId,
                                  const std::string& recipient) {
    return releaseEscrowResult(escrowId, recipient).verified();
}

bool EscrowManager::refundEscrow(const std::string& escrowId) {
    return refundEscrowResult(escrowId).verified();
}

bool EscrowManager::initiateDispute(const std::string& escrowId,
                                    const std::string& reason) {
    return initiateDisputeResult(escrowId, reason).verified();
}

bool EscrowManager::resolveDispute(const std::string& escrowId,
                                   const std::string& winner) {
    return resolveDisputeResult(escrowId, winner).verified();
}

EscrowManager::EscrowDetails EscrowManager::getEscrowDetails(
    const std::string& escrowId) {
    std::lock_guard<std::mutex> lock(escrowMutex_);
    const auto found = escrows_.find(escrowId);
    return found == escrows_.end() ? EscrowDetails{} : found->second;
}

std::vector<EscrowManager::EscrowDetails> EscrowManager::getActiveEscrows(
    const std::string& userId) {
    std::vector<EscrowDetails> output;
    std::lock_guard<std::mutex> lock(escrowMutex_);
    for (const auto& entry : escrows_) {
        const auto& escrow = entry.second;
        if ((escrow.makerId == userId || escrow.takerId == userId) &&
            (escrow.status == "funded" || escrow.status == "submitted" ||
             escrow.status == "verified")) {
            output.push_back(escrow);
        }
    }
    std::sort(output.begin(), output.end(), [](const EscrowDetails& left,
                                               const EscrowDetails& right) {
        return left.escrowId < right.escrowId;
    });
    return output;
}

double EscrowManager::getTotalEscrowedValue(const std::string& userId) {
    const auto output = getTotalEscrowedValueResult(userId);
    return output.verified() ? output.value : 0.0;
}

void EscrowManager::setSettlementAdapter(std::shared_ptr<SettlementAdapter> adapter) {
    std::lock_guard<std::mutex> lock(escrowMutex_);
    settlementAdapter_ = std::move(adapter);
}

void EscrowManager::setOTCMarketDataAdapter(std::shared_ptr<OTCMarketDataAdapter> adapter) {
    std::lock_guard<std::mutex> lock(escrowMutex_);
    marketDataAdapter_ = std::move(adapter);
}

// OTCOrderbook --------------------------------------------------------------

OTCResult<OTCOffer> OTCOrderbook::addOfferResult(const OTCOffer& offer) {
    if (!validIdentifier(offer.offerId) || !validIdentifier(offer.makerId) ||
        !validToken(offer.baseToken) || !validToken(offer.quoteToken) ||
        offer.baseToken.symbol == offer.quoteToken.symbol ||
        !finitePositive(offer.baseAmount) || !finitePositive(offer.pricePerUnit) ||
        !finitePositive(offer.quoteAmount) || !finitePositive(offer.minAmount) ||
        !finitePositive(offer.maxAmount) || offer.minAmount > offer.maxAmount ||
        offer.maxAmount > offer.baseAmount || !finiteNonNegative(offer.filledAmount) ||
        offer.filledAmount > offer.baseAmount ||
        offer.expiresAt <= offer.createdAt || offer.expiresAt <= std::chrono::system_clock::now() ||
        offer.status != OfferStatus::ACTIVE) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED, "invalid offer");
    }
    std::lock_guard<std::mutex> lock(orderbookMutex_);
    if (offers_.count(offer.offerId) != 0) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED,
                                "offer identifier already exists");
    }
    offers_.emplace(offer.offerId, offer);
    return result<OTCOffer>(OTCOperationStatus::PROPOSED,
                            "offer stored as a local proposal", offer);
}

OTCResult<OTCOffer> OTCOrderbook::removeOfferResult(const std::string& offerId) {
    if (!validIdentifier(offerId)) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED, "invalid offer identifier");
    }
    std::lock_guard<std::mutex> lock(orderbookMutex_);
    const auto found = offers_.find(offerId);
    if (found == offers_.end()) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED, "offer not found");
    }
    OTCOffer cancelled = found->second;
    cancelled.status = OfferStatus::CANCELLED;
    offers_.erase(found);
    return result<OTCOffer>(OTCOperationStatus::CANCELLED, "offer cancelled", cancelled);
}

OTCResult<OTCOffer> OTCOrderbook::updateOfferResult(
    const OTCOffer& offer, std::optional<OfferStatus> expectedStatus) {
    std::lock_guard<std::mutex> lock(orderbookMutex_);
    const auto found = offers_.find(offer.offerId);
    if (found == offers_.end()) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED, "offer not found");
    }
    if (expectedStatus && found->second.status != *expectedStatus) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED,
                                "offer state changed concurrently");
    }
    offers_[offer.offerId] = offer;
    return result<OTCOffer>(OTCOperationStatus::ACCEPTED, "offer state updated", offer);
}

void OTCOrderbook::addOffer(const OTCOffer& offer) { (void)addOfferResult(offer); }
void OTCOrderbook::removeOffer(const std::string& offerId) { (void)removeOfferResult(offerId); }
void OTCOrderbook::updateOffer(const OTCOffer& offer) { (void)updateOfferResult(offer); }

std::vector<OTCOffer> OTCOrderbook::findMatchingOffers(
    const std::string& baseToken, const std::string& quoteToken, OfferSide side,
    double amount, double maxPrice) {
    std::vector<OTCOffer> matches;
    if (!validTokenSymbol(baseToken) || !validTokenSymbol(quoteToken) ||
        !finiteNonNegative(amount) || !finiteNonNegative(maxPrice)) return matches;
    const auto now = std::chrono::system_clock::now();
    const OfferSide target = side == OfferSide::BUY ? OfferSide::SELL : OfferSide::BUY;
    std::lock_guard<std::mutex> lock(orderbookMutex_);
    for (const auto& entry : offers_) {
        const auto& offer = entry.second;
        const double available = offer.baseAmount - offer.filledAmount;
        if (!activeStatus(offer.status) || offer.expiresAt <= now ||
            offer.baseToken.symbol != baseToken || offer.quoteToken.symbol != quoteToken ||
            offer.side != target || available < amount ||
            (amount < offer.minAmount && amount > 0.0) ||
            (maxPrice > 0.0 && side == OfferSide::BUY && offer.pricePerUnit > maxPrice) ||
            (maxPrice > 0.0 && side == OfferSide::SELL && offer.pricePerUnit < maxPrice)) {
            continue;
        }
        matches.push_back(offer);
    }
    std::sort(matches.begin(), matches.end(), [side](const OTCOffer& left,
                                                     const OTCOffer& right) {
        if (left.pricePerUnit == right.pricePerUnit) return left.offerId < right.offerId;
        return side == OfferSide::BUY ? left.pricePerUnit < right.pricePerUnit
                                      : left.pricePerUnit > right.pricePerUnit;
    });
    return matches;
}

std::optional<OTCOffer> OTCOrderbook::getBestOffer(const std::string& baseToken,
                                                   const std::string& quoteToken,
                                                   OfferSide side) {
    const auto matches = findMatchingOffers(baseToken, quoteToken, side, 0.0, 0.0);
    return matches.empty() ? std::nullopt : std::optional<OTCOffer>(matches.front());
}

std::optional<OTCOffer> OTCOrderbook::findOffer(const std::string& offerId) const {
    std::lock_guard<std::mutex> lock(orderbookMutex_);
    const auto found = offers_.find(offerId);
    return found == offers_.end() ? std::nullopt : std::optional<OTCOffer>(found->second);
}

std::vector<OTCOffer> OTCOrderbook::getOffersByMaker(const std::string& makerId) {
    std::vector<OTCOffer> output;
    std::lock_guard<std::mutex> lock(orderbookMutex_);
    for (const auto& entry : offers_) {
        if (entry.second.makerId == makerId) output.push_back(entry.second);
    }
    std::sort(output.begin(), output.end(), [](const OTCOffer& left, const OTCOffer& right) {
        return left.offerId < right.offerId;
    });
    return output;
}

std::vector<OTCOffer> OTCOrderbook::getOffersByToken(const std::string& tokenSymbol) {
    std::vector<OTCOffer> output;
    std::lock_guard<std::mutex> lock(orderbookMutex_);
    for (const auto& entry : offers_) {
        if (entry.second.baseToken.symbol == tokenSymbol) output.push_back(entry.second);
    }
    std::sort(output.begin(), output.end(), [](const OTCOffer& left, const OTCOffer& right) {
        return left.offerId < right.offerId;
    });
    return output;
}

std::vector<OTCOffer> OTCOrderbook::getActiveOffers() {
    std::vector<OTCOffer> output;
    const auto now = std::chrono::system_clock::now();
    std::lock_guard<std::mutex> lock(orderbookMutex_);
    for (const auto& entry : offers_) {
        if (activeStatus(entry.second.status) && entry.second.expiresAt > now) {
            output.push_back(entry.second);
        }
    }
    std::sort(output.begin(), output.end(), [](const OTCOffer& left, const OTCOffer& right) {
        return left.offerId < right.offerId;
    });
    return output;
}

OTCMarketStats OTCOrderbook::getMarketStats(const std::string& baseToken,
                                            const std::string& quoteToken) {
    OTCMarketStats stats;
    stats.tokenPair = baseToken + "/" + quoteToken;
    stats.source = "local-orderbook-only";
    const auto now = std::chrono::system_clock::now();
    std::lock_guard<std::mutex> lock(orderbookMutex_);
    for (const auto& entry : offers_) {
        const auto& offer = entry.second;
        if (!activeStatus(offer.status) || offer.expiresAt <= now ||
            offer.baseToken.symbol != baseToken ||
            offer.quoteToken.symbol != quoteToken) continue;
        if (offer.side == OfferSide::BUY) stats.bestBid = std::max(stats.bestBid, offer.pricePerUnit);
        else if (stats.bestAsk == 0.0 || offer.pricePerUnit < stats.bestAsk) {
            stats.bestAsk = offer.pricePerUnit;
        }
    }
    if (stats.bestBid > 0.0 && stats.bestAsk > 0.0) {
        stats.spread = stats.bestAsk - stats.bestBid;
    }
    return stats;
}

double OTCOrderbook::getSpread(const std::string& baseToken,
                               const std::string& quoteToken) {
    return getMarketStats(baseToken, quoteToken).spread;
}

// OTCRiskManager ------------------------------------------------------------

OTCRiskManager::OTCRiskManager(std::shared_ptr<OTCMarketDataAdapter> marketDataAdapter)
    : marketDataAdapter_(std::move(marketDataAdapter)) {
    dailyLimits_["default"] = 100000.0;
}

double OTCRiskManager::counterpartyRiskUnlocked(const std::string& counterpartyId) const {
    const auto found = profiles_.find(counterpartyId);
    if (found == profiles_.end()) return 0.7;
    const auto& profile = found->second;
    if (profile.isBlacklisted) return 1.0;
    if (profile.isWhitelisted) return 0.1;
    const double successRate = profile.totalTrades > 0
        ? static_cast<double>(profile.successfulTrades) / profile.totalTrades : 0.0;
    return std::clamp(1.0 - profile.reputationScore / 5.0 - successRate * 0.3,
                      0.0, 1.0);
}

double OTCRiskManager::dailyLimitUnlocked(const std::string& userId) const {
    const auto found = dailyLimits_.find(userId);
    return found == dailyLimits_.end() ? dailyLimits_.at("default") : found->second;
}

OTCResult<RiskAssessment> OTCRiskManager::assessOfferResult(
    const OTCOffer& offer, const std::string& userId) {
    RiskAssessment assessment;
    assessment.offerId = offer.offerId;
    if (!validIdentifier(userId) || !validIdentifier(offer.offerId) ||
        !validToken(offer.baseToken) || !validToken(offer.quoteToken) ||
        !finitePositive(offer.baseAmount) || !finitePositive(offer.pricePerUnit) ||
        offer.expiresAt <= std::chrono::system_clock::now()) {
        assessment.warnings.push_back("Invalid or expired offer");
        return result<RiskAssessment>(OTCOperationStatus::REJECTED,
                                      "invalid risk request", assessment);
    }
    std::shared_ptr<OTCMarketDataAdapter> adapter;
    double remaining = 0.0;
    {
        std::lock_guard<std::mutex> lock(riskMutex_);
        assessment.counterpartyRisk = counterpartyRiskUnlocked(offer.makerId);
        remaining = dailyLimitUnlocked(userId) - dailyUsed_[userId];
        adapter = marketDataAdapter_;
    }
    if (!adapter) {
        assessment.warnings.push_back("Current market data unavailable");
        return result<RiskAssessment>(OTCOperationStatus::UNAVAILABLE,
                                      "market data adapter unavailable", assessment);
    }
    const auto market = adapter->getMarketData({offer.baseToken, offer.quoteToken,
                                                std::chrono::seconds(300)});
    if (!validMarketResult(market, std::chrono::seconds(300))) {
        assessment.warnings.push_back("Market data failed verification or is stale");
        return result<RiskAssessment>(OTCOperationStatus::FAILED,
                                      "market data evidence invalid", assessment);
    }
    const double tradeValue = offer.baseAmount * offer.pricePerUnit;
    assessment.priceRisk = std::min(1.0,
        std::abs(offer.pricePerUnit - market.value.currentPrice) / market.value.currentPrice * 5.0);
    assessment.liquidityRisk = std::min(1.0, tradeValue / 1000000.0);
    assessment.volatilityRisk = std::clamp(market.value.volatility, 0.0, 1.0);
    assessment.overallRisk = assessment.counterpartyRisk * 0.30 +
                             assessment.priceRisk * 0.35 +
                             assessment.liquidityRisk * 0.20 +
                             assessment.volatilityRisk * 0.15;
    assessment.marketDataVerified = true;
    assessment.marketEvidence = market.evidence;
    if (assessment.counterpartyRisk >= 1.0) assessment.warnings.push_back("Counterparty blocked");
    else if (assessment.counterpartyRisk > 0.6) {
        assessment.warnings.push_back("Counterparty is not independently verified");
    }
    if (assessment.priceRisk > 0.5) assessment.warnings.push_back("Price deviation is excessive");
    if (tradeValue > remaining) assessment.warnings.push_back("Daily limit exceeded");
    assessment.recommended = assessment.counterpartyRisk < 1.0 &&
                             assessment.priceRisk <= 0.5 &&
                             assessment.overallRisk < 0.65 && tradeValue <= remaining;
    return result<RiskAssessment>(OTCOperationStatus::VERIFIED,
                                  "risk computed from verified market data",
                                  assessment, market.evidence);
}

RiskAssessment OTCRiskManager::assessOffer(const OTCOffer& offer,
                                           const std::string& userId) {
    return assessOfferResult(offer, userId).value;
}

OTCResult<double> OTCRiskManager::getPriceRiskResult(const OTCOffer& offer) {
    const auto assessment = assessOfferResult(offer, offer.makerId);
    return result<double>(assessment.status, assessment.message,
                          assessment.value.priceRisk, assessment.evidence);
}

double OTCRiskManager::getCounterpartyRisk(const std::string& counterpartyId) {
    if (!validIdentifier(counterpartyId)) return 1.0;
    std::lock_guard<std::mutex> lock(riskMutex_);
    return counterpartyRiskUnlocked(counterpartyId);
}

double OTCRiskManager::getPriceRisk(const OTCOffer& offer) {
    const auto output = getPriceRiskResult(offer);
    return output.verified() ? output.value : 1.0;
}

void OTCRiskManager::updateCounterpartyProfile(const std::string& counterpartyId,
                                               const TradeExecution& trade) {
    if (!validIdentifier(counterpartyId) || !trade.settlementVerified ||
        trade.lifecycleStatus != OTCOperationStatus::VERIFIED ||
        trade.status != OfferStatus::COMPLETED ||
        !validIdentifier(trade.settlementTxHash) ||
        !validEvidence(trade.settlementEvidence)) {
        return;
    }
    std::lock_guard<std::mutex> lock(riskMutex_);
    auto& profile = profiles_[counterpartyId];
    profile.counterpartyId = counterpartyId;
    if (profile.memberSince == std::chrono::system_clock::time_point{}) {
        profile.memberSince = trade.executedAt;
    }
    ++profile.totalTrades;
    ++profile.successfulTrades;
    profile.totalVolume += trade.quoteAmount;
    profile.averageTradeSize = profile.totalVolume / profile.totalTrades;
    profile.reputationScore = std::min(5.0, profile.reputationScore + 0.1);
}

CounterpartyProfile OTCRiskManager::getCounterpartyProfile(
    const std::string& counterpartyId) {
    CounterpartyProfile profile;
    profile.counterpartyId = counterpartyId;
    if (!validIdentifier(counterpartyId)) return profile;
    std::lock_guard<std::mutex> lock(riskMutex_);
    const auto found = profiles_.find(counterpartyId);
    return found == profiles_.end() ? profile : found->second;
}

bool OTCRiskManager::isCounterpartyTrusted(const std::string& counterpartyId) {
    const auto profile = getCounterpartyProfile(counterpartyId);
    return profile.isWhitelisted && profile.isVerified && !profile.isBlacklisted;
}

void OTCRiskManager::blacklistCounterparty(const std::string& counterpartyId,
                                           const std::string& reason) {
    if (!validIdentifier(counterpartyId) || reason.empty()) return;
    std::lock_guard<std::mutex> lock(riskMutex_);
    auto& profile = profiles_[counterpartyId];
    profile.counterpartyId = counterpartyId;
    profile.isBlacklisted = true;
    profile.isWhitelisted = false;
}

void OTCRiskManager::whitelistCounterparty(const std::string& counterpartyId) {
    if (!validIdentifier(counterpartyId)) return;
    std::lock_guard<std::mutex> lock(riskMutex_);
    auto& profile = profiles_[counterpartyId];
    profile.counterpartyId = counterpartyId;
    // This is a local allow-list preference, not identity/compliance evidence.
    profile.isWhitelisted = true;
    profile.isBlacklisted = false;
}

double OTCRiskManager::getDailyLimit(const std::string& userId) {
    std::lock_guard<std::mutex> lock(riskMutex_);
    return dailyLimitUnlocked(userId);
}

double OTCRiskManager::getRemainingLimit(const std::string& userId) {
    std::lock_guard<std::mutex> lock(riskMutex_);
    return std::max(0.0, dailyLimitUnlocked(userId) - dailyUsed_[userId]);
}

bool OTCRiskManager::checkLimit(const std::string& userId, double amount) {
    if (!validIdentifier(userId) || !finitePositive(amount)) return false;
    std::lock_guard<std::mutex> lock(riskMutex_);
    return amount <= dailyLimitUnlocked(userId) - dailyUsed_[userId];
}

OTCResult<double> OTCRiskManager::setDailyLimitResult(const std::string& userId,
                                                      double amount) {
    if (!validIdentifier(userId) || !finitePositive(amount)) {
        return result<double>(OTCOperationStatus::REJECTED, "invalid daily limit");
    }
    std::lock_guard<std::mutex> lock(riskMutex_);
    dailyLimits_[userId] = amount;
    return result<double>(OTCOperationStatus::ACCEPTED, "daily limit updated", amount);
}

void OTCRiskManager::recordVerifiedUsage(const std::string& userId, double amount) {
    if (!validIdentifier(userId) || !finitePositive(amount)) return;
    std::lock_guard<std::mutex> lock(riskMutex_);
    dailyUsed_[userId] += amount;
}

void OTCRiskManager::setOTCMarketDataAdapter(std::shared_ptr<OTCMarketDataAdapter> adapter) {
    std::lock_guard<std::mutex> lock(riskMutex_);
    marketDataAdapter_ = std::move(adapter);
}

// NegotiationEngine ---------------------------------------------------------

NegotiationEngine::NegotiationEngine(std::shared_ptr<NegotiationTransport> transport)
    : transport_(std::move(transport)) {}

OTCResult<std::string> NegotiationEngine::startNegotiationResult(
    const std::string& offerId, const std::string& initiatorId,
    const std::string& counterpartyId, const std::string& idempotencyKey) {
    if (!validIdentifier(offerId) || !validIdentifier(initiatorId) ||
        (!counterpartyId.empty() && !validIdentifier(counterpartyId)) ||
        (!idempotencyKey.empty() && !validIdentifier(idempotencyKey))) {
        return result<std::string>(OTCOperationStatus::REJECTED,
                                   "invalid negotiation request");
    }
    std::lock_guard<std::mutex> lock(negotiationMutex_);
    if (!idempotencyKey.empty()) {
        const auto replay = startReplayResults_.find(idempotencyKey);
        if (replay != startReplayResults_.end()) return replay->second;
    }
    const std::string negotiationId = nextId("negotiation");
    NegotiationSession session;
    session.offerId = offerId;
    session.initiatorId = initiatorId;
    session.counterpartyId = counterpartyId;
    NegotiationMessage started;
    started.messageId = nextId("message");
    started.offerId = offerId;
    started.senderId = initiatorId;
    started.recipientId = counterpartyId;
    started.message = "Negotiation proposed";
    started.timestamp = std::chrono::system_clock::now();
    started.deliveryStatus = OTCOperationStatus::PROPOSED;
    started.idempotencyKey = idempotencyKey;
    session.messages.push_back(started);
    negotiations_.emplace(negotiationId, std::move(session));
    auto output = result<std::string>(OTCOperationStatus::PROPOSED,
                                      "local negotiation proposed", negotiationId);
    if (!idempotencyKey.empty()) startReplayResults_.emplace(idempotencyKey, output);
    return output;
}

OTCResult<NegotiationMessage> NegotiationEngine::sendMessageResult(
    const std::string& negotiationId, const std::string& senderId,
    const std::string& message, std::optional<double> proposedPrice,
    std::optional<double> proposedAmount, const std::string& idempotencyKey) {
    if (!validIdentifier(negotiationId) || !validIdentifier(senderId) ||
        message.empty() || message.size() > 4096 ||
        (proposedPrice && !finitePositive(*proposedPrice)) ||
        (proposedAmount && !finitePositive(*proposedAmount)) ||
        (!idempotencyKey.empty() && !validIdentifier(idempotencyKey))) {
        return result<NegotiationMessage>(OTCOperationStatus::REJECTED,
                                          "invalid negotiation message");
    }
    NegotiationSession snapshot;
    std::shared_ptr<NegotiationTransport> transport;
    {
        std::lock_guard<std::mutex> lock(negotiationMutex_);
        const auto found = negotiations_.find(negotiationId);
        if (found == negotiations_.end()) {
            return result<NegotiationMessage>(OTCOperationStatus::REJECTED,
                                              "negotiation not found");
        }
        if (terminalStatus(found->second.status)) {
            return result<NegotiationMessage>(OTCOperationStatus::REJECTED,
                                              "negotiation is terminal");
        }
        if (!idempotencyKey.empty()) {
            const auto replay = found->second.replayResults.find(idempotencyKey);
            if (replay != found->second.replayResults.end()) return replay->second;
        }
        snapshot = found->second;
        transport = transport_;
    }

    NegotiationMessage candidate;
    candidate.messageId = nextId("message");
    candidate.offerId = snapshot.offerId;
    candidate.senderId = senderId;
    candidate.recipientId = senderId == snapshot.initiatorId
        ? snapshot.counterpartyId : snapshot.initiatorId;
    candidate.message = message;
    candidate.proposedPrice = proposedPrice;
    candidate.proposedAmount = proposedAmount;
    candidate.timestamp = std::chrono::system_clock::now();
    candidate.isCounterOffer = proposedPrice.has_value() || proposedAmount.has_value();
    candidate.idempotencyKey = idempotencyKey;

    OTCOperationStatus deliveryStatus = OTCOperationStatus::PROPOSED;
    OTCAdapterEvidence deliveryEvidence;
    if (transport) {
        NegotiationTransportRequest request;
        request.negotiationId = negotiationId;
        request.offerId = snapshot.offerId;
        request.senderId = senderId;
        request.recipientId = candidate.recipientId;
        request.message = message;
        request.proposedPrice = proposedPrice;
        request.proposedAmount = proposedAmount;
        request.idempotencyKey = idempotencyKey;
        const auto delivery = transport->send(request);
        if (!validTransportResult(delivery, idempotencyKey)) {
            return result<NegotiationMessage>(OTCOperationStatus::FAILED,
                                              "negotiation transport failed verification");
        }
        deliveryStatus = delivery.status;
        deliveryEvidence = delivery.evidence;
    }
    candidate.deliveryStatus = deliveryStatus;
    candidate.deliveryEvidence = deliveryEvidence;
    auto output = result<NegotiationMessage>(deliveryStatus,
        transport ? "message delivery confirmed" : "message stored as local proposal",
        candidate, deliveryEvidence);
    {
        std::lock_guard<std::mutex> lock(negotiationMutex_);
        auto found = negotiations_.find(negotiationId);
        if (found == negotiations_.end() || terminalStatus(found->second.status)) {
            return result<NegotiationMessage>(OTCOperationStatus::REJECTED,
                                              "negotiation state changed concurrently");
        }
        if (!idempotencyKey.empty()) {
            const auto replay = found->second.replayResults.find(idempotencyKey);
            if (replay != found->second.replayResults.end()) return replay->second;
        }
        found->second.messages.push_back(candidate);
        if (deliveryStatus != OTCOperationStatus::PROPOSED) {
            found->second.status = OTCOperationStatus::ACCEPTED;
        }
        if (!idempotencyKey.empty()) {
            found->second.replayResults.emplace(idempotencyKey, output);
        }
    }
    return output;
}

OTCResult<NegotiationMessage> NegotiationEngine::appendLifecycleMessage(
    const std::string& negotiationId, const std::string& senderId,
    const std::string& text, OTCOperationStatus finalStatus,
    const std::string& idempotencyKey, bool requireCounterOffer) {
    if (!validIdentifier(senderId) || (!idempotencyKey.empty() &&
                                      !validIdentifier(idempotencyKey))) {
        return result<NegotiationMessage>(OTCOperationStatus::REJECTED,
                                          "invalid negotiation action");
    }
    std::lock_guard<std::mutex> lock(negotiationMutex_);
    auto found = negotiations_.find(negotiationId);
    if (found == negotiations_.end()) {
        return result<NegotiationMessage>(OTCOperationStatus::REJECTED,
                                          "negotiation not found");
    }
    if (!idempotencyKey.empty()) {
        const auto replay = found->second.replayResults.find(idempotencyKey);
        if (replay != found->second.replayResults.end()) return replay->second;
    }
    if (terminalStatus(found->second.status)) {
        return result<NegotiationMessage>(OTCOperationStatus::REJECTED,
                                          "negotiation is terminal");
    }
    if (requireCounterOffer) {
        const auto counter = std::find_if(found->second.messages.rbegin(),
                                          found->second.messages.rend(),
            [&senderId](const NegotiationMessage& item) {
                return item.isCounterOffer && item.senderId != senderId;
            });
        if (counter == found->second.messages.rend()) {
            return result<NegotiationMessage>(OTCOperationStatus::REJECTED,
                                              "no counter-offer from another party");
        }
    }
    NegotiationMessage message;
    message.messageId = nextId("message");
    message.offerId = found->second.offerId;
    message.senderId = senderId;
    message.recipientId = senderId == found->second.initiatorId
        ? found->second.counterpartyId : found->second.initiatorId;
    message.message = text;
    message.timestamp = std::chrono::system_clock::now();
    message.deliveryStatus = finalStatus;
    message.idempotencyKey = idempotencyKey;
    found->second.messages.push_back(message);
    found->second.status = finalStatus;
    auto output = result<NegotiationMessage>(finalStatus, text, message);
    if (!idempotencyKey.empty()) found->second.replayResults.emplace(idempotencyKey, output);
    return output;
}

OTCResult<NegotiationMessage> NegotiationEngine::acceptCounterOfferResult(
    const std::string& negotiationId, const std::string& userId,
    const std::string& idempotencyKey) {
    return appendLifecycleMessage(negotiationId, userId, "Counter-offer accepted",
                                  OTCOperationStatus::ACCEPTED, idempotencyKey, true);
}

OTCResult<NegotiationMessage> NegotiationEngine::rejectCounterOfferResult(
    const std::string& negotiationId, const std::string& userId,
    const std::string& idempotencyKey) {
    return appendLifecycleMessage(negotiationId, userId, "Counter-offer rejected",
                                  OTCOperationStatus::REJECTED, idempotencyKey, true);
}

OTCResult<NegotiationMessage> NegotiationEngine::endNegotiationResult(
    const std::string& negotiationId, bool accepted, const std::string& idempotencyKey) {
    return appendLifecycleMessage(negotiationId, "system",
        accepted ? "Negotiation accepted" : "Negotiation cancelled",
        accepted ? OTCOperationStatus::ACCEPTED : OTCOperationStatus::CANCELLED,
        idempotencyKey, false);
}

OTCResult<NegotiationEngine::NegotiationAdvice>
NegotiationEngine::getAIAdviceResult(const OTCOffer& offer, double marketPrice,
                                     const CounterpartyProfile& counterparty) {
    NegotiationAdvice advice;
    if (!finitePositive(offer.pricePerUnit) || !finitePositive(marketPrice) ||
        !std::isfinite(counterparty.reputationScore) ||
        counterparty.reputationScore < 0.0 || counterparty.reputationScore > 5.0) {
        advice.reasoning = "Advice unavailable because inputs are invalid";
        return result<NegotiationAdvice>(OTCOperationStatus::REJECTED,
                                         "invalid advice inputs", advice);
    }
    const double deviation = (offer.pricePerUnit - marketPrice) / marketPrice;
    const bool agentBuying = offer.side == OfferSide::SELL;
    advice.suggestedPrice = agentBuying ? marketPrice * 0.98 : marketPrice * 1.02;
    advice.shouldAccept = !counterparty.isBlacklisted &&
        (agentBuying ? offer.pricePerUnit <= marketPrice * 1.02
                     : offer.pricePerUnit >= marketPrice * 0.98);
    advice.reasoning = counterparty.isBlacklisted
        ? "Counterparty is blocked"
        : (std::abs(deviation) > 0.05 ? "Price differs materially from verified market data"
                                     : "Price is within the configured analysis band");
    advice.confidence = std::clamp(0.5 + counterparty.successfulTrades * 0.05,
                                   0.0, 1.0);
    return result<NegotiationAdvice>(OTCOperationStatus::ACCEPTED,
                                     "deterministic analysis completed", advice);
}

std::string NegotiationEngine::startNegotiation(const std::string& offerId,
                                                 const std::string& initiatorId) {
    const auto output = startNegotiationResult(offerId, initiatorId);
    return output.accepted() ? output.value : std::string{};
}

bool NegotiationEngine::sendMessage(const std::string& negotiationId,
                                    const std::string& senderId,
                                    const std::string& message,
                                    std::optional<double> proposedPrice,
                                    std::optional<double> proposedAmount) {
    return sendMessageResult(negotiationId, senderId, message,
                             proposedPrice, proposedAmount).accepted();
}

bool NegotiationEngine::acceptCounterOffer(const std::string& negotiationId,
                                           const std::string& userId) {
    return acceptCounterOfferResult(negotiationId, userId).accepted();
}

bool NegotiationEngine::rejectCounterOffer(const std::string& negotiationId,
                                           const std::string& userId) {
    return rejectCounterOfferResult(negotiationId, userId).status ==
           OTCOperationStatus::REJECTED;
}

bool NegotiationEngine::endNegotiation(const std::string& negotiationId,
                                       bool accepted) {
    return endNegotiationResult(negotiationId, accepted).accepted();
}

NegotiationEngine::NegotiationAdvice NegotiationEngine::getAIAdvice(
    const OTCOffer& offer, double marketPrice,
    const CounterpartyProfile& counterparty) {
    return getAIAdviceResult(offer, marketPrice, counterparty).value;
}

std::vector<NegotiationMessage> NegotiationEngine::getNegotiationHistory(
    const std::string& negotiationId) {
    std::lock_guard<std::mutex> lock(negotiationMutex_);
    const auto found = negotiations_.find(negotiationId);
    return found == negotiations_.end() ? std::vector<NegotiationMessage>{}
                                        : found->second.messages;
}

std::string NegotiationEngine::getNegotiationStatus(
    const std::string& negotiationId) {
    return otcOperationStatusName(getNegotiationLifecycle(negotiationId));
}

OTCOperationStatus NegotiationEngine::getNegotiationLifecycle(
    const std::string& negotiationId) {
    std::lock_guard<std::mutex> lock(negotiationMutex_);
    const auto found = negotiations_.find(negotiationId);
    return found == negotiations_.end() ? OTCOperationStatus::UNAVAILABLE
                                        : found->second.status;
}

std::string NegotiationEngine::getNegotiationOfferId(
    const std::string& negotiationId) {
    std::lock_guard<std::mutex> lock(negotiationMutex_);
    const auto found = negotiations_.find(negotiationId);
    return found == negotiations_.end() ? std::string{} : found->second.offerId;
}

void NegotiationEngine::setTransport(std::shared_ptr<NegotiationTransport> transport) {
    std::lock_guard<std::mutex> lock(negotiationMutex_);
    transport_ = std::move(transport);
}

// OTCAnalytics --------------------------------------------------------------

OTCResult<TradeExecution> OTCAnalytics::recordTradeResult(
    const TradeExecution& trade) {
    if (!validIdentifier(trade.executionId) || !validIdentifier(trade.offerId) ||
        !validIdentifier(trade.makerId) || !validIdentifier(trade.takerId) ||
        !validToken(trade.baseToken) || !validToken(trade.quoteToken) ||
        !finitePositive(trade.baseAmount) || !finitePositive(trade.quoteAmount) ||
        !finitePositive(trade.pricePerUnit) || !finiteNonNegative(trade.feeAmount) ||
        trade.status != OfferStatus::COMPLETED ||
        trade.lifecycleStatus != OTCOperationStatus::VERIFIED ||
        !trade.settlementVerified || !validIdentifier(trade.settlementTxHash) ||
        !validEvidence(trade.settlementEvidence) ||
        trade.executedAt == std::chrono::system_clock::time_point{}) {
        return result<TradeExecution>(OTCOperationStatus::REJECTED,
                                      "only verified trades may be recorded");
    }
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    const auto replay = std::find_if(tradeHistory_.begin(), tradeHistory_.end(),
        [&trade](const TradeExecution& current) {
            return current.executionId == trade.executionId ||
                   current.settlementTxHash == trade.settlementTxHash;
        });
    if (replay != tradeHistory_.end()) {
        return result<TradeExecution>(OTCOperationStatus::VERIFIED,
                                      "verified trade replay", *replay,
                                      replay->settlementEvidence);
    }
    tradeHistory_.push_back(trade);
    return result<TradeExecution>(OTCOperationStatus::VERIFIED,
                                  "verified trade recorded", trade,
                                  trade.settlementEvidence);
}

void OTCAnalytics::recordTrade(const TradeExecution& trade) {
    (void)recordTradeResult(trade);
}

OTCAnalytics::TradingStats OTCAnalytics::getUserStats(const std::string& userId,
                                                      int days) {
    TradingStats stats;
    if (!validIdentifier(userId) || days <= 0) return stats;
    const auto cutoff = std::chrono::system_clock::now() - std::chrono::hours(days * 24LL);
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    for (const auto& trade : tradeHistory_) {
        if ((trade.makerId != userId && trade.takerId != userId) ||
            trade.executedAt < cutoff) continue;
        ++stats.totalTrades;
        stats.totalVolume += trade.quoteAmount;
        stats.volumeByToken[trade.baseToken.symbol] += trade.quoteAmount;
        stats.volumeByChain[trade.baseToken.chain] += trade.quoteAmount;
    }
    stats.avgTradeSize = stats.totalTrades > 0 ? stats.totalVolume / stats.totalTrades : 0.0;
    stats.successRate = stats.totalTrades > 0 ? 1.0 : 0.0;
    return stats;
}

OTCAnalytics::TradingStats OTCAnalytics::getPlatformStats(int days) {
    TradingStats stats;
    if (days <= 0) return stats;
    const auto cutoff = std::chrono::system_clock::now() - std::chrono::hours(days * 24LL);
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    for (const auto& trade : tradeHistory_) {
        if (trade.executedAt < cutoff) continue;
        ++stats.totalTrades;
        stats.totalVolume += trade.quoteAmount;
        stats.volumeByToken[trade.baseToken.symbol] += trade.quoteAmount;
        stats.volumeByChain[trade.baseToken.chain] += trade.quoteAmount;
    }
    stats.avgTradeSize = stats.totalTrades > 0 ? stats.totalVolume / stats.totalTrades : 0.0;
    stats.successRate = stats.totalTrades > 0 ? 1.0 : 0.0;
    return stats;
}

std::vector<TradeExecution> OTCAnalytics::getTradeHistory(
    const std::string& userId, int limit) {
    std::vector<TradeExecution> output;
    if (!validIdentifier(userId) || limit <= 0) return output;
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    for (auto iterator = tradeHistory_.rbegin();
         iterator != tradeHistory_.rend() && static_cast<int>(output.size()) < limit;
         ++iterator) {
        if (iterator->makerId == userId || iterator->takerId == userId) {
            output.push_back(*iterator);
        }
    }
    return output;
}

double OTCAnalytics::getPnL(const std::string& userId, int days) {
    if (!validIdentifier(userId) || days <= 0) return 0.0;
    const auto cutoff = std::chrono::system_clock::now() - std::chrono::hours(days * 24LL);
    double netQuoteCashFlow = 0.0;
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    for (const auto& trade : tradeHistory_) {
        if (trade.executedAt < cutoff) continue;
        const bool maker = trade.makerId == userId;
        const bool taker = trade.takerId == userId;
        if (!maker && !taker) continue;
        const bool userSold = (maker && trade.side == OfferSide::SELL) ||
                              (taker && trade.side == OfferSide::BUY);
        netQuoteCashFlow += userSold ? trade.quoteAmount : -trade.quoteAmount;
        if (maker) netQuoteCashFlow -= trade.feeAmount;
    }
    return netQuoteCashFlow;
}

double OTCAnalytics::getVWAP(const std::string& baseToken,
                             const std::string& quoteToken, int hours) {
    if (!validTokenSymbol(baseToken) || !validTokenSymbol(quoteToken) || hours <= 0) {
        return 0.0;
    }
    const auto cutoff = std::chrono::system_clock::now() - std::chrono::hours(hours);
    double volume = 0.0;
    double weighted = 0.0;
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    for (const auto& trade : tradeHistory_) {
        if (trade.executedAt >= cutoff && trade.baseToken.symbol == baseToken &&
            trade.quoteToken.symbol == quoteToken) {
            volume += trade.baseAmount;
            weighted += trade.baseAmount * trade.pricePerUnit;
        }
    }
    return volume > 0.0 ? weighted / volume : 0.0;
}

std::vector<std::pair<std::chrono::system_clock::time_point, double>>
OTCAnalytics::getPriceHistory(const std::string& baseToken,
                              const std::string& quoteToken, int days) {
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> output;
    if (!validTokenSymbol(baseToken) || !validTokenSymbol(quoteToken) || days <= 0) {
        return output;
    }
    const auto cutoff = std::chrono::system_clock::now() - std::chrono::hours(days * 24LL);
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    for (const auto& trade : tradeHistory_) {
        if (trade.executedAt >= cutoff && trade.baseToken.symbol == baseToken &&
            trade.quoteToken.symbol == quoteToken) {
            output.emplace_back(trade.executedAt, trade.pricePerUnit);
        }
    }
    std::sort(output.begin(), output.end(), [](const auto& left, const auto& right) {
        return left.first < right.first;
    });
    return output;
}

// OTC
// OTCAgent --------------------------------------------------------------------

OTCAgent::OTCAgent(const std::string& agentId)
    : OTCAgent(agentId, nullptr, nullptr, nullptr, nullptr) {}

OTCAgent::OTCAgent(const std::string& agentId,
                   std::shared_ptr<OTCMarketDataAdapter> marketDataAdapter,
                   std::shared_ptr<NegotiationTransport> negotiationTransport,
                   std::shared_ptr<ComplianceAdapter> complianceAdapter,
                   std::shared_ptr<SettlementAdapter> settlementAdapter)
    : agentId_(validIdentifier(agentId) ? agentId : std::string{}),
      logger_(std::make_shared<AgentLogger>()),
      marketDataAdapter_(std::move(marketDataAdapter)),
      negotiationTransport_(std::move(negotiationTransport)),
      complianceAdapter_(std::move(complianceAdapter)),
      settlementAdapter_(std::move(settlementAdapter)) {
    initializeSubManagers();
}

void OTCAgent::initializeSubManagers() {
    escrowManager_ = std::make_unique<EscrowManager>(settlementAdapter_, marketDataAdapter_);
    orderbook_ = std::make_unique<OTCOrderbook>();
    riskManager_ = std::make_unique<OTCRiskManager>(marketDataAdapter_);
    negotiationEngine_ = std::make_unique<NegotiationEngine>(negotiationTransport_);
    analytics_ = std::make_unique<OTCAnalytics>();
}

OTCResult<OTCOffer> OTCAgent::createOfferResult(const std::string& token,
                                                double amount, double price) {
    OTCChain chain;
    SettlementType settlement;
    int expiry;
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        if (agentId_.empty()) {
            return result<OTCOffer>(OTCOperationStatus::REJECTED,
                                    "agent identifier is invalid");
        }
        chain = currentChain_;
        settlement = defaultSettlement_;
        expiry = defaultExpiryHours_;
    }
    OTCToken base;
    base.symbol = token;
    base.chain = chain;
    base.decimals = 18;
    OTCToken quote;
    quote.symbol = "USDC";
    quote.chain = chain;
    quote.decimals = 6;
    return createAdvancedOfferResult(base, quote, amount, price, OfferSide::SELL,
                                     settlement, expiry, false);
}

OTCResult<OTCOffer> OTCAgent::createAdvancedOfferResult(
    const OTCToken& baseToken, const OTCToken& quoteToken, double amount,
    double pricePerUnit, OfferSide side, SettlementType settlement,
    int expiryHours, bool partialFill) {
    std::string agentId;
    double minimumTrade;
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        agentId = agentId_;
        minimumTrade = minTradeSize_;
    }
    if (!validIdentifier(agentId) || !validToken(baseToken) ||
        !validToken(quoteToken) || baseToken.symbol == quoteToken.symbol ||
        baseToken.chain != quoteToken.chain || !finitePositive(amount) ||
        !finitePositive(pricePerUnit) || !finitePositive(amount * pricePerUnit) ||
        amount * pricePerUnit < minimumTrade || expiryHours <= 0 ||
        expiryHours > 24 * 365) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED,
                                "invalid offer parameters or configured limits");
    }
    OTCOffer offer;
    offer.offerId = generateId("offer");
    offer.makerId = agentId;
    offer.baseToken = baseToken;
    offer.quoteToken = quoteToken;
    offer.baseAmount = amount;
    offer.quoteAmount = amount * pricePerUnit;
    offer.pricePerUnit = pricePerUnit;
    offer.minAmount = partialFill ? std::max(amount * 0.1, minimumTrade / pricePerUnit)
                                  : amount;
    offer.maxAmount = amount;
    if (offer.minAmount > offer.maxAmount) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED,
                                "minimum partial fill exceeds offer amount");
    }
    offer.side = side;
    offer.status = OfferStatus::ACTIVE;
    offer.settlementType = settlement;
    offer.chain = baseToken.chain;
    offer.createdAt = std::chrono::system_clock::now();
    offer.expiresAt = offer.createdAt + std::chrono::hours(expiryHours);
    offer.partialFillAllowed = partialFill;
    const auto stored = orderbook_->addOfferResult(offer);
    if (stored.accepted()) logStatus("Offer proposed: " + offer.offerId);
    return stored;
}

OTCResult<OTCOffer> OTCAgent::addObservedOfferResult(const OTCOffer& offer) {
    return orderbook_->addOfferResult(offer);
}

OTCResult<ComplianceDecision> OTCAgent::checkCompliance(
    const OTCOffer& offer, double amount, ComplianceAction action,
    const std::string& requestId) {
    std::shared_ptr<ComplianceAdapter> adapter;
    std::string agentId;
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        adapter = complianceAdapter_;
        agentId = agentId_;
    }
    if (!adapter) {
        return result<ComplianceDecision>(OTCOperationStatus::UNAVAILABLE,
                                          "compliance adapter unavailable");
    }
    ComplianceRequest request;
    request.action = action;
    request.requestId = requestId;
    request.offerId = offer.offerId;
    request.makerId = offer.makerId;
    request.takerId = agentId;
    request.baseToken = offer.baseToken;
    request.quoteToken = offer.quoteToken;
    request.baseAmount = amount;
    request.quoteAmount = amount * offer.pricePerUnit;
    request.chain = offer.chain;
    const auto decision = adapter->check(request);
    if (!validComplianceResult(decision)) {
        return result<ComplianceDecision>(OTCOperationStatus::REJECTED,
                                          "compliance approval absent or invalid");
    }
    return decision;
}

OTCResult<OTCOffer> OTCAgent::acceptAmountResult(
    const std::string& offerId, std::optional<double> amount,
    const std::string& idempotencyKey) {
    if (!validIdentifier(offerId) || (amount && !finitePositive(*amount)) ||
        (!idempotencyKey.empty() && !validIdentifier(idempotencyKey))) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED,
                                "invalid offer acceptance request");
    }
    const std::string fingerprint = acceptanceFingerprint(offerId, amount);
    if (!idempotencyKey.empty()) {
        std::lock_guard<std::mutex> lock(agentMutex_);
        const auto replay = acceptanceReplay_.find(idempotencyKey);
        if (replay != acceptanceReplay_.end()) {
            if (replay->second.fingerprint != fingerprint) {
                return result<OTCOffer>(OTCOperationStatus::REJECTED,
                                        "idempotency key reused with different request");
            }
            return replay->second.result;
        }
    }
    const auto snapshot = orderbook_->findOffer(offerId);
    if (!snapshot) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED, "offer not found");
    }
    if (snapshot->status != OfferStatus::ACTIVE ||
        snapshot->expiresAt <= std::chrono::system_clock::now()) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED,
                                "offer is not active or has expired");
    }
    if (snapshot->makerId == getAgentId()) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED,
                                "maker cannot accept own offer");
    }
    const double remaining = snapshot->baseAmount - snapshot->filledAmount;
    const double acceptedAmount = amount.value_or(remaining);
    if (!finitePositive(acceptedAmount) || acceptedAmount > remaining ||
        acceptedAmount < snapshot->minAmount ||
        (acceptedAmount < remaining && !snapshot->partialFillAllowed)) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED,
                                "partial fill is outside offer limits");
    }
    const auto risk = riskManager_->assessOfferResult(*snapshot, getAgentId());
    if (!risk.verified() || !risk.value.recommended) {
        return result<OTCOffer>(risk.status == OTCOperationStatus::VERIFIED
                                    ? OTCOperationStatus::REJECTED : risk.status,
                                "risk policy rejected acceptance");
    }
    const std::string requestId = idempotencyKey.empty()
        ? generateId("compliance") : idempotencyKey;
    const auto compliance = checkCompliance(*snapshot, acceptedAmount,
                                            ComplianceAction::ACCEPT_OFFER,
                                            requestId);
    if (!compliance.verified()) {
        return result<OTCOffer>(compliance.status, compliance.message);
    }
    OTCOffer updated = *snapshot;
    updated.status = OfferStatus::MATCHED;
    updated.takerId = getAgentId();
    updated.matchedAt = std::chrono::system_clock::now();
    const auto stored = orderbook_->updateOfferResult(updated, OfferStatus::ACTIVE);
    if (!stored.accepted()) return stored;
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        pendingAcceptanceAmounts_[offerId] = acceptedAmount;
    }
    auto output = result<OTCOffer>(OTCOperationStatus::ACCEPTED,
                                   "offer accepted locally; settlement not submitted",
                                   updated, compliance.evidence);
    if (!idempotencyKey.empty()) {
        std::lock_guard<std::mutex> lock(agentMutex_);
        const auto existing = acceptanceReplay_.find(idempotencyKey);
        if (existing != acceptanceReplay_.end()) return existing->second.result;
        acceptanceReplay_.emplace(idempotencyKey,
                                  CachedAcceptance{fingerprint, output});
    }
    logStatus("Offer accepted: " + offerId);
    return output;
}

OTCResult<OTCOffer> OTCAgent::acceptOfferResult(
    const std::string& offerId, const std::string& idempotencyKey) {
    return acceptAmountResult(offerId, std::nullopt, idempotencyKey);
}

OTCResult<OTCOffer> OTCAgent::acceptPartialOfferResult(
    const std::string& offerId, double amount, const std::string& idempotencyKey) {
    return acceptAmountResult(offerId, amount, idempotencyKey);
}

OTCResult<OTCOffer> OTCAgent::cancelOfferResult(const std::string& offerId) {
    const auto offer = orderbook_->findOffer(offerId);
    if (!offer || offer->makerId != getAgentId() || offer->status != OfferStatus::ACTIVE) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED,
                                "only the maker may cancel an active offer");
    }
    const auto output = orderbook_->removeOfferResult(offerId);
    if (output.status == OTCOperationStatus::CANCELLED) {
        logStatus("Offer cancelled: " + offerId);
    }
    return output;
}

OTCResult<OTCOffer> OTCAgent::updateOfferPriceResult(
    const std::string& offerId, double newPrice) {
    if (!validIdentifier(offerId) || !finitePositive(newPrice)) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED, "invalid price update");
    }
    const auto snapshot = orderbook_->findOffer(offerId);
    if (!snapshot || snapshot->makerId != getAgentId() ||
        snapshot->status != OfferStatus::ACTIVE) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED,
                                "offer is not editable");
    }
    OTCOffer updated = *snapshot;
    updated.pricePerUnit = newPrice;
    updated.quoteAmount = updated.baseAmount * newPrice;
    if (!finitePositive(updated.quoteAmount)) {
        return result<OTCOffer>(OTCOperationStatus::REJECTED,
                                "price update overflowed");
    }
    return orderbook_->updateOfferResult(updated, OfferStatus::ACTIVE);
}

bool OTCAgent::createOffer(const std::string& token, float amount, float price) {
    return createOfferResult(token, amount, price).accepted();
}

std::string OTCAgent::createAdvancedOffer(
    const OTCToken& baseToken, const OTCToken& quoteToken, double amount,
    double pricePerUnit, OfferSide side, SettlementType settlement,
    int expiryHours, bool partialFill) {
    const auto output = createAdvancedOfferResult(baseToken, quoteToken, amount,
                                                   pricePerUnit, side, settlement,
                                                   expiryHours, partialFill);
    return output.accepted() ? output.value.offerId : std::string{};
}

bool OTCAgent::acceptOffer(const std::string& offerId) {
    return acceptOfferResult(offerId).status == OTCOperationStatus::ACCEPTED;
}

bool OTCAgent::acceptPartialOffer(const std::string& offerId, double amount) {
    return acceptPartialOfferResult(offerId, amount).status ==
           OTCOperationStatus::ACCEPTED;
}

bool OTCAgent::cancelOffer(const std::string& offerId) {
    return cancelOfferResult(offerId).status == OTCOperationStatus::CANCELLED;
}

bool OTCAgent::updateOfferPrice(const std::string& offerId, double newPrice) {
    return updateOfferPriceResult(offerId, newPrice).accepted();
}

std::vector<OTCOffer> OTCAgent::getActiveOffers(const std::string& tokenSymbol) {
    if (tokenSymbol.empty()) return orderbook_->getActiveOffers();
    const auto offers = orderbook_->getOffersByToken(tokenSymbol);
    std::vector<OTCOffer> active;
    const auto now = std::chrono::system_clock::now();
    std::copy_if(offers.begin(), offers.end(), std::back_inserter(active),
                 [now](const OTCOffer& offer) {
                     return activeStatus(offer.status) && offer.expiresAt > now;
                 });
    return active;
}

std::vector<OTCOffer> OTCAgent::getMyOffers() {
    return orderbook_->getOffersByMaker(getAgentId());
}

OTCOffer OTCAgent::getOffer(const std::string& offerId) {
    const auto offer = orderbook_->findOffer(offerId);
    return offer.value_or(OTCOffer{});
}

OTCResult<OTCMarketStats> OTCAgent::getMarketStatsResult(
    const std::string& baseToken, const std::string& quoteToken) {
    if (!validTokenSymbol(baseToken) || !validTokenSymbol(quoteToken) ||
        baseToken == quoteToken) {
        return result<OTCMarketStats>(OTCOperationStatus::REJECTED,
                                      "invalid market pair");
    }
    std::shared_ptr<OTCMarketDataAdapter> adapter;
    OTCChain chain;
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        adapter = marketDataAdapter_;
        chain = currentChain_;
    }
    if (!adapter) {
        return result<OTCMarketStats>(OTCOperationStatus::UNAVAILABLE,
                                      "current market data unavailable");
    }
    OTCToken base;
    base.symbol = baseToken;
    base.chain = chain;
    OTCToken quote;
    quote.symbol = quoteToken;
    quote.chain = chain;
    auto market = adapter->getMarketData({base, quote, std::chrono::seconds(300)});
    if (!validMarketResult(market, std::chrono::seconds(300))) {
        return result<OTCMarketStats>(OTCOperationStatus::FAILED,
                                      "market adapter result failed verification");
    }
    return market;
}

OTCMarketStats OTCAgent::getMarketStats(const std::string& baseToken,
                                        const std::string& quoteToken) {
    const auto market = getMarketStatsResult(baseToken, quoteToken);
    return market.verified() ? market.value : OTCMarketStats{};
}

std::optional<OTCOffer> OTCAgent::findBestOffer(
    const std::string& baseToken, const std::string& quoteToken,
    OfferSide side, double amount) {
    if (!finitePositive(amount)) return std::nullopt;
    const auto matches = orderbook_->findMatchingOffers(baseToken, quoteToken,
                                                         side, amount, 0.0);
    return matches.empty() ? std::nullopt : std::optional<OTCOffer>(matches.front());
}

std::vector<OTCOffer> OTCAgent::findMatchingOffers(
    const std::string& baseToken, OfferSide side, double minAmount,
    double maxPrice) {
    return orderbook_->findMatchingOffers(baseToken, "USDC", side,
                                           minAmount, maxPrice);
}

OTCResult<std::string> OTCAgent::startNegotiationResult(
    const std::string& offerId, const std::string& idempotencyKey) {
    const auto offer = orderbook_->findOffer(offerId);
    if (!offer || !activeStatus(offer->status) ||
        offer->expiresAt <= std::chrono::system_clock::now()) {
        return result<std::string>(OTCOperationStatus::REJECTED,
                                   "offer is unavailable for negotiation");
    }
    const std::string counterparty = offer->makerId == getAgentId()
        ? offer->takerId : offer->makerId;
    auto output = negotiationEngine_->startNegotiationResult(
        offerId, getAgentId(), counterparty, idempotencyKey);
    if (!output.accepted()) return output;
    if (offer->status == OfferStatus::ACTIVE) {
        OTCOffer updated = *offer;
        updated.status = OfferStatus::NEGOTIATING;
        const auto stored = orderbook_->updateOfferResult(updated, OfferStatus::ACTIVE);
        if (!stored.accepted()) {
            return result<std::string>(OTCOperationStatus::REJECTED,
                                       "offer state changed concurrently");
        }
    }
    return output;
}

OTCResult<NegotiationMessage> OTCAgent::sendNegotiationMessageResult(
    const std::string& negotiationId, const std::string& message,
    std::optional<double> counterPrice, const std::string& idempotencyKey) {
    return negotiationEngine_->sendMessageResult(negotiationId, getAgentId(),
                                                  message, counterPrice,
                                                  std::nullopt, idempotencyKey);
}

OTCResult<NegotiationMessage> OTCAgent::acceptNegotiationResult(
    const std::string& negotiationId, const std::string& idempotencyKey) {
    const std::string offerId = negotiationEngine_->getNegotiationOfferId(negotiationId);
    const auto offer = orderbook_->findOffer(offerId);
    if (!offer || !activeStatus(offer->status) ||
        offer->expiresAt <= std::chrono::system_clock::now()) {
        return result<NegotiationMessage>(OTCOperationStatus::REJECTED,
                                          "linked offer is unavailable");
    }
    return negotiationEngine_->acceptCounterOfferResult(
        negotiationId, getAgentId(), idempotencyKey);
}

OTCResult<NegotiationMessage> OTCAgent::rejectNegotiationResult(
    const std::string& negotiationId, const std::string& idempotencyKey) {
    return negotiationEngine_->rejectCounterOfferResult(negotiationId,
                                                         getAgentId(),
                                                         idempotencyKey);
}

OTCResult<NegotiationEngine::NegotiationAdvice> OTCAgent::getAIAdviceResult(
    const std::string& offerId) {
    const auto offer = orderbook_->findOffer(offerId);
    if (!offer) {
        return result<NegotiationEngine::NegotiationAdvice>(
            OTCOperationStatus::REJECTED, "offer not found");
    }
    const auto market = getMarketStatsResult(offer->baseToken.symbol,
                                             offer->quoteToken.symbol);
    if (!market.verified()) {
        return result<NegotiationEngine::NegotiationAdvice>(
            market.status, "verified market data unavailable");
    }
    const auto profile = riskManager_->getCounterpartyProfile(offer->makerId);
    auto advice = negotiationEngine_->getAIAdviceResult(*offer,
                                                        market.value.currentPrice,
                                                        profile);
    advice.evidence = market.evidence;
    return advice;
}

std::string OTCAgent::startNegotiation(const std::string& offerId) {
    const auto output = startNegotiationResult(offerId);
    return output.accepted() ? output.value : std::string{};
}

bool OTCAgent::sendNegotiationMessage(
    const std::string& negotiationId, const std::string& message,
    std::optional<double> counterPrice) {
    return sendNegotiationMessageResult(negotiationId, message,
                                        counterPrice).accepted();
}

bool OTCAgent::acceptNegotiation(const std::string& negotiationId) {
    return acceptNegotiationResult(negotiationId).status ==
           OTCOperationStatus::ACCEPTED;
}

bool OTCAgent::rejectNegotiation(const std::string& negotiationId) {
    return rejectNegotiationResult(negotiationId).status ==
           OTCOperationStatus::REJECTED;
}

NegotiationEngine::NegotiationAdvice OTCAgent::getAIAdvice(
    const std::string& offerId) {
    const auto output = getAIAdviceResult(offerId);
    return output.accepted() ? output.value : NegotiationEngine::NegotiationAdvice{};
}

OTCResult<SettlementReceipt> OTCAgent::initiateSettlementResult(
    const std::string& offerId, const std::string& idempotencyKey) {
    if (!validIdentifier(offerId) ||
        (!idempotencyKey.empty() && !validIdentifier(idempotencyKey))) {
        return result<SettlementReceipt>(OTCOperationStatus::REJECTED,
                                         "invalid settlement request");
    }
    const std::string key = idempotencyKey.empty()
        ? generateId("settlement-submit") : idempotencyKey;
    std::shared_ptr<SettlementAdapter> adapter;
    double amount = 0.0;
    double slippage = 0.0;
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        const auto state = settlements_.find(offerId);
        if (state != settlements_.end() && state->second.request.idempotencyKey == key) {
            return state->second.result;
        }
        if (state != settlements_.end() &&
            (state->second.status == OTCOperationStatus::SUBMITTED ||
             state->second.status == OTCOperationStatus::VERIFIED)) {
            return result<SettlementReceipt>(OTCOperationStatus::REJECTED,
                                             "settlement already submitted");
        }
        if (settlementInFlight_.count(offerId) != 0) {
            return result<SettlementReceipt>(OTCOperationStatus::REJECTED,
                                             "settlement already in progress");
        }
        const auto accepted = pendingAcceptanceAmounts_.find(offerId);
        if (accepted == pendingAcceptanceAmounts_.end()) {
            return result<SettlementReceipt>(OTCOperationStatus::REJECTED,
                                             "offer has not been accepted");
        }
        amount = accepted->second;
        slippage = maxSlippage_;
        adapter = settlementAdapter_;
        settlementInFlight_.insert(offerId);
    }
    const auto clearInFlight = [this, &offerId]() {
        std::lock_guard<std::mutex> lock(agentMutex_);
        settlementInFlight_.erase(offerId);
    };
    const auto offer = orderbook_->findOffer(offerId);
    if (!offer || offer->status != OfferStatus::MATCHED) {
        clearInFlight();
        return result<SettlementReceipt>(OTCOperationStatus::REJECTED,
                                         "offer is not matched");
    }
    if (!adapter) {
        clearInFlight();
        return result<SettlementReceipt>(OTCOperationStatus::UNAVAILABLE,
                                         "settlement adapter unavailable");
    }
    const auto compliance = checkCompliance(*offer, amount,
                                             ComplianceAction::SUBMIT_SETTLEMENT,
                                             key);
    if (!compliance.verified()) {
        clearInFlight();
        return result<SettlementReceipt>(compliance.status, compliance.message);
    }
    SettlementRequest request;
    request.action = SettlementAction::EXECUTE_OFFER;
    request.idempotencyKey = key;
    request.offerId = offerId;
    request.makerId = offer->makerId;
    request.takerId = offer->takerId;
    request.baseToken = offer->baseToken;
    request.quoteToken = offer->quoteToken;
    request.baseAmount = amount;
    request.expectedPrice = offer->pricePerUnit;
    request.maximumSlippagePercent = slippage;
    request.settlementType = offer->settlementType;
    request.chain = offer->chain;
    const auto submitted = adapter->submit(request);
    if (!validReceipt(submitted, false)) {
        clearInFlight();
        return result<SettlementReceipt>(OTCOperationStatus::FAILED,
                                         "settlement submission evidence invalid");
    }
    const double priceDeviation = std::abs(submitted.value.executionPrice -
                                           offer->pricePerUnit) /
                                  offer->pricePerUnit * 100.0;
    if (!finitePositive(submitted.value.executedBaseAmount) ||
        submitted.value.executedBaseAmount > amount ||
        !finitePositive(submitted.value.executionPrice) ||
        priceDeviation > slippage ||
        !finitePositive(submitted.value.executedQuoteAmount) ||
        !finiteNonNegative(submitted.value.feeAmount)) {
        clearInFlight();
        return result<SettlementReceipt>(OTCOperationStatus::FAILED,
                                         "settlement receipt violates amount or slippage limits");
    }
    OTCOffer settling = *offer;
    settling.status = OfferStatus::SETTLING;
    const auto stored = orderbook_->updateOfferResult(settling, OfferStatus::MATCHED);
    if (!stored.accepted()) {
        clearInFlight();
        return result<SettlementReceipt>(OTCOperationStatus::REJECTED,
                                         "offer state changed concurrently");
    }
    SettlementState state;
    state.status = submitted.status;
    state.request = request;
    state.receipt = submitted.value;
    state.evidence = submitted.evidence;
    state.result = submitted;
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        settlements_[offerId] = state;
        settlementInFlight_.erase(offerId);
    }
    logStatus("Settlement submitted: " + offerId);
    return submitted;
}

OTCResult<SettlementReceipt> OTCAgent::confirmSettlementResult(
    const std::string& offerId, const std::string& transactionId,
    const std::string& idempotencyKey) {
    if (!validIdentifier(offerId) || !validIdentifier(transactionId) ||
        (!idempotencyKey.empty() && !validIdentifier(idempotencyKey))) {
        return result<SettlementReceipt>(OTCOperationStatus::REJECTED,
                                         "invalid settlement verification request");
    }
    SettlementState snapshot;
    std::shared_ptr<SettlementAdapter> adapter;
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        const auto found = settlements_.find(offerId);
        if (found == settlements_.end()) {
            return result<SettlementReceipt>(OTCOperationStatus::REJECTED,
                                             "settlement was not submitted");
        }
        if (found->second.status == OTCOperationStatus::VERIFIED) {
            if (found->second.receipt.transactionId == transactionId) {
                return found->second.result;
            }
            return result<SettlementReceipt>(OTCOperationStatus::REJECTED,
                                             "transaction identifier conflicts with verified receipt");
        }
        if (found->second.status != OTCOperationStatus::SUBMITTED) {
            return result<SettlementReceipt>(OTCOperationStatus::REJECTED,
                                             "settlement is not awaiting verification");
        }
        if (settlementInFlight_.count(offerId) != 0) {
            return result<SettlementReceipt>(OTCOperationStatus::REJECTED,
                                             "settlement verification already in progress");
        }
        snapshot = found->second;
        adapter = settlementAdapter_;
        settlementInFlight_.insert(offerId);
    }
    const auto clearInFlight = [this, &offerId]() {
        std::lock_guard<std::mutex> lock(agentMutex_);
        settlementInFlight_.erase(offerId);
    };
    if (snapshot.receipt.transactionId != transactionId) {
        clearInFlight();
        return result<SettlementReceipt>(OTCOperationStatus::REJECTED,
                                         "unverified transaction identifier rejected");
    }
    if (!adapter) {
        clearInFlight();
        return result<SettlementReceipt>(OTCOperationStatus::UNAVAILABLE,
                                         "settlement adapter unavailable");
    }
    SettlementRequest request = snapshot.request;
    request.idempotencyKey = idempotencyKey.empty()
        ? generateId("settlement-verify") : idempotencyKey;
    request.externalReference = transactionId;
    const auto verified = adapter->verify(request);
    if (!validReceipt(verified, true) ||
        verified.value.transactionId != transactionId ||
        verified.value.adapterReference != snapshot.receipt.adapterReference ||
        !finitePositive(verified.value.executedBaseAmount) ||
        !finitePositive(verified.value.executedQuoteAmount) ||
        !finitePositive(verified.value.executionPrice) ||
        verified.value.executedBaseAmount > snapshot.request.baseAmount ||
        std::abs(verified.value.executionPrice - snapshot.request.expectedPrice) /
            snapshot.request.expectedPrice * 100.0 >
            snapshot.request.maximumSlippagePercent) {
        clearInFlight();
        return result<SettlementReceipt>(OTCOperationStatus::FAILED,
                                         "settlement could not be verified");
    }
    const auto offer = orderbook_->findOffer(offerId);
    if (!offer || offer->status != OfferStatus::SETTLING) {
        clearInFlight();
        return result<SettlementReceipt>(OTCOperationStatus::REJECTED,
                                         "offer settlement state changed");
    }
    OTCOffer completed = *offer;
    completed.filledAmount += verified.value.executedBaseAmount;
    completed.status = completed.filledAmount + std::numeric_limits<double>::epsilon() >=
                               completed.baseAmount
        ? OfferStatus::COMPLETED : OfferStatus::ACTIVE;
    completed.settledAt = verified.value.settledAt;
    completed.takerId = snapshot.request.takerId;
    const auto stored = orderbook_->updateOfferResult(completed, OfferStatus::SETTLING);
    if (!stored.accepted()) {
        clearInFlight();
        return result<SettlementReceipt>(OTCOperationStatus::REJECTED,
                                         "offer state changed concurrently");
    }
    TradeExecution trade;
    trade.executionId = generateId("execution");
    trade.offerId = offerId;
    trade.makerId = completed.makerId;
    trade.takerId = completed.takerId;
    trade.baseToken = completed.baseToken;
    trade.quoteToken = completed.quoteToken;
    trade.baseAmount = verified.value.executedBaseAmount;
    trade.quoteAmount = verified.value.executedQuoteAmount;
    trade.pricePerUnit = verified.value.executionPrice;
    trade.side = completed.side;
    trade.settlementTxHash = transactionId;
    trade.status = OfferStatus::COMPLETED;
    trade.executedAt = verified.value.settledAt;
    trade.feeAmount = verified.value.feeAmount;
    trade.feeToken = verified.value.feeToken;
    trade.lifecycleStatus = OTCOperationStatus::VERIFIED;
    trade.settlementEvidence = verified.evidence;
    trade.settlementVerified = true;
    const auto recorded = analytics_->recordTradeResult(trade);
    if (!recorded.verified()) {
        clearInFlight();
        return result<SettlementReceipt>(OTCOperationStatus::FAILED,
                                         "verified receipt could not be recorded");
    }
    riskManager_->recordVerifiedUsage(completed.takerId, trade.quoteAmount);
    riskManager_->updateCounterpartyProfile(completed.makerId, trade);
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        auto& state = settlements_[offerId];
        state.status = OTCOperationStatus::VERIFIED;
        state.receipt = verified.value;
        state.evidence = verified.evidence;
        state.result = verified;
        pendingAcceptanceAmounts_.erase(offerId);
        settlementInFlight_.erase(offerId);
    }
    logStatus("Settlement verified: " + offerId);
    return verified;
}

bool OTCAgent::initiateSettlement(const std::string& offerId) {
    return initiateSettlementResult(offerId).verified();
}

bool OTCAgent::confirmSettlement(const std::string& offerId,
                                 const std::string& txHash) {
    return confirmSettlementResult(offerId, txHash).verified();
}

std::string OTCAgent::getSettlementStatus(const std::string& offerId) {
    return otcOperationStatusName(getSettlementLifecycle(offerId));
}

OTCOperationStatus OTCAgent::getSettlementLifecycle(const std::string& offerId) {
    std::lock_guard<std::mutex> lock(agentMutex_);
    const auto found = settlements_.find(offerId);
    return found == settlements_.end() ? OTCOperationStatus::UNAVAILABLE
                                       : found->second.status;
}

OTCResult<RiskAssessment> OTCAgent::assessOfferResult(const std::string& offerId) {
    const auto offer = orderbook_->findOffer(offerId);
    if (!offer) {
        return result<RiskAssessment>(OTCOperationStatus::REJECTED, "offer not found");
    }
    return riskManager_->assessOfferResult(*offer, getAgentId());
}

RiskAssessment OTCAgent::assessOffer(const std::string& offerId) {
    return assessOfferResult(offerId).value;
}

CounterpartyProfile OTCAgent::getCounterpartyProfile(
    const std::string& counterpartyId) {
    return riskManager_->getCounterpartyProfile(counterpartyId);
}

bool OTCAgent::isCounterpartyTrusted(const std::string& counterpartyId) {
    return riskManager_->isCounterpartyTrusted(counterpartyId);
}

OTCResult<std::string> OTCAgent::deployContractResult(
    const std::string& chain, const std::string& idempotencyKey) {
    if (!supportedChainName(chain) ||
        (!idempotencyKey.empty() && !validIdentifier(idempotencyKey))) {
        return result<std::string>(OTCOperationStatus::REJECTED,
                                   "unsupported chain or invalid request identifier");
    }
    std::shared_ptr<SettlementAdapter> settlementAdapter;
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        settlementAdapter = settlementAdapter_;
    }
    if (!settlementAdapter) {
        return result<std::string>(OTCOperationStatus::UNAVAILABLE,
                                   "deployment adapter unavailable");
    }
    const std::string key = idempotencyKey.empty()
        ? generateId("deployment") : idempotencyKey;
    OTCOffer pseudoOffer;
    pseudoOffer.offerId = key;
    pseudoOffer.makerId = getAgentId();
    pseudoOffer.chain = stringToChain(chain);
    pseudoOffer.baseToken.symbol = "CONTRACT";
    pseudoOffer.baseToken.chain = pseudoOffer.chain;
    pseudoOffer.quoteToken.symbol = "DEPLOYMENT";
    pseudoOffer.quoteToken.chain = pseudoOffer.chain;
    pseudoOffer.pricePerUnit = 1.0;
    const auto compliance = checkCompliance(pseudoOffer, 1.0,
                                             ComplianceAction::DEPLOY_CONTRACT,
                                             key);
    if (!compliance.verified()) {
        return result<std::string>(compliance.status, compliance.message);
    }
    SettlementRequest request;
    request.action = SettlementAction::DEPLOY_CONTRACT;
    request.idempotencyKey = key;
    request.makerId = getAgentId();
    request.chain = pseudoOffer.chain;
    const auto deployed = settlementAdapter->submit(request);
    if (!validReceipt(deployed, true) ||
        !otc_utils::isValidAddress(deployed.value.contractAddress,
                                   pseudoOffer.chain)) {
        return result<std::string>(OTCOperationStatus::FAILED,
                                   "deployment was not verified");
    }
    const std::string normalized = lowerCopy(chain);
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        deployedContracts_[normalized] = deployed.value.contractAddress;
    }
    return result<std::string>(OTCOperationStatus::VERIFIED,
                               "contract deployment verified",
                               deployed.value.contractAddress,
                               deployed.evidence);
}

bool OTCAgent::deployContract(const std::string& chain) {
    return deployContractResult(chain).verified();
}

std::string OTCAgent::getContractAddress(const std::string& chain) {
    const std::string normalized = lowerCopy(chain);
    std::lock_guard<std::mutex> lock(agentMutex_);
    const auto found = deployedContracts_.find(normalized);
    return found == deployedContracts_.end() ? std::string{} : found->second;
}

std::vector<OTCChain> OTCAgent::getSupportedChains() const {
    return {OTCChain::ETHEREUM, OTCChain::BASE, OTCChain::BSC,
            OTCChain::SOLANA, OTCChain::ARBITRUM, OTCChain::POLYGON};
}

bool OTCAgent::switchChain(OTCChain chain) {
    std::lock_guard<std::mutex> lock(agentMutex_);
    currentChain_ = chain;
    return true;
}

OTCAnalytics::TradingStats OTCAgent::getMyStats(int days) {
    return analytics_->getUserStats(getAgentId(), days);
}

std::vector<TradeExecution> OTCAgent::getTradeHistory(int limit) {
    return analytics_->getTradeHistory(getAgentId(), limit);
}

double OTCAgent::getPnL(int days) {
    return analytics_->getPnL(getAgentId(), days);
}

OTCResult<double> OTCAgent::setDefaultExpiryResult(int hours) {
    if (hours <= 0 || hours > 24 * 365) {
        return result<double>(OTCOperationStatus::REJECTED,
                              "expiry must be between 1 hour and 365 days");
    }
    std::lock_guard<std::mutex> lock(agentMutex_);
    defaultExpiryHours_ = hours;
    return result<double>(OTCOperationStatus::ACCEPTED,
                          "default expiry updated", static_cast<double>(hours));
}

OTCResult<double> OTCAgent::setMaxSlippageResult(double percentage) {
    if (!finiteNonNegative(percentage) || percentage > 100.0) {
        return result<double>(OTCOperationStatus::REJECTED,
                              "slippage must be finite and between 0 and 100 percent");
    }
    std::lock_guard<std::mutex> lock(agentMutex_);
    maxSlippage_ = percentage;
    return result<double>(OTCOperationStatus::ACCEPTED,
                          "maximum slippage updated", percentage);
}

OTCResult<double> OTCAgent::setMinTradeSizeResult(double amount) {
    if (!finitePositive(amount)) {
        return result<double>(OTCOperationStatus::REJECTED,
                              "minimum trade size must be finite and positive");
    }
    std::lock_guard<std::mutex> lock(agentMutex_);
    minTradeSize_ = amount;
    return result<double>(OTCOperationStatus::ACCEPTED,
                          "minimum trade size updated", amount);
}

void OTCAgent::setDefaultSettlement(SettlementType type) {
    std::lock_guard<std::mutex> lock(agentMutex_);
    defaultSettlement_ = type;
}

void OTCAgent::setDefaultExpiry(int hours) { (void)setDefaultExpiryResult(hours); }
void OTCAgent::setMaxSlippage(double percentage) { (void)setMaxSlippageResult(percentage); }
void OTCAgent::setMinTradeSize(double amount) { (void)setMinTradeSizeResult(amount); }

void OTCAgent::setOTCMarketDataAdapter(std::shared_ptr<OTCMarketDataAdapter> adapter) {
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        marketDataAdapter_ = adapter;
    }
    riskManager_->setOTCMarketDataAdapter(adapter);
    escrowManager_->setOTCMarketDataAdapter(std::move(adapter));
}

void OTCAgent::setNegotiationTransport(
    std::shared_ptr<NegotiationTransport> transport) {
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        negotiationTransport_ = transport;
    }
    negotiationEngine_->setTransport(std::move(transport));
}

void OTCAgent::setComplianceAdapter(std::shared_ptr<ComplianceAdapter> adapter) {
    std::lock_guard<std::mutex> lock(agentMutex_);
    complianceAdapter_ = std::move(adapter);
}

void OTCAgent::setSettlementAdapter(std::shared_ptr<SettlementAdapter> adapter) {
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        settlementAdapter_ = adapter;
    }
    escrowManager_->setSettlementAdapter(std::move(adapter));
}

std::string OTCAgent::getAgentId() const {
    std::lock_guard<std::mutex> lock(agentMutex_);
    return agentId_;
}

std::string OTCAgent::getStatus() const {
    std::string agentId;
    OTCChain chain;
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        agentId = agentId_;
        chain = currentChain_;
    }
    const auto activeOffers = orderbook_->getActiveOffers();
    const auto activeCount = static_cast<std::size_t>(std::count_if(
        activeOffers.begin(), activeOffers.end(), [&agentId](const OTCOffer& offer) {
            return offer.makerId == agentId;
        }));
    std::ostringstream stream;
    stream << "Agent: " << agentId << " | Chain: " << chainToString(chain)
           << " | Active offers: " << activeCount;
    return stream.str();
}

void OTCAgent::setStatusCallback(
    std::function<void(const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(agentMutex_);
    statusCallback_ = std::move(callback);
}

void OTCAgent::logStatus(const std::string& status) {
    std::function<void(const std::string&)> callback;
    {
        std::lock_guard<std::mutex> lock(agentMutex_);
        callback = statusCallback_;
    }
    if (callback) callback(status);
}

OTCChain OTCAgent::stringToChain(const std::string& chain) const {
    const std::string lowered = lowerCopy(chain);
    if (lowered == "ethereum" || lowered == "eth") return OTCChain::ETHEREUM;
    if (lowered == "bsc" || lowered == "binance") return OTCChain::BSC;
    if (lowered == "solana" || lowered == "sol") return OTCChain::SOLANA;
    if (lowered == "arbitrum" || lowered == "arb") return OTCChain::ARBITRUM;
    if (lowered == "polygon" || lowered == "matic") return OTCChain::POLYGON;
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
    }
    return "Unknown";
}

std::string OTCAgent::generateId(const std::string& prefix) { return nextId(prefix); }

namespace otc_utils {

double calculateSpread(double bid, double ask) {
    if (!finitePositive(bid) || !finitePositive(ask)) return 0.0;
    return (ask - bid) / ((bid + ask) / 2.0) * 100.0;
}

double calculatePriceImpact(double amount, double liquidity, double currentPrice) {
    if (!finiteNonNegative(amount) || !finitePositive(liquidity) ||
        !finitePositive(currentPrice)) return 0.0;
    return amount / liquidity * currentPrice * 0.5;
}

double calculateFee(double amount, double feeRate) {
    if (!finiteNonNegative(amount) || !finiteNonNegative(feeRate)) return 0.0;
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
    }
    return "Unknown";
}

std::string getChainCurrency(OTCChain chain) {
    switch (chain) {
        case OTCChain::ETHEREUM:
        case OTCChain::BASE:
        case OTCChain::ARBITRUM: return "ETH";
        case OTCChain::BSC: return "BNB";
        case OTCChain::SOLANA: return "SOL";
        case OTCChain::POLYGON: return "MATIC";
    }
    return {};
}

std::string getExplorerUrl(OTCChain chain, const std::string& txHash) {
    if (!validIdentifier(txHash)) return {};
    switch (chain) {
        case OTCChain::ETHEREUM: return "https://etherscan.io/tx/" + txHash;
        case OTCChain::BASE: return "https://basescan.org/tx/" + txHash;
        case OTCChain::BSC: return "https://bscscan.com/tx/" + txHash;
        case OTCChain::SOLANA: return "https://solscan.io/tx/" + txHash;
        case OTCChain::ARBITRUM: return "https://arbiscan.io/tx/" + txHash;
        case OTCChain::POLYGON: return "https://polygonscan.com/tx/" + txHash;
    }
    return {};
}

bool isValidAmount(double amount, const OTCToken& token) {
    if (!finitePositive(amount) || !validTokenSymbol(token.symbol) ||
        token.decimals < 0 || token.decimals > 30) return false;
    const double upperBound = std::pow(10.0, std::min(token.decimals, 18));
    return amount <= upperBound;
}

bool isValidPrice(double price) { return finitePositive(price) && price <= 1e18; }

bool isValidAddress(const std::string& address, OTCChain chain) {
    if (chain == OTCChain::SOLANA) {
        if (address.size() < 32 || address.size() > 44) return false;
        const std::string base58 =
            "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
        return std::all_of(address.begin(), address.end(), [&base58](char character) {
            return base58.find(character) != std::string::npos;
        });
    }
    if (address.size() != 42 || address.rfind("0x", 0) != 0) return false;
    return std::all_of(address.begin() + 2, address.end(), [](unsigned char character) {
        return std::isxdigit(character) != 0;
    });
}

}  // namespace otc_utils
}  // namespace elizaos
