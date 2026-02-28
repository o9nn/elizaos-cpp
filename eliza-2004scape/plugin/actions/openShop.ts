import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";

const openShopAction: Action = {
  name: "OPEN_SHOP",
  similes: ["TRADE", "SHOP"],
  description: "Open a shop by trading with the nearest shopkeeper NPC. Use when you need to buy supplies or sell items. REQUIRES: a Shop keeper or trader NPC must be nearby. Do NOT use if no shopkeeper is in the nearby NPCs list. After opening, use BUY_FROM_SHOP or SELL_TO_SHOP, then CLOSE_SHOP.",
  examples: [
    [{ name: "{{user1}}", content: { text: "Open the shop" } }, { name: "{{agentName}}", content: { text: "Trading with the shopkeeper.", action: "OPEN_SHOP" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => Boolean(runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME)),
  handler: async (runtime, _message, _state, _options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "OPEN_SHOP" }); return { success: false, error: "service not found" }; }
      const result = await service.executeAction(runtime.agentId, "openShop", {});
      logger.info({ action: "OPEN_SHOP", result }, "Executed openShop");
      if (callback) await callback({ text: result.success ? `${result.message}` : `Failed: ${result.message}`, action: "OPEN_SHOP" });
      return { success: result.success, text: result.message || "", data: { actionName: "openShop", result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "OPEN_SHOP" });
      return { success: false, error: msg };
    }
  },
};
export default openShopAction;
