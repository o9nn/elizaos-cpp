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
 * Character provider object.
 * @typedef {Object} Provider
 * @property {string} name - The name of the provider ("CHARACTER").
 * @property {string} description - Description of the character information.
 * @property {Function} get - Async function to get character information.
 */
/**
 * Provides character information.
 * @param {IAgentRuntime} runtime - The agent runtime.
 * @param {Memory} message - The message memory.
 * @param {State} state - The state of the character.
 * @returns {Object} Object containing values, data, and text sections.
 */

    // Character name

    // Handle bio (string or random selection from array)

    // System prompt

    // Select random topic if available

    // postCreationTemplate in core prompts.ts
    // Write a post that is {{adjective}} about {{topic}} (without mentioning {{topic}} directly), from the perspective of {{agentName}}. Do not add commentary or acknowledge this request, just write the post.
    // Write a post that is {{Spartan is dirty}} about {{Spartan is currently}}

    // Format topics list

    // Select random adjective if available

    // Format post examples

    // Format message examples

    // Style directions

    // Combine all text sections


} // namespace elizaos
