#include <gtest/gtest.h>
#include "elizaos/otc_agent.hpp"

#include <atomic>
#include <cmath>
#include <limits>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

using namespace elizaos;

namespace {

OTCAdapterEvidence evidence(const std::string& reference = "evidence-1") {
    return {"fake-adapter", reference, std::chrono::system_clock::now(), true};
}

OTCToken token(const std::string& symbol, int decimals = 18) {
    OTCToken value;
    value.symbol = symbol;
    value.chain = OTCChain::BASE;
    value.decimals = decimals;
    return value;
}

OTCOffer observedOffer(const std::string& id = "external-offer-1",
                       double amount = 10.0, double price = 100.0,
                       bool partial = true) {
    OTCOffer offer;
    offer.offerId = id;
    offer.makerId = "maker-1";
    offer.baseToken = token("ETH");
    offer.quoteToken = token("USDC", 6);
    offer.baseAmount = amount;
    offer.quoteAmount = amount * price;
    offer.pricePerUnit = price;
    offer.minAmount = partial ? 1.0 : amount;
    offer.maxAmount = amount;
    offer.side = OfferSide::SELL;
    offer.status = OfferStatus::ACTIVE;
    offer.settlementType = SettlementType::INSTANT;
    offer.chain = OTCChain::BASE;
    offer.createdAt = std::chrono::system_clock::now();
    offer.expiresAt = offer.createdAt + std::chrono::hours(1);
    offer.partialFillAllowed = partial;
    return offer;
}

class FakeMarketData final : public OTCMarketDataAdapter {
public:
    OTCOperationStatus status = OTCOperationStatus::VERIFIED;
    double price = 100.0;
    double volatility = 0.1;
    bool stale = false;
    bool verifiedEvidence = true;
    std::function<void()> onCall;
    std::atomic<int> calls{0};

    OTCResult<OTCMarketStats> getMarketData(const MarketDataRequest& request) override {
        ++calls;
        if (onCall) onCall();
        OTCResult<OTCMarketStats> output;
        output.status = status;
        output.message = "fake market result";
        output.value.tokenPair = request.baseToken.symbol + "/" + request.quoteToken.symbol;
        output.value.currentPrice = price;
        output.value.volatility = volatility;
        output.value.asOf = std::chrono::system_clock::now() -
            (stale ? std::chrono::hours(1) : std::chrono::seconds(1));
        output.value.source = "fake-market";
        output.evidence = evidence("market-evidence");
        output.evidence.verified = verifiedEvidence;
        return output;
    }
};

class FakeCompliance final : public ComplianceAdapter {
public:
    bool approved = true;
    bool verifiedEvidence = true;
    std::function<void()> onCall;
    std::atomic<int> calls{0};
    std::vector<ComplianceRequest> requests;
    std::mutex mutex;

    OTCResult<ComplianceDecision> check(const ComplianceRequest& request) override {
        ++calls;
        if (onCall) onCall();
        {
            std::lock_guard<std::mutex> lock(mutex);
            requests.push_back(request);
        }
        OTCResult<ComplianceDecision> output;
        output.status = OTCOperationStatus::VERIFIED;
        output.value.approved = approved;
        output.value.decisionId = "decision-" + std::to_string(calls.load());
        output.value.validUntil = std::chrono::system_clock::now() + std::chrono::hours(1);
        output.evidence = evidence("compliance-evidence");
        output.evidence.verified = verifiedEvidence;
        return output;
    }
};

class FakeNegotiationTransport final : public NegotiationTransport {
public:
    bool fail = false;
    bool verifiedEvidence = true;
    std::function<void()> onCall;
    std::atomic<int> calls{0};

