#include "types.hpp"
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
 * Knowledge and memory management adapters for V1 compatibility
 * These provide conversion between V1 knowledge structures and V2 memory structures
 */

/**
 * Converts V1 KnowledgeItem to V2 Memory format
 * V1 KnowledgeItem has id and content, V2 Memory has more fields
 */
Memory knowledgeItemToMemory(KnowledgeItem item, UUID agentId, UUID roomId);

/**
 * Converts V2 Memory to V1 KnowledgeItem format
 * Extracts the essential knowledge information from memory
 */
KnowledgeItem memoryToKnowledgeItem(Memory memory);

/**
 * Converts V1 RAGKnowledgeItem to V2 Memory format
 * RAG knowledge items have additional embedding and metadata
 */
Memory ragKnowledgeToMemory(RAGKnowledgeItem ragItem, UUID roomId);

/**
 * Converts V2 Memory to V1 RAGKnowledgeItem format
 * Reconstructs RAG knowledge structure from memory
 */
RAGKnowledgeItem memoryToRagKnowledge(Memory memory);

/**
 * Memory table name mapping for different types of knowledge
 * V1 uses specific table names, V2 uses a more generic approach
 */

/**
 * Utility functions for knowledge scope and metadata handling
 */

/**
 * Determines if a knowledge item should be shared based on V1 conventions
 */
bool isSharedKnowledge(KnowledgeItem | RAGKnowledgeItem item);

/**
 * Creates appropriate metadata for knowledge items based on V1 patterns
 */
void createKnowledgeMetadata(std::optional<std::string> source, std::optional<bool> isShared);

/**
 * Filters knowledge items by scope (shared/private)
 */

/**
 * Converts knowledge search parameters from V1 to V2 format
 */

/**
 * Batch converts an array of KnowledgeItems to Memories
 */
std::vector<Memory> knowledgeItemsToMemories(const std::vector<KnowledgeItem>& items, UUID agentId, UUID roomId);

/**
 * Batch converts an array of Memories to KnowledgeItems
 */
std::vector<KnowledgeItem> memoriesToKnowledgeItems(const std::vector<Memory>& memories);

/**
 * Batch converts an array of RAGKnowledgeItems to Memories
 */
std::vector<Memory> ragKnowledgeItemsToMemories(const std::vector<RAGKnowledgeItem>& items, UUID roomId);

/**
 * Batch converts an array of Memories to RAGKnowledgeItems
 */
std::vector<RAGKnowledgeItem> memoriesToRagKnowledgeItems(const std::vector<Memory>& memories);
} // namespace elizaos
