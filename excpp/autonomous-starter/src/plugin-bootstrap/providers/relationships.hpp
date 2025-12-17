#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Asynchronously formats relationships based on their interaction strength.
 *
 * @param {IAgentRuntime} runtime The runtime instance.
 * @param {Relationship[]} relationships The relationships to be formatted.
 * @returns {Promise<string>} A formatted string of the relationships.
 */
std::future<void> formatRelationships(IAgentRuntime runtime, const std::vector<Relationship>& relationships);

/**
 * Provider for fetching relationships data.
 *
 * @type {Provider}
 * @property {string} name - The name of the provider ("RELATIONSHIPS").
 * @property {string} description - Description of the provider.
 * @property {Function} get - Asynchronous function to fetch relationships data.
 * @param {IAgentRuntime} runtime - The agent runtime object.
 * @param {Memory} message - The message object containing entity ID.
 * @returns {Promise<Object>} Object containing relationships data or error message.
 */
    // Get all relationships for the current user


} // namespace elizaos