    OTCResult<NegotiationTransportReceipt> send(
        const NegotiationTransportRequest& request) override {
        ++calls;
        if (onCall) onCall();
        OTCResult<NegotiationTransportReceipt> output;
        output.status = fail ? OTCOperationStatus::FAILED
                             : OTCOperationStatus::ACCEPTED;
        output.value.deliveryId = "delivery-" + std::to_string(calls.load());
        output.value.idempotencyKey = request.idempotencyKey;
        output.evidence = evidence("delivery-evidence");
        output.evidence.verified = verifiedEvidence;
        return output;
    }
};

class FakeSettlement final : public SettlementAdapter {
public:
    bool failSubmit = false;
    bool failVerify = false;
    bool invalidEvidence = false;
    bool badSlippage = false;
    std::function<void()> onSubmit;
    std::function<void()> onVerify;
    std::atomic<int> submitCalls{0};
    std::atomic<int> verifyCalls{0};
    std::string transactionId = "transaction-1";
    std::string adapterReference = "settlement-reference-1";
    std::mutex mutex;
    std::vector<SettlementRequest> submissions;
    std::vector<SettlementRequest> verifications;

    OTCResult<SettlementReceipt> submit(const SettlementRequest& request) override {
        ++submitCalls;
        if (onSubmit) onSubmit();
        {
            std::lock_guard<std::mutex> lock(mutex);
            submissions.push_back(request);
        }
        OTCResult<SettlementReceipt> output;
        const bool completesAction =
            request.action == SettlementAction::RELEASE_ESCROW ||
            request.action == SettlementAction::REFUND_ESCROW ||
            request.action == SettlementAction::DISPUTE_ESCROW ||
            request.action == SettlementAction::RESOLVE_DISPUTE ||
            request.action == SettlementAction::DEPLOY_CONTRACT;
        output.status = failSubmit ? OTCOperationStatus::FAILED
                                   : (completesAction ? OTCOperationStatus::VERIFIED
                                                      : OTCOperationStatus::SUBMITTED);
        output.value.transactionId = request.action == SettlementAction::DEPOSIT_ESCROW
            ? request.externalReference : transactionId;
        output.value.adapterReference = adapterReference;
        output.value.contractAddress = "0x1111111111111111111111111111111111111111";
        output.value.executedBaseAmount = request.baseAmount > 0.0 ? request.baseAmount : 1.0;
        output.value.executionPrice = request.expectedPrice > 0.0
            ? request.expectedPrice * (badSlippage ? 1.2 : 1.0) : 1.0;
        output.value.executedQuoteAmount = output.value.executedBaseAmount *
                                           output.value.executionPrice;
        output.value.feeAmount = 1.0;
        output.value.feeToken = "USDC";
        output.value.settledAt = std::chrono::system_clock::now();
        output.evidence = evidence("settlement-submit-evidence");
        output.evidence.verified = !invalidEvidence;
        return output;
    }

