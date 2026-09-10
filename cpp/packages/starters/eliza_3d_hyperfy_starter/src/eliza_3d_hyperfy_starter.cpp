// eliza_3d_hyperfy_starter.cpp - deterministic Hyperfy integration

#include "elizaos/eliza_3d_hyperfy_starter.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace elizaos {
namespace hyperfy {
namespace {

using json = nlohmann::json;

int64_t nowMilliseconds() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

const std::array<std::pair<MessageType, const char*>, 8> kMessageTypes{{
    {MessageType::CONNECT, "connect"},
    {MessageType::DISCONNECT, "disconnect"},
    {MessageType::POSITION_UPDATE, "position_update"},
    {MessageType::CHAT_MESSAGE, "chat_message"},
    {MessageType::ACTION, "action"},
    {MessageType::WORLD_STATE, "world_state"},
    {MessageType::HEARTBEAT, "heartbeat"},
    {MessageType::ERROR, "error"},
}};

const char* messageTypeName(MessageType type) {
    const auto it = std::find_if(kMessageTypes.begin(), kMessageTypes.end(),
                                 [type](const auto& entry) { return entry.first == type; });
    if (it == kMessageTypes.end()) {
        throw std::invalid_argument("unknown WebSocket message type");
    }
    return it->second;
}

MessageType parseMessageType(const std::string& name) {
    const auto it = std::find_if(kMessageTypes.begin(), kMessageTypes.end(),
                                 [&name](const auto& entry) { return name == entry.second; });
    if (it == kMessageTypes.end()) {
        throw std::invalid_argument("unknown WebSocket message type: " + name);
    }
    return it->first;
}

std::string scalarToString(const json& value) {
    if (value.is_string()) {
        return value.get<std::string>();
    }
    return value.dump();
}

Position3D positionFromJson(const json& value) {
    const json* position = &value;
    if (value.is_object() && value.contains("position")) {
        position = &value.at("position");
    }
    if (!position->is_object() || !position->contains("x") ||
        !position->contains("y") || !position->contains("z")) {
        throw std::invalid_argument("position payload must contain numeric x, y, and z");
    }

    Position3D result{
        position->at("x").get<double>(),
        position->at("y").get<double>(),
        position->at("z").get<double>(),
    };
    if (!std::isfinite(result.x) || !std::isfinite(result.y) || !std::isfinite(result.z)) {
        throw std::invalid_argument("position coordinates must be finite");
    }
    return result;
}

std::string formatDouble(double value) {
    if (!std::isfinite(value)) {
        throw std::invalid_argument("position coordinates must be finite");
    }
    std::ostringstream stream;
    stream << std::setprecision(std::numeric_limits<double>::max_digits10) << value;
    return stream.str();
}

}  // namespace

std::string WebSocketMessage::toJson() const {
    return json{{"type", messageTypeName(type)},
                {"payload", payload},
                {"sender", sender},
                {"timestamp", timestamp}}
        .dump();
}

WebSocketMessage WebSocketMessage::fromJson(const std::string& encoded) {
    try {
        const json value = json::parse(encoded);
        if (!value.is_object() || !value.contains("type") ||
            !value.contains("payload") || !value.contains("sender") ||
            !value.contains("timestamp")) {
            throw std::invalid_argument("WebSocket message is missing a required field");
        }
        if (!value.at("type").is_string() || !value.at("sender").is_string() ||
            !value.at("timestamp").is_number_integer()) {
            throw std::invalid_argument("WebSocket message contains an invalid field type");
        }

        WebSocketMessage result;
        result.type = parseMessageType(value.at("type").get<std::string>());
        result.payload = value.at("payload").is_string()
                             ? value.at("payload").get<std::string>()
                             : value.at("payload").dump();
        result.sender = value.at("sender").get<std::string>();
        result.timestamp = value.at("timestamp").get<int64_t>();
        return result;
    } catch (const std::invalid_argument&) {
        throw;
    } catch (const std::exception& error) {
        throw std::invalid_argument(std::string("invalid WebSocket message JSON: ") + error.what());
    }
}

std::string Position3D::toString() const {
    return formatDouble(x) + "," + formatDouble(y) + "," + formatDouble(z);
}

Position3D Position3D::fromString(const std::string& text) {
    std::istringstream stream(text);
    Position3D result;
    char firstComma = 0;
    char secondComma = 0;
    if (!(stream >> result.x >> firstComma >> result.y >> secondComma >> result.z) ||
        firstComma != ',' || secondComma != ',') {
        throw std::invalid_argument("position must use the form x,y,z");
    }
    stream >> std::ws;
    if (!stream.eof() || !std::isfinite(result.x) || !std::isfinite(result.y) ||
        !std::isfinite(result.z)) {
        throw std::invalid_argument("position contains trailing data or non-finite coordinates");
    }
    return result;
}

WebSocketClient::WebSocketClient() : connected_(false), running_(false) {}

WebSocketClient::~WebSocketClient() {
    disconnect();
    if (processingThread_.joinable() &&
        processingThread_.get_id() != std::this_thread::get_id()) {
        processingThread_.join();
    }
}

bool WebSocketClient::connect(const std::string& url, const std::string& authToken) {
    (void)authToken;
    if (url.empty()) {
        OnErrorCallback callback;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            callback = onError_;
        }
        if (callback) {
            callback("connect failed: URL is empty");
        }
        return false;
    }
    if (connected_.load()) {
        return true;
    }

