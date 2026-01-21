#include <gtest/gtest.h>
#include "elizaos/agentcomms.hpp"
#include <thread>
#include <chrono>
#include <atomic>

using namespace elizaos;

class AgentCommsTest : public ::testing::Test {
protected:
    void SetUp() override {
        comms = std::make_shared<AgentComms>();
    }
    
    void TearDown() override {
        comms->stop();
    }
    
    std::shared_ptr<AgentComms> comms;
};

TEST_F(AgentCommsTest, MessageCreation) {
    // Test basic message creation
    Message msg("test_id", MessageType::TEXT, "sender", "receiver", "test content");
    
    EXPECT_EQ(msg.id, "test_id");
    EXPECT_EQ(msg.type, MessageType::TEXT);
    EXPECT_EQ(msg.sender, "sender");
    EXPECT_EQ(msg.receiver, "receiver");
    EXPECT_EQ(msg.content, "test content");
    // Timestamp should be std::set (check it's not default value)
    auto epoch = std::chrono::system_clock::time_point{}; 
    EXPECT_NE(msg.timestamp, epoch);
}

TEST_F(AgentCommsTest, MessageAutoId) {
    // Test automatic ID generation
    Message msg1("", MessageType::TEXT, "sender", "receiver", "content1");
    // Add small delay to ensure different timestamps
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    Message msg2("", MessageType::TEXT, "sender", "receiver", "content2");
    
    EXPECT_FALSE(msg1.id.empty());
    EXPECT_FALSE(msg2.id.empty());
    EXPECT_NE(msg1.id, msg2.id); // Should be different
}

TEST_F(AgentCommsTest, ChannelCreation) {
    // Test channel creation
    auto channel = comms->createChannel("test_channel");
    
    ASSERT_NE(channel, nullptr);
    EXPECT_EQ(channel->getChannelId(), "test_channel");
    EXPECT_FALSE(channel->isActive()); // Should not be active initially
}

TEST_F(AgentCommsTest, ChannelRetrieval) {
    // Test channel retrieval
    auto channel1 = comms->createChannel("test_channel");
    auto channel2 = comms->getChannel("test_channel");
    
    EXPECT_EQ(channel1, channel2); // Should be the same instance
    
    auto nonexistent = comms->getChannel("nonexistent");
    EXPECT_EQ(nonexistent, nullptr);
}

TEST_F(AgentCommsTest, ChannelRemoval) {
    // Test channel removal
    comms->createChannel("test_channel");
    auto channel = comms->getChannel("test_channel");
    ASSERT_NE(channel, nullptr);
    
    comms->removeChannel("test_channel");
    auto removedChannel = comms->getChannel("test_channel");
    EXPECT_EQ(removedChannel, nullptr);
}

TEST_F(AgentCommsTest, MessageSending) {
    // Test message sending through channels
    auto channel = comms->createChannel("test_channel");
    comms->start();
    
    Message msg("", MessageType::TEXT, "sender", "receiver", "test message");
    bool sent = comms->sendMessage("test_channel", msg);
    
    EXPECT_TRUE(sent);
}

TEST_F(AgentCommsTest, MessageHandling) {
    // Test message handling
    auto channel = comms->createChannel("test_channel");
    
    std::atomic<bool> messageReceived(false);
    std::string receivedContent;
    
    channel->setMessageHandler([&](const Message& msg) {
        receivedContent = msg.content;
        messageReceived = true;
    });
    
    comms->start();
    
    Message msg("", MessageType::TEXT, "sender", "receiver", "test message");
    channel->sendMessage(msg);
    
    // Wait a bit for message processing
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    EXPECT_TRUE(messageReceived);
    EXPECT_EQ(receivedContent, "test message");
}

TEST_F(AgentCommsTest, GlobalMessageHandler) {
    // Test global message handler
    std::atomic<int> messagesReceived(0);
    
    comms->setGlobalMessageHandler([&](const Message&) {
        messagesReceived++;
    });
    
    auto channel1 = comms->createChannel("channel1");
    auto channel2 = comms->createChannel("channel2");
    
    comms->start();
    
    Message msg1("", MessageType::TEXT, "sender", "receiver", "message1");
    Message msg2("", MessageType::TEXT, "sender", "receiver", "message2");
    
    channel1->sendMessage(msg1);
    channel2->sendMessage(msg2);
    
    // Wait for processing
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    EXPECT_EQ(messagesReceived, 2);
}

TEST_F(AgentCommsTest, BroadcastMessage) {
    // Test message broadcasting
    std::atomic<int> messagesReceived(0);
    
    auto handler = [&](const Message&) {
        messagesReceived++;
    };
    
    comms->setGlobalMessageHandler(handler);
    
    auto channel1 = comms->createChannel("channel1");
    auto channel2 = comms->createChannel("channel2");
    auto channel3 = comms->createChannel("channel3");
    
    comms->start();
    
    Message broadcastMsg("", MessageType::TEXT, "broadcaster", "", "broadcast message");
    comms->broadcastMessage(broadcastMsg);
    
    // Wait for processing
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    EXPECT_EQ(messagesReceived, 3); // Should receive on all 3 channels
}

TEST_F(AgentCommsTest, ActiveChannels) {
    // Test getting active channels list
    comms->createChannel("channel1");
    comms->createChannel("channel2");
    comms->createChannel("channel3");
    
    comms->start();
    
    auto activeChannels = comms->getActiveChannels();
    EXPECT_EQ(activeChannels.size(), 3);
    
    // Check all channel names are present
    std::sort(activeChannels.begin(), activeChannels.end());
    EXPECT_EQ(activeChannels[0], "channel1");
    EXPECT_EQ(activeChannels[1], "channel2");
    EXPECT_EQ(activeChannels[2], "channel3");
}

TEST_F(AgentCommsTest, StartStopCycle) {
    // Test starting and stopping communication system
    auto channel = comms->createChannel("test_channel");
    
    EXPECT_FALSE(channel->isActive());
    
    comms->start();
    EXPECT_TRUE(channel->isActive());
    
    comms->stop();
    EXPECT_FALSE(channel->isActive());
}

TEST_F(AgentCommsTest, TCPConnector) {
    // Test basic TCP connector functionality
    TCPConnector connector;
    
    EXPECT_FALSE(connector.isConnected());
    
    bool connected = connector.connect("localhost:8080");
    EXPECT_TRUE(connected);
    EXPECT_TRUE(connector.isConnected());
    
    bool sent = connector.sendData("test data");
    EXPECT_TRUE(sent);
    
    connector.disconnect();
    EXPECT_FALSE(connector.isConnected());
}

TEST_F(AgentCommsTest, ConvenienceFunctions) {
    // Test convenience functions
    initializeComms();
    
    // Should not crash
    sendAgentMessage("test_channel", "test content", "test_sender");
    // May return false since channel doesn't exist, but shouldn't crash
    
    std::atomic<bool> handlerCalled(false);
    setGlobalMessageReceiver([&](const Message&) {
        handlerCalled = true;
    });
    
    shutdownComms();
    
    EXPECT_TRUE(true); // Test completed without crashing
}