    OTCResult<SettlementReceipt> verify(const SettlementRequest& request) override {
        ++verifyCalls;
        if (onVerify) onVerify();
        {
            std::lock_guard<std::mutex> lock(mutex);
            verifications.push_back(request);
        }
        OTCResult<SettlementReceipt> output;
        output.status = failVerify ? OTCOperationStatus::FAILED
                                   : OTCOperationStatus::VERIFIED;
        output.value.transactionId = request.externalReference;
        output.value.adapterReference = adapterReference;
        output.value.executedBaseAmount = request.baseAmount;
        output.value.executionPrice = request.expectedPrice * (badSlippage ? 1.2 : 1.0);
        output.value.executedQuoteAmount = output.value.executedBaseAmount *
                                           output.value.executionPrice;
        output.value.feeAmount = 1.0;
        output.value.feeToken = "USDC";
        output.value.settledAt = std::chrono::system_clock::now();
        output.evidence = evidence("settlement-verify-evidence");
        output.evidence.verified = !invalidEvidence;
        return output;
    }
};

struct Adapters {
    std::shared_ptr<FakeMarketData> market = std::make_shared<FakeMarketData>();
    std::shared_ptr<FakeNegotiationTransport> negotiation =
        std::make_shared<FakeNegotiationTransport>();
    std::shared_ptr<FakeCompliance> compliance = std::make_shared<FakeCompliance>();
    std::shared_ptr<FakeSettlement> settlement = std::make_shared<FakeSettlement>();
};

std::unique_ptr<OTCAgent> agentWith(const Adapters& adapters,
                                    const std::string& id = "taker-1") {
    return std::make_unique<OTCAgent>(id, adapters.market, adapters.negotiation,
                                      adapters.compliance, adapters.settlement);
}

OTCResult<OTCOffer> addAndAccept(OTCAgent& agent,
                                 const std::string& key = "accept-key-1",
                                 double amount = 10.0) {
    EXPECT_EQ(agent.addObservedOfferResult(observedOffer()).status,
              OTCOperationStatus::PROPOSED);
    return amount == 10.0 ? agent.acceptOfferResult("external-offer-1", key)
                          : agent.acceptPartialOfferResult("external-offer-1", amount, key);
}

TradeExecution verifiedTrade(const std::string& id, OfferSide side,
                             double quoteAmount, double fee) {
    TradeExecution trade;
    trade.executionId = id;
    trade.offerId = "offer-" + id;
    trade.makerId = "user-1";
    trade.takerId = "other-1";
    trade.baseToken = token("ETH");
    trade.quoteToken = token("USDC", 6);
    trade.baseAmount = quoteAmount / 100.0;
    trade.quoteAmount = quoteAmount;
    trade.pricePerUnit = 100.0;
    trade.side = side;
    trade.settlementTxHash = "tx-" + id;
    trade.status = OfferStatus::COMPLETED;
    trade.executedAt = std::chrono::system_clock::now();
    trade.feeAmount = fee;
    trade.feeToken = "USDC";
    trade.lifecycleStatus = OTCOperationStatus::VERIFIED;
    trade.settlementEvidence = evidence("trade-evidence-" + id);
    trade.settlementVerified = true;
    return trade;
}

}  // namespace

TEST(OTCDefaultMode, LocalOffersAreProposalsAndExternalTruthIsUnavailable) {
    OTCAgent agent("agent-1");
    auto created = agent.createOfferResult("ETH", 2.0, 100.0);
    ASSERT_EQ(created.status, OTCOperationStatus::PROPOSED);
    EXPECT_EQ(created.value.status, OfferStatus::ACTIVE);
    EXPECT_EQ(agent.getMyOffers().size(), 1U);
    EXPECT_EQ(agent.getMarketStatsResult("ETH", "USDC").status,
              OTCOperationStatus::UNAVAILABLE);
    EXPECT_EQ(agent.getMarketStats("ETH", "USDC").currentPrice, 0.0);
    EXPECT_FALSE(agent.initiateSettlement(created.value.offerId));
    EXPECT_FALSE(agent.deployContract("base"));
    EXPECT_TRUE(agent.getContractAddress("base").empty());
}

TEST(OTCValidation, RejectsMalformedNonFiniteExpiredAndOutOfLimitInputs) {
    OTCAgent agent("agent-1");
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double infinity = std::numeric_limits<double>::infinity();
    EXPECT_EQ(agent.createOfferResult("bad symbol!", 1.0, 100.0).status,
              OTCOperationStatus::REJECTED);
    EXPECT_EQ(agent.createOfferResult("ETH", nan, 100.0).status,
              OTCOperationStatus::REJECTED);
    EXPECT_EQ(agent.createOfferResult("ETH", 1.0, infinity).status,
              OTCOperationStatus::REJECTED);
    EXPECT_EQ(agent.setDefaultExpiryResult(0).status, OTCOperationStatus::REJECTED);
    EXPECT_EQ(agent.setMaxSlippageResult(nan).status, OTCOperationStatus::REJECTED);
    EXPECT_EQ(agent.setMaxSlippageResult(101.0).status, OTCOperationStatus::REJECTED);
    EXPECT_EQ(agent.setMinTradeSizeResult(-1.0).status, OTCOperationStatus::REJECTED);
    auto expired = observedOffer();
    expired.expiresAt = expired.createdAt - std::chrono::seconds(1);
    EXPECT_EQ(agent.addObservedOfferResult(expired).status, OTCOperationStatus::REJECTED);
    EXPECT_FALSE(otc_utils::isValidAddress("0x1234", OTCChain::BASE));
    EXPECT_FALSE(otc_utils::isValidAddress(std::string(32, '0'), OTCChain::SOLANA));
}

