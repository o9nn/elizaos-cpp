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
 * Extracts which goal the user wants to cancel
 */
std::future<TaskCancellation> extractTaskCancellation(IAgentRuntime runtime, Memory message, const std::vector<GoalData>& availableGoals, State state);

/**
 * The CANCEL_GOAL action allows users to cancel/delete a task.
 */

    // Check if *any* active GOALs exist

      // Get active goals for the entity

      // Extract which goal to cancel

        // Show the list of goals

      // Find the goal to cancel

      // Delete the goal

    // Multi-action: Cancel goal then list remaining to show updated goal state
    // Multi-action: Cancel goal then create new one to show goal replacement workflow


} // namespace elizaos
