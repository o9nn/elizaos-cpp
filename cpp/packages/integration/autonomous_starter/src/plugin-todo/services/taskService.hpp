#include "dbCompatibility.hpp"
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



/**
 * Task service that provides database-compatible task operations
 */
class TaskService {
  constructor(private runtime: IAgentRuntime) {}

  /**
   * Get tasks with database-compatible filtering
   */
      // Get raw tasks from runtime

      // Process tasks for database compatibility

  /**
   * Create a task with database-compatible data
   */
      // Pass params directly to runtime - it handles database-specific formatting internally

  /**
   * Update a task with database-compatible data
   */
      // Pass updates directly to runtime - it handles database-specific formatting internally

  /**
   * Normalize a task from the database
   */
    // Ensure tags is always an array

    // Ensure metadata is always an object

    // Normalize boolean fields in metadata

    // Normalize dates in metadata

  /**
   * Search tasks by name (case-insensitive)
   */
    
    // Case-insensitive search

/**
 * Create a task service instance for the runtime
 */
TaskService createTaskService(IAgentRuntime runtime); 
} // namespace elizaos
