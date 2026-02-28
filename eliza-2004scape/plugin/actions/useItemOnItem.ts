import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";

const useItemOnItemAction: Action = {
  name: "USE_ITEM_ON_ITEM",
  similes: ["COMBINE_ITEMS", "ITEM_ON_ITEM"],
  description: "Combine two inventory items together. Common uses: Knife+Logs (fletching), Tinderbox+Logs (firemaking), Needle+Leather (crafting), Hammer+Nails. REQUIRES: both items must be in your inventory. Do NOT use for specific skills that have dedicated actions (use FLETCH_LOGS, BURN_LOGS, CRAFT_LEATHER instead). Use when no dedicated action exists.",
  parameters: [
    { name: "sourceItem", description: "The item to use (e.g., 'Knife', 'Tinderbox', 'Needle')", required: true, schema: { type: "string" } },
    { name: "targetItem", description: "The item to use it on (e.g., 'Logs', 'Leather', 'Thread')", required: true, schema: { type: "string" } },
  ],
  examples: [
    [{ name: "{{user1}}", content: { text: "Use knife on logs" } }, { name: "{{agentName}}", content: { text: "Using knife on logs to fletch.", action: "USE_ITEM_ON_ITEM" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    return state.inventory.length >= 2; // need at least 2 items to combine
  },
  handler: async (runtime, message, _state, options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "USE_ITEM_ON_ITEM" }); return { success: false, error: "service not found" }; }

      let sourceItem: string | undefined;
      let targetItem: string | undefined;
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.sourceItem) sourceItem = params.sourceItem;
      if (params?.targetItem) targetItem = params.targetItem;

      // Fallback: parse "use X on Y" patterns from text
      if (!sourceItem || !targetItem) {
        const text = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`;
        const match = text.match(/use\s+(?:the\s+|my\s+|a\s+)?(\w+(?:\s+\w+)?)\s+on\s+(?:the\s+|my\s+|a\s+)?(\w+(?:\s+\w+)?)/i);
        if (match?.[1] && match?.[2]) {
          if (!sourceItem) sourceItem = match[1].trim();
          if (!targetItem) targetItem = match[2].trim();
        }
      }

      if (!sourceItem || !targetItem) {
        if (callback) await callback({ text: "USE_ITEM_ON_ITEM requires sourceItem and targetItem (e.g., 'use Knife on Logs')", action: "USE_ITEM_ON_ITEM" });
        return { success: false, error: "Missing item names" };
      }

      const result = await service.executeAction(runtime.agentId, "useItemOnItem", { sourceItem, targetItem });
      logger.info({ action: "USE_ITEM_ON_ITEM", sourceItem, targetItem, result }, "Executed useItemOnItem");
      if (callback) await callback({ text: result.success ? `Used ${sourceItem} on ${targetItem}. ${result.message || ""}` : `Failed: ${result.message}`, action: "USE_ITEM_ON_ITEM" });
      return { success: result.success, text: result.message || "", data: { actionName: "useItemOnItem", sourceItem, targetItem, result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "USE_ITEM_ON_ITEM" });
      return { success: false, error: msg };
    }
  },
};
export default useItemOnItemAction;
