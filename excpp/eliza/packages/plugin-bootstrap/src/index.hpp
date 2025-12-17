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
#include "actions/index.ts.hpp"
#include "elizaos/core.hpp"
#include "evaluators/index.ts.hpp"
#include "providers/index.ts.hpp"
#include "services/task.ts.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



* from './actions/index.ts';
* from './evaluators/index.ts';
* from './providers/index.ts';

/**
 * Represents media data containing a buffer of data and the media type.
 * @typedef {Object} MediaData
 * @property {Buffer} data - The buffer of data.
 * @property {string} mediaType - The type of media.
 */
using MediaData = {

/**
 * Escapes special characters in a string to make it JSON-safe.
 */
/* // Removing JSON specific helpers
std::string escapeForJson(const std::string& input);

std::string sanitizeJson(const std::string& rawJson);
*/

/**
 * Fetches media data from a list of attachments, supporting both HTTP URLs and local file paths.
 *
 * @param attachments Array of Media objects containing URLs or file paths to fetch media from
 * @returns Promise that resolves with an array of MediaData objects containing the fetched media data and content type
 */
/**
 * Fetches media data from given attachments.
 * @param {Media[]} attachments - Array of Media objects to fetch data from.
 * @returns {Promise<MediaData[]>} - A Promise that resolves with an array of MediaData objects.
 */

/**
 * Processes attachments by generating descriptions for supported media types.
 * Currently supports image description generation.
 *
 * @param {Media[]} attachments - Array of attachments to process
 * @param {IAgentRuntime} runtime - The agent runtime for accessing AI models
 * @returns {Promise<Media[]>} - Returns a new array of processed attachments with added description, title, and text properties
 */

/**
 * Determines whether to skip the shouldRespond logic based on room type and message source.
 * Supports both default values and runtime-configurable overrides via env settings.
 */
bool shouldBypassShouldRespond(IAgentRuntime runtime, std::optional<Room> room, std::optional<std::string> source);

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

    // Use runtime's run tracking for this message processing

    // Emit run started event

        // First, save the incoming message

        // Skip shouldRespond check for DM and VOICE_DM channels

        // Handle shouldRespond

          // Try to preprocess response by removing code blocks markers if present
          // let processedResponse = response.replace('```json', '').replaceAll('```', '').trim(); // No longer needed for XML

          // If an action is provided, the agent intends to respond in some way
          // Only exclude explicit non-response actions

          // Retry if missing required fields

            // Attempt to parse the XML response

            // Map parsed XML to Content type, handling potential missing fields

          // Check if this is still the latest response ID for this agent+room

            // Automatically determine if response is simple based on providers and actions
            // Simple = REPLY action with no providers used

          // Clean up the response ID

            // Log provider usage for simple responses

            // without actions there can't be more than one message
          // Handle the case where the agent decided not to respond

          // Check if we still have the latest response ID

          // Construct a minimal content object indicating ignore, include a generic thought

          // Call the callback directly with the ignore content

          // Also save this ignore action/thought to memory

          // Clean up the response ID since we handled it

          // Optionally, evaluate the decision to ignore (if relevant evaluators exist)

        // Emit run ended event on successful completion
        // Emit run ended event with error

/**
 * Handles the receipt of a reaction message and creates a memory in the designated memory manager.
 *
 * @param {Object} params - The parameters for the function.
 * @param {IAgentRuntime} params.runtime - The agent runtime object.
 * @param {Memory} params.message - The reaction message to be stored in memory.
 * @returns {void}
 */

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
 * Handles the generation of a post (like a Tweet) and creates a memory for it.
 *
 * @param {Object} params - The parameters for the function.
 * @param {IAgentRuntime} params.runtime - The agent runtime object.
 * @param {Memory} params.message - The post message to be processed.
 * @param {HandlerCallback} params.callback - The callback function to execute after processing.
 * @returns {Promise<void>}
 */
  // Ensure world exists first

  // Ensure timeline room exists

  // generate thought of which providers to use using messageHandlerTemplate

  // Compose state with relevant context for tweet generation

  // get twitterUserName

  // Retry if missing required fields

    // Parse XML

  // update stats with correct providers

  // Generate prompt for tweet content

  // Use TEXT_LARGE model as we expect structured XML text, not a JSON object

  // Parse the XML response

    // Handle the error appropriately, maybe retry or return an error state

  /**
   * Cleans up a tweet text by removing quotes and fixing newlines
   */
  std::string cleanupPostText(const std::string& text);

  // Cleanup the tweet text

  // Prepare media if included
  // const mediaData: MediaData[] = [];
  // if (jsonResponse.imagePrompt) {
  // 		prompt: jsonResponse.imagePrompt,
  // 		output: "no-schema",
  // 	});
  // 	try {
  // 		// Convert image prompt to Media format for fetchMediaData
  // 		const imagePromptMedia: any[] = images

  // 		// Fetch media using the utility function
  // 		mediaData.push(...fetchedMedia);
  // 	} catch (error) {
  // 		logger.error("Error fetching media for tweet:", error);
  // 	}
  // }

  // have we posted it before?

  // GPT 3.5/4: /(i\s+do\s+not|i'?m\s+not)\s+(feel\s+)?comfortable\s+generating\s+that\s+type\s+of\s+content|(inappropriate|explicit|offensive|communicate\s+respectfully|aim\s+to\s+(be\s+)?helpful)/i
  //const cohereRefusalRegex = /(request\s+cannot\s+be\s+processed|violates\s+(our\s+)?content\s+policy|not\s+permitted\s+by\s+usage\s+restrictions)/i

  // Create the response memory

  // Process the actions and execute the callback

  // // Run any configured evaluators
  // 	message,
  // 	state,
  // 	true, // Post generation is always a "responding" scenario
  // 	callback,
  // 	responseMessages,
  // );

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

  // this is jank, these events are not valid


} // namespace elizaos
