// Real E2E tests for Phase 3.3 Advanced Inter-Agent Communication:
// NegotiationProtocol, ContractManager, GossipProtocol, SecureMessaging,
// MessageTracer, and EnhancedAgentComms. These classes were declared in
// agentcomms.hpp but previously had no implementation; these tests validate
// the completed behavior end to end with real state transitions (no mocks).

#include "elizaos/agentcomms.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

using namespace elizaos;

// ===========================================================================
// NegotiationProtocol
// ===========================================================================

TEST(NegotiationProtocolTest, ProposeCreatesPendingSession) {
    NegotiationProtocol nego("agent-a");
    NegotiationProtocol::NegotiationTerms terms;
    terms.terms["price"] = "100";
    terms.value = 100.0;
    terms.deadline = std::chrono::system_clock::now() + std::chrono::hours(1);

    const std::string sessionId = nego.propose("agent-b", terms);
    ASSERT_FALSE(sessionId.empty());

    auto session = nego.getSession(sessionId);
    ASSERT_TRUE(session.has_value());
    EXPECT_EQ(session->state,
              NegotiationProtocol::NegotiationState::PROPOSED);
    EXPECT_EQ(session->currentTerms.proposer, "agent-a");
    EXPECT_EQ(session->currentTerms.responder, "agent-b");
    EXPECT_EQ(session->currentTerms.terms.at("price"), "100");
    EXPECT_EQ(nego.getPendingSessions().size(), 1u);
    EXPECT_TRUE(nego.getCompletedSessions().empty());
}

TEST(NegotiationProtocolTest, AcceptCompletesSessionAndFiresCallback) {
    NegotiationProtocol nego("agent-a");
    std::atomic<int> responses{0};
    NegotiationProtocol::NegotiationState lastState{};
    nego.onResponse([&](const std::string&,
                        NegotiationProtocol::NegotiationState state) {
        lastState = state;
        responses++;
    });

    NegotiationProtocol::NegotiationTerms terms;
    terms.deadline = std::chrono::system_clock::now() + std::chrono::hours(1);
    const std::string sessionId = nego.propose("agent-b", terms);

    EXPECT_TRUE(nego.accept(sessionId));
    EXPECT_EQ(responses.load(), 1);
    EXPECT_EQ(lastState, NegotiationProtocol::NegotiationState::ACCEPTED);
    EXPECT_EQ(nego.getCompletedSessions().size(), 1u);
    EXPECT_TRUE(nego.getPendingSessions().empty());

    // Double-accept must be rejected: the session is already terminal.
    EXPECT_FALSE(nego.accept(sessionId));
}

TEST(NegotiationProtocolTest, RejectRecordsReason) {
    NegotiationProtocol nego("agent-a");
    NegotiationProtocol::NegotiationTerms terms;
    terms.deadline = std::chrono::system_clock::now() + std::chrono::hours(1);
    const std::string sessionId = nego.propose("agent-b", terms);

    EXPECT_TRUE(nego.reject(sessionId, "price too high"));
    auto session = nego.getSession(sessionId);
    ASSERT_TRUE(session.has_value());
    EXPECT_EQ(session->state,
              NegotiationProtocol::NegotiationState::REJECTED);
    EXPECT_EQ(session->currentTerms.terms.at("rejection_reason"),
              "price too high");
}

TEST(NegotiationProtocolTest, CounterOfferBoundedByMaxCounterOffers) {
    NegotiationProtocol nego("agent-a");
    NegotiationProtocol::NegotiationTerms terms;
    terms.maxCounterOffers = 2;
    terms.deadline = std::chrono::system_clock::now() + std::chrono::hours(1);
    const std::string sessionId = nego.propose("agent-b", terms);

    NegotiationProtocol::NegotiationTerms counter1 = terms;
    counter1.value = 90.0;
    EXPECT_TRUE(nego.counterOffer(sessionId, counter1));

    NegotiationProtocol::NegotiationTerms counter2 = terms;
    counter2.value = 95.0;
    EXPECT_TRUE(nego.counterOffer(sessionId, counter2));

    // Third counter exceeds maxCounterOffers=2 → session expires.
    NegotiationProtocol::NegotiationTerms counter3 = terms;
    EXPECT_FALSE(nego.counterOffer(sessionId, counter3));
    auto session = nego.getSession(sessionId);
    ASSERT_TRUE(session.has_value());
    EXPECT_EQ(session->state,
              NegotiationProtocol::NegotiationState::EXPIRED);
    // Full history preserved: original + 2 counters.
    EXPECT_EQ(session->history.size(), 3u);
}

