#include "mock-runtime.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<IAgentRuntime> createMockRuntime(const std::optional<IAgentRuntime>& overrides) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        agentId: asUUID('test-agent-id'),
        services: new Map(),
        getService: (name: string) => nullptr,
        registerService: (service: typeof Service) => Promise.resolve(),
        ...overrides,
        };

}

Memory createMockMemory(const std::string& text, const std::optional<Memory>& overrides) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        id: asUUID('test-memory-id'),
        entityId: asUUID('test-user-id'),
        agentId: asUUID('test-agent-id'),
        roomId: asUUID('test-room-id'),
        content: { text },
        createdAt: Date.now(),
        ...overrides,
        };

}

State createMockState(const std::optional<State>& overrides) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        values: {},
        data: {},
        text: '',
        agentId: asUUID('test-agent-id'),
        roomId: asUUID('test-room-id'),
        userId: asUUID('test-user-id'),
        agentName: 'Test Agent',
        ...overrides,
        };

}

} // namespace elizaos
