// agentcomms_test.cpp - E2E tests for elizaos::AgentComms.
#include <gtest/gtest.h>
#include "elizaos/agentcomms.hpp"

#include <atomic>
#include <chrono>
#include <thread>

using namespace elizaos;

TEST(AgentCommsMessage, ConstructionAndMetadata) {
    Message m("id-1", MessageType::TEXT, "alice", "bob", "channel-1", "hello");
    EXPECT_EQ(m.id, "id-1");
    EXPECT_EQ(m.sender, "alice");
    EXPECT_EQ(m.receiver, "bob");

    m.setMetadata("priority", "high");
    EXPECT_TRUE(m.hasMetadata("priority"));
    EXPECT_EQ(m.getMetadata("priority"), "high");
    EXPECT_FALSE(m.hasMetadata("missing"));
    EXPECT_EQ(m.getMetadata("missing"), "");
}

TEST(AgentCommsParticipation, AddRemoveChannel) {
    AgentParticipation p("a-1");
    p.addChannelParticipation("c-1");
    EXPECT_TRUE(p.isParticipatingInChannel("c-1"));
    EXPECT_FALSE(p.isParticipatingInChannel("c-2"));
    p.removeChannelParticipation("c-1");
    EXPECT_FALSE(p.isParticipatingInChannel("c-1"));
}

TEST(AgentCommsParticipation, AddRemoveServer) {
    AgentParticipation p("a-1");
    p.addServerSubscription("s-1");
    EXPECT_TRUE(p.isSubscribedToServer("s-1"));
    p.removeServerSubscription("s-1");
    EXPECT_FALSE(p.isSubscribedToServer("s-1"));
}

TEST(AgentCommsUUID, GeneratesNonEmptyUuids) {
    auto u1 = UUIDMapper::generateUUID();
    auto u2 = UUIDMapper::generateUUID();
    EXPECT_FALSE(u1.empty());
    EXPECT_NE(u1, u2);
}

TEST(AgentCommsUUID, AgentSpecificUuidIsDeterministic) {
    auto a = UUIDMapper::createAgentSpecificUUID("agent-1", "res-x");
    auto b = UUIDMapper::createAgentSpecificUUID("agent-1", "res-x");
    auto c = UUIDMapper::createAgentSpecificUUID("agent-2", "res-x");
    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
}

class AgentCommsManagerTest : public ::testing::Test {
protected:
    AgentComms comms{"agent-1"};
};

TEST_F(AgentCommsManagerTest, AgentIdInitialization) {
    EXPECT_EQ(comms.getAgentId(), "agent-1");
    comms.setAgentId("agent-2");
    EXPECT_EQ(comms.getAgentId(), "agent-2");
}

TEST_F(AgentCommsManagerTest, CreateAndGetChannel) {
    auto ch = comms.createChannel("c-1", "s-1");
    ASSERT_NE(ch, nullptr);
    EXPECT_EQ(ch->getChannelId(), "c-1");
    EXPECT_EQ(ch->getServerId(), "s-1");

    auto got = comms.getChannel("c-1");
    EXPECT_NE(got, nullptr);
}

TEST_F(AgentCommsManagerTest, AddRemoveChannelParticipant) {
    comms.createChannel("c-1");
    EXPECT_TRUE(comms.addChannelParticipant("c-1", "alice"));
    EXPECT_TRUE(comms.isChannelParticipant("c-1", "alice"));
    EXPECT_TRUE(comms.removeChannelParticipant("c-1", "alice"));
    EXPECT_FALSE(comms.isChannelParticipant("c-1", "alice"));
}

TEST_F(AgentCommsManagerTest, ActiveChannelsList) {
    auto a = comms.createChannel("a");
    auto b = comms.createChannel("b");
    // Channels become "active" only after start().
    a->start();
    b->start();
    auto list = comms.getActiveChannels();
    EXPECT_EQ(list.size(), 2u);
    a->stop();
    b->stop();
}

TEST_F(AgentCommsManagerTest, RemoveChannel) {
    comms.createChannel("rem");
    comms.removeChannel("rem");
    EXPECT_EQ(comms.getChannel("rem"), nullptr);
}

TEST_F(AgentCommsManagerTest, ServerSubscription) {
    comms.subscribeToServer("srv-1", "agent-1");
    EXPECT_TRUE(comms.isSubscribedToServer("srv-1", "agent-1"));
    comms.unsubscribeFromServer("srv-1", "agent-1");
    EXPECT_FALSE(comms.isSubscribedToServer("srv-1", "agent-1"));
}

TEST_F(AgentCommsManagerTest, SendMessageDeliversToHandler) {
    auto ch = comms.createChannel("c-deliver");
    std::atomic<int> received{0};
    ch->setMessageHandler([&](const Message&) { ++received; });
    ch->start();

    Message m("m-1", MessageType::TEXT, "a", "b", "c-deliver", "payload");
    comms.sendMessage("c-deliver", m, false);

    // Allow the channel processing thread to run
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    ch->stop();
    EXPECT_GE(received.load(), 1);
}

TEST_F(AgentCommsManagerTest, BroadcastMessageReachesAllChannels) {
    auto a = comms.createChannel("c-a");
    auto b = comms.createChannel("c-b");
    std::atomic<int> total{0};
    a->setMessageHandler([&](const Message&) { ++total; });
    b->setMessageHandler([&](const Message&) { ++total; });
    a->start();
    b->start();

    Message m("bm-1", MessageType::TEXT, "x", "*", "*", "broadcast");
    comms.broadcastMessage(m, false);

    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    a->stop();
    b->stop();
    EXPECT_GE(total.load(), 2);
}
