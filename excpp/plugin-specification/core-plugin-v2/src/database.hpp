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
 * Database adapter class to be extended by individual database adapters.
 *
 * @template DB - The type of the database instance.
 * @abstract
 * @implements {IDatabaseAdapter}
 */
  /**
   * The database instance.
   */

  /**
   * Creates a new DatabaseAdapter instance.
   * @param db The database instance to use.
   */

  /**
   * Initialize the database adapter.
   * @returns A Promise that resolves when initialization is complete.
   */

  /**
   * Optional close method for the database adapter.
   * @returns A Promise that resolves when closing is complete.
   */

  /**
   * Retrieves a connection to the database.
   * @returns A Promise that resolves to the database connection.
   */

  /**
   * Retrieves an account by its ID.
   * @param entityIds The UUIDs of the user account to retrieve.
   * @returns A Promise that resolves to the Entity object or null if not found.
   */

  /**
   * Creates a new entities in the database.
   * @param entities The entity objects to create.
   * @returns A Promise that resolves when the account creation is complete.
   */

  /**
   * Updates an existing entity in the database.
   * @param entity The entity object with updated properties.
   * @returns A Promise that resolves when the account update is complete.
   */

  /**
   * Retrieves a single component by entity ID and type.
   * @param entityId The UUID of the entity the component belongs to
   * @param type The type identifier for the component
   * @param worldId Optional UUID of the world the component belongs to
   * @param sourceEntityId Optional UUID of the source entity
   * @returns Promise resolving to the Component if found, null otherwise
   */

  /**
   * Retrieves all components for an entity.
   * @param entityId The UUID of the entity to get components for
   * @param worldId Optional UUID of the world to filter components by
   * @param sourceEntityId Optional UUID of the source entity to filter by
   * @returns Promise resolving to array of Component objects
   */

  /**
   * Creates a new component in the database.
   * @param component The component object to create
   * @returns Promise resolving to true if creation was successful
   */

  /**
   * Updates an existing component in the database.
   * @param component The component object with updated properties
   * @returns Promise that resolves when the update is complete
   */

  /**
   * Deletes a component from the database.
   * @param componentId The UUID of the component to delete
   * @returns Promise that resolves when the deletion is complete
   */

  /**
   * Retrieves memories based on the specified parameters.
   * @param params An object containing parameters for the memory retrieval.
   * @returns A Promise that resolves to an array of Memory objects.
   */

  /**
   * Retrieves multiple memories by their IDs
   * @param memoryIds Array of UUIDs of the memories to retrieve
   * @param tableName Optional table name to filter memories by type
   * @returns Promise resolving to array of Memory objects
   */

  /**
   * Retrieves group chat memories from all rooms under a given server.
   * It fetches all room IDs associated with the `serverId`, then retrieves memories
   * from those rooms in descending order (latest to oldest), with an optional count limit.
   *
   * @param params - An object containing:
   *   - serverId: The server ID to fetch memories for.
   *   - count: (Optional) The maximum number of memories to retrieve.
   * @returns A promise that resolves to an array of Memory objects.
   */

  /**
   * Retrieves cached embeddings based on the specified query parameters.
   * @param params An object containing parameters for the embedding retrieval.
   * @returns A Promise that resolves to an array of objects containing embeddings and levenshtein scores.
   */

  /**
   * Logs an event or action with the specified details.
   * @param params An object containing parameters for the log entry.
   * @returns A Promise that resolves when the log entry has been saved.
   */

  /**
   * Retrieves logs based on the specified parameters.
   * @param params An object containing parameters for the log retrieval.
   * @returns A Promise that resolves to an array of Log objects.
   */

  /**
   * Deletes a log from the database.
   * @param logId The UUID of the log to delete.
   * @returns A Promise that resolves when the log has been deleted.
   */

  /**
   * Searches for memories based on embeddings and other specified parameters.
   * @param params An object containing parameters for the memory search.
   * @returns A Promise that resolves to an array of Memory objects.
   */

  /**
   * Creates a new memory in the database.
   * @param memory The memory object to create.
   * @param tableName The table where the memory should be stored.
   * @param unique Indicates if the memory should be unique.
   * @returns A Promise that resolves when the memory has been created.
   */

  /**
   * Updates an existing memory in the database.
   * @param memory The memory object with updated content and optional embedding
   * @returns Promise resolving to boolean indicating success
   */

  /**
   * Removes a specific memory from the database.
   * @param memoryId The UUID of the memory to remove.
   * @returns A Promise that resolves when the memory has been removed.
   */

  /**
   * Removes all memories associated with a specific room.
   * @param roomId The UUID of the room whose memories should be removed.
   * @param tableName The table from which the memories should be removed.
   * @returns A Promise that resolves when all memories have been removed.
   */

  /**
   * Counts the number of memories in a specific room.
   * @param roomId The UUID of the room for which to count memories.
   * @param unique Specifies whether to count only unique memories.
   * @param tableName Optional table name to count memories from.
   * @returns A Promise that resolves to the number of memories.
   */

  /**
   * Retrieves a world by its ID.
   * @param id The UUID of the world to retrieve.
   * @returns A Promise that resolves to the World object or null if not found.
   */

  /**
   * Retrieves all worlds for an agent.
   * @returns A Promise that resolves to an array of World objects.
   */

  /**
   * Creates a new world in the database.
   * @param world The world object to create.
   * @returns A Promise that resolves to the UUID of the created world.
   */

  /**
   * Updates an existing world in the database.
   * @param world The world object with updated properties.
   * @returns A Promise that resolves when the world has been updated.
   */

  /**
   * Removes a specific world from the database.
   * @param id The UUID of the world to remove.
   * @returns A Promise that resolves when the world has been removed.
   */

  /**
   * Retrieves the room ID for a given room, if it exists.
   * @param roomId The UUID of the room to retrieve.
   * @returns A Promise that resolves to the room ID or null if not found.
   */

  /**
   * Retrieves all rooms for a given world.
   * @param worldId The UUID of the world to retrieve rooms for.
   * @returns A Promise that resolves to an array of Room objects.
   */

  /**
   * Creates a new rooms with an optional specified ID.
   * @param roomId Optional UUID to assign to the new room.
   * @returns A Promise that resolves to the UUID of the created rooms.
   */

  /**
   * Updates a specific room in the database.
   * @param room The room object with updated properties.
   * @returns A Promise that resolves when the room has been updated.
   */

  /**
   * Removes a specific room from the database.
   * @param roomId The UUID of the room to remove.
   * @returns A Promise that resolves when the room has been removed.
   */

  /**
   * Retrieves room IDs for which a specific user is a participant.
   * @param entityId The UUID of the user.
   * @returns A Promise that resolves to an array of room IDs.
   */

  /**
   * Retrieves room IDs for which specific users are participants.
   * @param userIds An array of UUIDs of the users.
   * @returns A Promise that resolves to an array of room IDs.
   */

  /**
   * Adds users as a participant to a specific room.
   * @param entityIds The UUIDs of the users to add as a participant.
   * @param roomId The UUID of the room to which the user will be added.
   * @returns A Promise that resolves to a boolean indicating success or failure.
   */

  /**
   * Removes a user as a participant from a specific room.
   * @param entityId The UUID of the user to remove as a participant.
   * @param roomId The UUID of the room from which the user will be removed.
   * @returns A Promise that resolves to a boolean indicating success or failure.
   */

  /**
   * Retrieves participants associated with a specific account.
   * @param entityId The UUID of the account.
   * @returns A Promise that resolves to an array of Participant objects.
   */

  /**
   * Retrieves participants for a specific room.
   * @param roomId The UUID of the room for which to retrieve participants.
   * @returns A Promise that resolves to an array of UUIDs representing the participants.
   */

  /**
   * Creates a new relationship between two users.
   * @param params Object containing the relationship details including entity IDs, agent ID, optional tags and metadata
   * @returns A Promise that resolves to a boolean indicating success or failure of the creation.
   */

  /**
   * Retrieves a relationship between two users if it exists.
   * @param params Object containing the entity IDs and agent ID
   * @returns A Promise that resolves to the Relationship object or null if not found.
   */

  /**
   * Retrieves all relationships for a specific user.
   * @param params Object containing the user ID, agent ID and optional tags to filter by
   * @returns A Promise that resolves to an array of Relationship objects.
   */

  /**
   * Updates an existing relationship between two users.
   * @param params Object containing the relationship details to update including entity IDs, agent ID, optional tags and metadata
   * @returns A Promise that resolves to a boolean indicating success or failure of the update.
   */

  /**
   * Retrieves an agent by its ID.
   * @param agentId The UUID of the agent to retrieve.
   * @returns A Promise that resolves to the Agent object or null if not found.
   */

  /**
   * Retrieves all agents from the database.
   * @returns A Promise that resolves to an array of Agent objects.
   */

  /**
   * Creates a new agent in the database.
   * @param agent The agent object to create.
   * @returns A Promise that resolves to a boolean indicating success or failure of the creation.
   */

  /**
   * Updates an existing agent in the database.
   * @param agentId The UUID of the agent to update.
   * @param agent The agent object with updated properties.
   * @returns A Promise that resolves to a boolean indicating success or failure of the update.
   */

  /**
   * Deletes an agent from the database.
   * @param agentId The UUID of the agent to delete.
   * @returns A Promise that resolves to a boolean indicating success or failure of the deletion.
   */

  /**
   * Ensures an agent exists in the database.
   * @param agent The agent object to ensure exists.
   * @returns A Promise that resolves when the agent has been ensured to exist.
   */

  /**
   * Ensures an embedding dimension exists in the database.
   * @param dimension The dimension to ensure exists.
   * @returns A Promise that resolves when the embedding dimension has been ensured to exist.
   */

  /**
   * Retrieves a cached value by key from the database.
   * @param key The key to look up in the cache
   * @returns Promise resolving to the cached string value
   */

  /**
   * Sets a value in the cache with the given key.
   * @param params Object containing the cache key and value
   * @param key The key to store the value under
   * @param value The string value to cache
   * @returns Promise resolving to true if the cache was set successfully
   */

  /**
   * Deletes a value from the cache by key.
   * @param key The key to delete from the cache
   * @returns Promise resolving to true if the value was successfully deleted
   */

  /**
   * Creates a new task instance in the database.
   * @param task The task object to create
   * @returns Promise resolving to the UUID of the created task
   */

  /**
   * Retrieves tasks based on specified parameters.
   * @param params Object containing optional roomId and tags to filter tasks
   * @returns Promise resolving to an array of Task objects
   */

  /**
   * Retrieves a specific task by its ID.
   * @param id The UUID of the task to retrieve
   * @returns Promise resolving to the Task object if found, null otherwise
   */

  /**
   * Retrieves a specific task by its name.
   * @param name The name of the task to retrieve
   * @returns Promise resolving to the Task object if found, null otherwise
   */

  /**
   * Updates an existing task in the database.
   * @param id The UUID of the task to update
   * @param task Partial Task object containing the fields to update
   * @returns Promise resolving when the update is complete
   */

  /**
   * Deletes a task from the database.
   * @param id The UUID of the task to delete
   * @returns Promise resolving when the deletion is complete
   */


} // namespace elizaos
