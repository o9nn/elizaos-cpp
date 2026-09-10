#pragma once

#include "elizaos/core.hpp"
#include "elizaos/agentcomms.hpp"
#include "elizaos/agentlogger.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <cstdint>

namespace elizaos {

// MCP-specific JSON alias. Distinct from the `JsonValue` aliases declared by
// other elizaos headers (agentaction, agentagenda, characterfile, etc.) which
// alias to `std::unordered_map<std::string, std::any>`. By keeping MCP's JSON
// type isolated we let any TU include both `mcp_gateway.hpp` and any of those
// headers without triggering an alias collision.
using MCPJsonValue = nlohmann::json;

/**
 * @brief MCP Gateway: Model Context Protocol Gateway
 * 
 * A production-ready MCP gateway that aggregates multiple MCP servers
 * into a unified interface with complete x402 blockchain payment support.
 * 
 * Key Features:
 * - Multi-server aggregation
 * - Automatic namespacing
 * - Multi-transport support (STDIO, HTTP, SSE, WebSocket)
 * - Health monitoring and auto-reconnection
 * - x402 blockchain payments (USDC)
 * - API key authentication with tiered access
 */

// MCP Server configuration
struct MCPServerConfig {
    std::string name;
    std::string transport; // "stdio", "http", "sse", "websocket"
    std::string endpoint;
    std::unordered_map<std::string, std::string> environment;
    bool autoReconnect = true;
    int healthCheckInterval = 30; // seconds
};

// MCP Tool definition
struct MCPTool {
    std::string name;
    std::string namespace_;
    std::string description;
    MCPJsonValue inputSchema;
    std::function<MCPJsonValue(const MCPJsonValue&)> handler;
};

// MCP Resource definition
struct MCPResource {
    std::string uri;
    std::string namespace_;
    std::string mimeType;
    std::string description;
};

// Payment configuration (x402)
struct PaymentConfig {
    bool enabled = false;
    std::string usdcContractAddress;
    std::string recipientAddress;
    float pricePerCall = 0.001f; // USDC
    std::string chainId;
};

// API Key tier configuration
struct APIKeyTier {
    std::string tierName;
    int rateLimit; // calls per minute
    std::vector<std::string> allowedTools;
    float customPrice = 0.0f; // Override default price
};

/**
 * @brief MCP Gateway core class
 * 
 * Aggregates multiple MCP servers and provides unified access
 * with payment support and authentication.
 */
class MCPGateway {
public:
    MCPGateway(const std::string& gatewayId);
    ~MCPGateway();
    MCPGateway(const MCPGateway&) = delete;
    MCPGateway& operator=(const MCPGateway&) = delete;
    
    // Server management
    void addServer(const MCPServerConfig& config);
    void removeServer(const std::string& serverName);
    void reconnectServer(const std::string& serverName);
    std::vector<std::string> listServers() const;
    
    // Tool management
    void registerTool(const MCPTool& tool);
    void unregisterTool(const std::string& toolName);
    std::vector<MCPTool> listTools() const;
    std::vector<MCPTool> listToolsByNamespace(const std::string& namespace_) const;
    
    // Tool execution
    MCPJsonValue executeTool(const std::string& toolName, const MCPJsonValue& input, 
                          const std::string& apiKey = "");
    MCPJsonValue executeToolWithPayment(const std::string& toolName, const MCPJsonValue& input,
                                     const std::string& signature, const std::string& paymentProof);
    
    // Resource management
    void registerResource(const MCPResource& resource);
    void unregisterResource(const std::string& uri);
    std::vector<MCPResource> listResources() const;
    MCPJsonValue getResource(const std::string& uri) const;
    
    // Payment configuration
    void enablePayments(const PaymentConfig& config);
    void disablePayments();
    bool verifyPayment(const std::string& signature, const std::string& proof);
    
    // API Key management
    void createAPIKey(const std::string& key, const APIKeyTier& tier);
    void revokeAPIKey(const std::string& key);
    bool validateAPIKey(const std::string& key) const;
    APIKeyTier getAPIKeyTier(const std::string& key) const;
    
