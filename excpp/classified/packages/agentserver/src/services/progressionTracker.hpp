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
#include "capabilityProgressionService.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ProgressionTracker {
public:
    ProgressionTracker(IAgentRuntime runtime, CapabilityProgressionService progressionService);
    void stop();
    void setupEventListeners();
    std::future<void> handleShellCommandExecuted(std::optional<std::any> params);
    std::future<void> handleAgentNamed(const std::any& _params);
    std::future<void> handleGoalCreated(const std::any& _params);
    std::future<void> handleTodoCreated(const std::any& _params);
    std::future<void> handleBrowserActionPerformed(std::optional<std::any> params);
    std::future<void> handleFormSubmitted(std::optional<std::any> params);
    std::future<void> handleVisionActionPerformed(std::optional<std::any> params);
    std::future<void> handleMicrophoneUsed(std::optional<std::any> _params);
    std::future<void> handleCapabilityUsed(std::optional<std::any> params);
    std::future<void> trackShellCommand(const std::string& command, double exitCode);
    std::future<void> trackGoalCreation(const std::unordered_map<std::string, std::any>& goalData);
    std::future<void> trackTodoCreation(const std::unordered_map<std::string, std::any>& todoData);
    std::future<void> trackAgentNaming(const std::string& name);
    std::future<void> trackAction(const std::string& actionType, std::optional<std::unordered_map<std::string, std::any>> details);
    std::unordered_map<std::string, std::any> getProgressionStatus();
    void cleanup();

private:
    IAgentRuntime runtime_;
    CapabilityProgressionService progressionService_;
};


} // namespace elizaos
