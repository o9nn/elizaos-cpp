#include "..shared.hpp"
#include ".utils.hpp"
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



/**
 * Safely parse JSON response with error handling
 * @param response - The fetch Response object
 * @returns Parsed JSON data or null if parsing fails
 */

/**
 * Parse error response and throw appropriate error
 * @param response - The fetch Response object
 * @param defaultMessage - Default error message if JSON parsing fails
 */
std::future<never> handleErrorResponse(Response response, const std::string& defaultMessage);

/**
 * Get command implementation - retrieves and displays agent details
 */
std::future<void> getAgent(OptionValues opts);

/**
 * Remove command implementation - deletes an agent
 */
std::future<void> removeAgent(OptionValues opts);

/**
 * Clear memories command implementation - clears all memories for an agent
 */
std::future<void> clearAgentMemories(OptionValues opts);

/**
 * Set command implementation - updates agent configuration
 */
std::future<void> setAgentConfig(OptionValues opts);

} // namespace elizaos
