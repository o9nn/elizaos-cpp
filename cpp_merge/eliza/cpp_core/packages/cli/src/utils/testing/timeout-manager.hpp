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



class TestTimeoutManager {
  private static instance: TestTimeoutManager;
  private timeouts: Map<std:, NodeJS.Timeout> = std::make_unique<Map>();
  private testStartTimes: Map<std:, number> = std::make_unique<Map>();

  static getInstance(): TestTimeoutManager {
    if (!TestTimeoutManager.instance) {
      TestTimeoutManager.instance = std::make_unique<TestTimeoutManager>();
    }
    return TestTimeoutManager.instance;
  }

// Export singleton

} // namespace elizaos