TEST(OTCOfferLifecycle, UpdateCancelAndInvalidTransitionsAreDeterministic) {
    OTCAgent agent("maker-1");
    auto created = agent.createOfferResult("ETH", 2.0, 100.0);
    ASSERT_EQ(created.status, OTCOperationStatus::PROPOSED);
    auto updated = agent.updateOfferPriceResult(created.value.offerId, 110.0);
    ASSERT_TRUE(updated.accepted());
    EXPECT_DOUBLE_EQ(updated.value.quoteAmount, 220.0);
    auto cancelled = agent.cancelOfferResult(created.value.offerId);
    EXPECT_EQ(cancelled.status, OTCOperationStatus::CANCELLED);
    EXPECT_TRUE(agent.getMyOffers().empty());
    EXPECT_EQ(agent.cancelOfferResult(created.value.offerId).status,
              OTCOperationStatus::REJECTED);
    EXPECT_EQ(agent.updateOfferPriceResult(created.value.offerId, 120.0).status,
              OTCOperationStatus::REJECTED);
}

TEST(OTCOrderbook, MatchingUsesPairAmountPriceAndStableTieBreak) {
    OTCOrderbook orderbook;
    auto first = observedOffer("offer-2", 5.0, 101.0, true);
    auto second = observedOffer("offer-1", 5.0, 99.0, true);
    ASSERT_TRUE(orderbook.addOfferResult(first).accepted());
    ASSERT_TRUE(orderbook.addOfferResult(second).accepted());
    auto matches = orderbook.findMatchingOffers("ETH", "USDC", OfferSide::BUY,
                                                 2.0, 100.0);
    ASSERT_EQ(matches.size(), 1U);
    EXPECT_EQ(matches.front().offerId, "offer-1");
    auto stats = orderbook.getMarketStats("ETH", "USDC");
    EXPECT_DOUBLE_EQ(stats.bestAsk, 99.0);
    EXPECT_DOUBLE_EQ(stats.volume24h, 0.0);
    EXPECT_DOUBLE_EQ(stats.priceChange24h, 0.0);
    EXPECT_EQ(stats.source, "local-orderbook-only");
}

TEST(OTCMarketData, RejectsAdapterFailureInvalidEvidenceAndStaleness) {
    Adapters adapters;
    auto agent = agentWith(adapters);
    EXPECT_TRUE(agent->getMarketStatsResult("ETH", "USDC").verified());
    adapters.market->status = OTCOperationStatus::FAILED;
    EXPECT_EQ(agent->getMarketStatsResult("ETH", "USDC").status,
              OTCOperationStatus::FAILED);
    adapters.market->status = OTCOperationStatus::VERIFIED;
    adapters.market->verifiedEvidence = false;
    EXPECT_EQ(agent->getMarketStatsResult("ETH", "USDC").status,
              OTCOperationStatus::FAILED);
    adapters.market->verifiedEvidence = true;
    adapters.market->stale = true;
    EXPECT_EQ(agent->getMarketStatsResult("ETH", "USDC").status,
              OTCOperationStatus::FAILED);
}

TEST(OTCRisk, VerifiedMarketRiskAndDailyLimitsGateAcceptance) {
    Adapters adapters;
    auto agent = agentWith(adapters);
    ASSERT_TRUE(agent->addObservedOfferResult(observedOffer()).accepted());
    auto risk = agent->assessOfferResult("external-offer-1");
    ASSERT_TRUE(risk.verified());
    EXPECT_TRUE(risk.value.marketDataVerified);
    EXPECT_TRUE(risk.value.recommended);
    adapters.market->price = 50.0;
    risk = agent->assessOfferResult("external-offer-1");
    ASSERT_TRUE(risk.verified());
    EXPECT_FALSE(risk.value.recommended);
    EXPECT_EQ(agent->acceptOfferResult("external-offer-1", "risk-reject").status,
              OTCOperationStatus::REJECTED);
    EXPECT_EQ(agent->getOffer("external-offer-1").status, OfferStatus::ACTIVE);
}

