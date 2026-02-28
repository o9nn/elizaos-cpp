import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";

const pickupItemAction: Action = {
  name: "PICKUP_ITEM",
  similes: ["PICK_UP", "LOOT", "GRAB_ITEM", "TAKE_ITEM"],
  description: "Pick up an item from the ground. Use to collect loot after killing NPCs, or pick up items you see on the ground. REQUIRES: inventory must have free space (not full). Do NOT use if inventory is full. The item must appear in the Ground Items list. Specify the exact item name.",
  parameters: [
    { name: "itemName", description: "The name of the ground item to pick up (e.g., 'Bones', 'Logs', 'Coins')", required: true, schema: { type: "string" } },
  ],
  examples: [
    [{ name: "{{user1}}", content: { text: "Pick up the bones" } }, { name: "{{agentName}}", content: { text: "Picking up bones.", action: "PICKUP_ITEM" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    const hasGroundItems = state.groundItems.length > 0;
    const hasSpace = state.inventory.length < 28;
    return hasGroundItems && hasSpace;
  },
  handler: async (runtime, message, _state, options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "PICKUP_ITEM" }); return { success: false, error: "RS-SDK service not found" }; }

      let itemName: string | undefined;
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.itemName && typeof params.itemName === "string") itemName = params.itemName;

      if (!itemName) {
        const combinedText = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const botState = await service.getBotState(runtime.agentId);
        if (botState?.groundItems) {
          for (const gi of botState.groundItems) {
            if (combinedText.includes(gi.name.toLowerCase())) { itemName = gi.name; break; }
          }
        }
      }

      if (!itemName) {
        const textLower = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const commonItems = ["bones", "coins", "logs", "ashes", "feather", "raw shrimps", "bronze arrow"];
        for (const item of commonItems) { if (new RegExp(`\\b${item}\\b`, "i").test(textLower)) { itemName = item.split(" ").map(w => w.charAt(0).toUpperCase() + w.slice(1)).join(" "); break; } }
      }

      if (!itemName) { if (callback) await callback({ text: "PICKUP_ITEM requires an itemName", action: "PICKUP_ITEM" }); return { success: false, error: "Missing itemName" }; }

      const result = await service.executeAction(runtime.agentId, "pickupItem", { itemName });
      logger.info({ action: "PICKUP_ITEM", itemName, result }, "Executed pickupItem");
      if (callback) await callback({ text: result.success ? `Picked up ${itemName}. ${result.message || ""}` : `Failed: ${result.message}`, action: "PICKUP_ITEM" });
      return { success: result.success, text: result.message || "", data: { actionName: "pickupItem", itemName, result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "PICKUP_ITEM" });
      return { success: false, error: msg };
    }
  },
};
export default pickupItemAction;
