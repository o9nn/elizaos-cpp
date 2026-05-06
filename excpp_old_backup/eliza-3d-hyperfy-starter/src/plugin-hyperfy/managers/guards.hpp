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
 * Guards any async task and tracks if something is running.
 * Used to prevent behavior execution during active message processing.
 */

class AgentActivityLock {
    private count = 0;
  
    isActive(): boolean {
      return this.count > 0;
    }

  
} // namespace elizaos
