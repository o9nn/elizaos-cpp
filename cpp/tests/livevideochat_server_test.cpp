#include <gtest/gtest.h>

#include "elizaos/livevideochat_server.hpp"

#include <nlohmann/json.hpp>

#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#ifndef _WIN32
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

using namespace elizaos;
using Json = nlohmann::json;

namespace {

HttpRequest jsonRequest(std::string method, std::string path, std::string body) {
    HttpRequest request;
    request.method = std::move(method);
    request.path = std::move(path);
    request.body = std::move(body);
    request.headers["content-type"] = "application/json";
    return request;
}

class RecordingAdapter final : public WebSocketClientAdapter {
public:
    explicit RecordingAdapter(bool accept = true) : accept_(accept) {}

    bool send(const std::string& message) override {
        std::lock_guard<std::mutex> lock(mutex_);
        messages_.push_back(message);
        return accept_;
    }

    std::size_t count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return messages_.size();
    }

private:
    bool accept_;
    mutable std::mutex mutex_;
    std::vector<std::string> messages_;
};

#ifndef _WIN32
std::string socketRequest(int port, const std::string& wire) {
    const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return {};
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<std::uint16_t>(port));
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (::connect(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        ::close(fd);
        return {};
    }
    std::size_t sent = 0;
    while (sent < wire.size()) {
        const ssize_t written = ::send(fd, wire.data() + sent, wire.size() - sent, 0);
        if (written <= 0) {
            ::close(fd);
            return {};
        }
        sent += static_cast<std::size_t>(written);
    }
    ::shutdown(fd, SHUT_WR);
    std::string result;
    char buffer[1024];
    while (true) {
        const ssize_t received = ::recv(fd, buffer, sizeof(buffer), 0);
        if (received <= 0) break;
        result.append(buffer, static_cast<std::size_t>(received));
    }
    ::close(fd);
    return result;
}
#endif

VideoChatConfig serverOnlyConfig() {
    VideoChatConfig config;
    config.enable_video = false;
    config.enable_audio = false;
    return config;
}

} // namespace

TEST(LiveVideoChatServerTest, LoopbackHealthInfoRoutesAndRestart) {
#ifndef _WIN32
    LiveVideoChatServer server;
    ASSERT_TRUE(server.initialize(0));
    server.registerRoute("POST", "/echo", [](const HttpRequest& request) {
        HttpResponse response;
        response.body = Json{{"body", request.body}, {"query", request.query_params.at("q")}}.dump();
        return response;
    });
    ASSERT_TRUE(server.start());
    ASSERT_GT(server.getPort(), 0);

    const auto health = socketRequest(
        server.getPort(), "GET /health HTTP/1.1\r\nHost: localhost\r\n\r\n");
    EXPECT_NE(health.find("200 OK"), std::string::npos);
    EXPECT_NE(health.find("\"healthy\""), std::string::npos);

    const auto info = socketRequest(
        server.getPort(), "GET /api/info HTTP/1.1\r\nHost: localhost\r\n\r\n");
    EXPECT_NE(info.find("200 OK"), std::string::npos);
    EXPECT_NE(info.find(std::to_string(server.getPort())), std::string::npos);

    const std::string body = R"({"hello":"world"})";
    const auto echo = socketRequest(
        server.getPort(), "POST /echo?q=ok HTTP/1.1\r\nHost: localhost\r\nContent-Length: " +
                              std::to_string(body.size()) + "\r\n\r\n" + body);
    EXPECT_NE(echo.find("200 OK"), std::string::npos);
    EXPECT_NE(echo.find("\\\"hello\\\""), std::string::npos);
    EXPECT_NE(echo.find("\"query\":\"ok\""), std::string::npos);

    const auto wrong_method = socketRequest(
        server.getPort(), "PUT /health HTTP/1.1\r\nHost: localhost\r\n\r\n");
    EXPECT_NE(wrong_method.find("405 Method Not Allowed"), std::string::npos);

    server.stop();
    EXPECT_FALSE(server.isRunning());
    ASSERT_TRUE(server.start());
    EXPECT_TRUE(server.isRunning());
    server.stop();
#else
    GTEST_SKIP() << "POSIX loopback socket test";
#endif
}

