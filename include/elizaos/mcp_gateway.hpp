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
