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



/**
 * Provider that collects capability descriptions from all registered services
 */
/**
 * Provides capabilities information for the agent.
 *
 * @param {IAgentRuntime} runtime - The agent runtime instance.
 * @param {Memory} _message - The memory message object.
 * @returns {Promise<ProviderResult>} The provider result object containing capabilities information.
 */
      // Get all registered services

      // Extract capability descriptions from all services

        // Handle the fact that services are stored as arrays
          // Use the first service in the array for the capability description

      // Format the capabilities into a readable list


} // namespace elizaos
