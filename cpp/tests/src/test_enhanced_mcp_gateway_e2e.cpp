/**
 * E2E tests for the Phase 3.1 Enhanced MCP Transport System:
 *
 *   - WebSocketTransport lifecycle, loopback echo, stats, error paths
 *   - SSETransport lifecycle and command-ack loopback cycle
 *   - TransportMultiplexer registry, routing, broadcast, failover ordering
 *   - EnhancedMCPGateway transports, x402 payment verification, pooling,
 *     and enhanced statistics
 *
 * The "loopback://" URL scheme requests the deterministic in-process mode so
 * the whole suite runs without any network dependency (CI-safe), while
 * non-loopback URLs exercise the real TCP failure path deterministically.
 */
#include <gtest/gtest.h>

#include "elizaos/mcp_gateway.hpp"

#include <atomic>
#include <chrono>
#include <thread>

using namespace elizaos;

// ---------------------------------------------------------------------------
// WebSocketTransport
// ---------------------------------------------------------------------------

TEST(EnhancedMCPGatewayE2E, WebSocketLoopbackConnectSendEchoDisconnect) {
    WebSocketTransport::Config config;
    config.url = "loopback://ws-echo";
    WebSocketTransport transport(config);

    std::atomic<bool> connected{false};
    std::atomic<bool> disconnected{false};
    std::atomic<int> received{0};
    transport.onConnect([&connected]() { connected.store(true); });
    transport.onDisconnect([&disconnected](int code, const std::string&) {
        EXPECT_EQ(code, 1000);
        disconnected.store(true);
    });
    transport.setMessageHandler([&received](const MCPJsonValue& message) {
        EXPECT_TRUE(message.contains("method"));
        received.fetch_add(1);
    });

    EXPECT_EQ(transport.getState(), WebSocketTransport::State::DISCONNECTED);
    ASSERT_TRUE(transport.connect());
    EXPECT_TRUE(connected.load());
    EXPECT_EQ(transport.getState(), WebSocketTransport::State::CONNECTED);

    MCPJsonValue msg;
    msg["jsonrpc"] = "2.0";
    msg["method"] = "tools/list";
    msg["id"] = 1;
    transport.send(msg);
    transport.send(msg);
    EXPECT_EQ(received.load(), 2);

    auto stats = transport.getStats();
    EXPECT_EQ(stats.messagesSent, 2u);
    EXPECT_EQ(stats.messagesReceived, 2u);
    EXPECT_GT(stats.bytesTransferred, 0u);

    transport.disconnect();
    EXPECT_TRUE(disconnected.load());
    EXPECT_EQ(transport.getState(), WebSocketTransport::State::DISCONNECTED);
}

TEST(EnhancedMCPGatewayE2E, WebSocketRefusedConnectionReportsFailure) {
    WebSocketTransport::Config config;
    // Port 9 (discard) on localhost is virtually never listening; the TCP
    // connect must fail fast and deterministically.
    config.url = "ws://127.0.0.1:9/mcp";
    config.autoReconnect = false;
    WebSocketTransport transport(config);

    std::atomic<bool> erred{false};
    transport.onError([&erred](const std::string& error) {
        EXPECT_FALSE(error.empty());
        erred.store(true);
    });
    EXPECT_FALSE(transport.connect());
    EXPECT_EQ(transport.getState(), WebSocketTransport::State::FAILED);
    EXPECT_TRUE(erred.load());
}

TEST(EnhancedMCPGatewayE2E, WebSocketSendWhileDisconnectedRaisesError) {
    WebSocketTransport::Config config;
    config.url = "loopback://ws-idle";
    WebSocketTransport transport(config);

    std::atomic<bool> erred{false};
    transport.onError([&erred](const std::string&) { erred.store(true); });
    MCPJsonValue msg;
    msg["method"] = "ping";
    transport.send(msg);  // Never connected.
    EXPECT_TRUE(erred.load());
    EXPECT_EQ(transport.getStats().messagesSent, 0u);
}

