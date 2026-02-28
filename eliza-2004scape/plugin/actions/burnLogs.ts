import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";

const burnLogsAction: Action = {
  name: "BURN_LOGS",
  similes: ["LIGHT_FIRE", "MAKE_FIRE", "FIREMAKE"],
  description: "Burn logs with a tinderbox to train Firemaking. REQUIRES: Logs AND Tinderbox in inventory. Do NOT use if missing either item. Do NOT use indoors or in crowded areas (fires need open ground). Move to an open area first if fire fails.",
  examples: [
    [{ name: "{{user1}}", content: { text: "Burn some logs" } }, { name: "{{agentName}}", content: { text: "Burning logs for Firemaking XP.", action: "BURN_LOGS" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    const hasTinderbox = state.inventory.some(i => /tinderbox/i.test(i.name));
    const hasLogs = state.inventory.some(i => /logs/i.test(i.name));
    return hasTinderbox && hasLogs;
  },
  handler: async (runtime, message, _state, _options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "BURN_LOGS" }); return { success: false, error: "RS-SDK service not found" }; }

      const result = await service.executeAction(runtime.agentId, "burnLogs", {});
      logger.info({ action: "BURN_LOGS", result }, "Executed burnLogs");
      if (callback) await callback({ text: result.success ? `${result.message || "Burned logs."}` : `Failed: ${result.message}`, action: "BURN_LOGS" });
      return { success: result.success, text: result.message || "", data: { actionName: "burnLogs", result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "BURN_LOGS" });
      return { success: false, error: msg };
    }
  },
};
export default burnLogsAction;
