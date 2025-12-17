#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "redis.hpp"
#include "util.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Import the correct type from Hono

// Interface for our client metadata, not extending WSContext
struct ClientMetadata {
    std::string clientId;
    bool isAlive;
    std::unordered_set<std::string> rooms;
    WSContext ws;
};

class WebSocketManager {
public:
    std::future<std::string> redisKey(const std::string& rawKey);
    std::future<void> initialize(RedisCacheService redisCache);
    void handleConnectionOpen(WSContext ws);
    std::future<void> handleMessage(WSContext ws, const std::variant<std::string, std::vector<uint8_t>>& messageData);
    std::future<void> handleRoomEvent(ClientMetadata client, const std::string& event, const std::any& data);
    void handleConnectionClose(WSContext ws);
    void handleConnectionError(WSContext ws, const std::runtime_error& error);
    void startHeartbeat();
    std::future<void> joinRoom(const std::string& event, ClientMetadata client, const std::string& roomName);
    std::future<void> leaveRoom(const std::string& event, ClientMetadata client, const std::string& roomName);
    std::future<void> performClientCleanup(const std::string& clientId);
    std::future<void> broadcastToRoom(const std::string& roomName, const std::string& event, const std::any& data, std::optional<std::string> excludeClientId);
    bool sendToClient(const std::string& clientId, const std::string& event, const std::any& data);
    void close();
};


} // namespace elizaos