    // Health monitoring
    void startHealthMonitoring();
    void stopHealthMonitoring();
    std::unordered_map<std::string, std::string> getServerHealth() const;
    
    // Configuration
    void setNamespacing(bool enabled);
    void setConflictResolution(const std::string& strategy); // "error", "first", "last", "namespace"
    void setRateLimit(int requestsPerMinute);
    
    // Statistics
    struct Statistics {
        int totalRequests;
        int successfulRequests;
        int failedRequests;
        float totalRevenue; // USDC
        std::unordered_map<std::string, int> toolUsage;
    };
    Statistics getStatistics() const;
    
private:
    std::string gatewayId_;
    std::unordered_map<std::string, MCPServerConfig> servers_;
    std::unordered_map<std::string, MCPTool> tools_;
    std::unordered_map<std::string, MCPResource> resources_;
    std::unordered_map<std::string, APIKeyTier> apiKeys_;
    PaymentConfig paymentConfig_;
    Statistics stats_;
    bool namespacingEnabled_;
    std::string conflictResolution_;
    int rateLimit_;
    std::shared_ptr<AgentLogger> logger_;
    mutable std::mutex mutex_;

    // Health monitoring
    std::atomic<bool> monitorRunning_{false};
    std::thread monitorThread_;
    std::unordered_map<std::string, std::string> healthSnapshot_;
    
    // Internal methods
    std::string resolveNamespace(const std::string& serverName, const std::string& toolName);
    bool checkRateLimit(const std::string& apiKey);
    void updateStatistics(const std::string& toolName, bool success, float payment);
};

/**
 * @brief MCP Client for connecting to gateways
 * 
 * Client library for AI agents to connect to MCP gateways.
 */
class MCPClient {
public:
    MCPClient(const std::string& gatewayUrl, const std::string& apiKey = "");
    ~MCPClient() = default;
    
    // Discovery
    std::vector<MCPTool> discoverTools();
    std::vector<MCPResource> discoverResources();
    
    // Tool execution
    MCPJsonValue callTool(const std::string& toolName, const MCPJsonValue& input);
    MCPJsonValue callToolWithPayment(const std::string& toolName, const MCPJsonValue& input,
                                  const std::string& walletAddress);
    
    // Resource access
    MCPJsonValue getResource(const std::string& uri);
    
    // Payment setup
    void setWallet(const std::string& privateKey);
    std::string signPayment(float amount, const std::string& toolName);
    
private:
    std::string gatewayUrl_;
    std::string apiKey_;
    std::string walletPrivateKey_;
    std::shared_ptr<AgentLogger> logger_;
};

/**
 * @brief MCP Server wrapper for registering with gateway
 * 
 * Helper class for MCP server implementations.
 */
class MCPServer {
public:
    MCPServer(const std::string& serverName);
    ~MCPServer() = default;
    
    // Tool registration
    void registerTool(const std::string& name, const std::string& description,
                     const MCPJsonValue& schema,
                     std::function<MCPJsonValue(const MCPJsonValue&)> handler);
    
    // Resource registration
    void registerResource(const std::string& uri, const std::string& mimeType,
                         const std::string& description);
    
    // Connection
    void connectToGateway(const std::string& gatewayUrl);
    void disconnectFromGateway();
    
    // Request handling
    void startListening();
    void stopListening();
    
private:
    std::string serverName_;
    std::vector<MCPTool> tools_;
    std::vector<MCPResource> resources_;
    std::string gatewayUrl_;
    bool listening_;
    std::shared_ptr<AgentLogger> logger_;
};

} // namespace elizaos

// ============================================================================
// Phase 3.1: Enhanced MCP Transport System
// ============================================================================

namespace elizaos {

/**
 * WebSocket transport for real-time bidirectional MCP communication
 */
class WebSocketTransport {
public:
    enum class State {
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        RECONNECTING,
        FAILED
    };

