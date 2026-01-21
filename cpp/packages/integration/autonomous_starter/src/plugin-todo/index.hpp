#include "actions/cancelTodo.hpp"
#include "actions/completeTodo.hpp"
#include "actions/createTodo.hpp"
#include "actions/updateTodo.hpp"
#include "apis.hpp"
#include "elizaos/core.hpp"
#include "providers/todos.hpp"
#include "reminderService.hpp"
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



// Import actions

// Import providers

// Import services

/**
 * The TodoService class manages the todo functionality for the agent.
 */
class TodoService extends Service {
  static serviceType = "TODO" as ServiceTypeName;
  capabilityDescription =
    "The agent can manage to-do lists with daily recurring and one-off tasks";

  constructor(protected runtime: IAgentRuntime) {
    super(runtime);
  }

  /**
   * Starts the TodoService with the given runtime.
   * @param {IAgentRuntime} runtime - The runtime for the TodoService.
   * @returns {Promise<TodoService>} A promise that resolves with the TodoService instance.
   */

  /**
   * Stops the TodoService in the given agent runtime.
   * @param {IAgentRuntime} runtime - The agent runtime containing the service.
   * @returns {Promise<void>} A promise that resolves once the service has been stopped.
   */

  /**
   * Stops the TodoService.
   * @returns {Promise<void>} A promise that resolves once the service has been stopped.
   */
    // No specific cleanup needed for this service

  /**
   * Initializes the TodoService with the given configuration.
   * @param {Record<string, string>} config - The configuration for the TodoService.
   * @returns {Promise<void>} A promise that resolves once the service has been initialized.
   */

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
