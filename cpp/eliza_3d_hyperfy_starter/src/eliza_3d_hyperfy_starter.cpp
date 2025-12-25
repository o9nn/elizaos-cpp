#include "elizaos/eliza_3d_hyperfy_starter.hpp"
#include "elizaos/agentlogger.hpp"
#include <sstream>
#include <chrono>
#include <iostream>
#include <cmath>
#include <random>

namespace elizaos {
    namespace hyperfy {

        // =========================================================================
        // WebSocketMessage Implementation
        // =========================================================================

        std::string WebSocketMessage::toJson() const {
            std::ostringstream oss;
            oss << "{\"type\":" << static_cast<int>(type)
                << ",\"payload\":\"" << payload << "\""
                << ",\"sender\":\"" << sender << "\""
                << ",\"timestamp\":" << timestamp << "}";
            return oss.str();
        }

        WebSocketMessage WebSocketMessage::fromJson(const std::string& json) {
            WebSocketMessage msg;
            msg.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

            // Simple JSON parsing (production code would use a JSON library)
            size_t typePos = json.find("\"type\":");
            if (typePos != std::string::npos) {
                int typeVal = std::stoi(json.substr(typePos + 7));
                msg.type = static_cast<MessageType>(typeVal);
            }

            size_t payloadPos = json.find("\"payload\":\"");
            if (payloadPos != std::string::npos) {
                size_t start = payloadPos + 11;
                size_t end = json.find("\"", start);
                if (end != std::string::npos) {
                    msg.payload = json.substr(start, end - start);
                }
            }

            return msg;
        }

        // =========================================================================
        // Position3D Implementation
        // =========================================================================

        std::string Position3D::toString() const {
            std::ostringstream oss;
            oss << x << "," << y << "," << z;
            return oss.str();
        }

        Position3D Position3D::fromString(const std::string& str) {
            Position3D pos;
            std::stringstream ss(str);
            std::string item;

            if (std::getline(ss, item, ',')) pos.x = std::stod(item);
            if (std::getline(ss, item, ',')) pos.y = std::stod(item);
            if (std::getline(ss, item, ',')) pos.z = std::stod(item);

            return pos;
        }

        // =========================================================================
        // WebSocketClient Implementation (Simulation Layer)
        // =========================================================================

        WebSocketClient::WebSocketClient()
            : connected_(false), running_(false) {
        }

        WebSocketClient::~WebSocketClient() {
            disconnect();
        }

        bool WebSocketClient::connect(const std::string& url, const std::string& authToken) {
            std::lock_guard<std::mutex> lock(mutex_);

            if (connected_) {
                return true;
            }

            url_ = url;
            logInfo("WebSocket connecting to: " + url, "WebSocketClient");

            // Simulate connection handshake
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            // Validate URL format (basic check)
            if (url.find("ws://") != 0 && url.find("wss://") != 0) {
                if (onError_) {
                    onError_("Invalid WebSocket URL: " + url);
                }
                return false;
            }

            // Simulate auth token validation
            if (!authToken.empty()) {
                logInfo("Authenticating with token", "WebSocketClient");
            }

            connected_ = true;
            running_ = true;

            // Start message processing thread
            processingThread_ = std::thread(&WebSocketClient::messageProcessingLoop, this);

            if (onConnect_) {
                onConnect_();
            }

            logInfo("WebSocket connected successfully", "WebSocketClient");
            return true;
        }

        void WebSocketClient::disconnect() {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (!connected_) {
                    return;
                }

                running_ = false;
                connected_ = false;
            }

            cv_.notify_all();

            if (processingThread_.joinable()) {
                processingThread_.join();
            }

            if (onDisconnect_) {
                onDisconnect_("Client disconnected");
            }

            logInfo("WebSocket disconnected", "WebSocketClient");
        }

        bool WebSocketClient::isConnected() const {
            return connected_;
        }

        bool WebSocketClient::send(const WebSocketMessage& message) {
            if (!connected_) {
                logWarning("Cannot send: not connected", "WebSocketClient");
                return false;
            }

            std::lock_guard<std::mutex> lock(mutex_);
            outgoingMessages_.push(message);
            cv_.notify_one();

            logInfo("Message queued for sending: " + message.toJson(), "WebSocketClient");

            // Simulate response for certain message types
            if (message.type == MessageType::POSITION_UPDATE) {
                // Simulate server acknowledgment
                WebSocketMessage ack;
                ack.type = MessageType::WORLD_STATE;
                ack.payload = "position_updated";
                ack.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
                simulateIncomingMessage(ack);
            }

            return true;
        }