TEST(OTCCompliance, RejectionOrInvalidEvidenceCausesNoOfferMutation) {
    Adapters adapters;
    auto agent = agentWith(adapters);
    ASSERT_TRUE(agent->addObservedOfferResult(observedOffer()).accepted());
    adapters.compliance->approved = false;
    EXPECT_EQ(agent->acceptOfferResult("external-offer-1", "denied").status,
              OTCOperationStatus::REJECTED);
    EXPECT_EQ(agent->getOffer("external-offer-1").status, OfferStatus::ACTIVE);
    adapters.compliance->approved = true;
    adapters.compliance->verifiedEvidence = false;
    EXPECT_EQ(agent->acceptOfferResult("external-offer-1", "bad-evidence").status,
              OTCOperationStatus::REJECTED);
    EXPECT_EQ(agent->getOffer("external-offer-1").status, OfferStatus::ACTIVE);
}

TEST(OTCNegotiation, LocalCounterAcceptRejectCancelAndReplayAreExplicit) {
    NegotiationEngine engine;
    auto started = engine.startNegotiationResult("offer-1", "party-a", "party-b",
                                                  "start-key");
    ASSERT_EQ(started.status, OTCOperationStatus::PROPOSED);
    EXPECT_EQ(engine.startNegotiationResult("offer-1", "party-a", "party-b",
                                             "start-key").value,
              started.value);
    auto counter = engine.sendMessageResult(started.value, "party-b", "counter",
                                             95.0, 2.0, "message-key");
    ASSERT_EQ(counter.status, OTCOperationStatus::PROPOSED);
    EXPECT_EQ(engine.getNegotiationHistory(started.value).size(), 2U);
    auto replay = engine.sendMessageResult(started.value, "party-b", "counter",
                                            95.0, 2.0, "message-key");
    EXPECT_EQ(replay.value.messageId, counter.value.messageId);
    EXPECT_EQ(engine.getNegotiationHistory(started.value).size(), 2U);
    auto accepted = engine.acceptCounterOfferResult(started.value, "party-a",
                                                     "accept-key");
    EXPECT_EQ(accepted.status, OTCOperationStatus::ACCEPTED);
    EXPECT_EQ(engine.getNegotiationStatus(started.value), "accepted");
    EXPECT_EQ(engine.rejectCounterOfferResult(started.value, "party-a").status,
              OTCOperationStatus::REJECTED);

    auto second = engine.startNegotiationResult("offer-2", "party-a", "party-b");
    ASSERT_TRUE(second.accepted());
    ASSERT_TRUE(engine.sendMessageResult(second.value, "party-b", "counter", 99.0));
    EXPECT_EQ(engine.rejectCounterOfferResult(second.value, "party-a").status,
              OTCOperationStatus::REJECTED);
    EXPECT_EQ(engine.getNegotiationStatus(second.value), "rejected");

    auto third = engine.startNegotiationResult("offer-3", "party-a", "party-b");
    EXPECT_EQ(engine.endNegotiationResult(third.value, false).status,
              OTCOperationStatus::CANCELLED);
    EXPECT_EQ(engine.getNegotiationStatus(third.value), "cancelled");
}

TEST(OTCNegotiation, TransportRunsOutsideLocksAndFailureDoesNotAppend) {
    auto transport = std::make_shared<FakeNegotiationTransport>();
    NegotiationEngine engine(transport);
    const auto started = engine.startNegotiationResult("offer-1", "party-a", "party-b");
    ASSERT_TRUE(started.accepted());
    transport->onCall = [&]() {
        EXPECT_NE(engine.getNegotiationStatus(started.value), "unavailable");
    };
    auto sent = engine.sendMessageResult(started.value, "party-a", "hello",
                                         std::nullopt, std::nullopt, "send-key");
    EXPECT_EQ(sent.status, OTCOperationStatus::ACCEPTED);
    EXPECT_EQ(engine.getNegotiationHistory(started.value).size(), 2U);
    transport->fail = true;
    EXPECT_EQ(engine.sendMessageResult(started.value, "party-a", "not stored",
                                        std::nullopt, std::nullopt, "fail-key").status,
              OTCOperationStatus::FAILED);
    EXPECT_EQ(engine.getNegotiationHistory(started.value).size(), 2U);
}

