// test_attention_bus.cpp
//
// Comprehensive end-to-end tests for the inter-agent attention coordination
// center: AttentionMessageBus + AttentionTransferProtocol bus integration.
//
// These tests exercise real cross-agent-boundary behavior (no mocks): focus
// broadcast fan-out, out-of-band listeners, routed transfer accept/reject,
// unknown-target handling, shared swarm pools, and automatic unregistration
// on protocol destruction.

#include <gtest/gtest.h>

#include "elizaos/attention.hpp"
#include "elizaos/core.hpp"

#include <atomic>
#include <memory>
#include <string>
#include <vector>

using namespace elizaos;

namespace {

using Request = AttentionTransferProtocol::AttentionTransferRequest;
using Response = AttentionTransferProtocol::AttentionTransferResponse;

// ---------------------------------------------------------------------------
// Registration semantics
// ---------------------------------------------------------------------------

TEST(AttentionMessageBus, RegistrationIsIdempotentAndCounted) {
    AttentionMessageBus bus;
    EXPECT_EQ(bus.agentCount(), 0u);

    AttentionTransferProtocol a("agent-a");
    AttentionTransferProtocol b("agent-b");

    a.connectToBus(&bus);
    b.connectToBus(&bus);
    EXPECT_EQ(bus.agentCount(), 2u);
    EXPECT_TRUE(bus.isRegistered("agent-a"));
    EXPECT_TRUE(bus.isRegistered("agent-b"));
    EXPECT_TRUE(a.isConnectedToBus());

    // Re-connecting the same agent must not create a duplicate endpoint.
    a.connectToBus(&bus);
    EXPECT_EQ(bus.agentCount(), 2u);

    a.disconnectFromBus();
    EXPECT_EQ(bus.agentCount(), 1u);
    EXPECT_FALSE(bus.isRegistered("agent-a"));
    EXPECT_FALSE(a.isConnectedToBus());
}

TEST(AttentionMessageBus, ProtocolDestructionUnregistersFromBus) {
    AttentionMessageBus bus;
    AttentionTransferProtocol keeper("keeper");
    keeper.connectToBus(&bus);

    {
        AttentionTransferProtocol transient("transient");
        transient.connectToBus(&bus);
        EXPECT_EQ(bus.agentCount(), 2u);
    }  // transient destroyed here

    EXPECT_EQ(bus.agentCount(), 1u);
    EXPECT_FALSE(bus.isRegistered("transient"));
    EXPECT_TRUE(bus.isRegistered("keeper"));
}

// ---------------------------------------------------------------------------
// Focus broadcast fan-out
// ---------------------------------------------------------------------------

TEST(AttentionMessageBus, BroadcastFocusReachesOtherAgentsNotSelf) {
    AttentionMessageBus bus;
    AttentionTransferProtocol a("a");
    AttentionTransferProtocol b("b");
    AttentionTransferProtocol c("c");
    a.connectToBus(&bus);
    b.connectToBus(&bus);
    c.connectToBus(&bus);

    std::vector<UUID> bReceived;
    std::vector<UUID> cReceived;
    int aLocalHandlerCalls = 0;
    std::string aLocalSource;

    a.setFocusBroadcastHandler([&](const std::string& src, const std::vector<UUID>&) {
        // By design the broadcaster's own local handler fires exactly once with
        // its own id (back-compat). It must NOT receive a *remote* fan-out copy.
        ++aLocalHandlerCalls;
        aLocalSource = src;
    });
    b.setFocusBroadcastHandler([&](const std::string& src, const std::vector<UUID>& items) {
        EXPECT_EQ(src, "a");
        bReceived = items;
    });
    c.setFocusBroadcastHandler([&](const std::string& src, const std::vector<UUID>& items) {
        EXPECT_EQ(src, "a");
        cReceived = items;
    });

    const std::vector<UUID> focus{"mem-1", "mem-2"};
    const size_t reached = a.broadcastFocus(focus);

    EXPECT_EQ(reached, 2u);            // remote fan-out to b and c only
    EXPECT_EQ(bReceived, focus);
    EXPECT_EQ(cReceived, focus);
    EXPECT_EQ(aLocalHandlerCalls, 1);  // local handler fired once, not via remote fan-out
    EXPECT_EQ(aLocalSource, "a");      // and it saw its own id as the source
}

TEST(AttentionMessageBus, BusLessBroadcastStillTriggersLocalHandler) {
    AttentionTransferProtocol solo("solo");
    bool localFired = false;
    solo.setFocusBroadcastHandler([&](const std::string& src, const std::vector<UUID>&) {
        EXPECT_EQ(src, "solo");
        localFired = true;
    });
    const size_t reached = solo.broadcastFocus({"x"});
    EXPECT_EQ(reached, 0u);   // no bus -> no remote endpoints
    EXPECT_TRUE(localFired);  // local handler still invoked
}

TEST(AttentionMessageBus, OutOfBandListenerObservesEveryBroadcast) {
    AttentionMessageBus bus;
    AttentionTransferProtocol a("a");
    AttentionTransferProtocol b("b");
    a.connectToBus(&bus);
    b.connectToBus(&bus);

    std::atomic<int> observed{0};
    std::string lastSource;
    const size_t sub = bus.subscribe([&](const std::string& src, const std::vector<UUID>&) {
        observed.fetch_add(1);
        lastSource = src;
    });

    a.broadcastFocus({"i"});
    b.broadcastFocus({"j"});
    EXPECT_EQ(observed.load(), 2);
    EXPECT_EQ(lastSource, "b");

    bus.unsubscribe(sub);
    a.broadcastFocus({"k"});
    EXPECT_EQ(observed.load(), 2);  // listener removed, count unchanged
}

// ---------------------------------------------------------------------------
// Routed transfer requests
// ---------------------------------------------------------------------------

TEST(AttentionMessageBus, TransferRoutedToTargetAgentHandler) {
    AttentionMessageBus bus;
    AttentionTransferProtocol sender("sender");
    AttentionTransferProtocol receiver("receiver");
    sender.connectToBus(&bus);
    receiver.connectToBus(&bus);

    // The receiver decides via its own installed handler.
    bool receiverSaw = false;
    receiver.setTransferHandler([&](const Request& req) {
        receiverSaw = true;
        EXPECT_EQ(req.sourceAgentId, "sender");
        EXPECT_EQ(req.targetAgentId, "receiver");
        Response r;
        r.accepted = (req.attentionAmount <= 50.0);
        r.actualAmount = r.accepted ? req.attentionAmount : 0.0;
        r.message = r.accepted ? "ok" : "too much";
        return r;
    });

    Request req;
    req.sourceAgentId = "sender";
    req.targetAgentId = "receiver";
    req.memoryOrTaskId = "task-9";
    req.attentionAmount = 25.0;
    req.reason = "collaboration";

    Response resp = sender.requestTransfer(req);
    EXPECT_TRUE(receiverSaw);
    EXPECT_TRUE(resp.accepted);
    EXPECT_DOUBLE_EQ(resp.actualAmount, 25.0);
    EXPECT_EQ(resp.message, "ok");

    // Both endpoints should now track the transfer.
    EXPECT_EQ(sender.getActiveTransfers().size(), 1u);
    EXPECT_EQ(receiver.getActiveTransfers().size(), 1u);
    EXPECT_TRUE(receiver.getActiveTransfers().front().isIncoming);
    EXPECT_FALSE(sender.getActiveTransfers().front().isIncoming);
}

TEST(AttentionMessageBus, TransferToUnknownTargetIsRejectedNotCrashing) {
    AttentionMessageBus bus;
    AttentionTransferProtocol sender("sender");
    sender.connectToBus(&bus);

    Request req;
    req.sourceAgentId = "sender";
    req.targetAgentId = "ghost";  // never registered
    req.attentionAmount = 5.0;

    Response resp = sender.requestTransfer(req);
    EXPECT_FALSE(resp.accepted);
    EXPECT_NE(resp.message.find("not registered"), std::string::npos);
    EXPECT_EQ(sender.getActiveTransfers().size(), 0u);
}

TEST(AttentionMessageBus, RoutedTransferRejectionPropagates) {
    AttentionMessageBus bus;
    AttentionTransferProtocol sender("sender");
    AttentionTransferProtocol receiver("receiver");
    sender.connectToBus(&bus);
    receiver.connectToBus(&bus);

    // Receiver uses the default policy (reject > 10.0).
    Request req;
    req.sourceAgentId = "sender";
    req.targetAgentId = "receiver";
    req.attentionAmount = 99.0;

    Response resp = sender.requestTransfer(req);
    EXPECT_FALSE(resp.accepted);
    EXPECT_EQ(sender.getActiveTransfers().size(), 0u);
    EXPECT_EQ(receiver.getActiveTransfers().size(), 0u);
}

TEST(AttentionMessageBus, BusLessTransferUsesLocalDefaultPolicy) {
    AttentionTransferProtocol solo("solo");
    Request small;
    small.targetAgentId = "solo";  // self/no bus -> local policy
    small.attentionAmount = 3.0;
    EXPECT_TRUE(solo.requestTransfer(small).accepted);

    Request big;
    big.targetAgentId = "solo";
    big.attentionAmount = 1000.0;
    EXPECT_FALSE(solo.requestTransfer(big).accepted);
}

// ---------------------------------------------------------------------------
// Shared swarm pools
// ---------------------------------------------------------------------------

TEST(AttentionMessageBus, SharedPoolContributeWithdrawBalance) {
    AttentionMessageBus bus;
    EXPECT_DOUBLE_EQ(bus.getSharedPoolBalance("swarm"), 0.0);

    bus.contributeToSharedPool("swarm", 40.0);
    bus.contributeToSharedPool("swarm", 10.0);
    EXPECT_DOUBLE_EQ(bus.getSharedPoolBalance("swarm"), 50.0);

    const double w = bus.withdrawFromSharedPool("swarm", 30.0);
    EXPECT_DOUBLE_EQ(w, 30.0);
    EXPECT_DOUBLE_EQ(bus.getSharedPoolBalance("swarm"), 20.0);

    // Over-withdraw is clamped to the available balance and drains the pool.
    const double w2 = bus.withdrawFromSharedPool("swarm", 1000.0);
    EXPECT_DOUBLE_EQ(w2, 20.0);
    EXPECT_DOUBLE_EQ(bus.getSharedPoolBalance("swarm"), 0.0);

    // Withdrawing from an empty/unknown pool yields zero, never negative.
    EXPECT_DOUBLE_EQ(bus.withdrawFromSharedPool("none", 5.0), 0.0);
}

TEST(AttentionMessageBus, ProcessWideInstanceIsUsable) {
    auto& bus = AttentionMessageBus::instance();
    const std::string poolId = "instance-pool-" + generateUUID();
    bus.contributeToSharedPool(poolId, 7.0);
    EXPECT_DOUBLE_EQ(bus.getSharedPoolBalance(poolId), 7.0);
    EXPECT_DOUBLE_EQ(bus.withdrawFromSharedPool(poolId, 7.0), 7.0);
}

}  // namespace
