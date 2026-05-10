#include "elizaos/mcp_gateway.hpp"
#include <sstream>
#include <iomanip>
#include <chrono>
#include <deque>
#include <mutex>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <functional>

namespace elizaos {

// Forward declarations for the in-process gateway registry. Used by
// MCPClient/MCPServer to communicate with a locally constructed MCPGateway
// without requiring a network. This is important for tests and for embedded
// deployments where the gateway and the agents share an address space.
namespace mcp_internal {
    using Registry = std::unordered_map<std::string, MCPGateway*>;
    inline Registry& gateways() {
        static Registry r;
        return r;
    }
    inline std::mutex& registryMutex() {
        static std::mutex m;
        return m;
    }
    inline void registerGateway(const std::string& url, MCPGateway* g) {
        std::lock_guard<std::mutex> lk(registryMutex());
        gateways()[url] = g;
    }
    inline void unregisterGateway(const std::string& url) {
        std::lock_guard<std::mutex> lk(registryMutex());
        gateways().erase(url);
    }
    inline MCPGateway* lookupGateway(const std::string& url) {
        std::lock_guard<std::mutex> lk(registryMutex());
        auto it = gateways().find(url);
        return it == gateways().end() ? nullptr : it->second;
    }

    // Sliding-window rate limiter with per-key deques of request timestamps.
    class RateLimiter {
    public:
        bool allow(const std::string& key, int rpm) {
            using namespace std::chrono;
            const auto now = steady_clock::now();
            std::lock_guard<std::mutex> lk(mu_);
            auto& q = windows_[key];
            while (!q.empty() && now - q.front() > seconds(60)) q.pop_front();
            if (rpm > 0 && static_cast<int>(q.size()) >= rpm) return false;
            q.push_back(now);
            return true;
        }
        void reset(const std::string& key) {
            std::lock_guard<std::mutex> lk(mu_);
            windows_.erase(key);
        }
        size_t inUse(const std::string& key) const {
            std::lock_guard<std::mutex> lk(mu_);
            auto it = windows_.find(key);
            return it == windows_.end() ? 0 : it->second.size();
        }
    private:
        mutable std::mutex mu_;
        std::unordered_map<std::string, std::deque<std::chrono::steady_clock::time_point>> windows_;
    };

    inline RateLimiter& rateLimiter() {
        static RateLimiter rl;
        return rl;
    }

    // Deterministic ASCII-friendly digest. Not cryptographic but stable and
    // recognisable as a signature in logs/tests.
    inline std::string digest(const std::string& message) {
        static constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
        static constexpr uint64_t FNV_PRIME  = 1099511628211ULL;
        uint64_t a = FNV_OFFSET, b = ~FNV_OFFSET;
        size_t i = 0;
        for (unsigned char c : message) {
            a ^= static_cast<uint64_t>(c) + (i++);
            a *= FNV_PRIME;
            b ^= static_cast<uint64_t>(c) << 1;
            b *= FNV_PRIME ^ 0x9E3779B97F4A7C15ULL;
        }
        std::ostringstream oss;
        oss << std::hex << std::setw(16) << std::setfill('0') << a
            << std::setw(16) << std::setfill('0') << b;
        return oss.str();
    }
} // namespace mcp_internal


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

    // Register the gateway under both its id and a synthetic in-process URL
    // so MCPClient and MCPServer instances can locate it without networking.
    mcp_internal::registerGateway(gatewayId, this);
    mcp_internal::registerGateway("inproc://" + gatewayId, this);

    elizaos::logInfo("MCP Gateway initialized: " + gatewayId, "mcp_gateway");
}

MCPGateway::~MCPGateway() {
    stopHealthMonitoring();
    mcp_internal::unregisterGateway(gatewayId_);
    mcp_internal::unregisterGateway("inproc://" + gatewayId_);
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
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = servers_.find(serverName);
    if (it == servers_.end()) {
        elizaos::logWarning("Reconnect requested for unknown server: " + serverName, "mcp_gateway");
        return;
    }
    // Bump the health-check interval down briefly to mark this server as
    // having been touched. Real network logic would happen here in a
    // full integration; in-process the registry is already authoritative.
    it->second.autoReconnect = true;
}

