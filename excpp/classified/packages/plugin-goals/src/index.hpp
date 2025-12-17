#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "__tests__/e2e.hpp"
#include "actions/cancelGoal.hpp"
#include "actions/completeGoal.hpp"
#include "actions/confirmGoal.hpp"
#include "actions/createGoal.hpp"
#include "actions/updateGoal.hpp"
#include "apis.hpp"
#include "elizaos/core.hpp"
#include "providers/goals.hpp"
#include "schema.hpp"
#include "services/goalService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Import actions

// Import providers

// Import services

// Import schema

// Note: Table schemas are defined in schema.ts and will be automatically migrated

// Import tests

/**
 * The GoalsPlugin provides goal management functionality,
 * including creating, completing, updating, and canceling goals.
 */

      // Log current services to see what's registered

      // Goals tables will be created through the standard schema system
      // The tables are defined in schema.ts and will be migrated automatically
      // when the plugin schema is processed by the SQL plugin

      // Database migrations are handled by the SQL plugin

        // Note: Initial goals creation will be deferred until after migrations
        // This is handled by a separate initialization service

      // Log services again to see if anything changed

// Export service types for use in other plugins/tests

// Helper function to get the Goals service (handles case sensitivity)
std::optional<GoalService> getGoalsService(IAgentRuntime runtime);

} // namespace elizaos
