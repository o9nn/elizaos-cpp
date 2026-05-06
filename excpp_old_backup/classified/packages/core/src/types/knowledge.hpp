#include "memory.hpp"
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
 * Defines the scope or visibility of knowledge items within the agent's system.
 * - `SHARED`: Indicates knowledge that is broadly accessible, potentially across different agents or users if the system architecture permits.
 * - `PRIVATE`: Indicates knowledge that is restricted, typically to the specific agent or user context it belongs to.
 * This enum is used to manage access and retrieval of knowledge items, often in conjunction with `AgentRuntime.addKnowledge` or `AgentRuntime.getKnowledge` scopes.
 */
enum KnowledgeScope {
  SHARED = 'shared',
  PRIVATE = 'private',
}

/**
 * Specifies prefixes for keys used in caching mechanisms, helping to namespace cached data.
 * For example, `KNOWLEDGE` might be used to prefix keys for cached knowledge embeddings or processed documents.
 * This helps in organizing the cache and avoiding key collisions.
 * Used internally by caching strategies, potentially within `IDatabaseAdapter` cache methods or runtime caching layers.
 */
enum CacheKeyPrefix {
  KNOWLEDGE = 'knowledge',
}

/**
 * Represents an item within a directory listing, specifically for knowledge loading.
 * When an agent's `Character.knowledge` configuration includes a directory, this type
 * is used to specify the path to that directory and whether its contents should be treated as shared.
 * - `directory`: The path to the directory containing knowledge files.
 * - `shared`: An optional boolean (defaults to false) indicating if the knowledge from this directory is considered shared or private.
 */
struct DirectoryItem {
    std::string directory;
    std::optional<bool> shared;
};

/**
 * Represents a row structure, typically from a database query related to text chunking or processing.
 * This interface is quite minimal and seems to be a placeholder or a base for more specific chunk-related types.
 * The `id` would be the unique identifier for the chunk.
 * It might be used when splitting large documents into smaller, manageable pieces for embedding or analysis.
 */
struct ChunkRow {
    std::string id;
};

/**
 * Extended knowledge item that includes additional properties commonly used
 * in knowledge management systems like creation timestamps, embeddings, etc.
 * This maintains compatibility with the base KnowledgeItem while adding
 * commonly needed fields for storage and retrieval.
 */
  /** A Universally Unique Identifier for this specific knowledge item. */
  /** The actual content of the knowledge item, which must include text and can have other fields. */
  /** Optional metadata with extended document properties */
  /** Creation timestamp */
  /** Associated agent ID */
  /** Associated entity ID */
  /** Associated room ID */
  /** Associated world ID */
  /** Optional embedding vector for semantic search */
  /** Similarity score when retrieved via search */

/**
 * Service interface for knowledge management operations.
 * Provides methods for retrieving, storing, and searching knowledge items.
 */
struct IKnowledgeService {
    std::string tableName;
    double count;
    UUID agentId;
    UUID clientDocumentId;
    std::string content;
    std::string contentType;
    std::string originalFilename;
    UUID worldId;
    UUID roomId;
    UUID entityId;
    std::optional<UUID> agentId;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
    UUID clientDocumentId;
    UUID storedDocumentMemoryId;
    double fragmentCount;
    std::string query;
    UUID agentId;
    std::optional<double> limit;
};


} // namespace elizaos
