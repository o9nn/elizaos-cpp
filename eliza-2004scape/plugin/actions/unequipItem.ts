import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";

const unequipItemAction: Action = {
  name: "UNEQUIP_ITEM",
  similes: ["REMOVE_ITEM", "TAKE_OFF", "UNWIELD"],
  description: "Remove an equipped item back to inventory. Use to swap gear or free equipment for something better. REQUIRES: item must be currently equipped (shown in Equipment section). Do NOT use if inventory is full - drop something first. Do NOT use if the item is not equipped.",
  parameters: [
    { name: "itemName", description: "The name of the equipped item to remove (e.g., 'Bronze sword', 'Leather body')", required: true, schema: { type: "string" } },
  ],
  examples: [
    [{ name: "{{user1}}", content: { text: "Remove my bronze sword" } }, { name: "{{agentName}}", content: { text: "Unequipping bronze sword.", action: "UNEQUIP_ITEM" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    const hasEquipped = state.equipment.length > 0;
    const hasSpace = state.inventory.length < 28;
    return hasEquipped && hasSpace;
  },
  handler: async (runtime, message, _state, options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "UNEQUIP_ITEM" }); return { success: false, error: "RS-SDK service not found" }; }

      let itemName: string | undefined;
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.itemName && typeof params.itemName === "string") itemName = params.itemName;

      if (!itemName) {
        const combinedText = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const botState = await service.getBotState(runtime.agentId);
        if (botState?.equipment) {
          for (const eq of botState.equipment) {
            if (combinedText.includes(eq.name.toLowerCase())) { itemName = eq.name; break; }
          }
        }
      }

      if (!itemName) { if (callback) await callback({ text: "UNEQUIP_ITEM requires an itemName", action: "UNEQUIP_ITEM" }); return { success: false, error: "Missing itemName" }; }

      const result = await service.executeAction(runtime.agentId, "unequipItem", { itemName });
      logger.info({ action: "UNEQUIP_ITEM", itemName, result }, "Executed unequipItem");
      if (callback) await callback({ text: result.success ? `Unequipped ${itemName}. ${result.message || ""}` : `Failed: ${result.message}`, action: "UNEQUIP_ITEM" });
      return { success: result.success, text: result.message || "", data: { actionName: "unequipItem", itemName, result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "UNEQUIP_ITEM" });
      return { success: false, error: msg };
    }
  },
};
export default unequipItemAction;
