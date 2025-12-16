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
;
; // Import AgentControls type
// Import THREE types if needed, e.g., for metadata typing
// import type * as THREE from 'three';

enum NavigationType {
  ENTITY = 'entity',
  POSITION = 'position',
}

const navigationTargetExtractionTemplate = (thoughts?: string) => {
  return `
# Task:
Decide whether the agent should navigate to a specific **Entity** or a direct **Position** in the Hyperfy world.

# Navigation Types:
- "entity": Navigate to a known entity by its ID.
- "position": Navigate to a specific X,Z coordinate (e.g., from user input like "go to the fountain at 5, 10").

# Constraints:
- Only use **Entity IDs** listed in the current world state.
- Positions must be 2D coordinates in the format { "x": <number>, "z": <number> }.
- Never invent or assume entities that are not in the world state.
- Use "position" only if a direct coordinate is clearly specified or derivable.

# Agent Thought:
${thoughts || 'None'}

# World State:
{{hyperfyStatus}}

# Instructions:
You are **{{agentName}}**, a virtual agent in a Hyperfy world. Analyze the conversation and determine the most appropriate navigation type and target.

Return your answer as a JSON object in **one** of the following forms:

\`\`\`json
{
  "navigationType": "${NavigationType.ENTITY}",
  "parameter": { "entityId": "<string>" }
}
\`\`\`

or

\`\`\`json
{
  "navigationType": "${NavigationType.POSITION}",
  "parameter": { "position": { "x": 5, "z": 10 } }
}
\`\`\`

Only return the JSON object. Do not include any extra text or comments.
  `.trim();
};



const hyperfyGotoEntityAction: Action = {
    name: 'HYPERFY_GOTO_ENTITY',
    similes: ['GO_TO_ENTITY_IN_WORLD', 'MOVE_TO_ENTITY', 'NAVIGATE_TO_ENTITY'],
    description: 'Moves your character to a specified player, object, or world position; use when you need to approach something or go somewhere before interacting.',
    validate: async (runtime: IAgentRuntime): Promise<boolean> => {
      const service = runtime.getService<HyperfyService>(HyperfyService.serviceType);
      // Check if connected and if controls are available
      return !!service && service.isConnected() && !!service.getWorld()?.controls;
    },
    handler: async (
      runtime: IAgentRuntime,
      message: Memory,
      _state: State,
      options: { entityId?: string },
      callback: HandlerCallback,
      responses,
    ) => {
      const thoughtSnippets =
        responses
          ?.map((res) => res.content?.thought)
          .filter(Boolean)
          .join('\n') ?? '';

      const service = runtime.getService<HyperfyService>(HyperfyService.serviceType);
      const world = service?.getWorld(); // Use the getter
      const controls = world?.controls as AgentControls | undefined; // Get controls and cast type

      if (!service || !world || !controls) {
        logger.error('[GOTO Action] Hyperfy service, world, or controls not found.');
        await callback({ thought: 'Prerequisites failed.', error: "Cannot navigate. Hyperfy connection/controls unavailable." });
        return;
      }
      
      let navigationResult: any = null;

      try {
        const extractionState = await runtime.composeState(message);
        const prompt = composePromptFromState({
          state: extractionState,
          template: navigationTargetExtractionTemplate(thoughtSnippets),
        });

        navigationResult = await runtime.useModel(ModelType.OBJECT_LARGE, { prompt });
        logger.info('[GOTO Action] Navigation target extracted:', navigationResult);
      } catch (error) {
        logger.error(`[GOTO Action] Error during navigation target extraction:`, error);
        await callback({
          thought: 'Failed to extract navigation target.',
          text: 'Action failed: Could not determine a navigation target.',
          metadata: { error: 'extraction_failed' },
        });
        return;
      }

      if (
        !navigationResult ||
        !navigationResult.navigationType ||
        !navigationResult.parameter
      ) {
        await callback({
          thought: 'Navigation target missing or malformed.',
          text: 'Action failed: Invalid navigation target.',
          metadata: { error: 'invalid_navigation_target' },
        });
        return;
      }

      const { navigationType, parameter } = navigationResult;

      try {
        switch (navigationType) {
          case NavigationType.ENTITY: {
            const entityId = parameter?.entityId;
            if (!entityId) throw new Error('Missing entityId in parameter.');

            logger.info(`Navigating to entity ${entityId}`);
            await controls.followEntity(entityId);

            const targetEntity = world.entities.items.get(parameter.entityId);
            const entityName =
              targetEntity?.data?.name ||
              targetEntity?.blueprint?.name ||
              `entity ${entityId}`;

            await callback({
              text: `Arrived at ${entityName}.`,
              actions: ['HYPERFY_GOTO_ENTITY'],
              source: 'hyperfy',
            });
            break;
          }

          case NavigationType.POSITION: {
            const pos = parameter?.position;
            if (!pos || typeof pos.x !== 'number' || typeof pos.z !== 'number') {
              throw new Error('Invalid position coordinates.');
            }

            logger.info(`Navigating to position (${pos.x}, ${pos.z})`);
            await controls.goto(pos.x, pos.z);

            await callback({
              text: `Reached position (${pos.x}, ${pos.z}).`,
              actions: ['HYPERFY_GOTO_ENTITY'],
              source: 'hyperfy',
            });
            break;
          }

          default:
            throw new Error(`Unsupported navigation type: ${navigationType}`);
        }
      } catch (error: any) {
        logger.error(`[GOTO Action] Navigation failed:`, error);
        await callback({
          text: `Navigation failed: ${error.message}`,
          metadata: { error: 'navigation_error', detail: error.message },
        });
      }
    },
     examples: [
      // Example assumes an entity "Bob" exists with ID "entity123"
      [
        { name: '{{name1}}', content: { text: 'Go to Bob' } }, // LLM should infer/find ID or user provides via options
        { name: '{{name2}}', content: { text: 'Navigating towards Bob...', actions: ['HYPERFY_GOTO_ENTITY'], source: 'hyperfy' } } // Removed metadata
      ],
       [
        { name: '{{name1}}', content: { text: 'Find entity abcdef' } }, // Assuming entity abcdef exists
        { name: '{{name2}}', content: { text: 'Navigating towards entity abcdef...', actions: ['HYPERFY_GOTO_ENTITY'], source: 'hyperfy' } } // Removed metadata
      ],
      // Example for failure (entity not found)
      [
        { name: '{{name1}}', content: { text: 'Go to the missing chair' } }, // User might specify ID in options
        { name: '{{name2}}', content: { text: 'Error: Cannot navigate. Could not find location for entity chair999.' } } // Assuming ID was chair999
      ],
      [
        { name: '{{name1}}', content: { text: 'Go to the fountain at 12, 8' } },
        { name: '{{name2}}', content: { text: 'Navigating to position (12, 8)...', actions: ['HYPERFY_GOTO_ENTITY'], source: 'hyperfy' } }
      ],
      [
        { name: '{{name1}}', content: { text: 'Walk to coordinate x: 5 z: -3' } },
        { name: '{{name2}}', content: { text: 'Navigating to position (5, -3)...', actions: ['HYPERFY_GOTO_ENTITY'], source: 'hyperfy' } }
      ],
      [
        { name: '{{name1}}', content: { text: 'Move to 0, 0 in the world' } },
        { name: '{{name2}}', content: { text: 'Navigating to position (0, 0)...', actions: ['HYPERFY_GOTO_ENTITY'], source: 'hyperfy' } }
      ]
     ]
  }; 
} // namespace elizaos
