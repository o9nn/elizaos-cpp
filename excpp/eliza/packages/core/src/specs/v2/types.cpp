#include "types.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

UUID asUUID(const std::string& id) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!id || !/^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$/i.test(id)) {
            throw std::runtime_error(`Invalid UUID format: ${id}`);
        }
        return id;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

MessageMemory createMessageMemory(std::optional<std::any> params) {
    // NOTE: Auto-converted from TypeScript - may need refinement

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

std::string getMemoryText(Memory memory, auto defaultValue = '') {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return memory.content.text || defaultValue;

}

ServiceError createServiceError(unknown error, auto code = 'UNKNOWN_ERROR') {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (error instanceof Error) {
        return {
            code,
            message: error.message,
            cause: error,
            };
        }

        return {
            code,
            message: std::to_string(error),
            };

}

} // namespace elizaos
