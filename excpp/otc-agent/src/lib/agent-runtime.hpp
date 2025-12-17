#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "agent.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Serverless-compatible agent runtime with Drizzle ORM for Next.js
// DO NOT replace with an agent-simple.ts, it won't work!

// Global state for serverless environment persistence
struct GlobalElizaState {
    std::optional<bool> __elizaMigrationsRan;
    std::optional<bool> __elizaManagerLogged;
    std::optional<std::optional<AgentRuntime>> __elizaRuntime;
    typeof console::log log;
    typeof console::info info;
    typeof console::warn warn;
    typeof console::error error;
    typeof console::debug debug;
};

class AgentRuntimeManager {
public:
    AgentRuntimeManager();
    static AgentRuntimeManager getInstance();
    bool isReady();
    std::future<AgentRuntime> getRuntime();
    std::future<AgentRuntime> createRuntime();
    std::future<void> ensureBuiltInTables();
    std::future<Memory> handleMessage(const std::string& roomId, const std::string& entityId, std::optional<std::any> content);
};

// Export singleton instance

} // namespace elizaos