TEST(NegotiationProtocolTest, ExpiredDeadlineBlocksAcceptance) {
    NegotiationProtocol nego("agent-a");
    NegotiationProtocol::NegotiationTerms terms;
    terms.deadline = std::chrono::system_clock::now() - std::chrono::hours(1);
    const std::string sessionId = nego.propose("agent-b", terms);

    EXPECT_FALSE(nego.accept(sessionId));
    auto session = nego.getSession(sessionId);
    ASSERT_TRUE(session.has_value());
    EXPECT_EQ(session->state,
              NegotiationProtocol::NegotiationState::EXPIRED);
}

TEST(NegotiationProtocolTest, UnknownSessionRejected) {
    NegotiationProtocol nego("agent-a");
    EXPECT_FALSE(nego.accept("no-such-session"));
    EXPECT_FALSE(nego.reject("no-such-session"));
    EXPECT_FALSE(nego.getSession("no-such-session").has_value());
}

// ===========================================================================
// ContractManager
// ===========================================================================

TEST(ContractManagerTest, FullSigningLifecycle) {
    ContractManager mgr;
    const std::string id = mgr.createContract(
        {"agent-a", "agent-b"}, {{"deliverable", "42 widgets"}});
    ASSERT_FALSE(id.empty());

    EXPECT_FALSE(mgr.isFullySigned(id));
    EXPECT_FALSE(mgr.verifyContract(id));

    EXPECT_TRUE(mgr.signContract(id, "agent-a", "sig-a"));
    EXPECT_FALSE(mgr.isFullySigned(id));

    EXPECT_TRUE(mgr.signContract(id, "agent-b", "sig-b"));
    EXPECT_TRUE(mgr.isFullySigned(id));
    EXPECT_TRUE(mgr.verifyContract(id));

    auto contract = mgr.getContract(id);
    ASSERT_TRUE(contract.has_value());
    EXPECT_TRUE(contract->fullyExecuted);
    EXPECT_EQ(contract->terms.at("deliverable"), "42 widgets");
}

TEST(ContractManagerTest, NonPartySignatureRejected) {
    ContractManager mgr;
    const std::string id = mgr.createContract({"agent-a"}, {});
    EXPECT_FALSE(mgr.signContract(id, "intruder", "sig-x"));
    EXPECT_FALSE(mgr.signContract(id, "agent-a", "")); // empty signature
    EXPECT_FALSE(mgr.signContract("unknown-contract", "agent-a", "sig"));
    EXPECT_TRUE(mgr.createContract({}, {}).empty()); // no parties
}

TEST(ContractManagerTest, ObligationLifecycle) {
    ContractManager mgr;
    const std::string id = mgr.createContract({"agent-a", "agent-b"}, {});

    ContractManager::ContractObligation ob;
    ob.contractId = id;
    ob.responsible = "agent-a";
    ob.description = "ship widgets";
    ob.deadline = std::chrono::system_clock::now() + std::chrono::hours(48);
    mgr.addObligation(ob);

    auto pending = mgr.getPendingObligations("agent-a");
    ASSERT_EQ(pending.size(), 1u);
    EXPECT_EQ(pending[0].description, "ship widgets");
    ASSERT_FALSE(pending[0].obligationId.empty());

    EXPECT_TRUE(mgr.fulfillObligation(pending[0].obligationId));
    EXPECT_FALSE(mgr.fulfillObligation(pending[0].obligationId)); // idempotent
    EXPECT_TRUE(mgr.getPendingObligations("agent-a").empty());
    EXPECT_FALSE(mgr.fulfillObligation("unknown-obligation"));
}

TEST(ContractManagerTest, QueryContractsForAgent) {
    ContractManager mgr;
    mgr.createContract({"agent-a", "agent-b"}, {});
    mgr.createContract({"agent-a", "agent-c"}, {});
    mgr.createContract({"agent-b", "agent-c"}, {});

    EXPECT_EQ(mgr.getContractsForAgent("agent-a").size(), 2u);
    EXPECT_EQ(mgr.getContractsForAgent("agent-c").size(), 2u);
    EXPECT_TRUE(mgr.getContractsForAgent("agent-z").empty());
}

// ===========================================================================
// GossipProtocol
// ===========================================================================

