#include "elizaos/mcp_gateway.hpp"
#include <sstream>

namespace elizaos {

// MCPGateway implementation

MCPGateway::MCPGateway(const std::string& gatewayId)
    : gatewayId_(gatewayId)
    , namespacingEnabled_(true)
    , conflictResolution_("namespace")
    , rateLimit_(1000)
    , logger_(std::make_shared<AgentLogger>()) {
    
    stats_.totalRequests = 0;
    stats_.successfulRequests = 0;
    stats_.failedRequests = 0;
    stats_.totalRevenue = 0.0f;
    
    elizaos::logInfo("MCP Gateway initialized: " + gatewayId, "mcp_gateway");
}

void MCPGateway::addServer(const MCPServerConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    servers_[config.name] = config;
    elizaos::logInfo("Server added: " + config.name, "mcp_gateway");
}

void MCPGateway::removeServer(const std::string& serverName) {
    std::lock_guard<std::mutex> lock(mutex_);
    servers_.erase(serverName);
    elizaos::logInfo("Server removed: " + serverName, "mcp_gateway");
}

void MCPGateway::reconnectServer(const std::string& serverName) {
    elizaos::logInfo("Reconnecting server: " + serverName, "mcp_gateway");
    // Placeholder: Would implement reconnection logic
}

std::vector<std::string> MCPGateway::listServers() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> serverList;
    for (const auto& pair : servers_) {
        serverList.push_back(pair.first);
    }
    return serverList;
}

void MCPGateway::registerTool(const MCPTool& tool) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string fullName = tool.namespace_.empty() ? tool.name : tool.namespace_ + "::" + tool.name;
    tools_[fullName] = tool;
    elizaos::logInfo("Tool registered: " + fullName, "mcp_gateway");
}

void MCPGateway::unregisterTool(const std::string& toolName) {
    std::lock_guard<std::mutex> lock(mutex_);
    tools_.erase(toolName);
    elizaos::logInfo("Tool unregistered: " + toolName, "mcp_gateway");
}

std::vector<MCPTool> MCPGateway::listTools() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<MCPTool> toolList;
    for (const auto& pair : tools_) {
        toolList.push_back(pair.second);
    }
    return toolList;
}

std::vector<MCPTool> MCPGateway::listToolsByNamespace(const std::string& namespace_) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<MCPTool> filtered;
    for (const auto& pair : tools_) {
        if (pair.second.namespace_ == namespace_) {
            filtered.push_back(pair.second);
        }
    }
    return filtered;
}

JsonValue MCPGateway::executeTool(const std::string& toolName, const JsonValue& input, 
                                   const std::string& apiKey) {
    elizaos::logInfo("Executing tool: " + toolName, "mcp_gateway");
    
    // Check API key if provided
    if (!apiKey.empty() && !validateAPIKey(apiKey)) {
        JsonValue error;
        error["error"] = "Invalid API key";
        stats_.failedRequests++;
        return error;
    }
    
    // Check rate limit
    if (!apiKey.empty() && !checkRateLimit(apiKey)) {
        JsonValue error;
        error["error"] = "Rate limit exceeded";
        stats_.failedRequests++;
        return error;
    }
    
    // Find and execute tool
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tools_.find(toolName);
    if (it != tools_.end() && it->second.handler) {
        try {
            JsonValue result = it->second.handler(input);
            stats_.successfulRequests++;
            stats_.toolUsage[toolName]++;
            updateStatistics(toolName, true, 0.0f);
            return result;
        } catch (const std::exception& e) {
            JsonValue error;
            error["error"] = std::string("Execution failed: ") + e.what();
            stats_.failedRequests++;
            updateStatistics(toolName, false, 0.0f);
            return error;
        }
    }
    
    JsonValue error;
    error["error"] = "Tool not found: " + toolName;
    stats_.failedRequests++;
    return error;
}

JsonValue MCPGateway::executeToolWithPayment(const std::string& toolName, const JsonValue& input,
                                             const std::string& signature, const std::string& paymentProof) {
    elizaos::logInfo("Executing tool with payment: " + toolName, "mcp_gateway");
    
    // Verify payment
    if (!verifyPayment(signature, paymentProof)) {
        JsonValue error;
        error["error"] = "Payment verification failed";
        stats_.failedRequests++;
        return error;
    }
    
    // Execute tool and record payment
    JsonValue result = executeTool(toolName, input);
    if (result.find("error") == result.end()) {
        stats_.totalRevenue += paymentConfig_.pricePerCall;
    }
    
    return result;
}

