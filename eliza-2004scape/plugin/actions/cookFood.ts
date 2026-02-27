import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";

const cookFoodAction: Action = {
  name: "COOK_FOOD",
  similes: ["COOK", "COOK_FISH", "COOK_MEAT"],
  description: "Cook raw food on a nearby fire, range, or stove to train Cooking. REQUIRES: raw food in inventory (Raw shrimps, Raw chicken, etc.) AND a fire/range/stove in Nearby Objects. Do NOT use if no raw food in inventory. Do NOT use if no fire/range nearby. Cooked food can then be eaten with EAT_FOOD.",
  parameters: [
    { name: "itemName", description: "The name of the raw food to cook (e.g., 'Raw shrimps', 'Raw chicken')", required: true, schema: { type: "string" } },
  ],
  examples: [
    [{ name: "{{user1}}", content: { text: "Cook the raw shrimps" } }, { name: "{{agentName}}", content: { text: "Cooking raw shrimps on the fire.", action: "COOK_FOOD" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    const hasRawFood = state.inventory.some(i => /^raw /i.test(i.name));
    const hasFireOrRange = state.nearbyLocs.some(l => /fire|range|stove/i.test(l.name));
    return hasRawFood && hasFireOrRange;
  },
  handler: async (runtime, message, _state, options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "COOK_FOOD" }); return { success: false, error: "RS-SDK service not found" }; }

      let itemName: string | undefined;
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.itemName && typeof params.itemName === "string") itemName = params.itemName;

      if (!itemName) {
        const combinedText = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const botState = await service.getBotState(runtime.agentId);
        if (botState?.inventory) {
          for (const item of botState.inventory) {
            if (item.name.toLowerCase().startsWith("raw") && combinedText.includes(item.name.toLowerCase())) { itemName = item.name; break; }
          }
          // If no specific raw food mentioned, find any raw food in inventory
          if (!itemName) {
            const rawFood = botState.inventory.find(i => i.name.toLowerCase().startsWith("raw"));
            if (rawFood && (combinedText.includes("cook") || combinedText.includes("food"))) itemName = rawFood.name;
          }
        }
      }

      if (!itemName) { if (callback) await callback({ text: "COOK_FOOD requires an itemName (raw food)", action: "COOK_FOOD" }); return { success: false, error: "Missing itemName" }; }

      const result = await service.executeAction(runtime.agentId, "cookFood", { itemName });
      logger.info({ action: "COOK_FOOD", itemName, result }, "Executed cookFood");
      if (callback) await callback({ text: result.success ? `Cooked ${itemName}. ${result.message || ""}` : `Failed: ${result.message}`, action: "COOK_FOOD" });
      return { success: result.success, text: result.message || "", data: { actionName: "cookFood", itemName, result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "COOK_FOOD" });
      return { success: false, error: msg };
    }
  },
};
export default cookFoodAction;
