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



class LoadTestService extends Service {
  static serviceType = 'load-test';
  capabilityDescription = 'Provides load testing capabilities for agent scaling tests';

  private activeTests: Map<
    string,
    {
      startTime: number;
      messageCount: number;
      errors: number;
    }


} // namespace elizaos
