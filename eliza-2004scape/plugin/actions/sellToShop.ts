import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";

const sellToShopAction: Action = {
  name: "SELL_TO_SHOP",
  similes: ["SELL_ITEM", "SHOP_SELL"],
  description: "Sell an inventory item to the currently open shop for coins. REQUIRES: shop must be open (use OPEN_SHOP first) and item must be in inventory. Do NOT use if shop is not open. Do NOT sell essential tools (axe, pickaxe, tinderbox). Use CLOSE_SHOP when done. WARNING: General stores pay 0 GP when overstocked! Don't sell lots of the same item. Use specialized shops for better prices, or consider Thieving for money instead.",
  parameters: [
    { name: "itemName", description: "The name of the item to sell (e.g., 'Logs', 'Raw shrimps')", required: true, schema: { type: "string" } },
  ],
  examples: [
    [{ name: "{{user1}}", content: { text: "Sell my logs" } }, { name: "{{agentName}}", content: { text: "Selling logs to the shop.", action: "SELL_TO_SHOP" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    return state.inventory.length > 0; // need items to sell
  },
  handler: async (runtime, message, _state, options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "SELL_TO_SHOP" }); return { success: false, error: "RS-SDK service not found" }; }

      let itemName: string | undefined;
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.itemName && typeof params.itemName === "string") itemName = params.itemName;

      if (!itemName) {
        const combinedText = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const botState = await service.getBotState(runtime.agentId);
        if (botState?.inventory) {
          for (const item of botState.inventory) {
            if (combinedText.includes(item.name.toLowerCase())) { itemName = item.name; break; }
          }
        }
      }

      if (!itemName) { if (callback) await callback({ text: "SELL_TO_SHOP requires an itemName", action: "SELL_TO_SHOP" }); return { success: false, error: "Missing itemName" }; }

      const result = await service.executeAction(runtime.agentId, "sellToShop", { itemName });
      logger.info({ action: "SELL_TO_SHOP", itemName, result }, "Executed sellToShop");
      if (callback) await callback({ text: result.success ? `Sold ${itemName}. ${result.message || ""}` : `Failed: ${result.message}`, action: "SELL_TO_SHOP" });
      return { success: result.success, text: result.message || "", data: { actionName: "sellToShop", itemName, result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "SELL_TO_SHOP" });
      return { success: false, error: msg };
    }
  },
};
export default sellToShopAction;