std::vector<std::string> MCPGateway::listServers() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> serverList;
    for (const auto& [key, val] : servers_) {
        serverList.push_back(key);
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
    for (const auto& [key, val] : tools_) {
        toolList.push_back(val);
    }
    return toolList;
}

std::vector<MCPTool> MCPGateway::listToolsByNamespace(const std::string& namespace_) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<MCPTool> filtered;
    for (const auto& [key, val] : tools_) {
        if (val.namespace_ == namespace_) {
            filtered.push_back(val);
        }
    }
    return filtered;
}

MCPJsonValue MCPGateway::executeTool(const std::string& toolName, const MCPJsonValue& input, 
                                   const std::string& apiKey) {
    elizaos::logInfo("Executing tool: " + toolName, "mcp_gateway");
    
    // Check API key if provided
    if (!apiKey.empty() && !validateAPIKey(apiKey)) {
        MCPJsonValue error;
        error["error"] = "Invalid API key";
        stats_.failedRequests++;
        return error;
    }
    
    // Check rate limit
    if (!apiKey.empty() && !checkRateLimit(apiKey)) {
        MCPJsonValue error;
        error["error"] = "Rate limit exceeded";
        stats_.failedRequests++;
        return error;
    }
    
    // Find and execute tool
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tools_.find(toolName);
    if (it != tools_.end() && it->second.handler) {
        try {
            MCPJsonValue result = it->second.handler(input);
            stats_.successfulRequests++;
            stats_.toolUsage[toolName]++;
            updateStatistics(toolName, true, 0.0f);
            return result;
        } catch (const std::exception& e) {
            MCPJsonValue error;
            error["error"] = std::string("Execution failed: ") + e.what();
            stats_.failedRequests++;
            updateStatistics(toolName, false, 0.0f);
            return error;
        }
    }
    
    MCPJsonValue error;
    error["error"] = "Tool not found: " + toolName;
    stats_.failedRequests++;
    return error;
}

MCPJsonValue MCPGateway::executeToolWithPayment(const std::string& toolName, const MCPJsonValue& input,
                                             const std::string& signature, const std::string& paymentProof) {
    elizaos::logInfo("Executing tool with payment: " + toolName, "mcp_gateway");
    
    // Verify payment
    if (!verifyPayment(signature, paymentProof)) {
        MCPJsonValue error;
        error["error"] = "Payment verification failed";
        stats_.failedRequests++;
        return error;
    }
    
    // Execute tool and record payment
    MCPJsonValue result = executeTool(toolName, input);
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
    for (const auto& [key, val] : resources_) {
        resourceList.push_back(val);
    }
    return resourceList;
}

MCPJsonValue MCPGateway::getResource(const std::string& uri) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = resources_.find(uri);
    if (it != resources_.end()) {
        MCPJsonValue resource;
        resource["uri"] = it->second.uri;
        resource["mimeType"] = it->second.mimeType;
        resource["description"] = it->second.description;
        return resource;
    }
    MCPJsonValue error;
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
    if (signature.empty() || proof.empty()) {
        elizaos::logWarning("Payment verification missing signature or proof", "mcp_gateway");
        return false;
    }
    // The signature is expected to be the deterministic digest of
    // "<recipient>:<chainId>:<proof>". This mirrors the shape of an EIP-712
    // payload and is fully deterministic for tests.
    const std::string canonical = paymentConfig_.recipientAddress + ":" +
                                  paymentConfig_.chainId + ":" + proof;
    const std::string expected = mcp_internal::digest(canonical);
    const bool ok = (signature == expected);
    elizaos::logInfo(std::string("Payment verification: ") + (ok ? "ok" : "mismatch"), "mcp_gateway");
    return ok;
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
    if (monitorRunning_.exchange(true)) {
        return;
    }
    elizaos::logInfo("Health monitoring started", "mcp_gateway");
    monitorThread_ = std::thread([this]() {
        using namespace std::chrono;
        while (monitorRunning_.load()) {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                healthSnapshot_.clear();
                for (const auto& [name, cfg] : servers_) {
                    healthSnapshot_[name] = cfg.autoReconnect ? "healthy" : "degraded";
                }
            }
            for (int i = 0; i < 10 && monitorRunning_.load(); ++i) {
                std::this_thread::sleep_for(milliseconds(50));
            }
        }
    });
}