// ---------------------------------------------------------------------------
// SSETransport
// ---------------------------------------------------------------------------

TEST(EnhancedMCPGatewayE2E, SSELoopbackLifecycleAndCommandAck) {
    SSETransport::Config config;
    config.url = "loopback://sse-events";
    SSETransport transport(config);

    std::atomic<int> opens{0};
    std::atomic<int> acks{0};
    std::atomic<int> closes{0};
    transport.onEvent([&](const std::string& event, const MCPJsonValue& data) {
        if (event == "open") opens.fetch_add(1);
        if (event == "command-ack") {
            EXPECT_TRUE(data.contains("command"));
            EXPECT_TRUE(data["ack"].get<bool>());
            acks.fetch_add(1);
        }
        if (event == "close") closes.fetch_add(1);
    });

    EXPECT_FALSE(transport.isConnected());
    ASSERT_TRUE(transport.connect());
    EXPECT_TRUE(transport.isConnected());
    EXPECT_EQ(opens.load(), 1);

    MCPJsonValue command;
    command["method"] = "resources/list";
    transport.sendCommand(command);
    EXPECT_EQ(acks.load(), 1);

    transport.disconnect();
    EXPECT_FALSE(transport.isConnected());
    EXPECT_EQ(closes.load(), 1);
}

// ---------------------------------------------------------------------------
// TransportMultiplexer
// ---------------------------------------------------------------------------

TEST(EnhancedMCPGatewayE2E, MultiplexerRoutesAndBroadcasts) {
    TransportMultiplexer mux;

    std::atomic<int> handled{0};
    std::vector<std::string> seenIds;
    std::mutex seenMutex;
    mux.setMessageHandler([&](const std::string& transportId,
                              const MCPJsonValue& message) {
        (void)message;
        std::lock_guard<std::mutex> lock(seenMutex);
        seenIds.push_back(transportId);
        handled.fetch_add(1);
    });

    WebSocketTransport::Config wsConfig;
    wsConfig.url = "loopback://mux-ws";
    SSETransport::Config sseConfig;
    sseConfig.url = "loopback://mux-sse";
    mux.addWebSocket("ws-main", wsConfig);
    mux.addSSE("sse-main", sseConfig);
    mux.addStdio("stdio-main");

    auto states = mux.getTransportStates();
    ASSERT_EQ(states.size(), 3u);
    EXPECT_EQ(states.at("ws-main"), "connected");
    EXPECT_EQ(states.at("sse-main"), "connected");
    EXPECT_EQ(states.at("stdio-main"), "connected");

    // First transport added becomes primary automatically.
    EXPECT_EQ(mux.getPrimaryTransport(), "ws-main");
    mux.setPrimaryTransport("stdio-main");
    EXPECT_EQ(mux.getPrimaryTransport(), "stdio-main");
    mux.setFailoverOrder({"stdio-main", "ws-main", "sse-main"});
    EXPECT_EQ(mux.getFailoverOrder().size(), 3u);

    MCPJsonValue msg;
    msg["method"] = "ping";
    mux.send("stdio-main", msg);       // stdio routes through the handler
    EXPECT_GE(handled.load(), 1);

    const int beforeBroadcast = handled.load();
    mux.broadcast(msg);                // ws echo + sse ack + stdio handler
    EXPECT_GT(handled.load(), beforeBroadcast);

    auto counts = mux.getMessageCounts();
    EXPECT_GE(counts["stdio-main"], 2u);
    EXPECT_GE(counts["ws-main"], 1u);
    EXPECT_GE(counts["sse-main"], 1u);

    mux.removeTransport("sse-main");
    states = mux.getTransportStates();
    EXPECT_EQ(states.size(), 2u);
    EXPECT_EQ(states.count("sse-main"), 0u);
}