void MCPGateway::registerResource(const MCPResource& resource) {
    std::lock_guard<std::mutex> lock(mutex_);
    resources_[resource.uri] = resource;
    elizaos::logInfo("Resource registered: " + resource.uri, "mcp_gateway");
}

void MCPGateway::unregisterResource(const std::string& uri) {
    std::lock_guard<std::mutex> lock(mutex_);
    resources_.erase(uri);
    elizaos::logInfo("Resource unregistered: " + uri, "mcp_gateway");
}

std::vector<MCPResource> MCPGateway::listResources() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<MCPResource> resourceList;
    for (const auto& pair : resources_) {
        resourceList.push_back(pair.second);
    }
    return resourceList;
}

JsonValue MCPGateway::getResource(const std::string& uri) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = resources_.find(uri);
    if (it != resources_.end()) {
        JsonValue resource;
        resource["uri"] = it->second.uri;
        resource["mimeType"] = it->second.mimeType;
        resource["description"] = it->second.description;
        return resource;
    }
    JsonValue error;
    error["error"] = "Resource not found";
    return error;
}

void MCPGateway::enablePayments(const PaymentConfig& config) {
    paymentConfig_ = config;
    paymentConfig_.enabled = true;
    elizaos::logInfo("Payments enabled", "mcp_gateway");
}

void MCPGateway::disablePayments() {
    paymentConfig_.enabled = false;
    elizaos::logInfo("Payments disabled", "mcp_gateway");
}

bool MCPGateway::verifyPayment(const std::string& signature, const std::string& proof) {
    if (!paymentConfig_.enabled) {
        return true; // Payments not required
    }
    // Placeholder: Would implement x402 signature verification
    elizaos::logInfo("Verifying payment", "mcp_gateway");
    return !signature.empty() && !proof.empty();
}

void MCPGateway::createAPIKey(const std::string& key, const APIKeyTier& tier) {
    std::lock_guard<std::mutex> lock(mutex_);
    apiKeys_[key] = tier;
    elizaos::logInfo("API key created for tier: " + tier.tierName, "mcp_gateway");
}

void MCPGateway::revokeAPIKey(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    apiKeys_.erase(key);
    elizaos::logInfo("API key revoked", "mcp_gateway");
}

bool MCPGateway::validateAPIKey(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return apiKeys_.find(key) != apiKeys_.end();
}

APIKeyTier MCPGateway::getAPIKeyTier(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = apiKeys_.find(key);
    if (it != apiKeys_.end()) {
        return it->second;
    }
    return APIKeyTier();
}

void MCPGateway::startHealthMonitoring() {
    elizaos::logInfo("Health monitoring started", "mcp_gateway");
    // Placeholder: Would start monitoring thread
}

void MCPGateway::stopHealthMonitoring() {
    elizaos::logInfo("Health monitoring stopped", "mcp_gateway");
    // Placeholder: Would stop monitoring thread
}

std::unordered_map<std::string, std::string> MCPGateway::getServerHealth() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::unordered_map<std::string, std::string> health;
    for (const auto& pair : servers_) {
        health[pair.first] = "healthy"; // Placeholder
    }
    return health;
}

void MCPGateway::setNamespacing(bool enabled) {
    namespacingEnabled_ = enabled;
    elizaos::logInfo(std::string("Namespacing: ") + (enabled ? "enabled" : "disabled"), "mcp_gateway");
}

void MCPGateway::setConflictResolution(const std::string& strategy) {
    conflictResolution_ = strategy;
    elizaos::logInfo("Conflict resolution: " + strategy, "mcp_gateway");
}

void MCPGateway::setRateLimit(int requestsPerMinute) {
    rateLimit_ = requestsPerMinute;
    elizaos::logInfo("Rate limit set to: " + std::to_string(requestsPerMinute) + " req/min", "mcp_gateway");
}

MCPGateway::Statistics MCPGateway::getStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return stats_;
}

// Private methods

std::string MCPGateway::resolveNamespace(const std::string& serverName, const std::string& toolName) {
    if (namespacingEnabled_) {
        return serverName + "::" + toolName;
    }
    return toolName;
}

bool MCPGateway::checkRateLimit(const std::string& apiKey) {
    // Placeholder: Would implement rate limiting logic
    (void)apiKey;
    return true;
}

