#include <gtest/gtest.h>
#include "elizaos/agentcomms.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <algorithm>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <sstream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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

namespace {
class LoopbackTcpServer {
public:
    LoopbackTcpServer() {
        serverFd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        EXPECT_GE(serverFd_, 0);
        int opt = 1;
        EXPECT_EQ(::setsockopt(serverFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), 0);

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(0);
        EXPECT_EQ(::bind(serverFd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)), 0);
        EXPECT_EQ(::listen(serverFd_, 1), 0);

        socklen_t len = sizeof(addr);
        EXPECT_EQ(::getsockname(serverFd_, reinterpret_cast<sockaddr*>(&addr), &len), 0);
        port_ = ntohs(addr.sin_port);
        running_.store(true);
        worker_ = std::thread(&LoopbackTcpServer::serveOneClient, this);
    }

    ~LoopbackTcpServer() { stop(); }

    std::string connectionString() const {
        return "127.0.0.1:" + std::to_string(port_);
    }

    bool waitForPayload(const std::string& expected, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        return cv_.wait_for(lock, timeout, [&] { return received_.find(expected) != std::string::npos; });
    }

    void stop() {
        if (!running_.exchange(false)) return;
        if (serverFd_ >= 0) {
            ::shutdown(serverFd_, SHUT_RDWR);
            ::close(serverFd_);
            serverFd_ = -1;
        }
        if (clientFd_ >= 0) {
            ::shutdown(clientFd_, SHUT_RDWR);
            ::close(clientFd_);
            clientFd_ = -1;
        }
        if (worker_.joinable()) worker_.join();
    }

private:
    void serveOneClient() {
        clientFd_ = ::accept(serverFd_, nullptr, nullptr);
        if (clientFd_ < 0) return;
        char buffer[1024];
        const ssize_t n = ::recv(clientFd_, buffer, sizeof(buffer), 0);
        if (n > 0) {
            std::string payload(buffer, static_cast<std::size_t>(n));
            {
                std::lock_guard<std::mutex> lock(mutex_);
                received_ += payload;
            }
            cv_.notify_all();
            const std::string response = "ack:" + payload;
            (void)::send(clientFd_, response.data(), response.size(), MSG_NOSIGNAL);
        }
    }

    int serverFd_{-1};
    int clientFd_{-1};
    uint16_t port_{0};
    std::atomic<bool> running_{false};
    std::thread worker_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::string received_;
};
}

TEST_F(AgentCommsTest, TCPConnectorLoopbackE2E) {
    LoopbackTcpServer server;
    TCPConnector connector;
    
    EXPECT_FALSE(connector.isConnected());

    std::mutex callbackMutex;
    std::condition_variable callbackCv;
    std::string callbackData;
    connector.setDataHandler([&](const std::string& data) {
        {
            std::lock_guard<std::mutex> lock(callbackMutex);
            callbackData += data;
        }
        callbackCv.notify_all();
    });
    
    ASSERT_TRUE(connector.connect(server.connectionString()));
    EXPECT_TRUE(connector.isConnected());
    
    const std::string payload = "tcp-e2e-message";
    ASSERT_TRUE(connector.sendData(payload));
    EXPECT_TRUE(server.waitForPayload(payload, std::chrono::milliseconds(2000)));

    {
        std::unique_lock<std::mutex> lock(callbackMutex);
        ASSERT_TRUE(callbackCv.wait_for(lock, std::chrono::milliseconds(2000), [&] {
            return callbackData.find("ack:" + payload) != std::string::npos;
        }));
    }
    
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