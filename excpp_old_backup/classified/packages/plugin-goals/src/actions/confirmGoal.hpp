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



// Interface for confirmation data stored in state
struct PendingGoalData {
    std::string name;
    std::optional<std::string> description;
    'daily' | 'one-off' | 'aspirational' taskType;
    std::optional<1 | 2 | 3 | 4> priority;
    std::optional<bool> urgent;
    std::optional<std::string> dueDate;
    std::optional<'daily' | 'weekly' | 'monthly'> recurring;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

// Interface for confirmation response
struct ConfirmationResponse {
    bool isConfirmation;
    bool shouldProceed;
    std::optional<std::string> modifications;
};

/**
 * Template for extracting confirmation intent from user message
 */

/**
 * Extracts confirmation intent from the user's message
 */
std::future<ConfirmationResponse> extractConfirmationIntent(IAgentRuntime runtime, Memory message, PendingGoalData | null pendingTask, State state);

/**
 * The CONFIRM_GOAL action handles the confirmation step of goal creation
 */

    // This action is only valid if there's a pending goal in the state

      // Extract confirmation intent

        // User said something unrelated to the confirmation

        // User rejected the task
        // Clear the pending goal from state

      // User confirmed - create the task

      // Check for duplicates one more time

      // Create the goal

      // Clear the pending goal from state

      // Send success message

    // Multi-action: Create goal preview, confirm, then list all goals to show complete workflow


} // namespace elizaos