    struct ReceiveResult {
        enum class Status { MESSAGE, TIMEOUT, CLOSED, ERROR };
        Status status = Status::TIMEOUT;
        std::string payload;
        std::string error;
    };

    /**
     * Injectable transport boundary. Connected state requires connect() to
     * return true with a non-empty peer/adapter acknowledgement. cancel() must
     * unblock receive() so the owned worker can always be joined.
     */
    class Adapter {
    public:
        virtual ~Adapter() = default;
        virtual bool connect(const std::string& url,
                             const std::string& subprotocol,
                             std::chrono::milliseconds timeout,
                             std::string& acknowledgement,
                             std::string& error) = 0;
        virtual bool sendText(const std::string& payload,
                              bool expectsResponse,
                              std::string& error) = 0;
        virtual ReceiveResult receive(std::chrono::milliseconds timeout) = 0;
        virtual void cancel() noexcept = 0;
        virtual void close() noexcept = 0;
    };

    struct Config {
        std::string url;
        int connectionTimeout = 5000;      // ms
        int pingInterval = 30000;          // ms
        int maxReconnectAttempts = 5;
        int reconnectDelay = 1000;         // ms, doubles each attempt
        bool autoReconnect = true;
        std::string subprotocol = "mcp-v1";
        size_t maxPendingRequests = 64;
        size_t maxFrameBytes = 1024 * 1024;
        std::shared_ptr<Adapter> adapter;
    };

    explicit WebSocketTransport(const Config& config);
    ~WebSocketTransport();
    WebSocketTransport(const WebSocketTransport&) = delete;
    WebSocketTransport& operator=(const WebSocketTransport&) = delete;

    bool connect();
    void disconnect();
    State getState() const;

    // Fire-and-forget sends still require a valid JSON-RPC 2.0 envelope.
    void send(const MCPJsonValue& message);
    MCPJsonValue request(const std::string& method,
                         const MCPJsonValue& params = MCPJsonValue::object(),
                         std::chrono::milliseconds timeout = std::chrono::milliseconds::zero());
    MCPJsonValue requestWithId(uint64_t id,
                               const std::string& method,
                               const MCPJsonValue& params = MCPJsonValue::object(),
                               std::chrono::milliseconds timeout = std::chrono::milliseconds::zero());
    bool cancelRequest(uint64_t id);
    size_t getPendingRequestCount() const;

    using MessageHandler = std::function<void(const MCPJsonValue&)>;
    void setMessageHandler(MessageHandler handler);

    using ConnectHandler = std::function<void()>;
    using DisconnectHandler = std::function<void(int code, const std::string& reason)>;
    using ErrorHandler = std::function<void(const std::string& error)>;
    void onConnect(ConnectHandler handler);
    void onDisconnect(DisconnectHandler handler);
    void onError(ErrorHandler handler);

    struct Stats {
        uint64_t messagesSent = 0;
        uint64_t messagesReceived = 0;
        uint64_t bytesTransferred = 0;
        uint64_t reconnectAttempts = 0;
        uint64_t malformedMessages = 0;
        uint64_t duplicateOrLateResponses = 0;
        uint64_t timedOutRequests = 0;
        uint64_t cancelledRequests = 0;
        uint64_t rejectedRequests = 0;
        std::chrono::steady_clock::time_point lastActivity;
    };
    Stats getStats() const;

public:
    struct SharedState; // opaque implementation state

private:
    std::shared_ptr<SharedState> state_;
    std::thread receiveThread_;
    static void receiveLoop(const std::shared_ptr<SharedState>& state);
};

/** Server-Sent Events transport with an acknowledged command adapter. */
class SSETransport {
public:
    class Adapter {
    public:
        virtual ~Adapter() = default;
        virtual bool connect(const std::string& url,
                             std::chrono::milliseconds timeout,
                             std::string& acknowledgement,
                             std::string& error) = 0;
        virtual bool sendCommand(const std::string& payload,
                                 std::string& acknowledgement,
                                 std::string& error) = 0;
        virtual void cancel() noexcept = 0;
        virtual void close() noexcept = 0;
    };

