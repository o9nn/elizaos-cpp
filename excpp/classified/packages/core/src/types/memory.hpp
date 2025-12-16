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

import type { Content, UUID } from './primitives';

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
  DOCUMENT = 'document',
  FRAGMENT = 'fragment',
  MESSAGE = 'message',
  DESCRIPTION = 'description',
  CUSTOM = 'custom',
  ACTION = 'action',
  ACTION_RESULT = 'action_result',
  KNOWLEDGE = 'knowledge',
  EXPERIENCE = 'experience',
  REFLECTION = 'reflection',
}
/**
 * Defines the scope of a memory, indicating its visibility and accessibility.
 * - `shared`: The memory is accessible to multiple entities or across different contexts (e.g., a public fact).
 * - `private`: The memory is specific to a single entity or a private context (e.g., a user's personal preference).
 * - `room`: The memory is scoped to a specific room or channel.
 * This is used in `MemoryMetadata` to control how memories are stored and retrieved based on context.
 */
using MemoryScope = std::variant<'shared', 'private', 'room'>;

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
    MemoryType type;
    std::optional<std::string> source;
    std::optional<UUID> sourceId;
    std::optional<MemoryScope> scope;
    std::optional<double> timestamp;
    std::optional<std::vector<std::string>> tags;
};


interface DocumentMetadata extends BaseMetadata {
  type: MemoryType.DOCUMENT;

  source?: string;
  sourceId?: UUID;
  scope?: 'shared' | 'private' | 'room';
  timestamp?: number;
  tags?: string[];

  // Document-specific properties
  title?: string;
  filename?: string;
  originalFilename?: string;
  contentType?: string;
  fileType?: string; // MIME type
  fileExt?: string;
  fileSize?: number;
  size?: number;
  fragmentCount?: number;
  path?: string;
  documentId?: UUID;
  position?: number;
}

interface FragmentMetadata extends BaseMetadata {
  type: MemoryType.FRAGMENT;
  documentId: UUID;
  position: number;
}

interface MessageMetadata extends BaseMetadata {
  type: MemoryType.MESSAGE;
}

interface DescriptionMetadata extends BaseMetadata {
  type: MemoryType.DESCRIPTION;
}

interface CustomMetadata extends BaseMetadata {
  type: MemoryType.CUSTOM;
  [key: string]: unknown;
}

interface ActionMetadata extends BaseMetadata {
  type: MemoryType.ACTION;
  actionName: string;
  actionId: UUID;
  runId: UUID;
}

interface ActionResultMetadata extends BaseMetadata {
  type: MemoryType.ACTION_RESULT;
  actionName: string;
  actionId: UUID;
  runId: UUID;
  error?: boolean;
  totalSteps?: number;
  currentStep?: number;
}

interface KnowledgeMetadata extends BaseMetadata {
  type: MemoryType.KNOWLEDGE;
}

interface ExperienceMetadata extends BaseMetadata {
  type: MemoryType.EXPERIENCE;
}

interface ReflectionMetadata extends BaseMetadata {
  type: MemoryType.REFLECTION;
}

using MemoryMetadata = std::variant<, DocumentMetadata, FragmentMetadata, MessageMetadata, DescriptionMetadata, CustomMetadata, ActionMetadata, ActionResultMetadata, KnowledgeMetadata, ExperienceMetadata, ReflectionMetadata>;

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
interface MessageMemory extends Memory {
  metadata: MessageMetadata;
  content: Content & {
    text: string; // Message memories must have text content
  };
}

/**
 * Factory ;
  embedding?: number[];
}): MessageMemory {
  return {
    ...params,
    createdAt: Date.now(),
    metadata: {
      type: MemoryType.MESSAGE,
      timestamp: Date.now(),
      scope: params.agentId ? 'private' : 'shared',
    },
  };
}

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
 * Type guard to check if a memory metadata is an ActionMetadata
 * @param metadata The metadata to check
 * @returns True if the metadata is an ActionMetadata
 */


/**
 * Type guard to check if a memory metadata is an ActionResultMetadata
 * @param metadata The metadata to check
 * @returns True if the metadata is an ActionResultMetadata
 */


/**
 * Type guard to check if a memory metadata is a KnowledgeMetadata
 * @param metadata The metadata to check
 * @returns True if the metadata is a KnowledgeMetadata
 */


/**
 * Type guard to check if a memory metadata is an ExperienceMetadata
 * @param metadata The metadata to check
 * @returns True if the metadata is an ExperienceMetadata
 */


/**
 * Type guard to check if a memory metadata is a ReflectionMetadata
 * @param metadata The metadata to check
 * @returns True if the metadata is a ReflectionMetadata
 */


/**
 * Memory type guard for document memories
 */
 {
  return memory.metadata?.type === MemoryType.DOCUMENT;
}

/**
 * Memory type guard for fragment memories
 */
 {
  return memory.metadata?.type === MemoryType.FRAGMENT;
}

/**
 * Memory type guard for action memories
 */
 {
  return memory.metadata?.type === MemoryType.ACTION;
}

/**
 * Memory type guard for action result memories
 */
 {
  return memory.metadata?.type === MemoryType.ACTION_RESULT;
}

/**
 * Memory type guard for knowledge memories
 */
 {
  return memory.metadata?.type === MemoryType.KNOWLEDGE;
}

/**
 * Memory type guard for experience memories
 */
 {
  return memory.metadata?.type === MemoryType.EXPERIENCE;
}

/**
 * Memory type guard for reflection memories
 */
 {
  return memory.metadata?.type === MemoryType.REFLECTION;
}

/**
 * Safely access the text content of a memory
 * @param memory The memory to extract text from
 * @param defaultValue Optional default value if no text is found
 * @returns The text content or default value
 */


} // namespace elizaos
