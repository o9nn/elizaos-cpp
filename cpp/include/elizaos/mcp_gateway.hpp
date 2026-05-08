#pragma once

/**
 * ElizaOS C++ - MCPGateway Module
 *
 * Model Context Protocol gateway: tool and resource registry, payments,
 * API key management, and health monitoring.
 */

#include "elizaos.hpp"
#include "agentlogger.hpp"
#include <atomic>
#include <chrono>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// MCPJsonValue – lightweight JSON-like map used by MCP layer
// (kept separate to avoid colliding with elizaos::JsonValue)
// ============================================================================

using MCPJsonValue = std::unordered_map<std::string, std::string>;

// ============================================================================
// MCPServerConfig
// ============================================================================

struct MCPServerConfig {
    std::string name;
    std::string transport;       // "http", "stdio", "ws"
    std::string endpoint;
    bool        autoReconnect        = false;
    int         healthCheckInterval  = 60;   // seconds
    std::unordered_map<std::string, std::string> environment;
};

// ============================================================================
// MCPTool
// ============================================================================

struct MCPTool {
    std::string name;
    std::string namespace_;
    std::string description;
    JsonValue   inputSchema;
    std::function<JsonValue(const JsonValue&)> handler;
};

// ============================================================================
// MCPResource
// ============================================================================

struct MCPResource {
    std::string uri;
    std::string namespace_;
    std::string mimeType;
    std::string description;
    std::function<MCPJsonValue()> accessor;
};

// ============================================================================
// PaymentConfig
// ============================================================================

struct PaymentConfig {
    bool        enabled              = false;
    std::string usdcContractAddress;
    std::string recipientAddress;
    float       pricePerCall         = 0.001f;
    std::string chainId              = "1";
};

// ============================================================================
// APIKeyTier
// ============================================================================

struct APIKeyTier {
    std::string              tierName;
    int                      rateLimit    = 100;
    std::vector<std::string> allowedTools;
    float                    customPrice  = 0.001f;
};

// ============================================================================
// MCPGateway
// ============================================================================

class MCPGateway {
public:
    struct Statistics {
        uint64_t totalRequests       = 0;
        uint64_t successfulRequests  = 0;
        uint64_t failedRequests      = 0;
        float    totalRevenue        = 0.0f;
    };

    explicit MCPGateway(const std::string& gatewayId = "default");
    ~MCPGateway();

    // Server management
    void                     addServer(const MCPServerConfig& config);
    void                     removeServer(const std::string& serverName);
    void                     reconnectServer(const std::string& serverName);
    std::vector<std::string> listServers() const;

    // Tool management
    void                     registerTool(const MCPTool& tool);
    void                     unregisterTool(const std::string& toolName);
    std::vector<MCPTool>     listTools() const;
    std::vector<MCPTool>     listToolsByNamespace(const std::string& namespace_) const;

    // Tool execution
    MCPJsonValue executeTool(const std::string& toolName,
                              const MCPJsonValue& input,
                              const std::string& apiKey = "");
    MCPJsonValue executeToolWithPayment(const std::string& toolName,
                                        const MCPJsonValue& input,
                                        const std::string& paymentProof,
                                        const std::string& apiKey = "");

    // Resource management
    void                      registerResource(const MCPResource& resource);
    void                      unregisterResource(const std::string& uri);
    std::vector<MCPResource>  listResources() const;
    MCPJsonValue              getResource(const std::string& uri) const;

    // Payment
    void enablePayments(const PaymentConfig& config);
    void disablePayments();
    bool verifyPayment(const std::string& signature, const std::string& proof);

    // API key management
    void       createAPIKey(const std::string& key, const APIKeyTier& tier);
    void       revokeAPIKey(const std::string& key);
    bool       validateAPIKey(const std::string& key) const;
    APIKeyTier getAPIKeyTier(const std::string& key) const;

    // Health monitoring
    void startHealthMonitoring();
    void stopHealthMonitoring();
    std::unordered_map<std::string, std::string> getServerHealth() const;

    // Configuration
    void setNamespacing(bool enabled);
    void setConflictResolution(const std::string& strategy);
    void setRateLimit(int requestsPerMinute);

    Statistics getStatistics() const;
    const std::string& getGatewayId() const { return gatewayId_; }

private:
    std::string resolveNamespace(const std::string& serverName, const std::string& toolName);
    bool        checkRateLimit(const std::string& apiKey);
    void        updateStatistics(const std::string& toolName, bool success, float payment);

    std::string  gatewayId_;
    bool         namespacingEnabled_  = true;
    std::string  conflictResolution_  = "namespace";
    int          rateLimit_           = 1000;

    std::unordered_map<std::string, MCPServerConfig>                   servers_;
    std::unordered_map<std::string, MCPTool>                           tools_;
    std::unordered_map<std::string, MCPResource>                       resources_;
    std::unordered_map<std::string, APIKeyTier>                        apiKeys_;
    std::unordered_map<std::string, std::string>                       serverHealth_;

    PaymentConfig paymentsConfig_;
    Statistics    stats_;

    std::shared_ptr<AgentLogger> logger_;
    std::thread         healthThread_;
    std::atomic<bool>   healthRunning_{false};
    mutable std::mutex  mutex_;
    mutable std::mutex  statsMutex_;
};

// ============================================================================
// MCPClient
// ============================================================================

class MCPClient {
public:
    MCPClient(const std::string& gatewayUrl, const std::string& apiKey = "");

    std::vector<MCPTool>     discoverTools();
    std::vector<MCPResource> discoverResources();

    MCPJsonValue callTool(const std::string& toolName, const MCPJsonValue& input);
    MCPJsonValue callToolWithPayment(const std::string& toolName,
                                     const MCPJsonValue& input,
                                     const std::string& paymentProof);
    MCPJsonValue getResource(const std::string& uri);

    void        setWallet(const std::string& privateKey);
    std::string signPayment(float amount, const std::string& toolName);

private:
    std::string  gatewayUrl_;
    std::string  apiKey_;
    std::string  wallet_;
    MCPGateway*  gatewayPtr_ = nullptr;
};

// ============================================================================
// MCPServer
// ============================================================================

class MCPServer {
public:
    explicit MCPServer(const std::string& serverName);

    void registerTool(const std::string& name,
                      const std::string& description,
                      const MCPJsonValue& schema,
                      std::function<MCPJsonValue(const MCPJsonValue&)> handler);
    void registerResource(const std::string& uri,
                          const std::string& mimeType,
                          std::function<MCPJsonValue()> accessor);

    void connectToGateway(const std::string& gatewayUrl);
    void disconnectFromGateway();
    void startListening();
    void stopListening();

    const std::string& getServerName() const { return serverName_; }

private:
    std::string  serverName_;
    MCPGateway*  gatewayPtr_ = nullptr;
    bool         listening_  = false;
    std::vector<MCPTool>     tools_;
    std::vector<MCPResource> resources_;
};

} // namespace elizaos
