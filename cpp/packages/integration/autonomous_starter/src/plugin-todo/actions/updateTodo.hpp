#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Interface for task selection properties
struct TaskSelection {
    std::string taskId;
    std::string taskName;
    bool isFound;
};

// Interface for task update properties
struct TaskUpdate {
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<int> priority;
    std::optional<bool> urgent;
    std::optional<std::string> dueDate;
    std::optional<std::string> recurring;
};

/**
 * Template for extracting which task to update from the user's message
 */

/**
 * Template for extracting task update information
 */

/**
 * Extracts which task the user wants to update
 */
std::future<TaskSelection> extractTaskSelection(IAgentRuntime runtime, Memory message, const std::vector<Task>& availableTasks);

/**
 * Extracts what updates the user wants to make to the task
 */

/**
 * Applies updates to a task
 */
std::future<Task> applyTaskUpdate(IAgentRuntime runtime, Task task, TaskUpdate update);

/**
 * The UPDATE_TODO action allows users to modify an existing task.
 */

    // Simpler validation: Only check if *any* active (non-completed) TODO exists

      // Get all active todos for this room

      // Phase 1: Extract which task to update

      // Phase 2: Extract what updates to make

      // Phase 3: Apply the update directly


} // namespace elizaos
