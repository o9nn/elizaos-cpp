#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Interface for task completion properties
struct TaskCompletion {
    std::string taskId;
    std::string taskName;
    bool isFound;
};

/**
 * Template for extracting task completion information from user message
 */

/**
 * Extracts which task the user wants to mark as completed
 */
std::future<TaskCompletion> extractTaskCompletion(IAgentRuntime runtime, Memory message, const std::vector<TodoData>& availableTasks, State state);

/**
 * The COMPLETE_TODO action allows users to mark a task as completed.
 */

      // Get available tasks

      // Extract which task the user wants to complete

      // Find the task in the available tasks

      // Mark the task as completed

      // Generate response text based on task type


} // namespace elizaos