    disconnect();
    if (processingThread_.joinable()) {
        processingThread_.join();
    }

    OnConnectCallback callback;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        url_ = url;
        connected_.store(true);
        running_.store(true);
        callback = onConnect_;
        processingThread_ = std::thread(&WebSocketClient::messageProcessingLoop, this);
    }
    if (callback) {
        callback();
    }
    return true;
}

void WebSocketClient::disconnect() {
    bool wasConnected = false;
    {
        // Publish the stop predicate while holding the same mutex used by
        // messageProcessingLoop's condition-variable wait. Updating it without
        // this lock permits a notify to land between the worker's predicate
        // check and its wait, leaving disconnect() blocked forever in join().
        std::lock_guard<std::mutex> lock(mutex_);
        wasConnected = connected_.exchange(false);
        running_.store(false);
    }
    cv_.notify_all();

    if (processingThread_.joinable() &&
        processingThread_.get_id() != std::this_thread::get_id()) {
        processingThread_.join();
    }

    OnDisconnectCallback callback;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        callback = onDisconnect_;
    }
    if (wasConnected && callback) {
        callback("client disconnect");
    }
}

bool WebSocketClient::isConnected() const {
    return connected_.load();
}

bool WebSocketClient::send(const WebSocketMessage& message) {
    OnErrorCallback errorCallback;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (connected_.load()) {
            outgoingMessages_.push(message);
            return true;
        }
        errorCallback = onError_;
    }
    if (errorCallback) {
        errorCallback("send failed: not connected");
    }
    return false;
}

bool WebSocketClient::sendText(const std::string& text) {
    return send(WebSocketMessage{MessageType::CHAT_MESSAGE, text, "local", nowMilliseconds()});
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
        return {};
    }
    WebSocketMessage message = incomingMessages_.front();
    incomingMessages_.pop();
    return message;
}

bool WebSocketClient::injectIncomingMessage(const WebSocketMessage& message) {
    OnErrorCallback errorCallback;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!connected_.load()) {
            errorCallback = onError_;
        } else {
            incomingMessages_.push(message);
            dispatchMessages_.push(message);
        }
    }
    if (errorCallback) {
        errorCallback("inbound dispatch failed: not connected");
        return false;
    }
    cv_.notify_one();
    return true;
}

bool WebSocketClient::hasPendingOutgoingMessages() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return !outgoingMessages_.empty();
}

WebSocketMessage WebSocketClient::popOutgoingMessage() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (outgoingMessages_.empty()) {
        return {};
    }
    WebSocketMessage message = outgoingMessages_.front();
    outgoingMessages_.pop();
    return message;
}

void WebSocketClient::messageProcessingLoop() {
    while (true) {
        WebSocketMessage message;
        OnMessageCallback callback;
        OnErrorCallback errorCallback;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] {
                return !running_.load() || !dispatchMessages_.empty();
            });
            if (!running_.load()) {
                break;
            }
            message = dispatchMessages_.front();
            dispatchMessages_.pop();
            callback = onMessage_;
            errorCallback = onError_;
        }
        try {
            if (callback) {
                callback(message);
            }
        } catch (const std::exception& error) {
            if (errorCallback) {
                errorCallback(std::string("message callback failed: ") + error.what());
            }
        } catch (...) {
            if (errorCallback) {
                errorCallback("message callback failed: unknown error");
            }
        }
    }
}

