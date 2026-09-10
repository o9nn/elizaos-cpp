#include <gtest/gtest.h>
#include "elizaos/mcp_gateway.hpp"

#include <atomic>
#include <condition_variable>
#include <deque>
#include <future>
#include <mutex>
#include <thread>
#include <vector>

using namespace elizaos;
using namespace std::chrono_literals;

namespace {
class ScriptedAdapter final : public WebSocketTransport::Adapter {
public:
    bool accept = true;
    bool acknowledge = true;
    bool autoReply = true;
    bool blockReceive = false;
    std::atomic<int> connects{0};
    std::atomic<int> closes{0};

    bool connect(const std::string&, const std::string&, std::chrono::milliseconds,
                 std::string& ack, std::string& error) override {
        ++connects;
        std::lock_guard<std::mutex> lock(mutex_);
        closed_ = false;
        if (!accept) { error = "rejected by test adapter"; return false; }
        if (acknowledge) ack = "test-adapter-accepted";
        return true;
    }

    bool sendText(const std::string& payload, bool expectsResponse,
                  std::string& error) override {
        MCPJsonValue message;
        try { message = MCPJsonValue::parse(payload); }
        catch (...) { error = "bad outgoing JSON"; return false; }
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (closed_) { error = "adapter closed"; return false; }
            sent_.push_back(message);
            if (expectsResponse && autoReply) {
                incoming_.push_back(MCPJsonValue{{"jsonrpc", "2.0"},
                                                 {"id", message["id"]},
                                                 {"result", {{"method", message["method"]},
                                                              {"params", message["params"]}}}}.dump());
            }
        }
        cv_.notify_all();
        return true;
    }

    WebSocketTransport::ReceiveResult receive(std::chrono::milliseconds timeout) override {
        std::unique_lock<std::mutex> lock(mutex_);
        if (blockReceive) {
            cv_.wait(lock, [this] { return closed_; });
        } else if (!cv_.wait_for(lock, timeout, [this] { return closed_ || !incoming_.empty(); })) {
            return {};
        }
        if (closed_) return {WebSocketTransport::ReceiveResult::Status::CLOSED, {}, {}};
        std::string payload = std::move(incoming_.front());
        incoming_.pop_front();
        return {WebSocketTransport::ReceiveResult::Status::MESSAGE, std::move(payload), {}};
    }

    void cancel() noexcept override {
        { std::lock_guard<std::mutex> lock(mutex_); closed_ = true; }
        cv_.notify_all();
    }
    void close() noexcept override { ++closes; cancel(); }

    void inject(const std::string& payload) {
        { std::lock_guard<std::mutex> lock(mutex_); incoming_.push_back(payload); }
        cv_.notify_all();
    }
    void failReceive(const std::string& error) {
        inject(MCPJsonValue{{"__transport_error", error}}.dump());
        transportError_.store(true);
    }
    std::vector<MCPJsonValue> sent() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return sent_;
    }
    bool waitForSent(size_t count) {
        std::unique_lock<std::mutex> lock(mutex_);
        return cv_.wait_for(lock, 1s, [this, count] { return sent_.size() >= count; });
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<std::string> incoming_;
    std::vector<MCPJsonValue> sent_;
    bool closed_ = true;
    std::atomic<bool> transportError_{false};
};

class ReconnectingAdapter final : public WebSocketTransport::Adapter {
public:
    bool connect(const std::string&, const std::string&, std::chrono::milliseconds,
                 std::string& ack, std::string&) override {
        ++connects;
        { std::lock_guard<std::mutex> lock(mutex_); closed_ = false; }
        ack = "reconnect-accepted";
        return true;
    }
    bool sendText(const std::string&, bool, std::string&) override { return true; }
    WebSocketTransport::ReceiveResult receive(std::chrono::milliseconds timeout) override {
        std::unique_lock<std::mutex> lock(mutex_);
        if (disconnectNext_) { disconnectNext_ = false; return {WebSocketTransport::ReceiveResult::Status::CLOSED, {}, {}}; }
        cv_.wait_for(lock, timeout, [this] { return closed_ || disconnectNext_; });
        if (disconnectNext_) { disconnectNext_ = false; return {WebSocketTransport::ReceiveResult::Status::CLOSED, {}, {}}; }
        return closed_ ? WebSocketTransport::ReceiveResult{WebSocketTransport::ReceiveResult::Status::CLOSED, {}, {}} : WebSocketTransport::ReceiveResult{};
    }
    void cancel() noexcept override { { std::lock_guard<std::mutex> lock(mutex_); closed_ = true; } cv_.notify_all(); }
    void close() noexcept override { cancel(); }
    void breakConnection() { { std::lock_guard<std::mutex> lock(mutex_); disconnectNext_ = true; } cv_.notify_all(); }
    std::atomic<int> connects{0};
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    bool closed_ = true;
    bool disconnectNext_ = false;
};

