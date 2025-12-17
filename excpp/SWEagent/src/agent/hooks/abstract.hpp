#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Abstract hooks for agent lifecycle events
 * Converted from sweagent/agent/hooks/abstract.py
 */

// Forward declaration to avoid circular dependency
struct DefaultAgent {
    std::string name;
};

/**
 * Abstract base class for agent hooks
 */
    void onInit(DefaultAgent _agent);
    void onRunStart();
    void onStepStart();
    void onActionsGenerated(StepOutput _step);
    void onActionStarted(StepOutput _step);
    void onActionExecuted(StepOutput _step);
    void onStepDone(StepOutput _step, AgentInfo _info);
    void onRunDone(Trajectory _trajectory, AgentInfo _info);
    void onSetupAttempt();
    void onModelQuery(const std::vector<std::any>& _messages, const std::string& _agent);
    void onQueryMessageAdded(std::optional<std::any> _params);
    void onSetupDone();
    void onToolsInstallationStarted();

/**
 * Combined agent hook that manages multiple hooks
 */
class CombinedAgentHook {
public:
    CombinedAgentHook(std::optional<std::vector<AbstractAgentHook>> hooks);
    void addHook(AbstractAgentHook hook);
    std::vector<AbstractAgentHook> allHooks() const;
    void onInit(DefaultAgent _agent);
    void onRunStart();
    void onStepStart();
    void onActionsGenerated(StepOutput _step);
    void onActionStarted(StepOutput _step);
    void onActionExecuted(StepOutput _step);
    void onStepDone(StepOutput _step, AgentInfo _info);
    void onRunDone(Trajectory _trajectory, AgentInfo _info);
    void onSetupAttempt();
    void onModelQuery(const std::vector<std::any>& _messages, const std::string& _agent);
    void onQueryMessageAdded(std::optional<std::any> _params);
    void onSetupDone();
    void onToolsInstallationStarted();
};


} // namespace elizaos
