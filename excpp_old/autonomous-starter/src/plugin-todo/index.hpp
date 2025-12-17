#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "actions/cancelTodo.hpp"
#include "actions/completeTodo.hpp"
#include "actions/createTodo.hpp"
#include "actions/updateTodo.hpp"
#include "apis.hpp"
#include "elizaos/core.hpp"
#include "providers/todos.hpp"
#include "reminderService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Import actions

// Import providers

// Import services

/**
 * The TodoService class manages the todo functionality for the agent.
 */
class TodoService {
public:
    TodoService(IAgentRuntime protected runtime);
    std::future<TodoService> start(IAgentRuntime runtime);
    std::future<void> stop(IAgentRuntime runtime);
    std::future<void> stop();
    std::future<void> init(Record<string config, auto string>);
};

/**
 * The TodoPlugin provides task management functionality with daily recurring and one-off tasks,
 * including creating, completing, updating, and deleting tasks, as well as a point system for
 * task completion.
 */

    // Setup daily task reset for recurring tasks
    // This creates a task that will run at the start of each day to reset daily tasks

    // Check if worldId exists before creating the task
      // Continue initialization without the daily reset task
      // TOOD: Look and see if we already have this task. If we do, we don't need to create it again.

    // Register the task worker to handle daily task reset
      // Do we have to run this every single time?
            // Get all daily tasks across all rooms

            // Reset each completed daily task
                // Remove the completed tag so the task is active again


} // namespace elizaos