TEST(OTCSettlement, VerifiedReceiptIsRequiredAndRecordsExactlyOneActualTrade) {
    Adapters adapters;
    auto agent = agentWith(adapters);
    auto accepted = addAndAccept(*agent);
    ASSERT_EQ(accepted.status, OTCOperationStatus::ACCEPTED);
    EXPECT_TRUE(agent->getTradeHistory().empty());
    auto submitted = agent->initiateSettlementResult("external-offer-1", "submit-key");
    ASSERT_EQ(submitted.status, OTCOperationStatus::SUBMITTED);
    EXPECT_EQ(agent->getSettlementStatus("external-offer-1"), "submitted");
    EXPECT_FALSE(agent->initiateSettlement("external-offer-1"));
    EXPECT_TRUE(agent->getTradeHistory().empty());
    EXPECT_EQ(agent->confirmSettlementResult("external-offer-1", "invented-tx").status,
              OTCOperationStatus::REJECTED);
    auto verified = agent->confirmSettlementResult(
        "external-offer-1", submitted.value.transactionId, "verify-key");
    ASSERT_TRUE(verified.verified());
    EXPECT_EQ(agent->getSettlementStatus("external-offer-1"), "verified");
    ASSERT_EQ(agent->getTradeHistory().size(), 1U);
    EXPECT_EQ(agent->getTradeHistory().front().settlementTxHash,
              submitted.value.transactionId);
    EXPECT_TRUE(agent->confirmSettlementResult(
        "external-offer-1", submitted.value.transactionId, "other-key").verified());
    EXPECT_EQ(agent->getTradeHistory().size(), 1U);
    EXPECT_EQ(adapters.settlement->verifyCalls.load(), 1);
}

TEST(OTCSettlement, AdapterAndSlippageFailuresDoNotFabricateTradesOrCompletion) {
    Adapters adapters;
    auto agent = agentWith(adapters);
    ASSERT_TRUE(addAndAccept(*agent).accepted());
    adapters.settlement->failSubmit = true;
    EXPECT_EQ(agent->initiateSettlementResult("external-offer-1", "fail-submit").status,
              OTCOperationStatus::FAILED);
    EXPECT_TRUE(agent->getTradeHistory().empty());
    EXPECT_EQ(agent->getOffer("external-offer-1").status, OfferStatus::MATCHED);

    adapters.settlement->failSubmit = false;
    adapters.settlement->badSlippage = true;
    EXPECT_EQ(agent->initiateSettlementResult("external-offer-1", "bad-slip").status,
              OTCOperationStatus::FAILED);
    EXPECT_TRUE(agent->getTradeHistory().empty());
    EXPECT_EQ(agent->getOffer("external-offer-1").status, OfferStatus::MATCHED);
}

TEST(OTCSettlement, VerificationFailurePreservesSubmittedStateWithoutTrade) {
    Adapters adapters;
    auto agent = agentWith(adapters);
    ASSERT_TRUE(addAndAccept(*agent).accepted());
    const auto submitted = agent->initiateSettlementResult("external-offer-1", "submit");
    ASSERT_EQ(submitted.status, OTCOperationStatus::SUBMITTED);
    adapters.settlement->failVerify = true;
    EXPECT_EQ(agent->confirmSettlementResult("external-offer-1",
                                              submitted.value.transactionId,
                                              "verify-fail").status,
              OTCOperationStatus::FAILED);
    EXPECT_EQ(agent->getSettlementStatus("external-offer-1"), "submitted");
    EXPECT_EQ(agent->getOffer("external-offer-1").status, OfferStatus::SETTLING);
    EXPECT_TRUE(agent->getTradeHistory().empty());
}

