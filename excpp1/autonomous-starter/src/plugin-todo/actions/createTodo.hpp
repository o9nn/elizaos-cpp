#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".services/taskService.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Interface for parsed task data
struct TodoTaskInput {
    std::string name;
    std::optional<std::string> description;
    std::variant<"daily", "one-off", "aspirational"> taskType;
    std::optional<std::variant<1, 2, 3, 4>> priority;
    std::optional<bool> urgent;
    std::optional<std::string> dueDate;
    std::optional<std::variant<"daily", "weekly", "monthly">> recurring;
};

// Interface for choice options
struct ChoiceOption {
    std::string name;
    std::string description;
};

/**
 * Template for extracting todo information from the user's message.
 */

**IMPORTANT:** If the user message appears to be a simple confirmation (e.g., \"yes\", \"confirm\", \"ok\", \"looks good\", \"do it\"), and NOT a description of a new task, return an empty response like <response></response> or a response with a clear indication like <is_confirmation>true</is_confirmation> instead of trying to extract task details.

/**
 * Extracts todo information from the user's message.
 */

/**
 * The CREATE_TODO action allows the agent to create a new todo item.
 */

    // No validation needed if we create directly - let handler decide

      // Step 1: Compose state with relevant providers
      // Use the incoming message and specify providers. Ignore stateFromTrigger for composition.

      // Step 2: Extract todo info from the message using the composed state

      // Step 3: Duplicate Check (using state.data.TODOS if available, or fetch)
        // Fetch if not in state

      // Step 4: Create the task directly

      // Step 5: Send success message


} // namespace elizaos
