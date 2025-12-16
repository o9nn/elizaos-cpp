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
 * Provides a list of attachments sent during the current conversation, including names, descriptions, and summaries.
 * @type {Provider}
 * @property {string} name - The name of the provider (ATTACHMENTS).
 * @property {string} description - Description of the provider.
 * @property {boolean} dynamic - Indicates if the provider is dynamic.
 * @property {function} get - Asynchronous function that retrieves attachments based on the runtime and message provided.
 * @param {IAgentRuntime} runtime - The runtime environment for the agent.
 * @param {Memory} message - The message object containing content and attachments.
 * @returns {Object} An object containing values, data, and text about the attachments retrieved.
 */
    // Start with any attachments in the current message

    // Process attachments from recent messages

    // Format attachments for display

    // Create formatted text with header


} // namespace elizaos
