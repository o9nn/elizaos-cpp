#include ".templates.hpp"
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
 * Handles incoming messages and generates responses based on the provided runtime and message information.
 *
 * @param {MessageReceivedHandlerParams} params - The parameters needed for message handling, including runtime, message, and callback.
 * @returns {Promise<void>} - A promise that resolves once the message handling and response generation is complete.
 */
  // Set up timeout monitoring
    // Generate a new response ID
    // Get or create the agent-specific map

    // Set this as the latest response ID for this agent+room

    // Generate a unique run ID for tracking this message handler execution

    // Emit run started event

        // First, save the incoming message

        // Skip shouldRespond check for DM and VOICE_DM channels

        // Handle shouldRespond

        // Try to preprocess response by removing code blocks markers if present
        // let processedResponse = response.replace('```json', '').replaceAll('```', '').trim(); // No longer needed for XML

          // Retry if missing required fields

            // Attempt to parse the XML response

            // Map parsed XML to Content type, handling potential missing fields

              // Reorder actions so 'REPLY' always comes first if present

              // Deduplicate actions

          // Check if this is still the latest response ID for this agent+room

          // Clean up the response ID

          // Handle the case where the agent decided not to respond

          // Check if we still have the latest response ID

          // Construct a minimal content object indicating ignore, include a generic thought

          // Call the callback directly with the ignore content
          await callback(ignoreContent);

          // Also save this ignore action/thought to memory

          // Clean up the response ID since we handled it

          // Optionally, evaluate the decision to ignore (if relevant evaluators exist)
          // await runtime.evaluate(message, state, shouldRespond, callback, []);

        // Emit run ended event on successful completion
        // Emit run ended event with error

} // namespace elizaos
