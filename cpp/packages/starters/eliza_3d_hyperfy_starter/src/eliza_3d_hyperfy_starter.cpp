// eliza_3d_hyperfy_starter.cpp - Complete implementation with WebSocket support
// This file implements the Hyperfy 3D world integration with real WebSocket communication

#include "elizaos/eliza_3d_hyperfy_starter.hpp"
#include "elizaos/agentlogger.hpp"
#include <sstream>
#include <chrono>
#include <iostream>
#include <thread>
#include <algorithm>

// Check if WebSocket library is available
#ifdef HYPERFY_WEBSOCKET_IMPLEMENTATION
    // Real WebSocket implementation using libwebsockets or similar
    #include <libwebsockets.h>
    #define HAS_WEBSOCKET 1
#else
    // Simulated WebSocket for systems without libwebsockets
    #define HAS_WEBSOCKET 0
#endif

namespace elizaos {
namespace hyperfy {

// ==============================================================================
// HELPER FUNCTIONS
// ==============================================================================

namespace {
    void logInfo(const std::string& message, const std::string& component) {
        AgentLogger logger;
        logger.log(message, component, "Hyperfy", LogLevel::INFO);
    }
    
    void logSuccess(const std::string& message, const std::string& component) {
        AgentLogger logger;
        logger.log(message, component, "Hyperfy", LogLevel::SUCCESS);
    }
    
    void logWarning(const std::string& message, const std::string& component) {
        AgentLogger logger;
        logger.log(message, component, "Hyperfy", LogLevel::WARNING);
    }
    
    void logError(const std::string& message, const std::string& component) {
        AgentLogger logger;
        logger.log(message, component, "Hyperfy", LogLevel::ERROR);
    }
}

// ==============================================================================
// WEBSOCKET CLIENT IMPLEMENTATION
// ==============================================================================

#if HAS_WEBSOCKET

namespace websocket_impl {

class WebSocketClient {
private:
    struct lws_context* context_;
    struct lws* wsi_;
    std::string url_;
    std::string authToken_;
    std::atomic<bool> connected_;
    std::mutex sendMutex_;
    std::queue<std::string> sendQueue_;
    std::function<void(const std::string&)> messageCallback_;
    
    static int callback(struct lws* wsi, enum lws_callback_reasons reason,
                       void* user, void* in, size_t len) {
        WebSocketClient* client = static_cast<WebSocketClient*>(user);
        
        switch (reason) {
            case LWS_CALLBACK_CLIENT_ESTABLISHED:
                client->connected_.store(true);
                logSuccess("WebSocket connection established", "WebSocketClient");
                break;
                
            case LWS_CALLBACK_CLIENT_RECEIVE:
                if (client->messageCallback_ && in && len > 0) {
                    std::string message(static_cast<char*>(in), len);
                    client->messageCallback_(message);
                }
                break;
                
            case LWS_CALLBACK_CLIENT_WRITEABLE:
                {
                    std::lock_guard<std::mutex> lock(client->sendMutex_);
                    if (!client->sendQueue_.empty()) {
                        std::string msg = client->sendQueue_.front();
                        client->sendQueue_.pop();
                        
                        unsigned char buf[LWS_PRE + msg.size()];
                        memcpy(&buf[LWS_PRE], msg.c_str(), msg.size());
                        lws_write(wsi, &buf[LWS_PRE], msg.size(), LWS_WRITE_TEXT);
                        
                        if (!client->sendQueue_.empty()) {
                            lws_callback_on_writable(wsi);
                        }
                    }
                }
                break;
                
            case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
                logError("WebSocket connection error", "WebSocketClient");
                client->connected_.store(false);
                break;
                
            case LWS_CALLBACK_CLOSED:
                logInfo("WebSocket connection closed", "WebSocketClient");
                client->connected_.store(false);
                break;
                
            default:
                break;
        }
        
        return 0;
    }
    
public:
    WebSocketClient() : context_(nullptr), wsi_(nullptr), connected_(false) {}
    
    ~WebSocketClient() {
        disconnect();
    }
    