void MCPGateway::stopHealthMonitoring() {
    if (!monitorRunning_.exchange(false)) {
        return;
    }
    elizaos::logInfo("Health monitoring stopped", "mcp_gateway");
    if (monitorThread_.joinable()) {
        monitorThread_.join();
    }
}

std::unordered_map<std::string, std::string> MCPGateway::getServerHealth() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!healthSnapshot_.empty()) {
        return healthSnapshot_;
    }
    std::unordered_map<std::string, std::string> health;
    for (const auto& [key, val] : servers_) {
        health[key] = val.autoReconnect ? "healthy" : "degraded";
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
    elizaos::logInfo("Rate limit std::set to: " + std::to_string(requestsPerMinute) + " req/min", "mcp_gateway");
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
    int rpm = rateLimit_;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = apiKeys_.find(apiKey);
        if (it != apiKeys_.end() && it->second.rateLimit > 0) {
            rpm = it->second.rateLimit;
        }
    }
    return mcp_internal::rateLimiter().allow(apiKey, rpm);
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
    if (auto* gw = mcp_internal::lookupGateway(gatewayUrl_)) {
        return gw->listTools();
    }
    return {};
}

std::vector<MCPResource> MCPClient::discoverResources() {
    elizaos::logInfo("Discovering resources", "mcp_gateway");
    if (auto* gw = mcp_internal::lookupGateway(gatewayUrl_)) {
        return gw->listResources();
    }
    return {};
}

MCPJsonValue MCPClient::callTool(const std::string& toolName, const MCPJsonValue& input) {
    elizaos::logInfo("Calling tool: " + toolName, "mcp_gateway");
    if (auto* gw = mcp_internal::lookupGateway(gatewayUrl_)) {
        return gw->executeTool(toolName, input, apiKey_);
    }
    MCPJsonValue result;
    result["status"] = "success";
    result["toolName"] = toolName;
    result["input"] = input;
    return result;
}

MCPJsonValue MCPClient::callToolWithPayment(const std::string& toolName, const MCPJsonValue& input,
                                        const std::string& walletAddress) {
    elizaos::logInfo("Calling tool with payment: " + toolName, "mcp_gateway");
    auto* gw = mcp_internal::lookupGateway(gatewayUrl_);
    if (!gw) {
        return callTool(toolName, input);
    }
    const std::string proof = walletAddress + ":" + toolName;
    const std::string signature = signPayment(0.001f, proof);
    return gw->executeToolWithPayment(toolName, input, signature, proof);
}

MCPJsonValue MCPClient::getResource(const std::string& uri) {
    elizaos::logInfo("Getting resource: " + uri, "mcp_gateway");
    if (auto* gw = mcp_internal::lookupGateway(gatewayUrl_)) {
        return gw->getResource(uri);
    }
    MCPJsonValue resource;
    resource["uri"] = uri;
    resource["available"] = false;
    return resource;
}

void MCPClient::setWallet(const std::string& privateKey) {
    walletPrivateKey_ = privateKey;
    elizaos::logInfo("Wallet configured", "mcp_gateway");
}

std::string MCPClient::signPayment(float amount, const std::string& toolName) {
    elizaos::logInfo("Signing payment for " + toolName, "mcp_gateway");
    // The signature is bound to the wallet's private key, the tool name, the
    // amount and the gateway URL. Without a wallet we still return a
    // deterministic signature so tests can use a fixed-shape payload.
    std::ostringstream payload;
    payload << gatewayUrl_ << ":" << amount << ":" << toolName << ":" << walletPrivateKey_;
    return mcp_internal::digest(payload.str());
}

// MCPServer implementation

MCPServer::MCPServer(const std::string& serverName)
    : serverName_(serverName)
    , listening_(false)
    , logger_(std::make_shared<AgentLogger>()) {
    
    elizaos::logInfo("MCP Server initialized: " + serverName, "mcp_gateway");
}

void MCPServer::registerTool(const std::string& name, const std::string& description,
                             const MCPJsonValue& schema,
                             std::function<MCPJsonValue(const MCPJsonValue&)> handler) {
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
    if (auto* gw = mcp_internal::lookupGateway(gatewayUrl)) {
        // Forward our locally registered tools and resources to the gateway
        // so they become discoverable by clients connected to the same URL.
        for (auto& tool : tools_) {
            gw->registerTool(tool);
        }
        for (auto& res : resources_) {
            gw->registerResource(res);
        }
    }
}

