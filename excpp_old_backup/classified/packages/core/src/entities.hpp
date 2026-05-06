#include "index.hpp"
#include "logger.hpp"
#include "types.hpp"
#include "utils.hpp"
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
 * Template for resolving entity name within a conversation context.
 *
 * @type {string}
 */
/**
 * Entity Resolution Template for resolving entity names based on context and recent messages.
 *
 * Contains placeholders for message sender, agent, entities in the room, and recent messages.
 * Provides instructions for analyzing the context and resolving entity references.
 *
 * @return {string} entityResolutionTemplate - The template for resolving entity names with detailed instructions.
 */

/**
 * Get recent interactions between a source entity and candidate entities in a specific room.
 *
 * @param {IAgentRuntime} runtime - The runtime context for the agent.
 * @param {UUID} sourceEntityId - The ID of the source entity initiating interactions.
 * @param {Entity[]} candidateEntities - The list of candidate entities to evaluate interactions with.
 * @param {UUID} roomId - The ID of the room where interactions are taking place.
 * @param {Relationship[]} relationships - The relationships between the entities involved.
 * @returns {Promise<{ entity: Entity; interactions: Memory[]; count: number }[]>} - An array of objects containing the entity, recent interactions, and interaction count.
 */

  // Get recent messages from the room - just for context

    // First get direct replies using inReplyTo

    // Get relationship strength from metadata

    // Add bonus points for recent direct replies

    // Keep last few messages for context

  // Sort by interaction score descending

/**
 * Finds an entity by name in the given runtime environment.
 *
 * @param {IAgentRuntime} runtime - The agent runtime environment.
 * @param {Memory} message - The memory message containing relevant information.
 * @param {State} state - The current state of the system.
 * @returns {Promise<Entity | null>} A promise that resolves to the found entity or null if not found.
 */
std::future<Entity | null> findEntityByName(IAgentRuntime runtime, Memory message, State state);

/**
 * Function to create a unique UUID based on the runtime and base user ID.
 *
 * @param {RuntimeContext} runtime - The runtime context object.
 * @param {UUID|string} baseUserId - The base user ID to use in generating the UUID.
 * @returns {UUID} - The unique UUID generated based on the runtime and base user ID.
 */
  // If the base user ID is the agent ID, return it directly

  // Use a deterministic approach to generate a new UUID based on both IDs
  // This creates a unique ID for each user+agent combination while still being deterministic

  // Create a namespace UUID (version 5) from the combined string
  return stringToUuid(combinedString);

/**
 * Get details for a list of entities.
 */
/**
 * Retrieves entity details for a specific room from the database.
 *
 * @param {Object} params - The input parameters
 * @param {IAgentRuntime} params.runtime - The Agent Runtime instance
 * @param {UUID} params.roomId - The ID of the room to retrieve entity details for
 * @returns {Promise<Array>} - A promise that resolves to an array of unique entity details
 */

/**
 * Format entities into a string
 * @param entities - list of entities
 * @returns string
 */
/**
 * Format the given entities into a string representation.
 *
 * @param {Object} options - The options object.
 * @param {Entity[]} options.entities - The list of entities to format.
 * @returns {string} A formatted string representing the entities.
 */
void formatEntities({ entities: Entity[] } { entities });

} // namespace elizaos
