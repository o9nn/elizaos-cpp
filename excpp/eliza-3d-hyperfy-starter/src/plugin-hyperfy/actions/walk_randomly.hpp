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

;
// No longer need THREE here
; // Still need type for casting
;

// Restore constants for default values
const RANDOM_WALK_DEFAULT_INTERVAL = 4000; // ms (4 seconds)
const RANDOM_WALK_DEFAULT_MAX_DISTANCE = 30; // meters

// State management is now in AgentControls

const hyperfyWalkRandomlyAction: Action = {
    name: 'HYPERFY_WALK_RANDOMLY',
    similes: ['WANDER', 'PACE_AROUND', 'WALK_AROUND', 'MOVE_RANDOMLY'], // Reverted similes/desc
    description: 'Makes your character wander to random points nearby; use for idle behavior or ambient movement.',
    validate: async (runtime: IAgentRuntime): Promise<boolean> => {
      const service = runtime.getService<HyperfyService>(HyperfyService.serviceType);
      // Keep validation simple: Check if controls exist
      return !!service && service.isConnected() && !!service.getWorld()?.controls;
    },
    handler: async (
      runtime: IAgentRuntime,
      _message: Memory,
      _state: State,
      options: { interval?: number, distance?: number, command?: 'start' | 'stop' }, // Reverted options
      callback: HandlerCallback,
    ) => {
      const service = runtime.getService<HyperfyService>(HyperfyService.serviceType);
      const world = service?.getWorld();
      const controls = world?.controls as AgentControls | undefined;

      if (!service || !world || !controls) {
        logger.error('Hyperfy service, world, or controls not found for HYPERFY_WALK_RANDOMLY action.');
        await callback({ text: "Error: Cannot wander. Hyperfy connection/controls unavailable." });
        return;
      }

      // Check for specific methods from the reverted AgentControls
       if (typeof controls.startRandomWalk !== 'function' || typeof controls.stopRandomWalk !== 'function') {
           logger.error('AgentControls missing startRandomWalk or stopRandomWalk methods.');
           await callback({ text: "Error: Wander functionality not available in controls." });
           return;
       }

      const command = options?.command || 'start';
      // Use provided interval (in seconds) or default (in ms)
      const intervalMs = options?.interval ? options.interval * 1000 : RANDOM_WALK_DEFAULT_INTERVAL;
      const maxDistance = options?.distance || RANDOM_WALK_DEFAULT_MAX_DISTANCE;

      if (command === 'stop') {
          if (controls.getIsWalkingRandomly()) { // Use correct check
               controls.stopRandomWalk(); // Call correct stop method
              //  await callback({ text: "Stopped wandering.", actions: ['HYPERFY_WALK_RANDOMLY'], source: 'hyperfy', metadata: { status: 'stopped' } });
          } else {
              //  await callback({ text: "Was not wandering.", source: 'hyperfy' });
          }
      } else { // command === 'start'
          // Call startRandomWalk with calculated interval and distance
          controls.startRandomWalk(intervalMs, maxDistance);

          await callback({
             text: ``,
             actions: ['HYPERFY_WALK_RANDOMLY'],
             source: 'hyperfy',
             metadata: { status: 'started', intervalMs: intervalMs, maxDistance: maxDistance }
          });
      }
    },
     examples: [
        [
          { name: '{{name1}}', content: { text: 'Wander around for a bit.' } },
          { name: '{{name2}}', content: { text: 'Starting to wander randomly... (New target every ~5.0s)', actions: ['HYPERFY_WALK_RANDOMLY'], source: 'hyperfy' } }
        ],
         [
          { name: '{{name1}}', content: { text: 'Just pace around here.' } },
          { name: '{{name2}}', content: { text: 'Starting to wander randomly... (New target every ~5.0s)', actions: ['HYPERFY_WALK_RANDOMLY'], source: 'hyperfy' } }
        ],
        [
          { name: '{{name1}}', content: { text: 'Stop wandering.' } }, // Assumes it was wandering
          { name: '{{name2}}', content: { text: 'Stopped wandering.', actions: ['HYPERFY_WALK_RANDOMLY'], source: 'hyperfy' } }
        ]
       ]
}; 
} // namespace elizaos
