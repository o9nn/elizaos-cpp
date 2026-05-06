#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Formats an array of memories into a single string with each memory content text separated by a new line.
 *
 * @param {Memory[]} facts - An array of Memory objects to be formatted.
 * @returns {string} A single string containing all memory content text with new lines separating each text.
 */
/**
 * Formats an array of Memory objects into a string, joining them with newlines.
 *
 * @param {Memory[]} facts - An array of Memory objects to format.
 * @returns {string} The formatted string with each Memory object's text joined by newlines.
 */
void formatFacts(const std::vector<Memory>& facts);

/**
 * Function to get key facts that the agent knows.
 * @param {IAgentRuntime} runtime - The runtime environment for the agent.
 * @param {Memory} message - The message object containing relevant information.
 * @param {State} [_state] - Optional state information.
 * @returns {Object} An object containing values, data, and text related to the key facts.
 */
      // Parallelize initial data fetching operations including recentInteractions

      // join the text of the last 5 messages

      // join the two and deduplicate


} // namespace elizaos