    struct Config {
        std::string url;
        int connectionTimeout = 5000;
        bool autoReconnect = true;
        int reconnectDelay = 3000;
        std::shared_ptr<Adapter> adapter;
    };

    explicit SSETransport(const Config& config);
    ~SSETransport();
    SSETransport(const SSETransport&) = delete;
    SSETransport& operator=(const SSETransport&) = delete;

    bool connect();
    void disconnect();
    bool isConnected() const;

    using EventHandler = std::function<void(const std::string& event, const MCPJsonValue& data)>;
    void onEvent(EventHandler handler);
    bool sendCommand(const MCPJsonValue& command);

public:
    struct SharedState; // opaque implementation state

private:
    std::shared_ptr<SharedState> state_;
};

/** Connection multiplexer for acknowledged transports. */
class TransportMultiplexer {
public:
    TransportMultiplexer();
    ~TransportMultiplexer();
    TransportMultiplexer(const TransportMultiplexer&) = delete;
    TransportMultiplexer& operator=(const TransportMultiplexer&) = delete;

    void addWebSocket(const std::string& id, const WebSocketTransport::Config& config);
    void addSSE(const std::string& id, const SSETransport::Config& config);
    void addStdio(const std::string& id);
    void removeTransport(const std::string& id);
    std::unordered_map<std::string, std::string> getTransportStates() const;
    void send(const std::string& transportId, const MCPJsonValue& message);
    void broadcast(const MCPJsonValue& message);

    using MessageHandler = std::function<void(const std::string& transportId,
                                               const MCPJsonValue& message)>;
    void setMessageHandler(MessageHandler handler);
    void setPrimaryTransport(const std::string& id);
    void setFailoverOrder(const std::vector<std::string>& order);
    std::string getPrimaryTransport() const;
    std::vector<std::string> getFailoverOrder() const;
    std::unordered_map<std::string, uint64_t> getMessageCounts() const;

public:
    struct SharedState; // opaque implementation state

private:
    std::shared_ptr<SharedState> state_;
};

/**
 * Enhanced MCP Gateway with full transport support
 */
class EnhancedMCPGateway : public MCPGateway {
public:
    EnhancedMCPGateway(const std::string& gatewayId);
    ~EnhancedMCPGateway() = default;
    
    // Transport management
    TransportMultiplexer& getTransportMultiplexer();
    void enableWebSocketTransport(const WebSocketTransport::Config& config);
    void enableSSETransport(const SSETransport::Config& config);
    
    // Automatic failover
    void enableAutoFailover(bool enable = true);
    void setFailoverTimeout(std::chrono::milliseconds timeout);
    
    // Connection pooling
    void setConnectionPoolSize(size_t size);
    size_t getActiveConnections() const;
    
    // x402 payment verification with blockchain
    struct PaymentVerification {
        bool verified;
        std::string transactionHash;
        double amount;
        std::string payer;
        std::chrono::system_clock::time_point timestamp;
    };
    PaymentVerification verifyBlockchainPayment(const std::string& proof);
    
    // Enhanced statistics
    struct EnhancedStats {
        Statistics base;
        std::unordered_map<std::string, uint64_t> transportMessageCounts;
        double averageLatency;
        uint64_t failoverEvents;
        uint64_t connectionPoolHits;
    };
    EnhancedStats getEnhancedStatistics() const;
    
private:
    friend class TransportMultiplexer;
    std::unique_ptr<TransportMultiplexer> multiplexer_;
    bool autoFailoverEnabled_ = false;
    std::chrono::milliseconds failoverTimeout_{5000};
    size_t connectionPoolSize_ = 10;
    mutable uint64_t failoverEvents_ = 0;
    mutable uint64_t connectionPoolHits_ = 0;
    mutable std::mutex enhancedMutex_;
};

} // namespace elizaos
