#include "memory.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

MessageMemory createMessageMemory(std::optional<{
  id: UUID;
  entityId: UUID;
  agentId: UUID;
  roomId: UUID;
  content: Content> params) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        ...params,
        createdAt: Date.now(),
        metadata: {
            type: MemoryType.MESSAGE,
            timestamp: Date.now(),
            scope: params.agentId ? "private" : "shared",
            },
            };

}

metadata is DocumentMetadata isDocumentMetadata(MemoryMetadata metadata) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return metadata.type == MemoryType.DOCUMENT;

}

metadata is FragmentMetadata isFragmentMetadata(MemoryMetadata metadata) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return metadata.type == MemoryType.FRAGMENT;

}

metadata is MessageMetadata isMessageMetadata(MemoryMetadata metadata) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return metadata.type == MemoryType.MESSAGE;

}

metadata is DescriptionMetadata isDescriptionMetadata(MemoryMetadata metadata) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return metadata.type == MemoryType.DESCRIPTION;

}

metadata is CustomMetadata isCustomMetadata(MemoryMetadata metadata) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    metadata.type != MemoryType.DOCUMENT &&;
    metadata.type != MemoryType.FRAGMENT &&;
    metadata.type != MemoryType.MESSAGE &&;
    metadata.type != MemoryType.DESCRIPTION;
    );

}

memory is Memory & isDocumentMemory(Memory memory) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    metadata: DocumentMetadata
}

memory is Memory & isFragmentMemory(Memory memory) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    metadata: FragmentMetadata
}

std::string getMemoryText(Memory memory, auto defaultValue) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return memory.content.text || defaultValue;

}

} // namespace elizaos
