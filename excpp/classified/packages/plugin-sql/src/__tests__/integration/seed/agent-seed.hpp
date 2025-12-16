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

/**
 * Seed data for agent integration tests
 */
;

/**
 * Test agent data used across agent integration tests
 */
const testAgent: Agent = {
  name: 'Integration Test Agent',
  username: 'test_agent',
  bio: 'A test agent for integration tests',
  enabled: true,
  settings: {
    testSetting: 'test value',
  },
  createdAt: new Date().getTime(),
  updatedAt: new Date().getTime(),
};

} // namespace elizaos
