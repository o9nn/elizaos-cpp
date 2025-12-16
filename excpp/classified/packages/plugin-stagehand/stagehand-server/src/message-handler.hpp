#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
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
    MessageHandler(SessionManager private sessionManager, Logger private logger);
    std::future<Response> handleMessage(Message message, const std::string& clientId);
    std::future<Response> handleCreateSession(const std::string& requestId, const std::string& clientId);
    std::future<Response> handleDestroySession(const std::string& requestId, const std::string& sessionId);
    std::future<Response> handleNavigate(const std::string& requestId, const std::string& sessionId, const std::string& url);
    std::future<Response> handleGoBack(const std::string& requestId, const std::string& sessionId);
    std::future<Response> handleGoForward(const std::string& requestId, const std::string& sessionId);
    std::future<Response> handleRefresh(const std::string& requestId, const std::string& sessionId);
    std::future<Response> handleClick(const std::string& requestId, const std::string& sessionId, const std::string& description);
    std::future<Response> handleType(const std::string& requestId, const std::string& sessionId, const std::string& text, const std::string& field);
    std::future<Response> handleSelect(const std::string& requestId, const std::string& sessionId, const std::string& option, const std::string& dropdown);
    std::future<Response> handleExtract(const std::string& requestId, const std::string& sessionId, const std::string& instruction);
    std::future<Response> handleScreenshot(const std::string& requestId, const std::string& sessionId);
    std::future<Response> handleGetState(const std::string& requestId, const std::string& sessionId);
    std::future<Response> handleSolveCaptcha(const std::string& requestId, const std::string& sessionId);
    Response sessionNotFoundResponse(const std::string& requestId);
    Response handleHealth(const std::string& requestId);
};


} // namespace elizaos
