#include "elizaos/core.hpp"
#include "providers/recentMessages.hpp"
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
 * Extracts the text content from within a <response> XML tag.
 * @param text The input string potentially containing the <response> tag.
 * @returns The extracted text content, or null if the tag is not found or empty.
 */

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
 * Handles incoming messages and generates responses based on the provided runtime and message information.
 *
 * @param {MessageReceivedHandlerParams} params - The parameters needed for message handling, including runtime, message, and callback.
 * @returns {Promise<void>} - A promise that resolves once the message handling and response generation is complete.
 */
  // Generate a new response ID
  // Get or create the agent-specific map

  // Set this as the latest response ID for this agent+room

  // Generate a unique run ID for tracking this message handler execution

  // Emit run started event

  // Set up timeout monitoring

      // First, save the incoming message

      // Retry if missing required fields

        // Attempt to parse the XML response

      // Check if this is still the latest response ID for this agent+room

      // Clean up the response ID

      // Emit run ended event on successful completion
      // Emit run ended event with error

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

/**
 * Handles standardized server data for both WORLD_JOINED and WORLD_CONNECTED events
 */
    // Create/ensure the world exists for this server

    // First sync all rooms/channels

    // Then sync all users
      // Process entities in batches to avoid overwhelming the system

        // check if user is in any of these rooms in rooms

        // Process each user in the batch

        // Add a small delay between batches if not the last batch

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

      // Message sent tracking

      await handleServerSync(payload);

      await handleServerSync(payload);

        // Update entity to inactive

// Get the current file's directory

/**
 * Recursively gets all files in a directory with the given extension
 *
 * @param {string} dir - Directory to search
 * @param {string[]} extensions - File extensions to look for
 * @returns {string[]} - Array of file paths
 */
std::vector<std::string> getFilesRecursively(const std::string& dir, const std::vector<std::string>& extensions);

/**
 * Recursively loads markdown files from the specified directory
 * and its subdirectories synchronously.
 *
 * @param {string} directoryPath - The path to the directory containing markdown files
 * @returns {string[]} - Array of strings containing file contents with relative paths
 */
std::vector<std::string> loadDocumentation(const std::string& directoryPath);

/**
 * Initializes the character with the provided runtime, configuration, actions, providers, and evaluators.
 * Registers actions, providers, and evaluators to the runtime. Registers runtime events for "DISCORD_WORLD_JOINED" and "DISCORD_SERVER_CONNECTED".
 *
 * @param {Object} param - Object containing runtime, config, actions, providers, and evaluators.
 * @param {IAgentRuntime} param.runtime - The runtime instance to use.
 * @param {OnboardingConfig} param.config - The configuration for onboarding.
 * @param {Action[]} [param.actions] - Optional array of actions to register.
 * @param {Provider[]} [param.providers] - Optional array of providers to register.
 * @param {Evaluator[]} [param.evaluators] - Optional array of evaluators to register.
 */

    await initCharacter({ runtime });

      // get absolute path of workspaceRoot

      // check if any part of the path includes eliza/packages

                // TODO: addKnowledge method not available in current IAgentRuntime interface
                // await runtime.addKnowledge(
                //   knowledgeItem,
                //   defaultKnowledgeOptions,
                // );


} // namespace elizaos
