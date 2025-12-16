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



// Interface for goal selection properties
struct GoalSelection {
    std::string goalId;
    std::string goalName;
    bool isFound;
};

// Interface for goal update properties
struct GoalUpdate {
    std::optional<std::string> name;
    std::optional<std::string> description;
};

/**
 * Template for extracting which goal to update from the user's message
 */

/**
 * Template for extracting goal update information
 */

/**
 * Extracts which goal the user wants to update
 */
std::future<GoalSelection> extractGoalSelection(IAgentRuntime runtime, Memory message, const std::vector<GoalData>& availableGoals);

/**
 * Extracts what updates the user wants to make to the goal
 */
std::future<GoalUpdate | null> extractGoalUpdate(IAgentRuntime runtime, Memory message, GoalData goal);

/**
 * The UPDATE_GOAL action allows users to modify an existing goal.
 */

    // Check if any active goals exist

      // Check both agent and entity goals

      // Get all active goals (both agent and entity)

      // Phase 1: Extract which goal to update

      // Phase 2: Extract what updates to make

      // Phase 3: Apply the update

    // Multi-action: Update goal then list all goals to show modification workflow
    // Multi-action: Update goal then mark complete to demonstrate goal lifecycle completion


} // namespace elizaos
