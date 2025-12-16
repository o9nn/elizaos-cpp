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
const roomTestAgentId = uuidv4() as UUID;
const roomTestEntityId = uuidv4() as UUID;
const roomTestWorldId = uuidv4() as UUID;
const roomTestRoomId = uuidv4() as UUID;
const roomTestRoom2Id = uuidv4() as UUID;

// Test data for room integration tests
const roomTestAgent: Agent = {
  id: roomTestAgentId,
  name: 'Room Test Agent',
  bio: 'Test agent for room integration tests',
  settings: {
    profile: {
      short_description: 'Test agent for room integration tests',
    },
  },
  createdAt: new Date().getTime(),
  updatedAt: new Date().getTime(),
};

const roomTestEntity: Entity = {
  id: roomTestEntityId,
  names: ['Room Test Entity'],
  agentId: roomTestAgentId,
  components: [],
  metadata: {
    type: 'user',
    worldId: roomTestWorldId,
  },
};

const roomTestWorld: World = {
  id: roomTestWorldId,
  agentId: roomTestAgentId,
  name: 'Room Test World',
  serverId: 'test-server',
  metadata: {},
};

// Basic room test objects
const roomTestRooms: Room[] = [
  {
    id: roomTestRoomId,
    name: 'Room Test Room 1',
    agentId: roomTestAgentId,
    source: 'test',
    type: ChannelType.GROUP,
    worldId: roomTestWorldId,
    metadata: {
      description: 'Test description for room 1',
      tags: ['test', 'integration'],
    },
  },
  {
    id: roomTestRoom2Id,
    name: 'Room Test Room 2',
    agentId: roomTestAgentId,
    source: 'test',
    type: ChannelType.DM,
    worldId: roomTestWorldId,
    channelId: 'test-channel-id',
    serverId: 'test-server-id',
    metadata: {
      description: 'Test description for room 2',
      tags: ['integration'],
    },
  },
  {
    id: uuidv4() as UUID,
    name: 'Room Test Room 3',
    agentId: roomTestAgentId,
    source: 'discord',
    type: ChannelType.GROUP,
    worldId: roomTestWorldId,
    metadata: {
      description: 'Test description for room 3',
      isPrivate: true,
    },
  },
];

// Helper  - Updated`,
    metadata: {
      ...(room.metadata as Record<string, unknown>),
      updated: true,
      timestamp: Date.now(),
    },
  };
};

} // namespace elizaos
