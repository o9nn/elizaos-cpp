#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Represents a log entry
 */
struct Log {
    std::optional<UUID> id;
    UUID entityId;
    std::optional<UUID> roomId;
    std::string type;
    Date createdAt;
};

/**
 * Interface for database operations
 */
struct IDatabaseAdapter {
    std::any db;
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
    std::string state;
    UUID sourceEntityId;
    UUID targetEntityId;
    std::optional<std::vector<std::string>> tags;
    std::optional<Metadata> metadata;
    UUID sourceEntityId;
    UUID targetEntityId;
    std::optional<std::vector<std::string>> tags;
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
    std::optional<double> limit;
    std::optional<UUID> agentId;
    std::optional<bool> unique;
    std::optional<double> start;
    std::optional<double> end;
};

/**
 * Specialized memory search options
 */

/**
 * Represents a generic database connection object.
 * The actual type of this connection will depend on the specific database adapter implementation
 * (e.g., a connection pool object for PostgreSQL, a client instance for a NoSQL database).
 * This `unknown` type serves as a placeholder in the abstract `IDatabaseAdapter`.
 */
using DbConnection = unknown;

// Allowable vector dimensions

} // namespace elizaos
