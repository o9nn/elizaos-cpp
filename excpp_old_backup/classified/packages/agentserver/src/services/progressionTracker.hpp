#include "capabilityProgressionService.hpp"
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



class ProgressionTracker {
  private runtime: IAgentRuntime;
  private progressionService: CapabilityProgressionService;

  constructor(runtime: IAgentRuntime, progressionService: CapabilityProgressionService) {
    this.runtime = runtime;
    this.progressionService = progressionService;
    this.setupEventListeners();
  }

    // Event listeners are automatically cleaned up with the runtime
    // No manual cleanup needed for event-based approach

    // Skip setup in unlocked mode

    // Register event listeners for progression tracking

  // Event handlers for progression tracking

  // Method to track shell command execution - called from API endpoints
    // Emit event instead of direct tracking

  // Method to track goal creation - called from API endpoints
    // Emit event instead of direct tracking

  // Method to track todo creation - called from API endpoints
    // Emit event instead of direct tracking

  // Method to track agent naming - should be called when agent name changes
    // Emit event instead of direct tracking

  // Method to manually track specific actions

    // Emit appropriate events based on action type
        // For generic capability usage

  // Method to check and display current progression status

  // Cleanup method to stop intervals

} // namespace elizaos
