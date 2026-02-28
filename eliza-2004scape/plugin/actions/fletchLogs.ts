import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";

const fletchLogsAction: Action = {
  name: "FLETCH_LOGS",
  similes: ["FLETCH", "MAKE_BOW", "MAKE_ARROWS"],
  description: "Fletch logs into bows or arrow shafts to train Fletching. REQUIRES: Knife AND Logs in inventory. Do NOT use if missing either item. Do NOT use if inventory is full (need space for product). The bot auto-uses Knife on Logs and selects a product.",
  examples: [
    [{ name: "{{user1}}", content: { text: "Fletch the logs" } }, { name: "{{agentName}}", content: { text: "Fletching logs into items.", action: "FLETCH_LOGS" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true; // allow if can't check
    const hasKnife = state.inventory.some(i => /knife/i.test(i.name)) || state.equipment.some(i => /knife/i.test(i.name));
    const hasLogs = state.inventory.some(i => /logs/i.test(i.name));
    return hasKnife && hasLogs;
  },
  handler: async (runtime, _message, _state, _options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "FLETCH_LOGS" }); return { success: false, error: "service not found" }; }
      const result = await service.executeAction(runtime.agentId, "fletchLogs", {});
      logger.info({ action: "FLETCH_LOGS", result }, "Executed fletchLogs");
      if (callback) await callback({ text: result.success ? `${result.message}` : `Failed: ${result.message}`, action: "FLETCH_LOGS" });
      return { success: result.success, text: result.message || "", data: { actionName: "fletchLogs", result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "FLETCH_LOGS" });
      return { success: false, error: msg };
    }
  },
};
export default fletchLogsAction;
