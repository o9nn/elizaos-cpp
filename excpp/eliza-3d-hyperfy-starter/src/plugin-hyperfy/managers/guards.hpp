#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Guards any async task and tracks if something is running.
 * Used to prevent behavior execution during active message processing.
 */

class AgentActivityLock {
public:
    bool isActive();
    void enter();
    void exit();
};

  
} // namespace elizaos
