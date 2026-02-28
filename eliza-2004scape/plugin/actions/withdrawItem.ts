import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";

const withdrawItemAction: Action = {
  name: "WITHDRAW_ITEM",
  similes: ["BANK_WITHDRAW", "GET_FROM_BANK"],
  description: "Withdraw an item from the bank to your inventory. REQUIRES: bank must be open (use OPEN_BANK first) and inventory must have free space. Do NOT use if bank is not open. Do NOT use if inventory is full. Use CLOSE_BANK when done withdrawing.",
  parameters: [
    { name: "itemName", description: "The name of the item to withdraw from bank", required: true, schema: { type: "string" } },
  ],
  examples: [
    [{ name: "{{user1}}", content: { text: "Withdraw my axe from bank" } }, { name: "{{agentName}}", content: { text: "Withdrawing axe from bank.", action: "WITHDRAW_ITEM" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => Boolean(runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME)),
  handler: async (runtime, message, _state, options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "WITHDRAW_ITEM" }); return { success: false, error: "service not found" }; }

      let itemName: string | undefined;
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.itemName) itemName = params.itemName;
      if (!itemName) {
        const text = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const match = text.match(/withdraw\s+(?:my\s+|the\s+|a\s+)?(\w+(?:\s+\w+)?)/i);
        if (match?.[1]) itemName = match[1].trim();
      }
      if (!itemName) { if (callback) await callback({ text: "WITHDRAW_ITEM requires itemName", action: "WITHDRAW_ITEM" }); return { success: false, error: "Missing itemName" }; }

      const result = await service.executeAction(runtime.agentId, "withdrawItem", { itemName });
      logger.info({ action: "WITHDRAW_ITEM", itemName, result }, "Executed withdrawItem");
      if (callback) await callback({ text: result.success ? `Withdrew ${itemName}. ${result.message || ""}` : `Failed: ${result.message}`, action: "WITHDRAW_ITEM" });
      return { success: result.success, text: result.message || "", data: { actionName: "withdrawItem", itemName, result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "WITHDRAW_ITEM" });
      return { success: false, error: msg };
    }
  },
};
export default withdrawItemAction;
