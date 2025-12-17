#include ".schema.hpp"
#include ".types.hpp"
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
 * Service for managing goal data
 */
class GoalDataManager {
  private runtime: IAgentRuntime;

  constructor(runtime: IAgentRuntime) {
    this.runtime = runtime;
  }

  /**
   * Create a new goal
   */

    // Create the goal

    // Only include description if it's provided

    // Add tags if provided

  /**
   * Get goals with optional filters
   */

    // Get tags for all goals

    // Group tags by goal

    // Filter by tags if specified

  /**
   * Get a single goal by ID
   */

    // Get tags

  /**
   * Update a goal
   */

    // Update goal fields

    // Update tags if provided
      // Delete existing tags

      // Insert new tags

  /**
   * Delete a goal
   */

  /**
   * Get uncompleted goals
   */

  /**
   * Get completed goals
   */

  /**
   * Count goals with filters
   */

  /**
   * Get all goals for a specific owner (both completed and uncompleted)
   */

/**
 * Creates a GoalDataManager instance
 * @deprecated Use runtime.getService('goals') instead in production code.
 * This function is kept for test mocking purposes only.
 * @param runtime - The agent runtime
 * @returns A new GoalDataManager instance
 */
GoalDataManager createGoalDataService(IAgentRuntime runtime);

/**
 * Service wrapper for the GoalService to be registered with the plugin
 */
class GoalService extends Service {
  static serviceType = GoalServiceType.GOALS;
  // serviceName will be automatically derived as "Goal" from class name

  private goalDataManager: GoalDataManager | null = null;

  capabilityDescription = 'Manages goal data storage and retrieval';

  async stop(): Promise<void> {
    // Cleanup if needed
    this.goalDataManager = null;
  }

    // Initialize the goal data manager

    // Log current services to debug registration

  /**
   * Get the underlying GoalDataManager instance
   * @deprecated Use the service methods directly instead
   */

  // Proxy all GoalDataManager methods for easy access


} // namespace elizaos
