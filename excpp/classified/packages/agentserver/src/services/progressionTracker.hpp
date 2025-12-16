#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
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
    std::future<void> handleGoalCreated({ goalData: Record<string _params, auto unknown> });
    std::future<void> handleTodoCreated({ todoData: Record<string _params, auto unknown> });
    std::future<void> handleFormSubmitted(std::optional<{ details: Record<string> params, auto unknown> });
    std::future<void> trackShellCommand(const std::string& command, double exitCode);
    std::future<void> trackGoalCreation(Record<string goalData, auto unknown>);
    std::future<void> trackTodoCreation(Record<string todoData, auto unknown>);
    std::future<void> trackAgentNaming(const std::string& name);
    std::future<void> trackAction(const std::string& actionType, std::optional<Record<string> details, auto unknown>);
    std::unordered_map<std::string, unknown> getProgressionStatus();
    void cleanup();


} // namespace elizaos
