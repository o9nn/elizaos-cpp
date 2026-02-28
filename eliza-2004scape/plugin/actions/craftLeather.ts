import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";

const craftLeatherAction: Action = {
  name: "CRAFT_LEATHER",
  similes: ["CRAFT", "SEW_LEATHER", "MAKE_ARMOUR"],
  description: "Craft leather into armour to train Crafting. REQUIRES: Needle, Thread, AND Leather in inventory. Do NOT use if missing any required item. Do NOT use if inventory is full. The bot auto-selects a leather product to craft.",
  examples: [
    [{ name: "{{user1}}", content: { text: "Craft some leather" } }, { name: "{{agentName}}", content: { text: "Crafting leather armour.", action: "CRAFT_LEATHER" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    const hasNeedle = state.inventory.some(i => /needle/i.test(i.name));
    const hasThread = state.inventory.some(i => /thread/i.test(i.name));
    const hasLeather = state.inventory.some(i => /leather/i.test(i.name));
    return hasNeedle && hasThread && hasLeather;
  },
  handler: async (runtime, _message, _state, _options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "CRAFT_LEATHER" }); return { success: false, error: "service not found" }; }
      const result = await service.executeAction(runtime.agentId, "craftLeather", {});
      logger.info({ action: "CRAFT_LEATHER", result }, "Executed craftLeather");
      if (callback) await callback({ text: result.success ? `${result.message}` : `Failed: ${result.message}`, action: "CRAFT_LEATHER" });
      return { success: result.success, text: result.message || "", data: { actionName: "craftLeather", result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "CRAFT_LEATHER" });
      return { success: false, error: msg };
    }
  },
};
export default craftLeatherAction;
