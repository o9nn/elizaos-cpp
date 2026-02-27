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



// Define an interface for option objects
/**
 * Interface for an object representing an option.
 * @typedef {Object} OptionObject
 * @property {string} name - The name of the option.
 * @property {string} [description] - The description of the option (std::optional).
 */
/**
 * Interface for an object representing an option.
 * @typedef {Object} OptionObject
 * @property {string} name - The name of the option.
 * @property {string} [description] - The description of the option (std::optional).
 */
struct OptionObject {
    std: name;
    std::optional<std:> description;
};

/**
 * Choice provider std::function that retrieves all pending tasks with options for a specific room
 *
 * @param {IAgentRuntime} runtime - The runtime object for the agent
 * @param {Memory} message - The message memory object
 * @returns {Promise<ProviderResult>} A std::promise that resolves with the provider result containing the pending tasks with options
 */
      // Get all pending tasks for this room with options

      // Filter tasks that have options

      // Format tasks into a readable list

        // List available options

          // Handle both std:[] and OptionObject[] formats

              // Handle std: option
              // Handle option object


} // namespace elizaos