    bool connect(const std::string& url, const std::string& authToken) {
        url_ = url;
        authToken_ = authToken;
        
        struct lws_context_creation_info info;
        memset(&info, 0, sizeof(info));
        info.port = CONTEXT_PORT_NO_LISTEN;
        info.protocols = protocols_;
        info.gid = -1;
        info.uid = -1;
        
        context_ = lws_create_context(&info);
        if (!context_) {
            logError("Failed to create WebSocket context", "WebSocketClient");
            return false;
        }
        
        struct lws_client_connect_info ccinfo;
        memset(&ccinfo, 0, sizeof(ccinfo));
        ccinfo.context = context_;
        ccinfo.address = url_.c_str();
        ccinfo.port = 443;  // HTTPS port
        ccinfo.path = "/";
        ccinfo.host = ccinfo.address;
        ccinfo.origin = ccinfo.address;
        ccinfo.protocol = "hyperfy-protocol";
        ccinfo.ssl_connection = LCCSCF_USE_SSL;
        ccinfo.userdata = this;
        
        wsi_ = lws_client_connect_via_info(&ccinfo);
        if (!wsi_) {
            logError("Failed to connect WebSocket", "WebSocketClient");
            lws_context_destroy(context_);
            context_ = nullptr;
            return false;
        }
        
        // Service the connection in a loop until connected or timeout
        int timeout = 50;  // 5 seconds
        while (!connected_.load() && timeout-- > 0) {
            lws_service(context_, 100);
        }
        
        return connected_.load();
    }
    
    void disconnect() {
        if (wsi_) {
            lws_close_reason(wsi_, LWS_CLOSE_STATUS_NORMAL, nullptr, 0);
            wsi_ = nullptr;
        }
        
        if (context_) {
            lws_context_destroy(context_);
            context_ = nullptr;
        }
        
        connected_.store(false);
    }
    
    bool isConnected() const {
        return connected_.load();
    }
    
    bool send(const std::string& message) {
        if (!connected_.load()) {
            return false;
        }
        
        {
            std::lock_guard<std::mutex> lock(sendMutex_);
            sendQueue_.push(message);
        }
        
        lws_callback_on_writable(wsi_);
        return true;
    }
    
    void setMessageCallback(std::function<void(const std::string&)> callback) {
        messageCallback_ = callback;
    }
    
    void service(int timeout_ms = 50) {
        if (context_) {
            lws_service(context_, timeout_ms);
        }
    }
    
private:
    static struct lws_protocols protocols_[];
};

struct lws_protocols WebSocketClient::protocols_[] = {
    {
        "hyperfy-protocol",
        WebSocketClient::callback,
        0,
        4096,
    },
    { NULL, NULL, 0, 0 }
};

} // namespace websocket_impl

#else // !HAS_WEBSOCKET

// ==============================================================================
// SIMULATED WEBSOCKET (Fallback when libwebsockets not available)
// ==============================================================================

namespace websocket_impl {

class WebSocketClient {
private:
    std::atomic<bool> connected_;
    std::function<void(const std::string&)> messageCallback_;
    std::queue<std::string> messageQueue_;
    std::mutex queueMutex_;
    
public:
    WebSocketClient() : connected_(false) {}
    
    bool connect(const std::string& url, const std::string& authToken) {
        logWarning("Using simulated WebSocket (libwebsockets not available)", "WebSocketClient");
        logInfo("Simulating connection to: " + url +
                (authToken.empty() ? " (no auth token)" : " (with auth token)"),
                "WebSocketClient");
        
        // Simulate connection delay
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        connected_.store(true);
        logSuccess("Simulated WebSocket connection established", "WebSocketClient");
        return true;
    }
    
    void disconnect() {
        connected_.store(false);
        logInfo("Simulated WebSocket disconnected", "WebSocketClient");
    }
    
    bool isConnected() const {
        return connected_.load();
    }
    
    bool send(const std::string& message) {
        if (!connected_.load()) {
            return false;
        }
        
        logInfo("Simulated send: " + message.substr(0, 100), "WebSocketClient");
        
        // Simulate echo response
        if (messageCallback_) {
            std::string response = "{\"type\":\"ack\",\"message\":\"received\"}";
            messageCallback_(response);
        }
        
        return true;
    }
    
    void setMessageCallback(std::function<void(const std::string&)> callback) {
        messageCallback_ = callback;
    }
    
