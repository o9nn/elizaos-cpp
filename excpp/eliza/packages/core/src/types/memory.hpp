#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Memory type enumeration for built-in memory types
 */
using MemoryTypeAlias = std::string;

/**
 * Enumerates the built-in types of memories that can be stored and retrieved.
 * - `DOCUMENT`: Represents a whole document or a large piece of text.
 * - `FRAGMENT`: A chunk or segment of a `DOCUMENT`, often created for embedding and search.
 * - `MESSAGE`: A conversational message, typically from a user or the agent.
 * - `DESCRIPTION`: A descriptive piece of information, perhaps about an entity or concept.
 * - `CUSTOM`: For any other type of memory not covered by the built-in types.
 * This enum is used in `MemoryMetadata` to categorize memories and influences how they are processed or queried.
 */
enum MemoryType {
}
/**
 * Defines the scope of a memory, indicating its visibility and accessibility.
 * - `shared`: The memory is accessible to multiple entities or across different contexts (e.g., a public fact).
 * - `private`: The memory is specific to a single entity or a private context (e.g., a user's personal preference).
 * - `room`: The memory is scoped to a specific room or channel.
 * This is used in `MemoryMetadata` to control how memories are stored and retrieved based on context.
 */
using MemoryScope = std::string;

/**
 * Base interface for all memory metadata types.
 * It includes common properties for all memories, such as:
 * - `type`: The kind of memory (e.g., `MemoryType.MESSAGE`, `MemoryType.DOCUMENT`).
 * - `source`: An optional string indicating the origin of the memory (e.g., 'discord', 'user_input').
 * - `sourceId`: An optional UUID linking to a source entity or object.
 * - `scope`: The visibility scope of the memory (`shared`, `private`, or `room`).
 * - `timestamp`: An optional numerical timestamp (e.g., milliseconds since epoch) of when the memory was created or relevant.
 * - `tags`: Optional array of strings for categorizing or filtering memories.
 * Specific metadata types like `DocumentMetadata` or `MessageMetadata` extend this base.
 */
struct BaseMetadata {
    MemoryTypeAlias type;
    std::optional<std::string> source;
    std::optional<UUID> sourceId;
    std::optional<MemoryScope> scope;
    std::optional<double> timestamp;
    std::optional<std::vector<std::string>> tags;
};

using MemoryMetadata = std::variant<, DocumentMetadata, FragmentMetadata, MessageMetadata, DescriptionMetadata, CustomMetadata>;

/**
 * Represents a stored memory/message
 */
struct Memory {
    std::optional<UUID> id;
    UUID entityId;
    std::optional<UUID> agentId;
    std::optional<double> createdAt;
    Content content;
    std::optional<std::vector<double>> embedding;
    UUID roomId;
    std::optional<UUID> worldId;
    std::optional<bool> unique;
    std::optional<double> similarity;
    std::optional<MemoryMetadata> metadata;
};

/**
 * Specialized memory type for messages with enhanced type checking
 */

/**
 * Factory function to create a new message memory with proper defaults
 */

/**
 * Type guard to check if a memory metadata is a DocumentMetadata
 * @param metadata The metadata to check
 * @returns True if the metadata is a DocumentMetadata
 */

/**
 * Type guard to check if a memory metadata is a FragmentMetadata
 * @param metadata The metadata to check
 * @returns True if the metadata is a FragmentMetadata
 */

/**
 * Type guard to check if a memory metadata is a MessageMetadata
 * @param metadata The metadata to check
 * @returns True if the metadata is a MessageMetadata
 */

/**
 * Type guard to check if a memory metadata is a DescriptionMetadata
 * @param metadata The metadata to check
 * @returns True if the metadata is a DescriptionMetadata
 */

/**
 * Type guard to check if a memory metadata is a CustomMetadata
 * @param metadata The metadata to check
 * @returns True if the metadata is a CustomMetadata
 */

/**
 * Memory type guard for document memories
 */

/**
 * Memory type guard for fragment memories
 */

/**
 * Safely access the text content of a memory
 * @param memory The memory to extract text from
 * @param defaultValue Optional default value if no text is found
 * @returns The text content or default value
 */
std::string getMemoryText(Memory memory, auto defaultValue);

} // namespace elizaos