TEST(GossipProtocolTest, PeerManagement) {
    GossipProtocol gossip("agent-a", GossipProtocol::GossipConfig{});
    gossip.addPeer("agent-b");
    gossip.addPeer("agent-c");
    gossip.addPeer("agent-b"); // duplicate is a no-op
    EXPECT_EQ(gossip.getPeers().size(), 2u);

    gossip.removePeer("agent-b");
    auto peers = gossip.getPeers();
    ASSERT_EQ(peers.size(), 1u);
    EXPECT_EQ(peers[0], "agent-c");
}

TEST(GossipProtocolTest, SpreadDeliversToTopicSubscriberAndCountsStats) {
    GossipProtocol gossip("agent-a", GossipProtocol::GossipConfig{});
    gossip.addPeer("agent-b");
    gossip.addPeer("agent-c");

    std::atomic<int> received{0};
    std::string lastContent;
    gossip.subscribe("news", [&](const GossipProtocol::GossipMessage& msg) {
        lastContent = msg.content;
        EXPECT_EQ(msg.originAgent, "agent-a");
        EXPECT_EQ(msg.topic, "news");
        received++;
    });

    gossip.spread("hello village", "news");
    EXPECT_EQ(received.load(), 1);
    EXPECT_EQ(lastContent, "hello village");

    const auto stats = gossip.getStats();
    EXPECT_EQ(stats.messagesOriginated, 1u);
    EXPECT_EQ(stats.messagesForwarded, 2u); // fanout covers both peers

    gossip.unsubscribe("news");
    gossip.spread("second", "news");
    EXPECT_EQ(received.load(), 1); // unsubscribed handler not invoked
}

