#include "knowledge.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

Memory knowledgeItemToMemory(KnowledgeItem item, UUID agentId, UUID roomId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        id: item.id,
        entityId: agentId, // V2 uses entityId instead of userId
        agentId: agentId,
        content: item.content,
        roomId: roomId,
        createdAt: Date.now(),
        unique: false,
        };

}

KnowledgeItem memoryToKnowledgeItem(Memory memory) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        id: memory.id!,
        content: memory.content,
        };

}

Memory ragKnowledgeToMemory(RAGKnowledgeItem ragItem, UUID roomId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        id: ragItem.id,
        entityId: ragItem.agentId, // Map agentId to entityId
        agentId: ragItem.agentId,
        content: {
            text: ragItem.content.text,
            ...ragItem.content.metadata,
            },
            embedding: ragItem.embedding ? Array.from(ragItem.embedding) : std::nullopt,
            roomId: roomId,
            createdAt: ragItem.createdAt || Date.now(),
            similarity: ragItem.similarity,
            unique: false,
            };

}

RAGKnowledgeItem memoryToRagKnowledge(Memory memory) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        id: memory.id!,
        agentId: memory.agentId!,
        content: {
            text: memory.content.text || "",
            metadata: {
                // Extract metadata from content
                ...Object.keys(memory.content);
                .filter(key => key != "text");
                .reduce((meta, key) => ({
                    ...meta,
                    [key]: memory.content[key]
                    }), {}),
                    source: memory.content.source,
                    type: "knowledge",
                }
                },
                embedding: memory.embedding ? new Float32Array(memory.embedding) : std::nullopt,
                createdAt: memory.createdAt,
                similarity: memory.similarity,
                };

}

bool isSharedKnowledge(const std::variant<KnowledgeItem, RAGKnowledgeItem>& item) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if ('content' in item && typeof item.content == 'object') {
        // Check RAGKnowledgeItem metadata
        if ('metadata' in item.content && item.content.metadata.isShared == true) {
            return true;
        }
    }

    // Check general content for shared indicators
    if (item.content && typeof item.content == 'object' && 'shared' in item.content) {
        return item.content.shared == true;
    }

    return false;

}

void createKnowledgeMetadata(std::optional<std::string> source, std::optional<bool> isShared) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        type: "knowledge",
        source: source || "unknown",
        isShared: isShared || false,
        timestamp: Date.now(),
        };

}

void adaptKnowledgeSearchParams(std::optional<std::any> params) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        entityId: params.agentId, // V2 uses entityId
        agentId: params.agentId,
        embedding: params.embedding ? Array.from(params.embedding) : [],
        match_threshold: params.match_threshold || 0.8,
        count: params.match_count || params.limit || 10,
        query: params.query || params.searchText,
        tableName: KNOWLEDGE_TABLE_NAMES.KNOWLEDGE,
        };

}

std::vector<Memory> knowledgeItemsToMemories(const std::vector<KnowledgeItem>& items, UUID agentId, UUID roomId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return items.map(item => knowledgeItemToMemory(item, agentId, roomId));

}

std::vector<KnowledgeItem> memoriesToKnowledgeItems(const std::vector<Memory>& memories) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return memories.map(memory => memoryToKnowledgeItem(memory));

}

std::vector<Memory> ragKnowledgeItemsToMemories(const std::vector<RAGKnowledgeItem>& items, UUID roomId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return items.map(item => ragKnowledgeToMemory(item, roomId));

}

std::vector<RAGKnowledgeItem> memoriesToRagKnowledgeItems(const std::vector<Memory>& memories) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return memories.map(memory => memoryToRagKnowledge(memory));

}

} // namespace elizaos