HyperfyWorld::HyperfyWorld(const std::string& worldId, const std::string& wsUrl)
    : worldId_(worldId),
      wsUrl_(wsUrl),
      connected_(false),
      wsClient_(std::make_unique<WebSocketClient>()) {
    wsClient_->setOnMessage([this](const WebSocketMessage& message) {
        handleIncomingMessage(message);
    });
    wsClient_->setOnDisconnect([this](const std::string&) {
        connected_.store(false);
    });
    wsClient_->setOnError([this](const std::string& error) {
        updateState("last_error", error);
    });
}

HyperfyWorld::~HyperfyWorld() {
    disconnect();
}

bool HyperfyWorld::connect(const std::string& authToken) {
    if (connected_.load()) {
        return true;
    }
    if (!wsClient_->connect(wsUrl_, authToken)) {
        return false;
    }

    const WebSocketMessage authentication{
        MessageType::CONNECT,
        json{{"worldId", worldId_}, {"token", authToken}}.dump(),
        "client",
        nowMilliseconds(),
    };
    if (!wsClient_->send(authentication)) {
        wsClient_->disconnect();
        return false;
    }
    connected_.store(true);
    return true;
}

void HyperfyWorld::disconnect() {
    connected_.store(false);
    if (wsClient_) {
        wsClient_->disconnect();
    }
    std::lock_guard<std::mutex> lock(worldMutex_);
    worldState_.clear();
}

bool HyperfyWorld::isConnected() const {
    return connected_.load() && wsClient_ && wsClient_->isConnected();
}

void HyperfyWorld::updateState(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(worldMutex_);
    worldState_[key] = value;
}

std::string HyperfyWorld::getState(const std::string& key) const {
    std::lock_guard<std::mutex> lock(worldMutex_);
    const auto found = worldState_.find(key);
    return found == worldState_.end() ? std::string{} : found->second;
}

Position3D HyperfyWorld::getCurrentPosition() const {
    std::lock_guard<std::mutex> lock(worldMutex_);
    return currentPosition_;
}

void HyperfyWorld::setCurrentPosition(const Position3D& position) {
    if (!std::isfinite(position.x) || !std::isfinite(position.y) ||
        !std::isfinite(position.z)) {
        throw std::invalid_argument("position coordinates must be finite");
    }
    std::lock_guard<std::mutex> lock(worldMutex_);
    currentPosition_ = position;
    worldState_["position_x"] = formatDouble(position.x);
    worldState_["position_y"] = formatDouble(position.y);
    worldState_["position_z"] = formatDouble(position.z);
}

bool HyperfyWorld::sendMessage(const std::string& message) {
    if (!isConnected()) {
        return false;
    }
    const bool sent = wsClient_->send(WebSocketMessage{
        MessageType::CHAT_MESSAGE,
        message,
        worldId_,
        nowMilliseconds(),
    });
    if (sent) {
        updateState("last_sent_message", message);
    }
    return sent;
}

bool HyperfyWorld::moveToPosition(double x, double y, double z) {
    if (!isConnected()) {
        return false;
    }
    const Position3D position{x, y, z};
    if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z)) {
        return false;
    }
    const WebSocketMessage message{
        MessageType::POSITION_UPDATE,
        json{{"x", x}, {"y", y}, {"z", z}}.dump(),
        worldId_,
        nowMilliseconds(),
    };
    if (!wsClient_->send(message)) {
        return false;
    }
    setCurrentPosition(position);
    return true;
}

bool HyperfyWorld::performAction(const std::string& action,
                                 const std::string& parameters) {
    if (!isConnected() || action.empty()) {
        return false;
    }
    const bool sent = wsClient_->send(WebSocketMessage{
        MessageType::ACTION,
        json{{"action", action}, {"parameters", parameters}}.dump(),
        worldId_,
        nowMilliseconds(),
    });
    if (sent) {
        updateState("last_performed_action", action);
        updateState("last_action_parameters", parameters);
    }
    return sent;
}