TEST(LiveVideoChatServerTest, MalformedAndOversizedSocketRequestsAreRejected) {
#ifndef _WIN32
    LiveVideoChatServer server;
    ASSERT_TRUE(server.initialize(0));
    ASSERT_TRUE(server.start());
    const int port = server.getPort();

    EXPECT_NE(socketRequest(port, "GET /health HTTP/1.1\r\n\r\n").find("400 Bad Request"),
              std::string::npos);
    EXPECT_NE(socketRequest(port,
                            "POST /health HTTP/1.1\r\nHost: localhost\r\nContent-Length: nope\r\n\r\n")
                  .find("400 Bad Request"),
              std::string::npos);
    EXPECT_NE(socketRequest(port,
                            "POST /health HTTP/1.1\r\nHost: localhost\r\nContent-Length: 1048577\r\n\r\n")
                  .find("413 Payload Too Large"),
              std::string::npos);
    EXPECT_NE(socketRequest(port,
                            "POST /health HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n")
                  .find("400 Bad Request"),
              std::string::npos);
#else
    GTEST_SKIP() << "POSIX loopback socket test";
#endif
}

TEST(LiveVideoChatServerTest, HandlerDefaultsInjectionValidationAndFailure) {
    LiveVideoChatWithServer chat;
    ASSERT_TRUE(chat.initialize(serverOnlyConfig(), 0));

    const auto agent = jsonRequest("POST", "/alice/message", R"({"message":"hello"})");
    const auto transcription = jsonRequest("POST", "/alice/whisper", R"({"audio":"AA=="})");
    const auto signaling = jsonRequest(
        "POST", "/webrtc/signaling", R"({"type":"offer","sdp":"v=0"})");
    EXPECT_EQ(chat.handleAgentMessage(agent).status_code, 501);
    EXPECT_EQ(chat.handleWhisperTranscription(transcription).status_code, 501);
    EXPECT_EQ(chat.handleWebRTCSignaling(signaling).status_code, 501);

    chat.setAgentMessageHandler([](const HttpRequest& request) {
        HttpResponse response;
        response.status_code = 201;
        response.body = Json{{"accepted", Json::parse(request.body)["message"]}}.dump();
        return response;
    });
    EXPECT_EQ(chat.handleAgentMessage(agent).status_code, 201);

    chat.setTranscriptionHandler([](const HttpRequest&) -> HttpResponse {
        throw std::runtime_error("backend unavailable");
    });
    EXPECT_EQ(chat.handleWhisperTranscription(transcription).status_code, 503);

    chat.setSignalingHandler([](const HttpRequest&) {
        HttpResponse invalid;
        invalid.body = "fabricated non-json";
        return invalid;
    });
    EXPECT_EQ(chat.handleWebRTCSignaling(signaling).status_code, 503);

    auto malformed = agent;
    malformed.body = "not-json";
    EXPECT_EQ(chat.handleAgentMessage(malformed).status_code, 400);
    auto wrong_type = agent;
    wrong_type.headers.clear();
    EXPECT_EQ(chat.handleAgentMessage(wrong_type).status_code, 415);
    EXPECT_EQ(chat.handleAgentMessage(jsonRequest("POST", "/alice/message", R"({"message":""})"))
                  .status_code,
              422);
}

TEST(LiveVideoChatServerTest, SessionRegistryCrudAndConcurrentAccess) {
    LiveVideoChatWithServer chat;
    ASSERT_TRUE(chat.initialize(serverOnlyConfig(), 0));
    auto server = chat.getServer();
    ASSERT_NE(server, nullptr);

    constexpr int session_count = 32;
    std::vector<std::thread> threads;
    std::atomic<int> created{0};
    for (int index = 0; index < session_count; ++index) {
        threads.emplace_back([server, index, &created] {
            const auto request = jsonRequest(
                "POST", "/sessions",
                Json{{"session_id", "session-" + std::to_string(index)},
                     {"participants", Json::array({"alice", "bob"})}}
                    .dump());
            if (server->handleRequest(request).status_code == 201) ++created;
        });
    }
    for (auto& thread : threads) thread.join();
    EXPECT_EQ(created.load(), session_count);

    const auto list = server->handleRequest(HttpRequest{"GET", "/sessions", {}, {}, {}});
    ASSERT_EQ(list.status_code, 200);
    const auto sessions = Json::parse(list.body);
    EXPECT_EQ(sessions["total"].get<int>(), session_count);

    EXPECT_EQ(server->handleRequest(jsonRequest(
                  "POST", "/sessions", R"({"session_id":"session-0"})"))
                  .status_code,
              409);
    EXPECT_EQ(server->handleRequest(HttpRequest{"DELETE", "/sessions/session-0", {}, {}, {}})
                  .status_code,
              200);
    EXPECT_EQ(server->handleRequest(HttpRequest{"DELETE", "/sessions/session-0", {}, {}, {}})
                  .status_code,
              404);
    EXPECT_EQ(server->handleRequest(jsonRequest("POST", "/sessions", R"({"session_id":"bad/id"})"))
                  .status_code,
              422);
}

