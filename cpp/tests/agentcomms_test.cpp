// agentcomms_test.cpp - E2E tests for elizaos::AgentComms.
#include <gtest/gtest.h>
#include "elizaos/agentcomms.hpp"

#include <atomic>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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


namespace {
class AgentCommsLoopbackTcpServer {
public:
    AgentCommsLoopbackTcpServer() {
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
        worker_ = std::thread(&AgentCommsLoopbackTcpServer::serveOneClient, this);
    }

    ~AgentCommsLoopbackTcpServer() { stop(); }

    std::string connectionString() const {
        return "tcp://127.0.0.1:" + std::to_string(port_);
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

TEST_F(AgentCommsManagerTest, ValidationEnforcesConfiguredParticipationAndSubscription) {
    auto allowed = comms.createChannel("allowed-channel", "srv-allowed");
    auto blocked = comms.createChannel("blocked-channel", "srv-allowed");
    ASSERT_NE(allowed, nullptr);
    ASSERT_NE(blocked, nullptr);

    comms.addChannelParticipant("allowed-channel", "agent-1");
    comms.subscribeToServer("srv-allowed", "agent-1");

    std::atomic<int> received{0};
    allowed->setMessageHandler([&](const Message&) { ++received; });
    allowed->start();
    blocked->start();

    Message ok("ok-1", MessageType::TEXT, "peer", "agent-1", "allowed-channel", "payload");
    ok.server_id = "srv-allowed";
    EXPECT_TRUE(comms.sendMessage("allowed-channel", ok));

    Message wrongChannel("wrong-channel-1", MessageType::TEXT, "peer", "agent-1", "blocked-channel", "payload");
    wrongChannel.server_id = "srv-allowed";
    EXPECT_FALSE(comms.sendMessage("blocked-channel", wrongChannel));

    Message wrongServer("wrong-server-1", MessageType::TEXT, "peer", "agent-1", "allowed-channel", "payload");
    wrongServer.server_id = "srv-blocked";
    EXPECT_FALSE(comms.sendMessage("allowed-channel", wrongServer));

    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    allowed->stop();
    blocked->stop();
    EXPECT_GE(received.load(), 1);
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


TEST_F(AgentCommsManagerTest, TCPConnectorRejectsMalformedEndpoint) {
    TCPConnector connector;

    EXPECT_FALSE(connector.isConnected());
    EXPECT_FALSE(connector.connect("localhost"));
    EXPECT_FALSE(connector.isConnected());
    EXPECT_FALSE(connector.sendData("must-not-send"));
}

TEST_F(AgentCommsManagerTest, TCPConnectorLoopbackE2E) {
    AgentCommsLoopbackTcpServer server;
    TCPConnector connector;

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

    const std::string payload = "focused-tcp-authenticity";
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
