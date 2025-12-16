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

// Test IDs for relationship tests
const relationshipTestAgentId = uuidv4() as UUID;
const relationshipTestSourceEntityId = uuidv4() as UUID;
const relationshipTestTargetEntityId = uuidv4() as UUID;

// Test data for relationship integration tests
const relationshipTestAgent: Agent = {
  id: relationshipTestAgentId,
  name: 'Relationship Test Agent',
  bio: 'Test agent for relationship integration tests',
  settings: {
    profile: {
      short_description: 'Test agent for relationship integration tests',
    },
  },
  createdAt: new Date().getTime(),
  updatedAt: new Date().getTime(),
};

const relationshipTestSourceEntity: Entity = {
  id: relationshipTestSourceEntityId,
  names: ['Source Entity'],
  agentId: relationshipTestAgentId,
  components: [],
  metadata: {
    type: 'user',
  },
};

const relationshipTestTargetEntity: Entity = {
  id: relationshipTestTargetEntityId,
  names: ['Target Entity'],
  agentId: relationshipTestAgentId,
  components: [],
  metadata: {
    type: 'user',
  },
};

// Test relationships
const relationshipTestRelationships: Relationship[] = [
  {
    id: uuidv4() as UUID,
    sourceEntityId: relationshipTestSourceEntityId,
    targetEntityId: relationshipTestTargetEntityId,
    agentId: relationshipTestAgentId,
    tags: ['friend'],
    metadata: {
      type: 'social',
      strength: 'high',
    },
    createdAt: new Date().getTime().toString(),
  },
  {
    id: uuidv4() as UUID,
    sourceEntityId: relationshipTestTargetEntityId,
    targetEntityId: relationshipTestSourceEntityId,
    agentId: relationshipTestAgentId,
    tags: ['colleague'],
    metadata: {
      type: 'professional',
      strength: 'medium',
    },
    createdAt: new Date().getTime().toString(),
  },
];

// Helper 
): Relationship => {
  return {
    id: uuidv4() as UUID,
    sourceEntityId: sourceId,
    targetEntityId: targetId,
    agentId: relationshipTestAgentId,
    tags,
    metadata,
    createdAt: new Date().getTime().toString(),
  };
};

} // namespace elizaos