void HyperfyWorld::handleIncomingMessage(const WebSocketMessage& message) {
    try {
        switch (message.type) {
            case MessageType::CONNECT:
                connected_.store(true);
                break;
            case MessageType::DISCONNECT:
                connected_.store(false);
                break;
            case MessageType::POSITION_UPDATE:
                handlePositionUpdate(message.payload);
                break;
            case MessageType::CHAT_MESSAGE:
                updateState("last_chat_message", message.payload);
                updateState("last_chat_sender", message.sender);
                break;
            case MessageType::ACTION:
                updateState("last_action", message.payload);
                break;
            case MessageType::WORLD_STATE:
                handleWorldStateUpdate(message.payload);
                break;
            case MessageType::HEARTBEAT:
                updateState("last_heartbeat", std::to_string(message.timestamp));
                break;
            case MessageType::ERROR:
                updateState("last_error", message.payload);
                break;
        }
    } catch (const std::exception& error) {
        updateState("last_error", error.what());
    }
}

void HyperfyWorld::handleWorldStateUpdate(const std::string& payload) {
    const json state = json::parse(payload);
    if (!state.is_object()) {
        throw std::invalid_argument("world-state payload must be a JSON object");
    }

    bool hasPosition = false;
    Position3D position;
    if (state.contains("position")) {
        position = positionFromJson(state);
        hasPosition = true;
    }

    {
        std::lock_guard<std::mutex> lock(worldMutex_);
        for (const auto& item : state.items()) {
            worldState_[item.key()] = scalarToString(item.value());
        }
    }
    if (hasPosition) {
        setCurrentPosition(position);
    }
}

void HyperfyWorld::handlePositionUpdate(const std::string& payload) {
    setCurrentPosition(positionFromJson(json::parse(payload)));
}

void HyperfyWorld::handleWebSocketMessage(const std::string& message) {
    try {
        handleIncomingMessage(WebSocketMessage::fromJson(message));
    } catch (const std::exception& error) {
        updateState("last_error", error.what());
    }
}

std::string HyperfyWorld::escapeJson(const std::string& input) {
    const std::string encoded = json(input).dump();
    return encoded.substr(1, encoded.size() - 2);
}

ScenePerception HyperfyWorld::perceiveScene() {
    std::map<std::string, std::string> state;
    ScenePerception perception;
    {
        std::lock_guard<std::mutex> lock(worldMutex_);
        state = worldState_;
        perception.viewerPosition = currentPosition_;
    }

    const auto entities = state.find("entities");
    if (entities != state.end()) {
        try {
            const json list = json::parse(entities->second);
            if (!list.is_array()) {
                throw std::invalid_argument("entities must be an array");
            }
            for (const auto& value : list) {
                if (!value.is_object()) {
                    continue;
                }
                WorldEntity entity;
                entity.id = value.value("id", "");
                entity.type = value.value("type", "");
                entity.name = value.value("name", "");
                if (value.contains("position")) {
                    entity.position = positionFromJson(value);
                }
                if (value.contains("properties") && value.at("properties").is_object()) {
                    for (const auto& property : value.at("properties").items()) {
                        entity.properties[property.key()] = scalarToString(property.value());
                    }
                }
                if (entity.type == "player" || entity.type == "avatar") {
                    perception.nearbyPlayers.push_back(
                        entity.name.empty() ? entity.id : entity.name);
                }
                perception.visibleEntities.push_back(std::move(entity));
            }
        } catch (const std::exception& error) {
            state["last_error"] = error.what();
        }
    }

    const auto description = state.find("environmentDescription");
    const auto environment = state.find("environment");
    if (description != state.end()) {
        perception.environmentDescription = description->second;
    } else if (environment != state.end()) {
        perception.environmentDescription = environment->second;
    } else {
        perception.environmentDescription =
            "World " + worldId_ + " contains " +
            std::to_string(perception.visibleEntities.size()) + " visible entities";
    }
    perception.timestamp = nowMilliseconds();

    {
        std::lock_guard<std::mutex> lock(worldMutex_);
        const auto error = state.find("last_error");
        if (error != state.end()) {
            worldState_["last_error"] = error->second;
        }
        lastPerception_ = perception;
    }
    return perception;
}

