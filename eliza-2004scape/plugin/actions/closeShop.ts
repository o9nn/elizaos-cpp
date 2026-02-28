import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";

const closeShopAction: Action = {
  name: "CLOSE_SHOP",
  similes: ["EXIT_SHOP", "LEAVE_SHOP"],
  description: "Close the shop interface. Use ONLY after you are done buying/selling. Do NOT use if shop is not currently open. Always close the shop when finished.",
  examples: [
    [{ name: "{{user1}}", content: { text: "Close the shop" } }, { name: "{{agentName}}", content: { text: "Closing the shop.", action: "CLOSE_SHOP" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => Boolean(runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME)),
  handler: async (runtime, _message, _state, _options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "CLOSE_SHOP" }); return { success: false, error: "service not found" }; }
      const result = await service.executeAction(runtime.agentId, "closeShop", {});
      if (callback) await callback({ text: result.success ? `${result.message}` : `Failed: ${result.message}`, action: "CLOSE_SHOP" });
      return { success: result.success, text: result.message || "", data: { actionName: "closeShop", result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "CLOSE_SHOP" });
      return { success: false, error: msg };
    }
  },
};
export default closeShopAction;