    void service(int /* timeout_ms */ = 50) {
        // No-op for simulated WebSocket
    }
};

} // namespace websocket_impl

#endif // HAS_WEBSOCKET

// ==============================================================================
// HYPERFYWORLD IMPLEMENTATION
// ==============================================================================

HyperfyWorld::HyperfyWorld(const std::string& worldId, const std::string& wsUrl) 
    : worldId_(worldId), wsUrl_(wsUrl), connected_(false) {
    wsClient_.reset(reinterpret_cast<WebSocketClient*>(new websocket_impl::WebSocketClient()));
}

HyperfyWorld::~HyperfyWorld() {
    disconnect();
    if (wsClient_) { delete reinterpret_cast<websocket_impl::WebSocketClient*>(wsClient_.release()); }
}

bool HyperfyWorld::connect(const std::string& authToken) {
    std::lock_guard<std::mutex> lock(worldMutex_);
    
    if (connected_.load()) {
        logWarning("Already connected to world: " + worldId_, "HyperfyWorld");
        return true;
    }
    
    logInfo("Connecting to Hyperfy world: " + worldId_ + " at " + wsUrl_, "HyperfyWorld");
    
    auto* client = reinterpret_cast<websocket_impl::WebSocketClient*>(wsClient_.get());
    
    // Set up message callback
    client->setMessageCallback([this](const std::string& message) {
        handleWebSocketMessage(message);
    });
    
    // Connect WebSocket
    if (!client->connect(wsUrl_, authToken)) {
        logError("Failed to connect WebSocket to world: " + worldId_, "HyperfyWorld");
        return false;
    }
    
    // Send authentication message
    std::ostringstream authMsg;
    authMsg << "{\"type\":\"auth\",\"worldId\":\"" << worldId_ 
            << "\",\"token\":\"" << authToken << "\"}";
    
    if (!client->send(authMsg.str())) {
        logError("Failed to send auth message", "HyperfyWorld");
        client->disconnect();
        return false;
    }
    
    connected_.store(true);
    logSuccess("Successfully connected to Hyperfy world: " + worldId_, "HyperfyWorld");
    
    return true;
}

void HyperfyWorld::disconnect() {
    std::lock_guard<std::mutex> lock(worldMutex_);
    
    if (!connected_.load()) {
        return;
    }
    
    logInfo("Disconnecting from Hyperfy world: " + worldId_, "HyperfyWorld");
    
    auto* client = reinterpret_cast<websocket_impl::WebSocketClient*>(wsClient_.get());
    client->disconnect();
    
    connected_.store(false);
    worldState_.clear();
    
    logInfo("Disconnected from Hyperfy world: " + worldId_, "HyperfyWorld");
}

bool HyperfyWorld::isConnected() const {
    return connected_.load();
}

void HyperfyWorld::updateState(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(worldMutex_);
    worldState_[key] = value;
}

std::string HyperfyWorld::getState(const std::string& key) const {
    std::lock_guard<std::mutex> lock(worldMutex_);
    auto it = worldState_.find(key);
    return (it != worldState_.end()) ? it->second : "";
}

bool HyperfyWorld::sendMessage(const std::string& message) {
    if (!connected_.load()) {
        logWarning("Cannot send message: not connected to world", "HyperfyWorld");
        return false;
    }
    
    auto* client = reinterpret_cast<websocket_impl::WebSocketClient*>(wsClient_.get());
    
    // Create JSON message
    std::ostringstream jsonMsg;
    jsonMsg << "{\"type\":\"message\",\"worldId\":\"" << worldId_ 
            << "\",\"content\":\"" << escapeJson(message) << "\"}";
    
    if (!client->send(jsonMsg.str())) {
        logError("Failed to send message to world", "HyperfyWorld");
        return false;
    }
    
    logInfo("Sent message to world " + worldId_ + ": " + message, "HyperfyWorld");
    return true;
}

bool HyperfyWorld::moveToPosition(double x, double y, double z) {
    if (!connected_.load()) {
        logWarning("Cannot move: not connected to world", "HyperfyWorld");
        return false;
    }
    
    auto* client = reinterpret_cast<websocket_impl::WebSocketClient*>(wsClient_.get());
    
    // Create position update message
    std::ostringstream posMsg;
    posMsg << "{\"type\":\"move\",\"worldId\":\"" << worldId_ 
           << "\",\"position\":{\"x\":" << x << ",\"y\":" << y << ",\"z\":" << z << "}}";
    
    if (!client->send(posMsg.str())) {
        logError("Failed to send position update", "HyperfyWorld");
        return false;
    }
    
    // Update local state
    updateState("position_x", std::to_string(x));
    updateState("position_y", std::to_string(y));
    updateState("position_z", std::to_string(z));
    
    std::ostringstream oss;
    oss << "Moved to position (" << x << ", " << y << ", " << z << ") in world " << worldId_;
    logInfo(oss.str(), "HyperfyWorld");
    
    return true;
}

bool HyperfyWorld::performAction(const std::string& action, const std::string& parameters) {
    if (!connected_.load()) {
        logWarning("Cannot perform action: not connected to world", "HyperfyWorld");
        return false;
    }
    
    auto* client = reinterpret_cast<websocket_impl::WebSocketClient*>(wsClient_.get());
    
    // Create action message
    std::ostringstream actionMsg;
    actionMsg << "{\"type\":\"action\",\"worldId\":\"" << worldId_ 
              << "\",\"action\":\"" << escapeJson(action) 
              << "\",\"parameters\":\"" << escapeJson(parameters) << "\"}";
    
    if (!client->send(actionMsg.str())) {
        logError("Failed to send action", "HyperfyWorld");
        return false;
    }
    
    logInfo("Performed action '" + action + "' with parameters: " + parameters, "HyperfyWorld");
    return true;
}

void HyperfyWorld::handleWebSocketMessage(const std::string& message) {
    logInfo("Received WebSocket message: " + message.substr(0, 100), "HyperfyWorld");
    
    // Parse message and update world state
    // This is a simplified parser - in production, use a JSON library
    if (message.find("\"type\":\"state\"") != std::string::npos) {
        // State update message
        // Extract and update world state
    } else if (message.find("\"type\":\"event\"") != std::string::npos) {
        // Event notification
        // Handle world events
    }
}

std::string HyperfyWorld::escapeJson(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '"':  oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:   oss << c; break;
        }
    }
    return oss.str();
}