        bool WebSocketClient::sendText(const std::string& text) {
            WebSocketMessage msg;
            msg.type = MessageType::CHAT_MESSAGE;
            msg.payload = text;
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
            while (running_) {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait_for(lock, std::chrono::milliseconds(100), [this] {
                    return !running_ || !incomingMessages_.empty();
                });

                // Process incoming messages
                while (!incomingMessages_.empty() && onMessage_) {
                    WebSocketMessage msg = incomingMessages_.front();
                    incomingMessages_.pop();
                    lock.unlock();
                    onMessage_(msg);
                    lock.lock();
                }
            }
        }

        void WebSocketClient::simulateIncomingMessage(const WebSocketMessage& msg) {
            std::lock_guard<std::mutex> lock(mutex_);
            incomingMessages_.push(msg);
            cv_.notify_one();
        }

        // =========================================================================
        // HyperfyWorld Implementation
        // =========================================================================

        HyperfyWorld::HyperfyWorld(const std::string& worldId, const std::string& wsUrl)
            : worldId_(worldId), wsUrl_(wsUrl), connected_(false) {
            wsClient_ = std::make_unique<WebSocketClient>();

            // Set up message handler
            wsClient_->setOnMessage([this](const WebSocketMessage& msg) {
                handleIncomingMessage(msg);
            });

            wsClient_->setOnError([this](const std::string& error) {
                logError("WebSocket error: " + error, "HyperfyWorld");
            });
        }

        HyperfyWorld::~HyperfyWorld() {
            disconnect();
        }

        bool HyperfyWorld::connect(const std::string& authToken) {
            std::lock_guard<std::mutex> lock(worldMutex_);

            if (connected_.load()) {
                return true;
            }

            logInfo("Connecting to Hyperfy world: " + worldId_ + " at " + wsUrl_, "HyperfyWorld");

            // Connect via WebSocket
            if (!wsClient_->connect(wsUrl_, authToken)) {
                logError("Failed to establish WebSocket connection", "HyperfyWorld");
                return false;
            }

            // Send connect message
            WebSocketMessage connectMsg;
            connectMsg.type = MessageType::CONNECT;
            connectMsg.payload = worldId_;
            connectMsg.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

            if (!wsClient_->send(connectMsg)) {
                wsClient_->disconnect();
                return false;
            }

            connected_.store(true);

            // Initialize position
            currentPosition_ = {0.0, 0.0, 0.0};
            updateState("position_x", "0.0");
            updateState("position_y", "0.0");
            updateState("position_z", "0.0");

            logInfo("Successfully connected to Hyperfy world: " + worldId_, "HyperfyWorld");
            return true;
        }

