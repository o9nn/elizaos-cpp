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
#include "elizaos/core.hpp"
#include "service.hpp"
#include "systems/controls.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



 // Import AgentControls type

      // Valid only if connected and controls are available
      // Optional: Could check if getIsNavigating() or getIsPatrolling() is true

        // Should not happen if validate works, but good practice

      // Check for required method

        // Call the stop navigation method


} // namespace elizaos