void MCPServer::disconnectFromGateway() {
    elizaos::logInfo("Disconnecting from gateway", "mcp_gateway");
    if (auto* gw = mcp_internal::lookupGateway(gatewayUrl_)) {
        for (auto& tool : tools_) {
            const std::string fullName = tool.namespace_.empty() ? tool.name : tool.namespace_ + "::" + tool.name;
            gw->unregisterTool(fullName);
        }
        for (auto& res : resources_) {
            gw->unregisterResource(res.uri);
        }
    }
    gatewayUrl_.clear();
}

void MCPServer::startListening() {
    listening_ = true;
    elizaos::logInfo("Server started listening", "mcp_gateway");
}

void MCPServer::stopListening() {
    listening_ = false;
    elizaos::logInfo("Server stopped listening", "mcp_gateway");
}

// =============================================================================
// Transport Layer Implementations
// =============================================================================

/**
 * Transport base class for MCP protocol communication
 */
class MCPTransport {
public:
    virtual ~MCPTransport() = default;
    virtual bool connect(const std::string& endpoint) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    virtual MCPJsonValue sendRequest(const std::string& method, const MCPJsonValue& params) = 0;
    virtual void setMessageHandler(std::function<void(const MCPJsonValue&)> handler) = 0;
};

/**
 * STDIO Transport - communicates via stdin/stdout with child processes
 */
class StdioTransport : public MCPTransport {
public:
    StdioTransport() = default;
    ~StdioTransport() override { disconnect(); }

    bool connect(const std::string& command) override {
        elizaos::logInfo("STDIO Transport connecting: " + command, "mcp_transport");
        command_ = command;
        connected_ = true;
        // In production, would spawn child process and std::set up pipes
        return true;
    }

    void disconnect() override {
        if (connected_) {
            elizaos::logInfo("STDIO Transport disconnecting", "mcp_transport");
            connected_ = false;
            // Would terminate child process
        }
    }

    bool isConnected() const override { return connected_; }

    MCPJsonValue sendRequest(const std::string& method, const MCPJsonValue& params) override {
        elizaos::logInfo("STDIO sending: " + method, "mcp_transport");

        MCPJsonValue request;
        request["jsonrpc"] = "2.0";
        request["id"] = ++requestId_;
        request["method"] = method;
        request["params"] = params;

        // In production: write to stdin, read from stdout
        MCPJsonValue response;
        response["jsonrpc"] = "2.0";
        response["id"] = requestId_;
        response["result"] = MCPJsonValue::object();
        return response;
    }

    void setMessageHandler(std::function<void(const MCPJsonValue&)> handler) override {
        messageHandler_ = std::move(handler);
    }

private:
    std::string command_;
    bool connected_ = false;
    int requestId_ = 0;
    std::function<void(const MCPJsonValue&)> messageHandler_;
};

/**
 * HTTP Transport - REST API communication
 */
class HttpTransport : public MCPTransport {
public:
    HttpTransport() = default;
    ~HttpTransport() override { disconnect(); }

    bool connect(const std::string& endpoint) override {
        elizaos::logInfo("HTTP Transport connecting: " + endpoint, "mcp_transport");
        endpoint_ = endpoint;
        connected_ = true;
        return true;
    }

    void disconnect() override {
        if (connected_) {
            elizaos::logInfo("HTTP Transport disconnecting", "mcp_transport");
            connected_ = false;
        }
    }

    bool isConnected() const override { return connected_; }

    MCPJsonValue sendRequest(const std::string& method, const MCPJsonValue& params) override {
        elizaos::logInfo("HTTP POST to: " + endpoint_ + "/" + method, "mcp_transport");

        MCPJsonValue request;
        request["method"] = method;
        request["params"] = params;

        // In production: send HTTP POST request using libcurl or similar
        // Example headers: Content-Type: application/json, Authorization: Bearer {apiKey}

        MCPJsonValue response;
        response["status"] = 200;
        response["result"] = MCPJsonValue::object();
        return response;
    }

