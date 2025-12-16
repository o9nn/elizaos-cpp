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
const participantTestAgentId = uuidv4() as UUID;
const participantTestEntityId = uuidv4() as UUID;
const participantTestRoomId = uuidv4() as UUID;
const participantTestWorldId = uuidv4() as UUID;

// Test data for participant integration tests
const participantTestAgent: Agent = {
  id: participantTestAgentId,
  name: 'Participant Test Agent',
  bio: 'Test agent for participant integration tests',
  settings: {
    profile: {
      short_description: 'Test agent for participant integration tests',
    },
  },
  createdAt: new Date().getTime(),
  updatedAt: new Date().getTime(),
};

const participantTestEntity: Entity = {
  id: participantTestEntityId,
  names: ['Participant Test Entity'],
  agentId: participantTestAgentId,
  components: [],
  metadata: {
    type: 'user',
    worldId: participantTestWorldId,
  },
};

const participantTestWorld: World = {
  id: participantTestWorldId,
  agentId: participantTestAgentId,
  name: 'Participant Test World',
  serverId: 'test-server',
  metadata: {},
};

const participantTestRoom: Room = {
  id: participantTestRoomId,
  name: 'Participant Test Room',
  agentId: participantTestAgentId,
  source: 'test',
  type: ChannelType.GROUP,
  worldId: participantTestWorldId,
  metadata: {},
};

} // namespace elizaos
