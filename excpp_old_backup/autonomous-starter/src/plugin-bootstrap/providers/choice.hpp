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



// Define an interface for option objects
/**
 * Interface for an object representing an option.
 * @typedef {Object} OptionObject
 * @property {string} name - The name of the option.
 * @property {string} [description] - The description of the option (optional).
 */
struct OptionObject {
    std::string name;
    std::optional<std::string> description;
};

/**
 * Choice provider function that retrieves all pending tasks with options for a specific room
 *
 * @param {IAgentRuntime} runtime - The runtime object for the agent
 * @param {Memory} message - The message memory object
 * @returns {Promise<ProviderResult>} A promise that resolves with the provider result containing the pending tasks with options
 */
    // Get all pending tasks for this room with options

    // Filter tasks that have options

    // Format tasks into a readable list

      // List available options

          // Handle mixed or unexpected format - log error or provide generic message


} // namespace elizaos