TEST(LiveVideoChatServerTest, ClientAdapterRequiresAcknowledgmentAndCallbacksRunOutsideLocks) {
    LiveVideoChatServer server;
    auto accepting = std::make_shared<RecordingAdapter>();
    auto rejecting = std::make_shared<RecordingAdapter>(false);
    EXPECT_FALSE(server.sendWebSocketMessage("alice", "hello"));
    ASSERT_TRUE(server.registerWebSocketClient("alice", accepting));
    ASSERT_TRUE(server.registerWebSocketClient("bob", rejecting));
    EXPECT_FALSE(server.registerWebSocketClient("alice", accepting));
    EXPECT_FALSE(server.sendWebSocketMessage("alice", "hello"));
    ASSERT_TRUE(server.acknowledgeWebSocketClient("alice"));
    ASSERT_TRUE(server.acknowledgeWebSocketClient("bob"));
    EXPECT_TRUE(server.sendWebSocketMessage("alice", "hello"));
    EXPECT_FALSE(server.sendWebSocketMessage("bob", "hello"));

    std::atomic<int> inbound{0};
    server.setWebSocketHandler([&](const std::string& id, const std::string&) {
        ++inbound;
        server.unregisterWebSocketClient(id);
    });
    EXPECT_TRUE(server.handleWebSocketMessage("alice", "from-client"));
    EXPECT_EQ(inbound.load(), 1);
    EXPECT_FALSE(server.sendWebSocketMessage("alice", "after-unregister"));

    server.broadcastMessage("broadcast");
    EXPECT_EQ(accepting->count(), 1U);
    EXPECT_EQ(rejecting->count(), 2U);
}

TEST(LiveVideoChatServerTest, TeardownIsPromptAndExternalServerReferenceFailsSafely) {
#ifndef _WIN32
    {
        LiveVideoChatServer server;
        ASSERT_TRUE(server.initialize(0));
        ASSERT_TRUE(server.start());
        const int client = ::socket(AF_INET, SOCK_STREAM, 0);
        ASSERT_GE(client, 0);
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_port = htons(static_cast<std::uint16_t>(server.getPort()));
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        ASSERT_EQ(::connect(client, reinterpret_cast<sockaddr*>(&address), sizeof(address)), 0);
        ASSERT_EQ(::send(client, "GET /health", 11, 0), 11);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        const auto started = std::chrono::steady_clock::now();
        server.stop();
        EXPECT_LT(std::chrono::steady_clock::now() - started, std::chrono::milliseconds(500));
        ::close(client);
    }

    {
        LiveVideoChatServer server;
        ASSERT_TRUE(server.initialize(0));
        server.registerRoute("GET", "/stop", [&server](const HttpRequest&) {
            server.stop();
            HttpResponse response;
            response.body = R"({"stopped":true})";
            return response;
        });
        ASSERT_TRUE(server.start());
        const auto stopped = socketRequest(
            server.getPort(), "GET /stop HTTP/1.1\r\nHost: localhost\r\n\r\n");
        EXPECT_NE(stopped.find("200 OK"), std::string::npos);
        server.stop();
        ASSERT_TRUE(server.start());
        server.stop();
    }

    for (int iteration = 0; iteration < 30; ++iteration) {
        const auto started = std::chrono::steady_clock::now();
        {
            LiveVideoChatServer server;
            ASSERT_TRUE(server.initialize(0));
            ASSERT_TRUE(server.start());
        }
        EXPECT_LT(std::chrono::steady_clock::now() - started, std::chrono::seconds(2));
    }
#endif

    std::shared_ptr<LiveVideoChatServer> server;
    {
        LiveVideoChatWithServer chat;
        ASSERT_TRUE(chat.initialize(serverOnlyConfig(), 0));
        server = chat.getServer();
    }
    ASSERT_NE(server, nullptr);
    const auto unavailable = server->handleRequest(
        jsonRequest("POST", "/alice/message", R"({"message":"hello"})"));
    EXPECT_EQ(unavailable.status_code, 503);
}
