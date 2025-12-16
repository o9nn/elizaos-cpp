#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"
#include "process-manager.js.hpp"
#include "types.hpp"
#include "websocket-client.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class BrowserSession {
public:
};

class StagehandService {
public:
    StagehandService(IAgentRuntime protected runtime);
    void start(IAgentRuntime runtime);
    void stop(IAgentRuntime runtime);
    void initialize();
    void stop();
    std::future<BrowserSession> createSession(const std::string& sessionId);
    std::variant<Promise<BrowserSession, undefined>> getSession(const std::string& sessionId);
    std::variant<Promise<BrowserSession, undefined>> getCurrentSession();
    std::future<BrowserSession> getOrCreateSession();
    std::future<void> destroySession(const std::string& sessionId);
    StagehandWebSocketClient getClient();
    std::future<void> waitForReady(auto maxAttempts = 60, auto delayMs = 3000);

private:
    StagehandProcessManager processManager_;
    StagehandWebSocketClient client_;
};


} // namespace elizaos