        void HyperfyWorld::disconnect() {
            std::lock_guard<std::mutex> lock(worldMutex_);

            if (!connected_.load()) {
                return;
            }

            logInfo("Disconnecting from Hyperfy world: " + worldId_, "HyperfyWorld");

            // Send disconnect message
            WebSocketMessage disconnectMsg;
            disconnectMsg.type = MessageType::DISCONNECT;
            disconnectMsg.payload = worldId_;
            disconnectMsg.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

            wsClient_->send(disconnectMsg);
            wsClient_->disconnect();

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

        Position3D HyperfyWorld::getCurrentPosition() const {
            std::lock_guard<std::mutex> lock(worldMutex_);
            return currentPosition_;
        }

        void HyperfyWorld::setCurrentPosition(const Position3D& pos) {
            std::lock_guard<std::mutex> lock(worldMutex_);
            currentPosition_ = pos;
            worldState_["position_x"] = std::to_string(pos.x);
            worldState_["position_y"] = std::to_string(pos.y);
            worldState_["position_z"] = std::to_string(pos.z);
        }

        bool HyperfyWorld::sendMessage(const std::string& message) {
            if (!connected_.load()) {
                logWarning("Cannot send message: not connected to world", "HyperfyWorld");
                return false;
            }

            logInfo("Sending message to world " + worldId_ + ": " + message, "HyperfyWorld");

            WebSocketMessage chatMsg;
            chatMsg.type = MessageType::CHAT_MESSAGE;
            chatMsg.payload = message;
            chatMsg.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

            return wsClient_->send(chatMsg);
        }

        bool HyperfyWorld::moveToPosition(double x, double y, double z) {
            if (!connected_.load()) {
                logWarning("Cannot move: not connected to world", "HyperfyWorld");
                return false;
            }

            std::ostringstream oss;
            oss << "Moving to position (" << x << ", " << y << ", " << z << ") in world " << worldId_;
            logInfo(oss.str(), "HyperfyWorld");

            // Create position update message
            WebSocketMessage posMsg;
            posMsg.type = MessageType::POSITION_UPDATE;

            std::ostringstream payload;
            payload << x << "," << y << "," << z;
            posMsg.payload = payload.str();
            posMsg.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

            if (wsClient_->send(posMsg)) {
                // Update local position
                Position3D newPos = {x, y, z};
                setCurrentPosition(newPos);
                return true;
            }

            return false;
        }

        bool HyperfyWorld::performAction(const std::string& action, const std::string& parameters) {
            if (!connected_.load()) {
                logWarning("Cannot perform action: not connected to world", "HyperfyWorld");
                return false;
            }

            logInfo("Performing action '" + action + "' with parameters: " + parameters, "HyperfyWorld");

            WebSocketMessage actionMsg;
            actionMsg.type = MessageType::ACTION;
            actionMsg.payload = action + ":" + parameters;
            actionMsg.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

            return wsClient_->send(actionMsg);
        }

        ScenePerception HyperfyWorld::perceiveScene() {
            std::lock_guard<std::mutex> lock(worldMutex_);

            lastPerception_.viewerPosition = currentPosition_;
            lastPerception_.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

            // Simulate scene perception with random entities
            lastPerception_.visibleEntities.clear();
            lastPerception_.nearbyPlayers.clear();

            // Generate some simulated entities based on position
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> entityCount(2, 5);
            std::uniform_real_distribution<> offset(-10.0, 10.0);

            int numEntities = entityCount(gen);
            for (int i = 0; i < numEntities; i++) {
                WorldEntity entity;
                entity.id = "entity_" + std::to_string(i);
                entity.type = (i % 2 == 0) ? "object" : "npc";
                entity.name = "Entity " + std::to_string(i);
                entity.position.x = currentPosition_.x + offset(gen);
                entity.position.y = currentPosition_.y;
                entity.position.z = currentPosition_.z + offset(gen);
                lastPerception_.visibleEntities.push_back(entity);
            }

            // Add some nearby players
            lastPerception_.nearbyPlayers.push_back("Player_Alpha");
            lastPerception_.nearbyPlayers.push_back("Player_Beta");

            // Generate environment description
            std::ostringstream envDesc;
            envDesc << "Open area at position (" << currentPosition_.x << ", "
                    << currentPosition_.y << ", " << currentPosition_.z << "). "
                    << "Visible entities: " << numEntities << ", "
                    << "Nearby players: " << lastPerception_.nearbyPlayers.size();
            lastPerception_.environmentDescription = envDesc.str();

            logInfo("Scene perception complete: " + lastPerception_.environmentDescription, "HyperfyWorld");

            return lastPerception_;
        }

        bool HyperfyWorld::sendHeartbeat() {
            if (!connected_.load()) {
                return false;
            }

            WebSocketMessage heartbeatMsg;
            heartbeatMsg.type = MessageType::HEARTBEAT;
            heartbeatMsg.payload = worldId_;
            heartbeatMsg.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

            return wsClient_->send(heartbeatMsg);
        }

        void HyperfyWorld::handleIncomingMessage(const WebSocketMessage& msg) {
            switch (msg.type) {
                case MessageType::WORLD_STATE:
                    handleWorldStateUpdate(msg.payload);
                    break;
                case MessageType::POSITION_UPDATE:
                    handlePositionUpdate(msg.payload);
                    break;
                case MessageType::CHAT_MESSAGE:
                    logInfo("Received chat message: " + msg.payload, "HyperfyWorld");
                    break;
                case MessageType::ERROR:
                    logError("World error: " + msg.payload, "HyperfyWorld");
                    break;
                default:
                    logInfo("Received message type: " + std::to_string(static_cast<int>(msg.type)),
                            "HyperfyWorld");
                    break;
            }
        }

        void HyperfyWorld::handleWorldStateUpdate(const std::string& payload) {
            logInfo("World state update: " + payload, "HyperfyWorld");
            // Parse and update world state as needed
        }

        void HyperfyWorld::handlePositionUpdate(const std::string& payload) {
            Position3D pos = Position3D::fromString(payload);
            setCurrentPosition(pos);
            logInfo("Position updated to: " + payload, "HyperfyWorld");
        }

        // =========================================================================
        // HyperfyService Implementation
        // =========================================================================

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

            logInfo("HyperfyService started successfully", "HyperfyService");
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

            // Wait for service thread to complete
            if (serviceThread_.joinable()) {
                serviceThread_.join();
            }

            // Disconnect from world
            disconnectFromWorld();

            // Cleanup managers
            for (auto& [name, manager] : managers_) {
                manager->cleanup();
            }
            managers_.clear();
            actions_.clear();

            logInfo("HyperfyService stopped", "HyperfyService");
        }

