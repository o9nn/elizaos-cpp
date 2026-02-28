import {
  type Action,
  type ActionExample,
  type ActionResult,
  type HandlerCallback,
  type HandlerOptions,
  type IAgentRuntime,
  logger,
  type Memory,
  type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";

const fishAction: Action = {
  name: "FISH",
  similes: ["CATCH_FISH", "GO_FISHING", "FISHING"],
  description:
    "Fish at the nearest fishing spot to catch fish and train Fishing. REQUIRES: fishing equipment (Small fishing net, Fly fishing rod, or Harpoon) in inventory. Do NOT use if inventory is full or no fishing spot is visible in Nearby NPCs. Fishing spots appear as NPCs named 'Fishing spot' (they are NPCs, not objects, and they MOVE over time). Uses Net option by default (for Shrimp/Anchovies). IMPORTANT: For level 1 fishing, go to Draynor Village (3087, 3230) - Lumbridge area has NO small net spots! Different spots offer Net, Bait, Lure, or Harpoon options visible in the NPC's option list.",
  examples: [
    [
      {
        name: "{{user1}}",
        content: { text: "Go fishing" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Starting to fish.", action: "FISH" },
      },
    ],
    [
      {
        name: "{{user1}}",
        content: { text: "Catch some fish" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Fishing now.", action: "FISH" },
      },
    ],
  ] as ActionExample[][],

  validate: async (runtime: IAgentRuntime): Promise<boolean> => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    const hasFishingGear = state.inventory.some(i => /fishing net|fishing rod|harpoon|lobster pot/i.test(i.name));
    const hasFishingSpot = state.nearbyNpcs.some(n => /fishing spot/i.test(n.name));
    const hasSpace = state.inventory.length < 28;
    return hasFishingGear && hasFishingSpot && hasSpace;
  },

  handler: async (
    runtime: IAgentRuntime,
    message: Memory,
    _state: State | undefined,
    _options?: HandlerOptions,
    callback?: HandlerCallback
  ): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) {
        logger.error("RS-SDK service not found");
        if (callback) {
          await callback({
            text: "Cannot fish - RS-SDK service not available.",
            action: "FISH",
          });
        }
        return { success: false, error: "RS-SDK service not found" };
      }

      const result = await service.executeAction(runtime.agentId, "fish", {});

      logger.info({ action: "FISH", result }, "Executed fish action");

      if (callback) {
        await callback({
          text: result.success
            ? `Started fishing. ${result.message || ""}`
            : `Failed to fish: ${result.message}`,
          action: "FISH",
        });
      }

      return {
        success: result.success,
        text: result.message || (result.success ? "Fishing" : "Failed to fish"),
        data: { actionName: "fish", result },
      };
    } catch (error) {
      logger.error({ error }, "Failed to execute FISH action");
      const errorMessage = error instanceof Error ? error.message : "Unknown error";
      if (callback) {
        await callback({
          text: `Error fishing: ${errorMessage}`,
          action: "FISH",
        });
      }
      return { success: false, error: errorMessage };
    }
  },
};

export default fishAction;