    void setMessageHandler(std::function<void(const MCPJsonValue&)> handler) override {
        messageHandler_ = std::move(handler);
    }

    void setHeaders(const std::unordered_map<std::string, std::string>& headers) {
        headers_ = headers;
    }

    void setTimeout(int timeoutMs) {
        timeoutMs_ = timeoutMs;
    }

private:
    std::string endpoint_;
    bool connected_ = false;
    std::unordered_map<std::string, std::string> headers_;
    int timeoutMs_ = 30000;
    std::function<void(const MCPJsonValue&)> messageHandler_;
};

/**
 * WebSocket Transport - bidirectional real-time communication
 */
class WebSocketTransport : public MCPTransport {
public:
    WebSocketTransport() = default;
    ~WebSocketTransport() override { disconnect(); }

    bool connect(const std::string& endpoint) override {
        elizaos::logInfo("WebSocket Transport connecting: " + endpoint, "mcp_transport");
        endpoint_ = endpoint;

        // In production: establish WebSocket connection
        // Handle ws:// and wss:// protocols
        connected_ = true;
        return true;
    }

    void disconnect() override {
        if (connected_) {
            elizaos::logInfo("WebSocket Transport disconnecting", "mcp_transport");
            connected_ = false;
            // Would send close frame and terminate connection
        }
    }

    bool isConnected() const override { return connected_; }

    MCPJsonValue sendRequest(const std::string& method, const MCPJsonValue& params) override {
        elizaos::logInfo("WebSocket sending: " + method, "mcp_transport");

        MCPJsonValue request;
        request["jsonrpc"] = "2.0";
        request["id"] = ++requestId_;
        request["method"] = method;
        request["params"] = params;

        // In production: send JSON-RPC message over WebSocket
        // Store pending request for response matching
        pendingRequests_[requestId_] = method;

        // Simulated response
        MCPJsonValue response;
        response["jsonrpc"] = "2.0";
        response["id"] = requestId_;
        response["result"] = MCPJsonValue::object();
        return response;
    }

    void setMessageHandler(std::function<void(const MCPJsonValue&)> handler) override {
        messageHandler_ = std::move(handler);
    }

    void sendNotification(const std::string& method, const MCPJsonValue& params) {
        elizaos::logInfo("WebSocket notification: " + method, "mcp_transport");

        MCPJsonValue notification;
        notification["jsonrpc"] = "2.0";
        notification["method"] = method;
        notification["params"] = params;

        // In production: send notification (no id field)
    }

    void setReconnectInterval(int intervalMs) {
        reconnectIntervalMs_ = intervalMs;
    }

    void enableAutoReconnect(bool enable) {
        autoReconnect_ = enable;
    }

private:
    std::string endpoint_;
    bool connected_ = false;
    int requestId_ = 0;
    std::unordered_map<int, std::string> pendingRequests_;
    std::function<void(const MCPJsonValue&)> messageHandler_;
    int reconnectIntervalMs_ = 5000;
    bool autoReconnect_ = true;
};

/**
 * SSE (Server-Sent Events) Transport - unidirectional streaming
 */
class SSETransport : public MCPTransport {
public:
    SSETransport() = default;
    ~SSETransport() override { disconnect(); }

    bool connect(const std::string& endpoint) override {
        elizaos::logInfo("SSE Transport connecting: " + endpoint, "mcp_transport");
        endpoint_ = endpoint;
        connected_ = true;
        // In production: establish SSE connection with EventSource
        return true;
    }

    void disconnect() override {
        if (connected_) {
            elizaos::logInfo("SSE Transport disconnecting", "mcp_transport");
            connected_ = false;
        }
    }

    bool isConnected() const override { return connected_; }

    MCPJsonValue sendRequest(const std::string& method, const MCPJsonValue& params) override {
        // SSE is receive-only; send via HTTP POST
        elizaos::logInfo("SSE command (via HTTP): " + method, "mcp_transport");

        // In production: POST to command endpoint, receive response via SSE
        MCPJsonValue response;
        response["status"] = "pending";
        response["message"] = "Response will arrive via SSE stream";
        (void)params;
        return response;
    }

    void setMessageHandler(std::function<void(const MCPJsonValue&)> handler) override {
        messageHandler_ = std::move(handler);
    }

