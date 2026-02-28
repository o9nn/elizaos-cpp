import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";

const depositItemAction: Action = {
  name: "DEPOSIT_ITEM",
  similes: ["BANK_ITEM", "STORE_ITEM", "DEPOSIT"],
  description: "Deposit an inventory item into the bank (deposits all of that item). REQUIRES: bank must be open (use OPEN_BANK first). Do NOT use if bank is not open. Do NOT use if the item is not in your inventory. Use CLOSE_BANK when done depositing.",
  parameters: [
    { name: "itemName", description: "The name of the item to deposit (e.g., 'Logs', 'Bones')", required: true, schema: { type: "string" } },
  ],
  examples: [
    [{ name: "{{user1}}", content: { text: "Deposit all my logs" } }, { name: "{{agentName}}", content: { text: "Depositing logs into the bank.", action: "DEPOSIT_ITEM" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    // Allow if we have items to deposit - bank open check happens in handler
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    return state.inventory.length > 0;
  },
  handler: async (runtime, message, _state, options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "DEPOSIT_ITEM" }); return { success: false, error: "RS-SDK service not found" }; }

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

      if (!itemName) { if (callback) await callback({ text: "DEPOSIT_ITEM requires an itemName", action: "DEPOSIT_ITEM" }); return { success: false, error: "Missing itemName" }; }

      const result = await service.executeAction(runtime.agentId, "depositItem", { itemName, amount: -1 });
      logger.info({ action: "DEPOSIT_ITEM", itemName, result }, "Executed depositItem");
      if (callback) await callback({ text: result.success ? `Deposited ${itemName}. ${result.message || ""}` : `Failed: ${result.message}`, action: "DEPOSIT_ITEM" });
      return { success: result.success, text: result.message || "", data: { actionName: "depositItem", itemName, result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "DEPOSIT_ITEM" });
      return { success: false, error: msg };
    }
  },
};
export default depositItemAction;