TEST(OTCReplay, AcceptanceIsIdempotentAndConflictingReuseIsRejected) {
    Adapters adapters;
    auto agent = agentWith(adapters);
    ASSERT_TRUE(agent->addObservedOfferResult(observedOffer()).accepted());
    auto first = agent->acceptPartialOfferResult("external-offer-1", 2.0, "same-key");
    ASSERT_EQ(first.status, OTCOperationStatus::ACCEPTED);
    auto replay = agent->acceptPartialOfferResult("external-offer-1", 2.0, "same-key");
    EXPECT_EQ(replay.value.matchedAt, first.value.matchedAt);
    EXPECT_EQ(adapters.compliance->calls.load(), 1);
    EXPECT_EQ(agent->acceptPartialOfferResult("external-offer-1", 3.0,
                                               "same-key").status,
              OTCOperationStatus::REJECTED);
    EXPECT_EQ(adapters.compliance->calls.load(), 1);
}

TEST(OTCAnalytics, PnLAndMarketHistoryUseVerifiedStoredTradesOnly) {
    OTCAnalytics analytics;
    auto unverified = verifiedTrade("one", OfferSide::SELL, 1000.0, 2.0);
    unverified.settlementVerified = false;
    EXPECT_EQ(analytics.recordTradeResult(unverified).status,
              OTCOperationStatus::REJECTED);
    EXPECT_DOUBLE_EQ(analytics.getPnL("user-1"), 0.0);
    auto sale = verifiedTrade("sale", OfferSide::SELL, 1000.0, 2.0);
    auto purchase = verifiedTrade("purchase", OfferSide::BUY, 400.0, 1.0);
    ASSERT_TRUE(analytics.recordTradeResult(sale).verified());
    ASSERT_TRUE(analytics.recordTradeResult(purchase).verified());
    EXPECT_TRUE(analytics.recordTradeResult(sale).verified());
    EXPECT_EQ(analytics.getTradeHistory("user-1").size(), 2U);
    EXPECT_DOUBLE_EQ(analytics.getPnL("user-1"), 597.0);
    EXPECT_DOUBLE_EQ(analytics.getVWAP("ETH", "USDC"), 100.0);
    EXPECT_EQ(analytics.getPriceHistory("ETH", "USDC").size(), 2U);
    EXPECT_DOUBLE_EQ(analytics.getVWAP("BTC", "USDC"), 0.0);
    EXPECT_TRUE(analytics.getPriceHistory("BTC", "USDC").empty());
}

TEST(OTCConcurrency, MonotonicIdsAreUniqueAndStatusCallbacksAreReentrant) {
    OTCAgent agent("maker-1");
    agent.setMinTradeSize(1.0);
    std::atomic<int> callbacks{0};
    agent.setStatusCallback([&](const std::string&) {
        ++callbacks;
        EXPECT_FALSE(agent.getStatus().empty());
    });
    constexpr int threadCount = 8;
    static constexpr int offersPerThread = 25;
    std::vector<std::thread> threads;
    for (int thread = 0; thread < threadCount; ++thread) {
        threads.emplace_back([&agent, thread]() {
            for (int index = 0; index < offersPerThread; ++index) {
                const auto created = agent.createOfferResult(
                    "TOK" + std::to_string(thread), 1.0, 10.0 + index);
                EXPECT_EQ(created.status, OTCOperationStatus::PROPOSED);
            }
        });
    }
    for (auto& thread : threads) thread.join();
    const auto offers = agent.getMyOffers();
    ASSERT_EQ(offers.size(), static_cast<std::size_t>(threadCount * offersPerThread));
    std::set<std::string> ids;
    for (const auto& offer : offers) ids.insert(offer.offerId);
    EXPECT_EQ(ids.size(), offers.size());
    EXPECT_EQ(callbacks.load(), threadCount * offersPerThread);
}

