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

const chopTreeAction: Action = {
  name: "CHOP_TREE",
  similes: ["CUT_TREE", "WOODCUT", "CHOP_WOOD"],
  description:
    "Chop down the nearest tree to gather logs and train Woodcutting. REQUIRES: axe in inventory or equipped. Do NOT use if inventory is full (drop or bank items first). Do NOT use if no axe is available. The bot auto-finds the nearest tree. Bronze axes available from Bob's Axes in Lumbridge (3230, 3203) for 16gp. Good tree locations: Lumbridge (3200, 3220) for normal trees, Draynor (3089, 3233) for willows at level 30+.",
  examples: [
    [
      {
        name: "{{user1}}",
        content: { text: "Chop down that tree" },
      },
      {
        name: "{{agentName}}",
        content: { text: "I'll start chopping the tree.", action: "CHOP_TREE" },
      },
    ],
    [
      {
        name: "{{user1}}",
        content: { text: "Cut some wood" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Starting to cut wood now.", action: "CHOP_TREE" },
      },
    ],
  ] as ActionExample[][],

  validate: async (runtime: IAgentRuntime): Promise<boolean> => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    // Validate: need axe in inventory or equipment, and a tree nearby
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true; // allow if can't check yet
    const hasAxe = state.inventory.some(i => /axe/i.test(i.name)) || state.equipment.some(i => /axe/i.test(i.name));
    return hasAxe;
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
            text: "Cannot chop tree - RS-SDK service not available.",
            action: "CHOP_TREE",
          });
        }
        return { success: false, error: "RS-SDK service not found" };
      }

      const result = await service.executeAction(runtime.agentId, "chopTree", {});

      logger.info({ action: "CHOP_TREE", result }, "Executed chopTree action");

      if (callback) {
        await callback({
          text: result.success
            ? `Successfully started chopping the tree. ${result.message || ""}`
            : `Failed to chop tree: ${result.message}`,
          action: "CHOP_TREE",
        });
      }

      return {
        success: result.success,
        text: result.message || (result.success ? "Chopping tree" : "Failed to chop tree"),
        data: { actionName: "chopTree", result },
      };
    } catch (error) {
      logger.error({ error }, "Failed to execute CHOP_TREE action");
      const errorMessage = error instanceof Error ? error.message : "Unknown error";
      if (callback) {
        await callback({
          text: `Error chopping tree: ${errorMessage}`,
          action: "CHOP_TREE",
        });
      }
      return { success: false, error: errorMessage };
    }
  },
};

export default chopTreeAction;
