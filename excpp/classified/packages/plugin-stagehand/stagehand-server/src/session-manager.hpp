#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "logger.js.hpp"
#include "playwright-installer.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct BrowserSession {
    std::string id;
    std::string clientId;
    Stagehand stagehand;
    Date createdAt;
};

class SessionManager {
public:
    SessionManager(Logger private logger, PlaywrightInstaller private playwrightInstaller);
    std::future<BrowserSession> createSession(const std::string& sessionId, const std::string& clientId);
    BrowserSession getSession(const std::string& sessionId);
    std::future<void> destroySession(const std::string& sessionId);
    std::vector<BrowserSession> getClientSessions(const std::string& clientId);
    std::future<void> cleanupClientSessions(const std::string& clientId);
    std::future<void> cleanup();
};


} // namespace elizaos