TEST(OTCReentrancy, AllAdaptersCanQueryAgentWithoutDeadlocking) {
    Adapters adapters;
    auto agent = agentWith(adapters);
    auto reenter = [&]() { EXPECT_FALSE(agent->getStatus().empty()); };
    adapters.market->onCall = reenter;
    adapters.compliance->onCall = reenter;
    adapters.settlement->onSubmit = reenter;
    adapters.settlement->onVerify = reenter;
    adapters.negotiation->onCall = reenter;
    ASSERT_TRUE(agent->addObservedOfferResult(observedOffer()).accepted());
    auto negotiation = agent->startNegotiationResult("external-offer-1", "start");
    ASSERT_TRUE(negotiation.accepted());
    EXPECT_TRUE(agent->sendNegotiationMessageResult(negotiation.value, "hello",
                                                     std::nullopt, "message").accepted());
    auto offer = agent->getOffer("external-offer-1");
    offer.status = OfferStatus::ACTIVE;
    // Negotiation set the local offer to NEGOTIATING; acceptance correctly rejects until
    // the maker/taker workflow chooses a terminal negotiation state.
    EXPECT_EQ(agent->acceptOfferResult("external-offer-1", "accept").status,
              OTCOperationStatus::REJECTED);

    auto second = observedOffer("external-offer-2");
    ASSERT_TRUE(agent->addObservedOfferResult(second).accepted());
    ASSERT_TRUE(agent->acceptOfferResult("external-offer-2", "accept-2").accepted());
    const auto submitted = agent->initiateSettlementResult("external-offer-2", "submit-2");
    ASSERT_EQ(submitted.status, OTCOperationStatus::SUBMITTED);
    EXPECT_TRUE(agent->confirmSettlementResult("external-offer-2",
                                                submitted.value.transactionId,
                                                "verify-2").verified());
}

TEST(OTCEscrow, DefaultRejectsAndFakeReceiptsDriveValidTransitionsAndReplay) {
    EscrowManager unavailable;
    EXPECT_EQ(unavailable.createEscrowResult("maker", "taker", token("ETH"),
                                              1.0, OTCChain::BASE).status,
              OTCOperationStatus::UNAVAILABLE);
    auto settlement = std::make_shared<FakeSettlement>();
    auto market = std::make_shared<FakeMarketData>();
    EscrowManager escrow(settlement, market);
    auto created = escrow.createEscrowResult("maker", "taker", token("ETH"),
                                              1.0, OTCChain::BASE, "escrow-create");
    ASSERT_EQ(created.status, OTCOperationStatus::SUBMITTED);
    auto replay = escrow.createEscrowResult("maker", "taker", token("ETH"),
                                             1.0, OTCChain::BASE, "escrow-create");
    EXPECT_EQ(replay.value.escrowId, created.value.escrowId);
    EXPECT_EQ(settlement->submitCalls.load(), 1);
    auto deposited = escrow.depositToEscrowResult(created.value.escrowId,
                                                   "deposit-transaction",
                                                   "deposit-key");
    ASSERT_EQ(deposited.value.status, "funded");
    auto released = escrow.releaseEscrowResult(created.value.escrowId,
                                                "recipient", "release-key");
    EXPECT_EQ(released.status, OTCOperationStatus::VERIFIED);
    EXPECT_EQ(released.value.status, "released");
    EXPECT_EQ(escrow.refundEscrowResult(created.value.escrowId).status,
              OTCOperationStatus::REJECTED);
}

TEST(OTCTeardown, RepeatedConstructionAdapterReplacementAndDestructionAreSafe) {
    for (int iteration = 0; iteration < 200; ++iteration) {
        Adapters adapters;
        auto agent = agentWith(adapters, "agent-" + std::to_string(iteration));
        EXPECT_TRUE(agent->createOfferResult("ETH", 2.0, 100.0).accepted());
        agent->setOTCMarketDataAdapter(nullptr);
        agent->setNegotiationTransport(nullptr);
        agent->setComplianceAdapter(nullptr);
        agent->setSettlementAdapter(nullptr);
        EXPECT_EQ(agent->getMarketStatsResult("ETH", "USDC").status,
                  OTCOperationStatus::UNAVAILABLE);
    }
}