void MCPGateway::updateStatistics(const std::string& toolName, bool success, float payment) {
    (void)toolName; // Will be used when implementing detailed per-tool statistics
    stats_.totalRequests++;
    if (success) {
        stats_.successfulRequests++;
    } else {
        stats_.failedRequests++;
    }
    stats_.totalRevenue += payment;
}

// MCPClient implementation

MCPClient::MCPClient(const std::string& gatewayUrl, const std::string& apiKey)
    : gatewayUrl_(gatewayUrl)
    , apiKey_(apiKey)
    , logger_(std::make_shared<AgentLogger>()) {
    
    elizaos::logInfo("MCP Client initialized for: " + gatewayUrl, "mcp_gateway");
}

std::vector<MCPTool> MCPClient::discoverTools() {
    elizaos::logInfo("Discovering tools", "mcp_gateway");
    // Placeholder: Would query gateway for available tools
    return {};
}

std::vector<MCPResource> MCPClient::discoverResources() {
    elizaos::logInfo("Discovering resources", "mcp_gateway");
    // Placeholder: Would query gateway for available resources
    return {};
}

JsonValue MCPClient::callTool(const std::string& toolName, const JsonValue& input) {
    (void)toolName; (void)input;
    elizaos::logInfo("Calling tool: " + toolName, "mcp_gateway");
    // Placeholder: Would send HTTP/WebSocket request to gateway
    JsonValue result;
    result["status"] = "success";
    return result;
}

JsonValue MCPClient::callToolWithPayment(const std::string& toolName, const JsonValue& input,
                                        const std::string& walletAddress) {
    (void)walletAddress;
    elizaos::logInfo("Calling tool with payment: " + toolName, "mcp_gateway");
    // Placeholder: Would sign payment and call tool
    return callTool(toolName, input);
}

JsonValue MCPClient::getResource(const std::string& uri) {
    elizaos::logInfo("Getting resource: " + uri, "mcp_gateway");
    // Placeholder: Would fetch resource from gateway
    JsonValue resource;
    resource["uri"] = uri;
    return resource;
}

void MCPClient::setWallet(const std::string& privateKey) {
    walletPrivateKey_ = privateKey;
    elizaos::logInfo("Wallet configured", "mcp_gateway");
}

std::string MCPClient::signPayment(float amount, const std::string& toolName) {
    elizaos::logInfo("Signing payment for " + toolName, "mcp_gateway");
    // Placeholder: Would sign payment with wallet
    (void)amount; (void)toolName;
    return "signature_placeholder";
}

// MCPServer implementation

MCPServer::MCPServer(const std::string& serverName)
    : serverName_(serverName)
    , listening_(false)
    , logger_(std::make_shared<AgentLogger>()) {
    
    elizaos::logInfo("MCP Server initialized: " + serverName, "mcp_gateway");
}

void MCPServer::registerTool(const std::string& name, const std::string& description,
                             const JsonValue& schema,
                             std::function<JsonValue(const JsonValue&)> handler) {
    MCPTool tool;
    tool.name = name;
    tool.namespace_ = serverName_;
    tool.description = description;
    tool.inputSchema = schema;
    tool.handler = handler;
    
    tools_.push_back(tool);
    elizaos::logInfo("Tool registered: " + name, "mcp_gateway");
}

void MCPServer::registerResource(const std::string& uri, const std::string& mimeType,
                                 const std::string& description) {
    MCPResource resource;
    resource.uri = uri;
    resource.namespace_ = serverName_;
    resource.mimeType = mimeType;
    resource.description = description;
    
    resources_.push_back(resource);
    elizaos::logInfo("Resource registered: " + uri, "mcp_gateway");
}

void MCPServer::connectToGateway(const std::string& gatewayUrl) {
    gatewayUrl_ = gatewayUrl;
    elizaos::logInfo("Connecting to gateway: " + gatewayUrl, "mcp_gateway");
    // Placeholder: Would establish connection
}

void MCPServer::disconnectFromGateway() {
    elizaos::logInfo("Disconnecting from gateway", "mcp_gateway");
    // Placeholder: Would close connection
}

void MCPServer::startListening() {
    listening_ = true;
    elizaos::logInfo("Server started listening", "mcp_gateway");
    // Placeholder: Would start request handler
}

void MCPServer::stopListening() {
    listening_ = false;
    elizaos::logInfo("Server stopped listening", "mcp_gateway");
    // Placeholder: Would stop request handler
}

} // namespace elizaos