bool HyperfyWorld::dispatchIncomingMessage(const WebSocketMessage& message) {
    return wsClient_ && wsClient_->injectIncomingMessage(message);
}

bool HyperfyWorld::sendHeartbeat() {
    if (!isConnected()) {
        return false;
    }
    const int64_t timestamp = nowMilliseconds();
    const bool sent = wsClient_->send(WebSocketMessage{
        MessageType::HEARTBEAT,
        {},
        worldId_,
        timestamp,
    });
    if (sent) {
        updateState("last_heartbeat_sent", std::to_string(timestamp));
    }
    return sent;
}

HyperfyService::HyperfyService() : running_(false) {}

HyperfyService::~HyperfyService() {
    stop();
}

bool HyperfyService::start(const HyperfyConfig& config) {
    std::vector<std::pair<std::string, std::shared_ptr<HyperfyManager>>> managers;
    std::shared_ptr<HyperfyService> self;
    {
        std::lock_guard<std::mutex> lock(serviceMutex_);
        if (running_.load()) {
            return true;
        }
        config_ = config;
        running_.store(true);
        for (const auto& entry : managers_) {
            managers.push_back(entry);
            managerInitialized_[entry.first] = false;
        }
        try {
            self = shared_from_this();
        } catch (const std::bad_weak_ptr&) {
        }
        serviceThread_ = std::thread(&HyperfyService::serviceLoop, this);
    }

    if (self) {
        for (const auto& entry : managers) {
            const bool initialized = entry.second && entry.second->initialize(self);
            std::lock_guard<std::mutex> lock(serviceMutex_);
            if (managers_.count(entry.first) && managers_.at(entry.first) == entry.second) {
                managerInitialized_[entry.first] = initialized;
            }
        }
    }
    return true;
}

void HyperfyService::stop() {
    running_.store(false);
    serviceCv_.notify_all();
    if (serviceThread_.joinable() && serviceThread_.get_id() != std::this_thread::get_id()) {
        serviceThread_.join();
    }

    std::shared_ptr<HyperfyWorld> world;
    std::vector<std::shared_ptr<HyperfyManager>> managers;
    {
        std::lock_guard<std::mutex> lock(serviceMutex_);
        world = std::move(world_);
        for (const auto& entry : managers_) {
            if (managerInitialized_[entry.first] && entry.second) {
                managers.push_back(entry.second);
            }
            managerInitialized_[entry.first] = false;
        }
    }
    if (world) {
        world->disconnect();
    }
    for (const auto& manager : managers) {
        manager->cleanup();
    }
}

std::shared_ptr<HyperfyWorld> HyperfyService::getWorld() const {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return world_;
}

bool HyperfyService::connectToWorld(const std::string& worldId,
                                    const std::string& wsUrl,
                                    const std::string& authToken) {
    std::string resolvedUrl;
    {
        std::lock_guard<std::mutex> lock(serviceMutex_);
        if (!running_.load()) {
            return false;
        }
        if (world_ && world_->isConnected() && world_->getWorldId() == worldId) {
            return true;
        }
        resolvedUrl = wsUrl.empty() ? config_.wsUrl : wsUrl;
    }

    auto world = std::make_shared<HyperfyWorld>(worldId, resolvedUrl);
    if (!world->connect(authToken)) {
        return false;
    }

    std::shared_ptr<HyperfyWorld> previous;
    {
        std::lock_guard<std::mutex> lock(serviceMutex_);
        if (!running_.load()) {
            world->disconnect();
            return false;
        }
        previous = std::move(world_);
        world_ = std::move(world);
    }
    if (previous) {
        previous->disconnect();
    }
    return true;
}

void HyperfyService::disconnectFromWorld() {
    std::shared_ptr<HyperfyWorld> world;
    {
        std::lock_guard<std::mutex> lock(serviceMutex_);
        world = std::move(world_);
    }
    if (world) {
        world->disconnect();
    }
}

void HyperfyService::registerAction(const std::string& name,
                                    std::shared_ptr<HyperfyAction> action) {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    if (action) {
        actions_[name] = std::move(action);
    } else {
        actions_.erase(name);
    }
}

std::shared_ptr<HyperfyAction> HyperfyService::getAction(const std::string& name) const {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    const auto found = actions_.find(name);
    return found == actions_.end() ? nullptr : found->second;
}

