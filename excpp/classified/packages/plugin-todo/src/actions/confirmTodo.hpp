#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Interface for confirmation data stored in state
struct PendingTodoData {
    std::string name;
    std::optional<std::string> description;
    std::variant<'daily', 'one-off', 'aspirational'> taskType;
    std::optional<std::variant<1, 2, 3, 4>> priority;
    std::optional<bool> urgent;
    std::optional<std::string> dueDate;
    std::optional<std::variant<'daily', 'weekly', 'monthly'>> recurring;
    std::optional<std::vector<std::string>> tags;
    std::optional<TodoMetadata> metadata;
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
std::future<ConfirmationResponse> extractConfirmationIntent(IAgentRuntime runtime, Memory message, const std::optional<PendingTodoData>& pendingTask, State state);

/**
 * The CONFIRM_TODO action handles the confirmation step of todo creation
 */

    // This action is only valid if there's a pending todo in the state

      // Extract confirmation intent

        // User said something unrelated to the confirmation

        // User rejected the task
        // Clear the pending todo from state

      // User confirmed - create the task

      // Check for duplicates one more time

      // Create the task

      // Clear the pending todo from state

      // Send success message

    // Multi-action example: CREATE_TODO_PREVIEW followed by CONFIRM_TODO and LIST_TODOS
    // Standard confirmation flow


} // namespace elizaos