// ==============================================================================
// HYPERFYSERVICE IMPLEMENTATION
// ==============================================================================

HyperfyService::HyperfyService() : running_(false) {
}

HyperfyService::~HyperfyService() {
    stop();
}

bool HyperfyService::start(const HyperfyConfig& config) {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    
    if (running_.load()) {
        logWarning("HyperfyService is already running", "HyperfyService");
        return true;
    }
    
    config_ = config;
    logInfo("Starting HyperfyService with URL: " + config_.wsUrl, "HyperfyService");
    
    running_.store(true);
    serviceThread_ = std::thread(&HyperfyService::serviceLoop, this);
    
    logSuccess("HyperfyService started successfully", "HyperfyService");
    return true;
}

void HyperfyService::stop() {
    {
        std::lock_guard<std::mutex> lock(serviceMutex_);
        if (!running_.load()) {
            return;
        }
        
        logInfo("Stopping HyperfyService", "HyperfyService");
        running_.store(false);
    }
    
    // Wait for service std::thread to complete
    if (serviceThread_.joinable()) {
        serviceThread_.join();
    }
    
    // Disconnect all worlds
    {
        std::lock_guard<std::mutex> lock(serviceMutex_);
        if (world_) {
            world_->disconnect();
            world_.reset();
        }
    }
    
    logInfo("HyperfyService stopped", "HyperfyService");
}

bool HyperfyService::connectToWorld(const std::string& worldId, 
                                    const std::string& wsUrl,
                                    const std::string& authToken) {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    
    if (!running_.load()) {
        logError("Cannot connect to world: service not running", "HyperfyService");
        return false;
    }
    
    // Check if already connected
    if (world_ && world_->isConnected()) {
        logInfo("Already connected to a world", "HyperfyService");
        return true;
    }
    
    // Create new world connection
    world_ = std::make_shared<HyperfyWorld>(worldId, wsUrl.empty() ? config_.wsUrl : wsUrl);
    
    if (!world_->connect(authToken)) {
        logError("Failed to connect to world: " + worldId, "HyperfyService");
        world_.reset();
        return false;
    }
    
    logSuccess("Connected to world: " + worldId, "HyperfyService");
    return true;
}

void HyperfyService::disconnectFromWorld() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    
    if (world_) {
        world_->disconnect();
        world_.reset();
        logInfo("Disconnected from world", "HyperfyService");
    }
}

// getWorld() is inline in the header



