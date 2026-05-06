#include ".pointsService.hpp"
#include ".services/taskService.hpp"
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
std::future<TaskCompletion> extractTaskCompletion(IAgentRuntime runtime, Memory message, const std::vector<Task>& availableTasks, State state);

/**
 * Processes a daily task completion, updating streak and reactivating for tomorrow
 */
  // Get current streak (default to 0 if not set)

  // Calculate points - base points for daily completion + streak bonus

  // Mark as completed for today by adding a completed tag temporarily
  // Update metadata BEFORE adding points to ensure it has the pointsAwarded

  // Award points to the user

/**
 * Processes a one-off task completion, checking if it was completed on time
 */

  // Check if the task had a due date and if it's overdue

  // Calculate points based on priority, urgency, and whether it was completed on time

  // Mark the task as completed

  // Award points to the user

/**
 * Processes an aspirational goal completion
 */
  // Fixed points for completing an aspirational goal

  // Mark the task as completed

  // Award points to the user

/**
 * The COMPLETE_TODO action allows users to mark a task as completed.
 */

    // Only validate if there are active (non-completed) todos in the current room
      // Filter out completed AND confirmation tasks

      // Fetch room details directly to get worldId
      // Get all incomplete todos for this room

      // Filter out already completed tasks

      // Extract which task the user wants to complete

      // Find the task in the available tasks

      // Process the task completion based on its type

        // Process daily task

        // Process one-off task

        // Process aspirational goal

        // Generic completion for any other todo type


} // namespace elizaos