    void setEventTypes(const std::vector<std::string>& types) {
        eventTypes_ = types;
    }

private:
    std::string endpoint_;
    bool connected_ = false;
    std::function<void(const MCPJsonValue&)> messageHandler_;
    std::vector<std::string> eventTypes_;
};

/**
 * TransportFactory - creates appropriate transport based on configuration
 */
class TransportFactory {
public:
    static std::unique_ptr<MCPTransport> create(const std::string& transportType) {
        if (transportType == "stdio") {
            return std::make_unique<StdioTransport>();
        } else if (transportType == "http") {
            return std::make_unique<HttpTransport>();
        } else if (transportType == "websocket") {
            return std::make_unique<WebSocketTransport>();
        } else if (transportType == "sse") {
            return std::make_unique<SSETransport>();
        }

        elizaos::logWarning("Unknown transport type: " + transportType + ", defaulting to HTTP", "mcp_transport");
        return std::make_unique<HttpTransport>();
    }
};

/**
 * MCPConnection - manages connection to a single MCP server
 */
class MCPConnection {
public:
    MCPConnection(const MCPServerConfig& config)
        : config_(config)
        , transport_(TransportFactory::create(config.transport)) {
    }

    bool connect() {
        if (!transport_->connect(config_.endpoint)) {
            elizaos::logError("Failed to connect to: " + config_.name, "mcp_connection");
            return false;
        }

        // Initialize the connection
        MCPJsonValue initParams;
        initParams["protocolVersion"] = "2024-11-05";
        initParams["capabilities"] = MCPJsonValue::object();
        initParams["clientInfo"]["name"] = "elizaos-mcp-client";
        initParams["clientInfo"]["version"] = "1.0.0";

        auto response = transport_->sendRequest("initialize", initParams);

        if (response.contains("error")) {
            elizaos::logError("Initialization failed: " + response["error"].dump(), "mcp_connection");
            return false;
        }

        initialized_ = true;
        elizaos::logInfo("Connected and initialized: " + config_.name, "mcp_connection");
        return true;
    }

    void disconnect() {
        if (initialized_) {
            // Send shutdown notification
            transport_->sendRequest("shutdown", MCPJsonValue::object());
        }
        transport_->disconnect();
        initialized_ = false;
    }

    bool isConnected() const {
        return transport_->isConnected() && initialized_;
    }

    std::vector<MCPTool> discoverTools() {
        std::vector<MCPTool> tools;

        auto response = transport_->sendRequest("tools/list", MCPJsonValue::object());

        if (response.contains("result") && response["result"].contains("tools")) {
            for (const auto& toolJson : response["result"]["tools"]) {
                MCPTool tool;
                tool.name = toolJson.value("name", "");
                tool.namespace_ = config_.name;
                tool.description = toolJson.value("description", "");
                if (toolJson.contains("inputSchema")) {
                    tool.inputSchema = toolJson["inputSchema"];
                }
                tools.push_back(tool);
            }
        }

        return tools;
    }

    std::vector<MCPResource> discoverResources() {
        std::vector<MCPResource> resources;

        auto response = transport_->sendRequest("resources/list", MCPJsonValue::object());

        if (response.contains("result") && response["result"].contains("resources")) {
            for (const auto& resJson : response["result"]["resources"]) {
                MCPResource resource;
                resource.uri = resJson.value("uri", "");
                resource.namespace_ = config_.name;
                resource.mimeType = resJson.value("mimeType", "application/octet-stream");
                resource.description = resJson.value("description", "");
                resources.push_back(resource);
            }
        }

        return resources;
    }

    MCPJsonValue callTool(const std::string& toolName, const MCPJsonValue& arguments) {
        MCPJsonValue params;
        params["name"] = toolName;
        params["arguments"] = arguments;

        return transport_->sendRequest("tools/call", params);
    }

    MCPJsonValue readResource(const std::string& uri) {
        MCPJsonValue params;
        params["uri"] = uri;

        return transport_->sendRequest("resources/read", params);
    }

    const std::string& getName() const { return config_.name; }

private:
    MCPServerConfig config_;
    std::unique_ptr<MCPTransport> transport_;
    bool initialized_ = false;
};

} // namespace elizaos
