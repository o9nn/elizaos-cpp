#include "elizaos/core.hpp"
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
;

// Test IDs
const worldTestAgentId = uuidv4() as UUID;
const worldTestEntityId = uuidv4() as UUID;

// Test data for world integration tests
const worldTestAgent: Agent = {
  id: worldTestAgentId,
  name: 'World Test Agent',
  bio: 'Test agent for world integration tests',
  settings: {
    profile: {
      short_description: 'Test agent for world integration tests',
    },
  },
  createdAt: new Date().getTime(),
  updatedAt: new Date().getTime(),
};

const worldTestEntity: Entity = {
  id: worldTestEntityId,
  names: ['World Test Entity'],
  agentId: worldTestAgentId,
  components: [],
  metadata: {
    type: 'user',
  },
};

// Basic test worlds
const worldTestWorlds: World[] = [
  {
    id: uuidv4() as UUID,
    agentId: worldTestAgentId,
    name: 'Test World 1',
    serverId: 'test-server-1',
    metadata: {
      ownership: {
        ownerId: worldTestEntityId,
      },
      roles: {
        [worldTestEntityId]: Role.OWNER,
      },
    },
  },
  {
    id: uuidv4() as UUID,
    agentId: worldTestAgentId,
    name: 'Test World 2',
    serverId: 'test-server-2',
    metadata: {
      ownership: {
        ownerId: worldTestEntityId,
      },
    },
  },
  {
    id: uuidv4() as UUID,
    agentId: worldTestAgentId,
    name: 'Test World 3',
    serverId: 'test-server-3',
    metadata: {
      custom: 'value',
      tags: ['test', 'integration'],
    },
  },
];

} // namespace elizaos
