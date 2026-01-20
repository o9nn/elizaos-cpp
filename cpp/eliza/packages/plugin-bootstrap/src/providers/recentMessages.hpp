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
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Move getRecentInteractions outside the provider
/**
 * Retrieves the recent interactions between two entities in a specific context.
 *
 * @param {IAgentRuntime} runtime - The agent runtime object.
 * @param {UUID} sourceEntityId - The UUID of the source entity.
 * @param {UUID} targetEntityId - The UUID of the target entity.
 * @param {UUID} excludeRoomId - The UUID of the room to exclude from the search.
 * @returns {Promise<Memory[]>} A promise that resolves to an array of Memory objects representing recent interactions.
 */
/**
 * Retrieves the recent interactions between two entities in different rooms excluding a specific room.
 * @param {IAgentRuntime} runtime - The agent runtime object.
 * @param {UUID} sourceEntityId - The UUID of the source entity.
 * @param {UUID} targetEntityId - The UUID of the target entity.
 * @param {UUID} excludeRoomId - The UUID of the room to exclude from the search.
 * @returns {Promise<Memory[]>} An array of Memory objects representing recent interactions between the two entities.
 */
  // Find all rooms where sourceEntityId and targetEntityId are participants

  // Check the existing memories in the database
    // filter out the current room id from rooms

/**
 * A provider object that retrieves recent messages, interactions, and memories based on a given message.
 * @typedef {object} Provider
 * @property {string} name - The name of the provider ("RECENT_MESSAGES").
 * @property {string} description - A description of the provider's purpose ("Recent messages, interactions and other memories").
 * @property {number} position - The position of the provider (100).
 * @property {Function} get - Asynchronous function that retrieves recent messages, interactions, and memories.
 * @param {IAgentRuntime} runtime - The runtime context for the agent.
 * @param {Memory} message - The message to retrieve data from.
 * @returns {object} An object containing data, values, and text sections.
 */

      // Parallelize initial data fetching operations including recentInteractions

      // Default to message format if room is not found or type is undefined

      // Format recent messages and posts in parallel

      // Create formatted text with headers

      // If there are no messages at all, and no current message to process, return a specific message.

      // Preload all necessary entities for both types of interactions

      // Only proceed if there are interactions to process
        // Get unique entity IDs that aren't the runtime agent

        // Create a Set for faster lookup

        // Add entities already fetched in entitiesData to the map

        // Get the remaining entities that weren't already loaded
        // Use Set difference for efficient filtering

        // Only fetch the entities we don't already have

      // Format recent message interactions
        // Format messages using the pre-fetched entities

      // Format recent post interactions
        // Combine pre-loaded entities with any other entities

        // Add entities from interactionEntityMap that aren't already in entities

      // Process both types of interactions in parallel

      // Combine all text sections
        // Only add received message and focus headers if there are messages or a current message to process

      // Return a default state in case of error, similar to the empty message list

} // namespace elizaos