WebSocketTransport::Config configFor(const std::shared_ptr<WebSocketTransport::Adapter>& adapter) {
    WebSocketTransport::Config config;
    config.url = "adapter://test";
    config.adapter = adapter;
    config.autoReconnect = false;
    config.connectionTimeout = 100;
    config.maxPendingRequests = 4;
    return config;
}
}

TEST(MCPTransportTruth, HandshakeRequiresPositiveAcknowledgement) {
    auto adapter = std::make_shared<ScriptedAdapter>();
    adapter->acknowledge = false;
    WebSocketTransport transport(configFor(adapter));
    EXPECT_FALSE(transport.connect());
    EXPECT_EQ(transport.getState(), WebSocketTransport::State::FAILED);

    adapter->acknowledge = true;
    ASSERT_TRUE(transport.connect());
    EXPECT_EQ(transport.getState(), WebSocketTransport::State::CONNECTED);
}

TEST(MCPTransportTruth, DefaultSseAndTlsUnavailableFailTruthfully) {
    SSETransport::Config sseConfig;
    sseConfig.url = "http://127.0.0.1:9/events";
    SSETransport sse(sseConfig);
    EXPECT_FALSE(sse.connect());
    EXPECT_FALSE(sse.isConnected());

    WebSocketTransport::Config wsConfig;
    wsConfig.url = "wss://example.invalid/mcp";
    wsConfig.autoReconnect = false;
    WebSocketTransport ws(wsConfig);
    EXPECT_FALSE(ws.connect());
    EXPECT_EQ(ws.getState(), WebSocketTransport::State::FAILED);
}

TEST(MCPTransportJsonRpc, RejectsMalformedOutgoingAndIncomingFrames) {
    auto adapter = std::make_shared<ScriptedAdapter>();
    WebSocketTransport transport(configFor(adapter));
    std::atomic<int> errors{0};
    transport.onError([&](const std::string&) { ++errors; });
    ASSERT_TRUE(transport.connect());

    transport.send(MCPJsonValue{{"method", "missing-version"}});
    adapter->inject("not json");
    adapter->inject(R"({"jsonrpc":"1.0","id":1,"result":{}})");
    adapter->inject(R"({"jsonrpc":"2.0","id":1,"result":{},"error":{"code":1,"message":"both"}})");
    for (int i = 0; i < 100 && transport.getStats().malformedMessages < 3; ++i) std::this_thread::sleep_for(2ms);
    EXPECT_EQ(adapter->sent().size(), 0u);
    EXPECT_GE(errors.load(), 4);
    EXPECT_EQ(transport.getStats().malformedMessages, 3u);
}

TEST(MCPTransportJsonRpc, CorrelatesConcurrentRequestsOutOfOrder) {
    auto adapter = std::make_shared<ScriptedAdapter>();
    adapter->autoReply = false;
    WebSocketTransport transport(configFor(adapter));
    ASSERT_TRUE(transport.connect());
    std::vector<std::future<MCPJsonValue>> futures;
    for (uint64_t id = 1; id <= 4; ++id) {
        futures.emplace_back(std::async(std::launch::async, [&transport, id] {
            return transport.requestWithId(id, "work", MCPJsonValue{{"value", id}}, 1s);
        }));
    }
    ASSERT_TRUE(adapter->waitForSent(4));
    for (uint64_t id = 4; id >= 1; --id) {
        adapter->inject(MCPJsonValue{{"jsonrpc", "2.0"}, {"id", id},
                                     {"result", {{"value", id}}}}.dump());
    }
    for (uint64_t id = 1; id <= 4; ++id) {
        const auto response = futures[id - 1].get();
        EXPECT_EQ(response["id"], id);
        EXPECT_EQ(response["result"]["value"], id);
    }
    EXPECT_EQ(transport.getPendingRequestCount(), 0u);
}

