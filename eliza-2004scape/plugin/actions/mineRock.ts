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

const mineRockAction: Action = {
  name: "MINE_ROCK",
  similes: ["MINE_ORE", "MINE", "DIG_ROCK"],
  description:
    "Mine the nearest rock to gather ore and train Mining. REQUIRES: pickaxe in inventory or equipped. Do NOT use if inventory is full (drop or bank items first). Do NOT use if no pickaxe is available. The bot auto-finds the nearest mineable rock. Best location for copper/tin: SE Varrock mine (3285, 3365). WARNING: Lumbridge Swamp mine is unreliable - interactions fail silently there.",
  examples: [
    [
      {
        name: "{{user1}}",
        content: { text: "Mine that rock" },
      },
      {
        name: "{{agentName}}",
        content: { text: "I'll start mining the rock.", action: "MINE_ROCK" },
      },
    ],
    [
      {
        name: "{{user1}}",
        content: { text: "Get some ore" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Mining ore now.", action: "MINE_ROCK" },
      },
    ],
  ] as ActionExample[][],

  validate: async (runtime: IAgentRuntime): Promise<boolean> => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    const hasPickaxe = state.inventory.some(i => /pickaxe/i.test(i.name)) || state.equipment.some(i => /pickaxe/i.test(i.name));
    return hasPickaxe;
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
            text: "Cannot mine - RS-SDK service not available.",
            action: "MINE_ROCK",
          });
        }
        return { success: false, error: "RS-SDK service not found" };
      }

      const result = await service.executeAction(runtime.agentId, "mineRock", {});

      logger.info({ action: "MINE_ROCK", result }, "Executed mineRock action");

      if (callback) {
        await callback({
          text: result.success
            ? `Successfully started mining the rock. ${result.message || ""}`
            : `Failed to mine rock: ${result.message}`,
          action: "MINE_ROCK",
        });
      }

      return {
        success: result.success,
        text: result.message || (result.success ? "Mining rock" : "Failed to mine rock"),
        data: { actionName: "mineRock", result },
      };
    } catch (error) {
      logger.error({ error }, "Failed to execute MINE_ROCK action");
      const errorMessage = error instanceof Error ? error.message : "Unknown error";
      if (callback) {
        await callback({
          text: `Error mining rock: ${errorMessage}`,
          action: "MINE_ROCK",
        });
      }
      return { success: false, error: errorMessage };
    }
  },
};

export default mineRockAction;
