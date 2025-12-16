#include ".service.hpp"
#include ".systems/controls.hpp"
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



 // Import AgentControls type

      // Valid only if connected and controls are available
      // Optional: Could check if getIsNavigating() or getIsPatrolling() is true

        // Should not happen if validate works, but good practice
        await callback({ text: "Error: Cannot stop movement. Hyperfy connection/controls unavailable." });

      // Check for required method
           await callback({ text: "Error: Stop functionality not available in controls." });

        // Call the stop navigation method

        await callback({ text: `Error stopping movement: ${error.message}` });
} // namespace elizaos
