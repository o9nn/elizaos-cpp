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
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Interface for parsed task data
struct TodoTaskInput {
    std::string name;
    std::optional<std::string> description;
    std::string taskType;
    std::optional<int> priority;
    std::optional<bool> urgent;
    std::optional<std::string> dueDate;
    std::optional<std::string> recurring;
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

    // Ensure state has the required State properties

      // Step 1: Check if this is part of a confirmation flow

      // Get the data service early for all operations

        // Step 2: Extract task info from current message

      // Step 3: No need to get the data service again, we already have it

      // Step 4: Duplicate Check

      // Step 5: Create the task using the data service

      // Step 6: Send success message

    // Multi-action example: CREATE_TODO followed by LIST_TODOS
    // Single action with confirmation
    // Multi-action example: CREATE_TODO then COMPLETE_TODO
    // Daily task example
    // Multi-action example: CREATE_TODO with urgent priority then GET_ACTIVE_TODOS
    // Aspirational goal example


} // namespace elizaos