TEST(GossipProtocolTest, AntiEntropyStartStopIsPromptAndIdempotent) {
    GossipProtocol gossip("agent-a", GossipProtocol::GossipConfig{});
    const auto start = std::chrono::steady_clock::now();
    gossip.startAntiEntropy(std::chrono::milliseconds(10'000));
    gossip.startAntiEntropy(std::chrono::milliseconds(10'000)); // second is no-op
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    gossip.stopAntiEntropy();
    gossip.stopAntiEntropy(); // idempotent
    const auto elapsed = std::chrono::steady_clock::now() - start;
    // Even with a 10s interval the stop must interrupt the sleep promptly.
    EXPECT_LT(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed)
                  .count(),
              5'000);
}

// ===========================================================================
// SecureMessaging
// ===========================================================================

TEST(SecureMessagingTest, KeyGenerationAndPublicKeyExposure) {
    SecureMessaging alice("alice");
    EXPECT_TRUE(alice.getPublicKey().empty());
    const auto keys = alice.generateKeyPair();
    EXPECT_FALSE(keys.publicKey.empty());
    EXPECT_FALSE(keys.privateKey.empty());
    EXPECT_EQ(alice.getPublicKey(), keys.publicKey);
}

TEST(SecureMessagingTest, EncryptDecryptRoundTrip) {
    SecureMessaging alice("alice");
    SecureMessaging bob("bob");
    const auto aliceKeys = alice.generateKeyPair();
    const auto bobKeys = bob.generateKeyPair();
    alice.registerPeerPublicKey("bob", bobKeys.publicKey);
    bob.registerPeerPublicKey("alice", aliceKeys.publicKey);

    const std::string plaintext = "the eagle lands at dawn";
    const std::string ciphertext = alice.encrypt(plaintext, "bob");
    ASSERT_FALSE(ciphertext.empty());
    EXPECT_NE(ciphertext, plaintext);

    EXPECT_EQ(bob.decrypt(ciphertext), plaintext);
}

TEST(SecureMessagingTest, EncryptWithoutPeerKeyFails) {
    SecureMessaging alice("alice");
    alice.generateKeyPair();
    EXPECT_TRUE(alice.encrypt("secret", "stranger").empty());
}

TEST(SecureMessagingTest, SignAndVerify) {
    SecureMessaging alice("alice");
    const auto aliceKeys = alice.generateKeyPair();

    const std::string message = "I agree to the terms";
    const std::string signature = alice.sign(message);
    ASSERT_FALSE(signature.empty());

    // Self-verification.
    EXPECT_TRUE(alice.verify(message, signature, "alice"));
    // Tampered message must fail.
    EXPECT_FALSE(alice.verify("I agree to nothing", signature, "alice"));

    // Peer verification through registered public key.
    SecureMessaging bob("bob");
    bob.generateKeyPair();
    bob.registerPeerPublicKey("alice", aliceKeys.publicKey);
    EXPECT_TRUE(bob.verify(message, signature, "alice"));
    EXPECT_FALSE(bob.verify(message, "sig_forged", "alice"));
    EXPECT_FALSE(bob.verify(message, signature, "unknown-signer"));
}

TEST(SecureMessagingTest, SecureChannelEstablishment) {
    SecureMessaging alice("alice");
    SecureMessaging bob("bob");
    const auto bobKeys = bob.generateKeyPair();
    alice.generateKeyPair();

    EXPECT_FALSE(alice.hasSecureChannel("bob"));
    EXPECT_TRUE(alice.establishSecureChannel("bob").empty()); // no key yet

    alice.registerPeerPublicKey("bob", bobKeys.publicKey);
    const std::string channel = alice.establishSecureChannel("bob");
    EXPECT_FALSE(channel.empty());
    EXPECT_TRUE(alice.hasSecureChannel("bob"));
}

// ===========================================================================
// MessageTracer
// ===========================================================================

TEST(MessageTracerTest, TraceLifecycleRecordsEventsAndLatency) {
    MessageTracer tracer;
    tracer.startTrace("msg-1");
    tracer.addTraceEvent("msg-1", "routed");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    tracer.endTrace("msg-1", true);

    auto trace = tracer.getTrace("msg-1");
    ASSERT_TRUE(trace.has_value());
    EXPECT_TRUE(trace->delivered);
    EXPECT_GE(trace->events.size(), 3u); // started, routed, delivered
    EXPECT_GT(trace->totalLatencyMs, 0.0);

    const auto stats = tracer.getStats();
    EXPECT_EQ(stats.tracesStarted, 1u);
    EXPECT_EQ(stats.tracesCompleted, 1u);
    EXPECT_GT(stats.averageDeliveryLatency, 0.0);
}

TEST(MessageTracerTest, EventsOnUnknownTraceAreIgnored) {
    MessageTracer tracer;
    tracer.addTraceEvent("ghost", "event");
    tracer.endTrace("ghost", true);
    EXPECT_FALSE(tracer.getTrace("ghost").has_value());
    EXPECT_EQ(tracer.getStats().tracesCompleted, 0u);
}

TEST(MessageTracerTest, DeadLetterQueueAndRetry) {
    MessageTracer tracer;
    Message original;
    original.id = "msg-dead-1";
    original.content = "undeliverable";

    MessageTracer::DeadLetter dl;
    dl.originalMessage = original;
    dl.failureReason = "peer offline";
    dl.deliveryAttempts = 1;
    dl.lastAttempt = std::chrono::system_clock::now();
    tracer.addDeadLetter(dl);

    EXPECT_EQ(tracer.getDeadLetters().size(), 1u);
    EXPECT_EQ(tracer.getStats().deadLettersCount, 1u);

    EXPECT_TRUE(tracer.retryDeadLetter("msg-dead-1"));
    EXPECT_TRUE(tracer.getDeadLetters().empty());
    EXPECT_EQ(tracer.getStats().retriesAttempted, 1u);
    EXPECT_FALSE(tracer.retryDeadLetter("msg-dead-1")); // already retried
}

TEST(MessageTracerTest, PurgeDropsOnlyExpiredDeadLetters) {
    MessageTracer tracer;
    Message oldMsg;
    oldMsg.id = "old";
    MessageTracer::DeadLetter oldDl;
    oldDl.originalMessage = oldMsg;
    oldDl.lastAttempt =
        std::chrono::system_clock::now() - std::chrono::hours(48);
    tracer.addDeadLetter(oldDl);

    Message freshMsg;
    freshMsg.id = "fresh";
    MessageTracer::DeadLetter freshDl;
    freshDl.originalMessage = freshMsg;
    freshDl.lastAttempt = std::chrono::system_clock::now();
    tracer.addDeadLetter(freshDl);

    tracer.purgeDeadLetters(std::chrono::hours(24));
    auto remaining = tracer.getDeadLetters();
    ASSERT_EQ(remaining.size(), 1u);
    EXPECT_EQ(remaining[0].originalMessage.id, "fresh");
}

// ===========================================================================
// EnhancedAgentComms (integration)
// ===========================================================================

TEST(EnhancedAgentCommsTest, SubsystemAccessorsAreLive) {
    EnhancedAgentComms comms("agent-e");
    EXPECT_EQ(comms.getAgentId(), "agent-e");

    // Negotiation subsystem is functional through the facade.
    NegotiationProtocol::NegotiationTerms terms;
    terms.deadline = std::chrono::system_clock::now() + std::chrono::hours(1);
    const std::string sessionId =
        comms.getNegotiationProtocol().propose("agent-f", terms);
    EXPECT_TRUE(comms.getNegotiationProtocol().accept(sessionId));

    // Contract subsystem is functional through the facade.
    const std::string contractId = comms.getContractManager().createContract(
        {"agent-e", "agent-f"}, {{"scope", "test"}});
    EXPECT_TRUE(
        comms.getContractManager().signContract(contractId, "agent-e", "s1"));
    EXPECT_TRUE(
        comms.getContractManager().signContract(contractId, "agent-f", "s2"));
    EXPECT_TRUE(comms.getContractManager().verifyContract(contractId));

    const auto stats = comms.getEnhancedStats();
    EXPECT_EQ(stats.negotiationsCompleted, 1u);
    EXPECT_EQ(stats.contractsExecuted, 1u);
}

TEST(EnhancedAgentCommsTest, GossipEnableAndSpread) {
    EnhancedAgentComms comms("agent-g");
    GossipProtocol::GossipConfig config;
    config.fanout = 2;
    comms.enableGossipProtocol(config);
    comms.getGossipProtocol().addPeer("agent-h");

    std::atomic<int> seen{0};
    comms.getGossipProtocol().subscribe(
        "status", [&](const GossipProtocol::GossipMessage&) { seen++; });
    comms.spreadMessage("all systems nominal", "status");
    EXPECT_EQ(seen.load(), 1);
    EXPECT_EQ(comms.getEnhancedStats().gossipMessagesSpread, 1u);
}

TEST(EnhancedAgentCommsTest, EncryptedSendRequiresEnablement) {
    EnhancedAgentComms comms("agent-i");
    auto channel = comms.createChannel("secure-chan");
    ASSERT_NE(channel, nullptr);
    channel->start();

    std::atomic<int> received{0};
    std::string receivedContent;
    channel->setMessageHandler([&](const Message& m) {
        receivedContent = m.content;
        received++;
    });

    Message msg;
    msg.id = "enc-1";
    msg.sender = "agent-i";
    msg.receiver = "agent-j";
    msg.channel_id = "secure-chan";
    msg.content = "classified payload";

    // Without enableEncryption() the send must be refused (no plaintext
    // downgrade) — nothing arrives on the channel.
    comms.sendEncrypted(msg);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(received.load(), 0);

    // After enabling encryption and registering the peer key, the encrypted
    // payload flows through the real channel and differs from the plaintext.
    comms.enableEncryption();
    SecureMessaging peer("agent-j");
    const auto peerKeys = peer.generateKeyPair();
    comms.getSecureMessaging().registerPeerPublicKey("agent-j",
                                                     peerKeys.publicKey);
    comms.sendEncrypted(msg);
    for (int i = 0; i < 100 && received.load() == 0; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    EXPECT_EQ(received.load(), 1);
    EXPECT_FALSE(receivedContent.empty());
    EXPECT_NE(receivedContent, "classified payload");

    channel->stop();
}

TEST(EnhancedAgentCommsTest, TracedDeliveryThroughRealChannel) {
    EnhancedAgentComms comms("agent-k");
    comms.enableTracing();
    auto channel = comms.createChannel("traced-chan");
    ASSERT_NE(channel, nullptr);
    channel->start();

    std::atomic<int> received{0};
    channel->setMessageHandler([&](const Message&) { received++; });

    Message msg;
    msg.id = "traced-1";
    msg.sender = "agent-k";
    msg.channel_id = "traced-chan";
    msg.content = "observable payload";

    const std::string traceId = comms.traceMessage(msg);
    EXPECT_EQ(traceId, "traced-1");
    for (int i = 0; i < 100 && received.load() == 0; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    EXPECT_EQ(received.load(), 1);

    auto trace = comms.getMessageTracer().getTrace(traceId);
    ASSERT_TRUE(trace.has_value());
    EXPECT_TRUE(trace->delivered);

    // Delivery to a nonexistent channel is recorded as a dead letter.
    Message lost;
    lost.id = "traced-lost";
    lost.channel_id = "no-such-channel";
    const std::string lostTrace = comms.traceMessage(lost);
    EXPECT_FALSE(lostTrace.empty());
    auto deadLetters = comms.getMessageTracer().getDeadLetters();
    ASSERT_EQ(deadLetters.size(), 1u);
    EXPECT_EQ(deadLetters[0].originalMessage.id, "traced-lost");

    channel->stop();
}

TEST(EnhancedAgentCommsTest, TracingDisabledReturnsEmptyTraceId) {
    EnhancedAgentComms comms("agent-l");
    Message msg;
    msg.id = "untraced";
    EXPECT_TRUE(comms.traceMessage(msg).empty());
}
