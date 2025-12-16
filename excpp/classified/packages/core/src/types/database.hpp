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

import type { Agent } from './agent';
import type { Component, Entity, Participant, Relationship, Room, World } from './environment';
import type { Memory, MemoryMetadata } from './memory';
import type { Metadata, UUID } from './primitives';
import type { Task } from './task';

/**
 * Represents a log entry
 */
struct Log {
    std::optional<UUID> id;
    UUID entityId;
    std::optional<UUID> roomId;
    { [key: string]: unknown } body;
    std::string type;
    Date createdAt;
};


/**
 * Interface for database operations
 */
struct IDatabaseAdapter {
    std::any db;
    UUID entityId;
    std::string type;
    std::optional<UUID> worldId;
    std::optional<UUID> sourceEntityId;
    std::optional<UUID> entityId;
    std::optional<UUID> agentId;
    std::optional<double> count;
    std::optional<bool> unique;
    std::string tableName;
    std::optional<double> start;
    std::optional<double> end;
    std::optional<UUID> roomId;
    std::optional<UUID> worldId;
    std::string tableName;
    std::vector<UUID> roomIds;
    std::optional<double> limit;
    std::string query_table_name;
    double query_threshold;
    std::string query_input;
    std::string query_field_name;
    std::string query_field_sub_name;
    double query_match_count;
    { [key: string]: unknown } body;
    UUID entityId;
    UUID roomId;
    std::string type;
    UUID entityId;
    std::optional<UUID> roomId;
    std::optional<std::string> type;
    std::optional<double> count;
    std::optional<double> offset;
    std::vector<double> embedding;
    std::optional<double> match_threshold;
    std::optional<double> count;
    std::optional<bool> unique;
    std::string tableName;
    std::optional<std::string> query;
    std::optional<UUID> roomId;
    std::optional<UUID> worldId;
    std::optional<UUID> entityId;
    UUID roomId;
    UUID entityId;
    'FOLLOWED' | 'MUTED' | null state;
    UUID sourceEntityId;
    UUID targetEntityId;
    std::optional<std::vector<std::string>> tags;
    std::optional<Metadata> metadata;
    UUID sourceEntityId;
    UUID targetEntityId;
    UUID worldId;
    std::optional<double> count;
    std::optional<std::string> tableName;
};


/**
 * Result interface for embedding similarity searches
 */
struct EmbeddingSearchResult {
    std::vector<double> embedding;
    double levenshtein_score;
};


/**
 * Options for memory retrieval operations
 */
struct MemoryRetrievalOptions {
    UUID roomId;
    std::optional<double> count;
    std::optional<bool> unique;
    std::optional<double> start;
    std::optional<double> end;
    std::optional<UUID> agentId;
};


/**
 * Options for memory search operations
 */
struct MemorySearchOptions {
    std::vector<double> embedding;
    std::optional<double> match_threshold;
    std::optional<double> count;
    UUID roomId;
    std::optional<UUID> agentId;
    std::optional<bool> unique;
    std::optional<std::optional<MemoryMetadata>> metadata;
};


/**
 * Options for multi-room memory retrieval
 */
struct MultiRoomMemoryOptions {
    std::vector<UUID> roomIds;
    std::optional<double> limit;
    std::optional<UUID> agentId;
};


/**
 * Unified options pattern for memory operations
 * Provides a simpler, more consistent interface
 */
struct UnifiedMemoryOptions {
    UUID roomId;
    std::optional<number; // Unified naming (replacing 'count')> limit;
    std::optional<UUID; // Common optional parameter> agentId;
    std::optional<boolean; // Common flag for duplication control> unique;
    std::optional<number; // Pagination start> start;
    std::optional<number; // Pagination end> end;
};


/**
 * Specialized memory search options
 */
interface UnifiedSearchOptions extends UnifiedMemoryOptions {
  embedding: number[];
  similarity?: number; // Clearer name than 'match_threshold'
}

/**
 * Represents a generic database connection object.
 * The actual type of this connection will depend on the specific database adapter implementation
 * (e.g., a connection pool object for PostgreSQL, a client instance for a NoSQL database).
 * This `unknown` type serves as a placeholder in the abstract `IDatabaseAdapter`.
 */
using DbConnection = unknown;

// Allowable vector dimensions
const VECTOR_DIMS = {
  SMALL: 384,
  MEDIUM: 512,
  LARGE: 768,
  XL: 1024,
  XXL: 1536,
  XXXL: 3072,
} as const;

} // namespace elizaos
