#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "actions/index.ts.hpp"
#include "elizaos/core.hpp"
#include "evaluators/index.js.hpp"
#include "providers/index.js.hpp"
#include "services/embedding.js.hpp"
#include "services/task.js.hpp"
#include "templates/index.js.hpp"
#include "utils/index.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// 

/**
 * Multi-step workflow execution result
 */
struct MultiStepActionResult {
    bool success;
    std::optional<std::string> text;
    std::optional<std::variant<std::string, Error>> error;
};

/**
 * Handles incoming messages and generates responses based on the provided runtime and message information.
 *
 * @param {MessagePayload} payload - The message payload containing runtime, message, and callback.
 * @returns {Promise<void>} - A promise that resolves once the message handling and response generation is complete.
 */
  // Set up timeout monitoring

    // Generate a new response ID
    
    // Check if this is a job request (x402 paid API)
    // Job requests are isolated one-off operations that don't need race tracking
    
    // Get or create the agent-specific map

    // Only track response IDs for non-job messages
    // Job requests bypass race tracking since they're isolated operations
      // Log when we're updating the response ID

      // Set this as the latest response ID for this agent+room

    // Use runtime's run tracking for this message processing

    // Emit run started event
      // this shouldn't be a standard
      // but we need to expose content somewhere

        // First, save the incoming message

        // Check if memory already exists (it might have been created by MessageBusService)

            // Create memory with the existing ID (preserving external IDs)
            // Use the created memory with the actual ID returned by the database
          // Queue with high priority for messages with pre-existing IDs
          // No ID, create new memory and queue embedding
          // Set the ID on the message for downstream processing
          // Create a memory object with the new ID for queuing

        // default LLM to off
          // allow some other subsystem to handle this event
          // maybe emit an event

          // Emit run ended event on successful completion

          // Emit run ended event on successful completion

        // Race check before we send anything
        // IMPORTANT: Bypass race check for job requests (x402 paid API)
        // Job requests are one-off operations that must always complete

            // Log provider usage for simple responses
            // without actions there can't be more than one message
                return callback(content);

        // Clean up the response ID since we handled it

              return callback(content);

        // ok who are they

              // server/guild name?
            // how do I get worldName

        // get available actions

        // generate data of interest
          // to strip out

        // Emit run ended event on successful completion
        // Emit run ended event with error

using StrategyMode = std::variant<'simple', 'actions', 'none'>;
using StrategyResult = std::variant<{

std::future<StrategyResult> runMultiStepCore(auto { runtime, auto message, auto state, { runtime: IAgentRuntime callback }, Memory message, State state, std::optional<HandlerCallback }> callback);

/**
 * Handles message deletion events by removing the corresponding memory from the agent's memory store.
 *
 * @param {Object} params - The parameters for the function.
 * @param {IAgentRuntime} params.runtime - The agent runtime object.
 * @param {Memory} params.message - The message memory that was deleted.
 * @returns {void}
 */

/**
 * Handles channel cleared events by removing all message memories from the specified room.
 *
 * @param {Object} params - The parameters for the function.
 * @param {IAgentRuntime} params.runtime - The agent runtime object.
 * @param {UUID} params.roomId - The room ID to clear message memories from.
 * @param {string} params.channelId - The original channel ID.
 * @param {number} params.memoryCount - Number of memories found.
 * @returns {void}
 */

    // Get all message memories for this room

    // Delete each message memory

/**
 * Syncs a single user into an entity
 */
/**
 * Asynchronously sync a single user with the specified parameters.
 *
 * @param {UUID} entityId - The unique identifier for the entity.
 * @param {IAgentRuntime} runtime - The runtime environment for the agent.
 * @param {any} user - The user object to sync.
 * @param {string} serverId - The unique identifier for the server.
 * @param {string} channelId - The unique identifier for the channel.
 * @param {ChannelType} type - The type of channel.
 * @param {string} source - The source of the user data.
 * @returns {Promise<void>} A promise that resolves once the user is synced.
 */

    // Ensure we're not using WORLD type and that we have a valid channelId

    // Create world with ownership metadata for DM connections (onboarding)

    // Verify the world was created with proper metadata

/**
 * Handles standardized server data for both WORLD_JOINED and WORLD_CONNECTED events
 */

/**
 * Handles control messages for enabling or disabling UI elements in the frontend
 * @param {Object} params - Parameters for the handler
 * @param {IAgentRuntime} params.runtime - The runtime instance
 * @param {Object} params.message - The control message
 * @param {string} params.source - Source of the message
 */

    // Here we would use a WebSocket service to send the control message to the frontend
    // This would typically be handled by a registered service with sendMessage capability

    // Get any registered WebSocket service

        // Send the control message through the WebSocket service

        // Update entity to inactive

    // actions.replyAction,
    // actions.ignoreAction,


} // namespace elizaos
