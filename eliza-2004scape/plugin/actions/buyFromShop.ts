import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";

const buyFromShopAction: Action = {
  name: "BUY_FROM_SHOP",
  similes: ["BUY_ITEM", "PURCHASE", "SHOP_BUY"],
  description: "Buy an item from the currently open shop. REQUIRES: shop must be open (use OPEN_SHOP first). Do NOT use if shop is not open. Do NOT use if you don't have enough coins. Specify the item name to buy. Use CLOSE_SHOP when done.",
  parameters: [
    { name: "itemName", description: "The name of the item to buy (e.g., 'Bronze axe', 'Pot', 'Bucket')", required: true, schema: { type: "string" } },
  ],
  examples: [
    [{ name: "{{user1}}", content: { text: "Buy a bronze axe" } }, { name: "{{agentName}}", content: { text: "Buying a bronze axe from the shop.", action: "BUY_FROM_SHOP" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    return state.inventory.length < 28; // need space for purchased item
  },
  handler: async (runtime, message, _state, options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "BUY_FROM_SHOP" }); return { success: false, error: "RS-SDK service not found" }; }

      let itemName: string | undefined;
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.itemName && typeof params.itemName === "string") itemName = params.itemName;

      if (!itemName) {
        const patterns = [/buy\s+(?:a\s+|an\s+|the\s+)?(\w+(?:\s+\w+)?)/i, /purchase\s+(?:a\s+|an\s+)?(\w+(?:\s+\w+)?)/i];
        const textToSearch = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`;
        for (const p of patterns) { const m = textToSearch.match(p); if (m?.[1]) { itemName = m[1].trim(); break; } }
      }

      if (!itemName) { if (callback) await callback({ text: "BUY_FROM_SHOP requires an itemName", action: "BUY_FROM_SHOP" }); return { success: false, error: "Missing itemName" }; }

      const result = await service.executeAction(runtime.agentId, "buyFromShop", { itemName });
      logger.info({ action: "BUY_FROM_SHOP", itemName, result }, "Executed buyFromShop");
      if (callback) await callback({ text: result.success ? `Bought ${itemName}. ${result.message || ""}` : `Failed: ${result.message}`, action: "BUY_FROM_SHOP" });
      return { success: result.success, text: result.message || "", data: { actionName: "buyFromShop", itemName, result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "BUY_FROM_SHOP" });
      return { success: false, error: msg };
    }
  },
};
export default buyFromShopAction;
