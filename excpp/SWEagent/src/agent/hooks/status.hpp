#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "abstract.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Status hook for updating agent status
 * Converted from sweagent/agent/hooks/status.py
 */

/**
 * Hook for setting status during agent execution
 */
class SetStatusAgentHook {
public:
    void onSetupAttempt();
    void update(const std::string& message);
    void onStepStart();
    void onStepDone(StepOutput _step, AgentInfo info);
    void onToolsInstallationStarted();

private:
    std::string id_;
};


} // namespace elizaos