TEST(EnhancedMCPGatewayE2E, MultiplexerRemovingPrimaryFallsBackToFailover) {
    TransportMultiplexer mux;
    mux.addStdio("primary");
    mux.addStdio("backup");
    mux.setFailoverOrder({"backup"});
    mux.setPrimaryTransport("primary");
    mux.removeTransport("primary");
    EXPECT_EQ(mux.getPrimaryTransport(), "backup");
}

// ---------------------------------------------------------------------------
// EnhancedMCPGateway
// ---------------------------------------------------------------------------

TEST(EnhancedMCPGatewayE2E, GatewayTransportsAndPooling) {
    EnhancedMCPGateway gateway("e2e-gateway");

    WebSocketTransport::Config wsConfig;
    wsConfig.url = "loopback://gw-ws";
    gateway.enableWebSocketTransport(wsConfig);

    SSETransport::Config sseConfig;
    sseConfig.url = "loopback://gw-sse";
    gateway.enableSSETransport(sseConfig);

    gateway.enableAutoFailover(true);
    gateway.setFailoverTimeout(std::chrono::milliseconds(1500));
    gateway.setConnectionPoolSize(8);

    EXPECT_EQ(gateway.getActiveConnections(), 2u);

    auto& mux = gateway.getTransportMultiplexer();
    MCPJsonValue msg;
    msg["method"] = "tools/list";
    mux.broadcast(msg);

    auto stats = gateway.getEnhancedStatistics();
    EXPECT_GE(stats.failoverEvents, 1u);
    EXPECT_GE(stats.connectionPoolHits, 1u);
    uint64_t total = 0;
    for (const auto& kv : stats.transportMessageCounts) total += kv.second;
    EXPECT_GE(total, 2u);
    EXPECT_GE(stats.averageLatency, 0.0);
}

TEST(EnhancedMCPGatewayE2E, X402PaymentVerification) {
    EnhancedMCPGateway gateway("e2e-gateway-pay");

    // Valid deterministic x402 proof: "txhash:payer:amount".
    auto ok = gateway.verifyBlockchainPayment("0xabc123:0xPayerAddress:0.005");
    EXPECT_TRUE(ok.verified);
    EXPECT_EQ(ok.transactionHash, "0xabc123");
    EXPECT_EQ(ok.payer, "0xPayerAddress");
    EXPECT_DOUBLE_EQ(ok.amount, 0.005);

    // Malformed proofs must be rejected without throwing.
    EXPECT_FALSE(gateway.verifyBlockchainPayment("").verified);
    EXPECT_FALSE(gateway.verifyBlockchainPayment("garbage").verified);
    EXPECT_FALSE(gateway.verifyBlockchainPayment("nohash:payer:1.0").verified);
    EXPECT_FALSE(gateway.verifyBlockchainPayment("0xabc123:payer:-1").verified);
    EXPECT_FALSE(gateway.verifyBlockchainPayment("0xabc123:payer:zero").verified);
    EXPECT_FALSE(gateway.verifyBlockchainPayment("0xabc123::1.0").verified);
}

TEST(EnhancedMCPGatewayE2E, GatewayInheritsBaseFunctionality) {
    EnhancedMCPGateway gateway("e2e-gateway-base");

    MCPTool tool;
    tool.name = "echo";
    tool.description = "echoes its input";
    tool.inputSchema = MCPJsonValue::object();
    tool.handler = [](const MCPJsonValue& input) { return input; };
    gateway.registerTool(tool);

    auto tools = gateway.listTools();
    ASSERT_FALSE(tools.empty());

    MCPJsonValue input;
    input["value"] = 42;
    auto result = gateway.executeTool("echo", input);
    // The base gateway wraps results; the call must succeed without throwing
    // and count in statistics.
    auto stats = gateway.getEnhancedStatistics();
    EXPECT_GE(stats.base.totalRequests, 1);
}
