#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



  // Browser environment
  // Node.js environment

struct StagehandMessage {
    std::string type;
    std::optional<std::string> requestId;
};

class StagehandWebSocketClient {
public:
    StagehandWebSocketClient(const std::string& private serverUrl);
    std::future<void> connect();
    std::future<void> attemptReconnect();
    std::future<StagehandMessage> sendMessage(const std::string& type, const std::any& data);
    void disconnect();
    bool isConnected();
    Promise< navigate(const std::string& sessionId, const std::string& url);
    Promise< getState(const std::string& sessionId);
    Promise< goBack(const std::string& sessionId);
    Promise< goForward(const std::string& sessionId);
    Promise< refresh(const std::string& sessionId);
    std::future<StagehandMessage> click(const std::string& sessionId, const std::string& description);
    std::future<StagehandMessage> type(const std::string& sessionId, const std::string& text, const std::string& field);
    std::future<StagehandMessage> select(const std::string& sessionId, const std::string& option, const std::string& dropdown);
    std::future<StagehandMessage> extract(const std::string& sessionId, const std::string& instruction);
    std::future<StagehandMessage> screenshot(const std::string& sessionId);
    std::future<StagehandMessage> solveCaptcha(const std::string& sessionId);
    std::future<bool> health();
};


} // namespace elizaos
