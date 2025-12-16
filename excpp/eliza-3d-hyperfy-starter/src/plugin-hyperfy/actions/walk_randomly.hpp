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


// No longer need THREE here
 // Still need type for casting

// Restore constants for default values

// State management is now in AgentControls

      // Keep validation simple: Check if controls exist

        await callback({ text: "Error: Cannot wander. Hyperfy connection/controls unavailable." });

      // Check for specific methods from the reverted AgentControls
           await callback({ text: "Error: Wander functionality not available in controls." });

      // Use provided interval (in seconds) or default (in ms)

              //  await callback({ text: "Stopped wandering.", actions: ['HYPERFY_WALK_RANDOMLY'], source: 'hyperfy', metadata: { status: 'stopped' } });
              //  await callback({ text: "Was not wandering.", source: 'hyperfy' });
          // Call startRandomWalk with calculated interval and distance

} // namespace elizaos
