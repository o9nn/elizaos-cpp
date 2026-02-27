import {
  type Action,
  type ActionExample,
  type ActionResult,
  type HandlerCallback,
  type HandlerOptions,
  type IAgentRuntime,
  logger,
  type Memory,
  type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";

const useItemAction: Action = {
  name: "USE_ITEM",
  similes: ["USE", "ACTIVATE_ITEM", "CLICK_ITEM"],
  description:
    "Use an inventory item's primary action (e.g., Bury bones for Prayer XP, Drink potion, Read scroll). Do NOT use for eating food (use EAT_FOOD), equipping gear (use EQUIP_ITEM), dropping items (use DROP_ITEM), or combining items (use USE_ITEM_ON_ITEM). The item must be in your inventory.",
  parameters: [
    {
      name: "itemName",
      description: "The name of the item to use (e.g., 'Bones', 'Tinderbox', 'Rope')",
      required: true,
      schema: { type: "string" },
    },
  ],
  examples: [
    [
      {
        name: "{{user1}}",
        content: { text: "Bury the bones" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Using the bones to bury them.", action: "USE_ITEM" },
      },
    ],
    [
      {
        name: "{{user1}}",
        content: { text: "Use the tinderbox" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Using the tinderbox.", action: "USE_ITEM" },
      },
    ],
  ] as ActionExample[][],

  validate: async (runtime: IAgentRuntime): Promise<boolean> => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    return state.inventory.length > 0; // must have items to use
  },

  handler: async (
    runtime: IAgentRuntime,
    message: Memory,
    _state: State | undefined,
    options?: HandlerOptions,
    callback?: HandlerCallback
  ): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) {
        logger.error("RS-SDK service not found");
        if (callback) {
          await callback({ text: "Cannot use item - RS-SDK service not available.", action: "USE_ITEM" });
        }
        return { success: false, error: "RS-SDK service not found" };
      }

      let itemName: string | undefined;

      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.itemName && typeof params.itemName === "string") {
        itemName = params.itemName;
      }

      // Fallback: search LLM response for known inventory items (most reliable)
      if (!itemName) {
        const llmResponse = getCurrentLlmResponse();
        const combinedText = `${llmResponse} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();

        // Match known items from actual inventory
        const service2 = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
        if (service2) {
          const botState = await service2.getBotState(runtime.agentId);
          if (botState?.inventory) {
            for (const invItem of botState.inventory) {
              const itemLower = invItem.name.toLowerCase();
              if (combinedText.includes(itemLower)) {
                itemName = invItem.name;
                break;
              }
            }
          }
        }
      }

      // Second fallback: common usable items by word boundary
      if (!itemName) {
        const textLower = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const usableItems = ["bones", "tinderbox", "rope", "pot", "bucket", "jug", "hammer", "chisel", "knife"];
        for (const item of usableItems) {
          const wordBoundary = new RegExp(`\\b${item}\\b`, "i");
          if (wordBoundary.test(textLower)) {
            itemName = item.charAt(0).toUpperCase() + item.slice(1);
            break;
          }
        }
      }

      if (!itemName) {
        const errorMsg = "USE_ITEM requires an itemName - specify what to use (e.g., 'use Bones')";
        logger.warn({ params }, errorMsg);
        if (callback) {
          await callback({ text: errorMsg, action: "USE_ITEM" });
        }
        return { success: false, error: errorMsg };
      }

      const result = await service.executeAction(runtime.agentId, "useItem", { itemName });

      logger.info({ action: "USE_ITEM", itemName, result }, "Executed useItem action");

      if (callback) {
        await callback({
          text: result.success
            ? `Used ${itemName}. ${result.message || ""}`
            : `Failed to use ${itemName}: ${result.message}`,
          action: "USE_ITEM",
        });
      }

      return {
        success: result.success,
        text: result.message || (result.success ? `Used ${itemName}` : `Failed to use ${itemName}`),
        data: { actionName: "useItem", itemName, result },
      };
    } catch (error) {
      logger.error({ error }, "Failed to execute USE_ITEM action");
      const errorMessage = error instanceof Error ? error.message : "Unknown error";
      if (callback) {
        await callback({ text: `Error using item: ${errorMessage}`, action: "USE_ITEM" });
      }
      return { success: false, error: errorMessage };
    }
  },
};

export default useItemAction;
