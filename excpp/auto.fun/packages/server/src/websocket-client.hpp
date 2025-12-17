#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "redis.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class WebSocketClient {
public:
    WebSocketClient();
    std::future<void> emit(const std::string& room, const std::string& event, const std::any& data);
    std::future<void> emitToClient(const std::string& clientId, const std::string& event, const std::any& data);
    void to(const std::string& room);
};

// Helper function to get websocket client instance
// Since WebSocketClient now just wraps the singleton manager,
// we might not even need this class/function anymore.
// However, keeping it maintains the existing interface.

WebSocketClient getWebSocketClient();

} // namespace elizaos
