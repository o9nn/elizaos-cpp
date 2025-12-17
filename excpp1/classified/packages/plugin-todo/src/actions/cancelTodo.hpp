#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Interface for task cancellation properties
struct TaskCancellation {
    std::string taskId;
    std::string taskName;
    bool isFound;
};

/**
 * Template for extracting task cancellation information from user message
 */

/**
 * Extracts which task the user wants to cancel
 */
std::future<TaskCancellation> extractTaskCancellation(IAgentRuntime runtime, Memory message, const std::vector<TodoData>& availableTasks, State state);

/**
 * The CANCEL_TODO action allows users to cancel/delete a task.
 */

      // Get all todos for this room

      // Extract which task the user wants to cancel

      // Find the task in the available tasks

      // Delete the task


} // namespace elizaos
