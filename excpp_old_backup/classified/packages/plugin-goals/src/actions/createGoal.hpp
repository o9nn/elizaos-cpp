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



// Interface for parsed goal data
struct GoalInput {
    std::string name;
    std::optional<std::string> description;
    'agent' | 'entity' ownerType;
};

// Interface for similarity check result
struct SimilarityCheckResult {
    bool hasSimilar;
    std::optional<std::string> similarGoalName;
    double confidence;
};

/**
 * Template for extracting goal information from the user's message.
 */

/**
 * Template for checking if a similar goal already exists
 */

/**
 * Extracts goal information from the user's message.
 */
std::future<GoalInput | null> extractGoalInfo(IAgentRuntime runtime, Memory message, State state);

/**
 * Checks if a similar goal already exists
 */
std::future<SimilarityCheckResult> checkForSimilarGoal(IAgentRuntime runtime, GoalInput newGoal, const std::vector<std::any>& existingGoals);

/**
 * The CREATE_GOAL action allows the agent to create a new goal.
 */

      // Extract goal information from the message

      // Determine owner ID based on ownerType

      // Check for existing similar goals

      // Calculate active goal count

      // Step 6: Create the goal

      // Step 7: Send success message with guidance based on goal count

    // Multi-action: Create goal then list all goals to show chaining workflow
    // Multi-action: Create goal then confirm it to demonstrate goal lifecycle workflow


} // namespace elizaos
