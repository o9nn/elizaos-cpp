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
#include "actions/quote.hpp"
#include "elizaos/core.hpp"
#include "providers/ai16z.hpp"
#include "providers/otcDesk.hpp"
#include "providers/providers.hpp"
#include "providers/quote.hpp"
#include "providers/recentMessages.hpp"
#include "providers/shaw.hpp"
#include "providers/token.hpp"
#include "services/quoteService.hpp"
#include "services/userSessionStorage.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Helper type for entity metadata from various sources (web, discord, etc.)
struct EntitySourceMetadata {
    std::optional<std::string> username;
    std::optional<std::string> name;
};

// Helper function to safely get entity metadata for a source
EntitySourceMetadata getEntitySourceMetadata(const std::optional<Entity>& entity, const std::string& source);

// Interface for websocket service with sendMessage capability
struct WebSocketServiceWithSendMessage {
    std::string type;
};

/**
 * Extracts the text content from within a <response> XML tag.
 * @param text The input string potentially containing the <response> tag.
 * @returns The extracted text content, or null if the tag is not found or empty.
 */
std::string extractResponseText(const std::string& text);

/**
 * Represents media data containing a buffer of data and the media type.
 * @typedef {Object} MediaData
 * @property {Buffer} data - The buffer of data.
 * @property {string} mediaType - The type of media.
 */
using MediaData = {

// Helper functions for response ID tracking in serverless environment
std::future<std::string> getLatestResponseId(IAgentRuntime runtime, const std::string& roomId);

std::future<void> setLatestResponseId(IAgentRuntime runtime, const std::string& roomId, const std::string& responseId);

std::future<void> clearLatestResponseId(IAgentRuntime runtime, const std::string& roomId);

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

  // Set this as the latest response ID for this room (using runtime cache for serverless)

  // Generate a unique run ID for tracking this message handler execution

  // Emit run started event

  // Set up timeout monitoring

    // First, save the incoming message

    // Retry if missing required fields

      // Attempt to parse the XML response

    // Check if this is still the latest response ID for this room

    // Clean up the response ID

    // Parse actions from response - support both XML tags and function-call syntax

    // Parse XML format: <action>CREATE_OTC_QUOTE</action>

    // Parse function-call format: CREATE_OTC_QUOTE({...})

    // Parse and save quote if present in response (don't trigger action handler)
      // Worst possible deal defaults (lowest discount, longest lockup)

      // Simple regex-based parsing (server-side compatible)

        // Add to indexes

    // Create response memory with parsed actions

    // Process actions if any were found

      // Process actions first, which will call the action handler

          // The action handler provides the actual response text

          // Save the response to database

          // Send to frontend
      // No actions - save and send the response

    // Emit run ended event on successful completion

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

      // Check for required fields

      // TypeScript should know these are defined now, but we'll use type assertions to be explicit

      // Update entity to inactive


} // namespace elizaos