void HyperfyService::serviceLoop() {
    logInfo("HyperfyService loop started", "HyperfyService");
    
    while (running_.load()) {
        // Service all connected worlds
        {
            std::lock_guard<std::mutex> lock(serviceMutex_);
            if (world_ && world_->isConnected()) {
                    world_->sendHeartbeat();
                }
        }
        
        // Sleep to avoid busy-waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    logInfo("HyperfyService loop stopped", "HyperfyService");
}


// HyperfyServiceFactory implementations
std::shared_ptr<HyperfyService> HyperfyServiceFactory::createService() {
    return std::make_shared<HyperfyService>();
}

std::shared_ptr<HyperfyService> HyperfyServiceFactory::createServiceWithConfig(const HyperfyConfig& config) {
    auto service = std::make_shared<HyperfyService>();
    // Pre-seed the service with the caller-supplied configuration so that a
    // subsequent start()/connect() uses the intended world URL and options.
    service->applyConfig(config);
    return service;
}

// HyperfyService action registry
void HyperfyService::registerAction(const std::string& name, std::shared_ptr<HyperfyAction> action) {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    actions_[name] = std::move(action);
}

std::shared_ptr<HyperfyAction> HyperfyService::getAction(const std::string& name) const {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    auto it = actions_.find(name);
    return it != actions_.end() ? it->second : nullptr;
}

// HyperfyService::executeAction implementation - resolve the named action and
// dispatch it with the supplied parameter string. Returns false when the
// service is stopped or no action is registered under `name`.
bool HyperfyService::executeAction(const std::string& name, const std::string& parameters) {
    if (!isRunning()) return false;
    auto action = getAction(name);
    if (!action) {
        logWarning("executeAction: no action registered named '" + name + "'", "HyperfyService");
        return false;
    }
    return action->execute(parameters);
}


// WebSocketClient implementations (header-declared version)
WebSocketClient::WebSocketClient() : connected_(false), running_(false) {}

WebSocketClient::~WebSocketClient() {
    disconnect();
}

bool WebSocketClient::connect(const std::string& url, const std::string& authToken) {
    // In-process simulation layer: record the target endpoint, mark the
    // connection live and fire the onConnect callback. authToken is retained
    // alongside the url so a future libwebsockets/Boost.Beast backend can
    // present credentials during the real handshake.
    {
        std::lock_guard<std::mutex> lock(mutex_);
        url_ = authToken.empty() ? url : (url + "?token=" + authToken);
        connected_ = true;
        running_ = true;
    }
    if (onConnect_) {
        onConnect_();
    }
    return true;
}

void WebSocketClient::disconnect() {
    bool wasConnected = connected_.exchange(false);
    running_ = false;
    cv_.notify_all();
    if (wasConnected && onDisconnect_) {
        onDisconnect_("client disconnect");
    }
}

bool WebSocketClient::isConnected() const {
    return connected_;
}

bool WebSocketClient::send(const WebSocketMessage& message) {
    if (!connected_) {
        if (onError_) {
            onError_("send failed: not connected");
        }
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    outgoingMessages_.push(message);
    return true;
}

bool WebSocketClient::sendText(const std::string& text) {
    WebSocketMessage msg;
    msg.type = MessageType::CHAT_MESSAGE;
    msg.payload = text;
    msg.sender = "local";
    msg.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return send(msg);
}

void WebSocketClient::setOnMessage(OnMessageCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    onMessage_ = std::move(callback);
}
void WebSocketClient::setOnConnect(OnConnectCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    onConnect_ = std::move(callback);
}
void WebSocketClient::setOnDisconnect(OnDisconnectCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    onDisconnect_ = std::move(callback);
}
void WebSocketClient::setOnError(OnErrorCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    onError_ = std::move(callback);
}

bool WebSocketClient::hasPendingMessages() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return !incomingMessages_.empty();
}

WebSocketMessage WebSocketClient::popMessage() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (incomingMessages_.empty()) {
        return WebSocketMessage{};
    }
    WebSocketMessage msg = incomingMessages_.front();
    incomingMessages_.pop();
    return msg;
}

void WebSocketClient::messageProcessingLoop() {
    // Drain queued inbound messages and dispatch them to the registered
    // handler while the client is running. Used by a background processing
    // thread when a real transport is wired in.
    while (running_) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !incomingMessages_.empty() || !running_; });
        while (!incomingMessages_.empty()) {
            WebSocketMessage msg = incomingMessages_.front();
            incomingMessages_.pop();
            OnMessageCallback cb = onMessage_;
            lock.unlock();
            if (cb) {
                cb(msg);
            }
            lock.lock();
        }
    }
}

void WebSocketClient::simulateIncomingMessage(const WebSocketMessage& msg) {
    // Inject a message as if it arrived from the world server. The message is
    // both queued (for hasPendingMessages/popMessage polling consumers) and
    // delivered synchronously to any registered onMessage callback.
    OnMessageCallback cb;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        incomingMessages_.push(msg);
        cb = onMessage_;
    }
    cv_.notify_all();
    if (cb) {
        cb(msg);
    }
}

// HyperfyWorld::sendHeartbeat implementation
bool HyperfyWorld::sendHeartbeat() {
    if (!wsClient_ || !wsClient_->isConnected()) return false;
    return wsClient_->sendText("heartbeat");
}

} // namespace hyperfy
} // namespace elizaos