        void HyperfyService::serviceLoop() {
            auto lastHeartbeat = std::chrono::steady_clock::now();

            while (running_.load()) {
                // Service heartbeat and maintenance tasks
                if (world_ && world_->isConnected()) {
                    auto now = std::chrono::steady_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - lastHeartbeat).count();

                    // Send heartbeat at configured interval
                    if (elapsed >= config_.heartbeatIntervalMs) {
                        world_->sendHeartbeat();
                        lastHeartbeat = now;
                    }
                }

                // Sleep for a short interval
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        bool HyperfyService::connectToWorld(const std::string& worldId, const std::string& wsUrl,
                                             const std::string& authToken) {
            std::lock_guard<std::mutex> lock(serviceMutex_);

            if (world_ && world_->isConnected()) {
                disconnectFromWorld();
            }

            world_ = std::make_shared<HyperfyWorld>(worldId, wsUrl);
            bool connected = world_->connect(authToken);

            if (connected) {
                logInfo("Successfully connected to Hyperfy world: " + worldId, "HyperfyService");
            } else {
                logError("Failed to connect to Hyperfy world: " + worldId, "HyperfyService");
                world_.reset();
            }

            return connected;
        }

        void HyperfyService::disconnectFromWorld() {
            if (world_) {
                world_->disconnect();
                world_.reset();
            }
        }

        void HyperfyService::registerAction(const std::string& name, std::shared_ptr<HyperfyAction> action) {
            std::lock_guard<std::mutex> lock(serviceMutex_);
            actions_[name] = action;
            logInfo("Registered Hyperfy action: " + name, "HyperfyService");
        }

        std::shared_ptr<HyperfyAction> HyperfyService::getAction(const std::string& name) const {
            auto it = actions_.find(name);
            return (it != actions_.end()) ? it->second : nullptr;
        }

        bool HyperfyService::executeAction(const std::string& name, const std::string& parameters) {
            auto action = getAction(name);
            if (!action) {
                logWarning("Action not found: " + name, "HyperfyService");
                return false;
            }

            logInfo("Executing action: " + name, "HyperfyService");
            return action->execute(parameters);
        }

        void HyperfyService::registerManager(const std::string& name, std::shared_ptr<HyperfyManager> manager) {
            std::lock_guard<std::mutex> lock(serviceMutex_);
            managers_[name] = manager;
            manager->initialize(shared_from_this());
            logInfo("Registered Hyperfy manager: " + name, "HyperfyService");
        }

        std::shared_ptr<HyperfyManager> HyperfyService::getManager(const std::string& name) const {
            auto it = managers_.find(name);
            return (it != managers_.end()) ? it->second : nullptr;
        }

        // =========================================================================
        // Action Implementations
        // =========================================================================

        GotoAction::GotoAction(std::shared_ptr<HyperfyService> service) : service_(service) {
        }

        bool GotoAction::execute(const std::string& parameters) {
            // Parse parameters for x, y, z coordinates (format: "x,y,z")
            Position3D target = Position3D::fromString(parameters);

            auto world = service_->getWorld();
            if (world) {
                return world->moveToPosition(target.x, target.y, target.z);
            } else {
                logError("No world connected for goto action", "GotoAction");
                return false;
            }
        }

        ReplyAction::ReplyAction(std::shared_ptr<HyperfyService> service) : service_(service) {
        }

        bool ReplyAction::execute(const std::string& parameters) {
            auto world = service_->getWorld();
            if (world) {
                return world->sendMessage(parameters);
            } else {
                logError("No world connected for reply action", "ReplyAction");
                return false;
            }
        }

        PerceptionAction::PerceptionAction(std::shared_ptr<HyperfyService> service) : service_(service) {
        }

        bool PerceptionAction::execute(const std::string& parameters) {
            (void)parameters; // Suppress unused parameter warning

            auto world = service_->getWorld();
            if (!world) {
                logError("No world connected for perception action", "PerceptionAction");
                return false;
            }

            logInfo("Analyzing 3D scene...", "PerceptionAction");

            // Perform scene perception
            ScenePerception perception = world->perceiveScene();

            // Log perception results
            std::ostringstream result;
            result << "Scene perception complete:\n"
                   << "  Position: " << perception.viewerPosition.toString() << "\n"
                   << "  Visible entities: " << perception.visibleEntities.size() << "\n"
                   << "  Nearby players: " << perception.nearbyPlayers.size() << "\n"
                   << "  Environment: " << perception.environmentDescription;

            logInfo(result.str(), "PerceptionAction");
            return true;
        }

        // =========================================================================
        // Factory Implementation
        // =========================================================================

        std::shared_ptr<HyperfyService> HyperfyServiceFactory::createService() {
            auto service = std::make_shared<HyperfyService>();

            // Register default actions
            service->registerAction("goto", std::make_shared<GotoAction>(service));
            service->registerAction("reply", std::make_shared<ReplyAction>(service));
            service->registerAction("perception", std::make_shared<PerceptionAction>(service));

            return service;
        }

        std::shared_ptr<HyperfyService> HyperfyServiceFactory::createServiceWithConfig(const HyperfyConfig& config) {
            auto service = createService();
            service->start(config);
            return service;
        }

    } // namespace hyperfy
} // namespace elizaos