TEST(MCPTransportJsonRpc, TimeoutCancellationBoundAndDuplicateSuppression) {
    auto adapter = std::make_shared<ScriptedAdapter>();
    adapter->autoReply = false;
    auto config = configFor(adapter);
    config.maxPendingRequests = 1;
    WebSocketTransport transport(config);
    ASSERT_TRUE(transport.connect());

    auto first = std::async(std::launch::async, [&] { return transport.requestWithId(10, "slow", MCPJsonValue::object(), 1s); });
    ASSERT_TRUE(adapter->waitForSent(1));
    EXPECT_EQ(transport.requestWithId(11, "overflow", MCPJsonValue::object(), 20ms)["error"]["code"], -32005);
    EXPECT_TRUE(transport.cancelRequest(10));
    EXPECT_EQ(first.get()["error"]["code"], -32800);

    auto timed = transport.requestWithId(12, "timeout", MCPJsonValue::object(), 20ms);
    EXPECT_EQ(timed["error"]["code"], -32003);
    adapter->inject(R"({"jsonrpc":"2.0","id":12,"result":{"late":true}})");
    adapter->inject(R"({"jsonrpc":"2.0","id":12,"result":{"duplicate":true}})");
    for (int i = 0; i < 100 && transport.getStats().duplicateOrLateResponses < 2; ++i) std::this_thread::sleep_for(2ms);
    EXPECT_EQ(transport.getStats().duplicateOrLateResponses, 2u);
    EXPECT_EQ(transport.requestWithId(12, "reuse", MCPJsonValue::object(), 20ms)["error"]["code"], -32600);
}

TEST(MCPTransportLifecycle, ReconnectRequiresFreshAcknowledgement) {
    auto adapter = std::make_shared<ReconnectingAdapter>();
    auto config = configFor(adapter);
    config.autoReconnect = true;
    config.maxReconnectAttempts = 2;
    config.reconnectDelay = 1;
    WebSocketTransport transport(config);
    ASSERT_TRUE(transport.connect());
    adapter->breakConnection();
    for (int i = 0; i < 200 && adapter->connects.load() < 2; ++i) std::this_thread::sleep_for(2ms);
    EXPECT_GE(adapter->connects.load(), 2);
    EXPECT_EQ(transport.getState(), WebSocketTransport::State::CONNECTED);
    EXPECT_GE(transport.getStats().reconnectAttempts, 1u);
}

TEST(MCPTransportLifecycle, ShutdownUnblocksBlockedReceiveAndCallbacksCanReenter) {
    auto adapter = std::make_shared<ScriptedAdapter>();
    adapter->blockReceive = true;
    WebSocketTransport transport(configFor(adapter));
    transport.onConnect([&] { EXPECT_EQ(transport.getState(), WebSocketTransport::State::CONNECTED); });
    transport.onDisconnect([&](int, const std::string&) { EXPECT_EQ(transport.getState(), WebSocketTransport::State::DISCONNECTED); });
    ASSERT_TRUE(transport.connect());
    const auto start = std::chrono::steady_clock::now();
    transport.disconnect();
    EXPECT_LT(std::chrono::steady_clock::now() - start, 500ms);
}

TEST(MCPTransportLoopback, ExplicitLoopbackHasObservableAckAndCorrelation) {
    WebSocketTransport::Config config;
    config.url = "loopback://deterministic";
    WebSocketTransport transport(config);
    ASSERT_TRUE(transport.connect());
    auto response = transport.request("tools/list", MCPJsonValue::object(), 500ms);
    ASSERT_TRUE(response.contains("result"));
    EXPECT_EQ(response["result"]["method"], "tools/list");

    SSETransport::Config sseConfig;
    sseConfig.url = "loopback://events";
    SSETransport sse(sseConfig);
    ASSERT_TRUE(sse.connect());
    EXPECT_TRUE(sse.sendCommand(MCPJsonValue{{"jsonrpc", "2.0"}, {"method", "ping"}}));
}

TEST(MCPTransportMultiplexer, FailedTransportIsNotConnectedOrPrimary) {
    auto adapter = std::make_shared<ScriptedAdapter>();
    adapter->accept = false;
    TransportMultiplexer mux;
    mux.addWebSocket("failed", configFor(adapter));
    const auto states = mux.getTransportStates();
    ASSERT_EQ(states.count("failed"), 1u);
    EXPECT_EQ(states.at("failed"), "failed");
    EXPECT_TRUE(mux.getPrimaryTransport().empty());
}
