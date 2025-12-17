#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "captcha-handler.js.hpp"
#include "logger.js.hpp"
#include "session-manager.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Message {
    std::string type;
    std::string requestId;
    std::optional<std::string> sessionId;
    std::optional<std::any> data;
};

struct Response {
    std::string type;
    std::string requestId;
    bool success;
    std::optional<std::any> data;
    std::optional<std::string> error;
};

class MessageHandler {
public:
    MessageHandler();
    std::future<std::string> handleMessage(Message message, const std::string& clientId);
    std::future<std::string> handleCreateSession(const std::string& requestId, const std::string& clientId);
    std::future<std::string> handleDestroySession(const std::string& requestId, const std::string& sessionId);
    std::future<std::string> handleNavigate(const std::string& requestId, const std::string& sessionId, const std::string& url);
    std::future<std::string> handleGoBack(const std::string& requestId, const std::string& sessionId);
    std::future<std::string> handleGoForward(const std::string& requestId, const std::string& sessionId);
    std::future<std::string> handleRefresh(const std::string& requestId, const std::string& sessionId);
    std::future<std::string> handleClick(const std::string& requestId, const std::string& sessionId, const std::string& description);
    std::future<std::string> handleType(const std::string& requestId, const std::string& sessionId, const std::string& text, const std::string& field);
    std::future<std::string> handleSelect(const std::string& requestId, const std::string& sessionId, const std::string& option, const std::string& dropdown);
    std::future<std::string> handleExtract(const std::string& requestId, const std::string& sessionId, const std::string& instruction);
    std::future<std::string> handleScreenshot(const std::string& requestId, const std::string& sessionId);
    std::future<std::string> handleGetState(const std::string& requestId, const std::string& sessionId);
    std::future<std::string> handleSolveCaptcha(const std::string& requestId, const std::string& sessionId);
    std::string sessionNotFoundResponse(const std::string& requestId);
    std::string handleHealth(const std::string& requestId);
};


} // namespace elizaos
