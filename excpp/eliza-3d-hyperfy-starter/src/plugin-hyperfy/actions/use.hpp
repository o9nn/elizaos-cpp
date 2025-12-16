#include ".service.hpp"
#include ".systems/actions.hpp"
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
;
;

// Template to extract entity to interact with
const useItemTemplate = `
# Task: Decide if the agent should interact with an entity (e.g. pick up or activate) based on recent context.
# DO NOT assume the last message has a command. Look at overall context.
# ONLY return entity IDs that exist in the Hyperfy World State.

{{providers}}

# Instructions:
Decide if the agent should use/interact with a specific entity based on the conversation and world state.

Response format:
\`\`\`json
{
  "entityId": "<string>" // or null if none
}
\`\`\`
`;

const hyperfyUseItemAction: Action = {
  name: 'HYPERFY_USE_ITEM',
  similes: ['INTERACT_WITH_ITEM', 'USE_NEARBY_OBJECT', 'PICK_UP_ITEM'],
  description: 'Walks to and interacts with a nearby usable item (like picking it up); use when a player asks you to use or grab something.',
  validate: async (runtime: IAgentRuntime): Promise<boolean> => {
    const service = runtime.getService<HyperfyService>(HyperfyService.serviceType);
    const world = service?.getWorld();
    return !!service && service.isConnected() && !!world?.controls && !!world?.actions;
  },
  handler: async (
    runtime: IAgentRuntime,
    message: Memory,
    _state: State,
    options: { entityId?: string },
    callback: HandlerCallback
  ) => {
    const service = runtime.getService<HyperfyService>(HyperfyService.serviceType);
    const world = service?.getWorld();
    const controls = world?.controls as AgentControls;
    const actions = world?.actions as AgentActions | undefined;

    if (!service || !world || !actions) {
      logger.error('Hyperfy service, world, or actions not found for HYPERFY_USE_ITEM action.');
      await callback({ text: "Error: Cannot use item. Agent action system unavailable." });
      return;
    }

    let targetEntityId = options?.entityId;

    if (!targetEntityId) {
      logger.info('[USE ITEM] No entityId provided, attempting LLM extraction...');
      try {
        const useState = await runtime.composeState(message, ['HYPERFY_WORLD_STATE', 'RECENT_MESSAGES'], true);
        const prompt = composePromptFromState({ state: useState, template: useItemTemplate });
        const response = await runtime.useModel(ModelType.OBJECT_SMALL, { prompt });

        if (response?.entityId && typeof response.entityId === 'string') {
          targetEntityId = response.entityId;
          logger.info(`[USE ITEM] Extracted entity ID: ${targetEntityId}`);
        } else {
          logger.warn('[USE ITEM] No valid entityId extracted.');
        }
      } catch (err) {
        logger.error('[USE ITEM] Extraction failed:', err);
      }
    }

    if (!targetEntityId) {
      logger.warn('[USE ITEM] No suitable item found to use based on the context.');
      return;
    }

    
    const entity = world.entities.items.get(targetEntityId);
    const targetPosition = entity?.root?.position
    if (!targetPosition) {
      await callback({
        text: `Could not locate entity ${targetEntityId}.`,
        metadata: { error: 'entity_not_found' }
      });
      return;
    }

    await controls.goto(targetPosition.x, targetPosition.z);

    logger.info(`[USE ITEM] Attempting to use item with entity ID: ${targetEntityId}`);
    actions.performAction(targetEntityId);

    await callback({
      text: `Using item: ${targetEntityId}`,
      actions: ['HYPERFY_USE_ITEM'],
      source: 'hyperfy',
      metadata: { targetEntityId, status: 'triggered' }
    });
  },
  examples: [
    [
      { name: '{{name1}}', content: { text: 'Pick up the book.' } },
      { name: '{{name2}}', content: { text: 'Using item: book123', actions: ['HYPERFY_USE_ITEM'], source: 'hyperfy' } }
    ],
    [
      { name: '{{name1}}', content: { text: 'Interact with the glowing orb.' } },
      { name: '{{name2}}', content: { text: 'Using item: orb888', actions: ['HYPERFY_USE_ITEM'], source: 'hyperfy' } }
    ],
    [
      { name: '{{name1}}', content: { text: 'Do we need to pick something up?' } },
      { name: '{{name2}}', content: { text: 'No suitable item found to use based on the context.' } }
    ]
  ]
};

} // namespace elizaos