bool HyperfyService::executeAction(const std::string& name,
                                   const std::string& parameters) {
    if (!running_.load()) {
        return false;
    }
    const auto action = getAction(name);
    return action && action->execute(parameters);
}

void HyperfyService::registerManager(const std::string& name,
                                     std::shared_ptr<HyperfyManager> manager) {
    std::shared_ptr<HyperfyManager> previous;
    bool previousInitialized = false;
    bool initializeNow = false;
    std::shared_ptr<HyperfyService> self;
    {
        std::lock_guard<std::mutex> lock(serviceMutex_);
        const auto found = managers_.find(name);
        if (found != managers_.end()) {
            previous = found->second;
            previousInitialized = managerInitialized_[name];
        }
        if (manager) {
            managers_[name] = manager;
            managerInitialized_[name] = false;
        } else {
            managers_.erase(name);
            managerInitialized_.erase(name);
        }
        initializeNow = running_.load() && static_cast<bool>(manager);
        if (initializeNow) {
            try {
                self = shared_from_this();
            } catch (const std::bad_weak_ptr&) {
                initializeNow = false;
            }
        }
    }

    if (previous && previous != manager && previousInitialized) {
        previous->cleanup();
    }
    if (initializeNow) {
        const bool initialized = manager->initialize(self);
        std::lock_guard<std::mutex> lock(serviceMutex_);
        const auto found = managers_.find(name);
        if (found != managers_.end() && found->second == manager) {
            managerInitialized_[name] = initialized;
        } else if (initialized) {
            manager->cleanup();
        }
    }
}

std::shared_ptr<HyperfyManager> HyperfyService::getManager(const std::string& name) const {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    const auto found = managers_.find(name);
    return found == managers_.end() ? nullptr : found->second;
}

void HyperfyService::serviceLoop() {
    while (running_.load()) {
        std::shared_ptr<HyperfyWorld> world;
        int heartbeatInterval = 1;
        {
            std::lock_guard<std::mutex> lock(serviceMutex_);
            world = world_;
            heartbeatInterval = std::max(1, config_.heartbeatIntervalMs);
        }
        if (world && world->isConnected()) {
            world->sendHeartbeat();
        }

        std::unique_lock<std::mutex> lock(serviceMutex_);
        serviceCv_.wait_for(lock, std::chrono::milliseconds(heartbeatInterval),
                            [this] { return !running_.load(); });
    }
}

GotoAction::GotoAction(std::shared_ptr<HyperfyService> service)
    : service_(std::move(service)) {}

bool GotoAction::execute(const std::string& parameters) {
    try {
        const Position3D position = Position3D::fromString(parameters);
        const auto service = service_.lock();
        const auto world = service ? service->getWorld() : nullptr;
        return world && world->moveToPosition(position.x, position.y, position.z);
    } catch (const std::exception&) {
        return false;
    }
}

ReplyAction::ReplyAction(std::shared_ptr<HyperfyService> service)
    : service_(std::move(service)) {}

bool ReplyAction::execute(const std::string& parameters) {
    const auto service = service_.lock();
    const auto world = service ? service->getWorld() : nullptr;
    return world && world->sendMessage(parameters);
}

PerceptionAction::PerceptionAction(std::shared_ptr<HyperfyService> service)
    : service_(std::move(service)) {}

bool PerceptionAction::execute(const std::string& parameters) {
    (void)parameters;
    const auto service = service_.lock();
    const auto world = service ? service->getWorld() : nullptr;
    if (!world || !world->isConnected()) {
        return false;
    }
    world->perceiveScene();
    return true;
}

std::shared_ptr<HyperfyService> HyperfyServiceFactory::createService() {
    auto service = std::make_shared<HyperfyService>();
    service->registerAction("goto", std::make_shared<GotoAction>(service));
    service->registerAction("reply", std::make_shared<ReplyAction>(service));
    service->registerAction("perception", std::make_shared<PerceptionAction>(service));
    return service;
}

std::shared_ptr<HyperfyService> HyperfyServiceFactory::createServiceWithConfig(
    const HyperfyConfig& config) {
    auto service = createService();
    service->applyConfig(config);
    return service;
}

}  // namespace hyperfy
}  // namespace elizaos
