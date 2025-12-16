#include ".types.hpp"
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

;

/**
 * Simple mock runtime for tests - just use createMockRuntime() and cast to IAgentRuntime when needed
 */
): Partial<IAgentRuntime> {
  return {
    agentId: asUUID('test-agent-id'),
    services: new Map(),
    getService: (name: string) => null,
    registerService: (service: typeof Service) => Promise.resolve(),
    ...overrides,
  };
}

/**
 * Simple mock memory creator
 */
): Memory {
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

/**
 * Simple mock state creator
 */
): State {
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
