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

const equipItemAction: Action = {
  name: "EQUIP_ITEM",
  similes: ["WEAR_ITEM", "WIELD_ITEM", "EQUIP", "WEAR"],
  description:
    "Equip a weapon, armor, or accessory from your inventory. Use when you have a better weapon/armor to wear, or need to equip tools. REQUIRES: the item must be in your inventory (not already equipped). Do NOT use if the item is already equipped (check Equipment section). Do NOT use for non-equippable items.",
  parameters: [
    {
      name: "itemName",
      description: "The name of the item to equip (e.g., 'Bronze sword', 'Iron platebody')",
      required: true,
      schema: { type: "string" },
    },
  ],
  examples: [
    [
      {
        name: "{{user1}}",
        content: { text: "Equip my bronze sword" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Equipping the bronze sword.", action: "EQUIP_ITEM" },
      },
    ],
    [
      {
        name: "{{user1}}",
        content: { text: "Wear the iron armor" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Putting on the iron armor.", action: "EQUIP_ITEM" },
      },
    ],
  ] as ActionExample[][],

  validate: async (runtime: IAgentRuntime): Promise<boolean> => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    return state.inventory.length > 0; // must have items to equip
  },

  handler: async (
    runtime: IAgentRuntime,
    message: Memory,
    state: State | undefined,
    options?: HandlerOptions,
    callback?: HandlerCallback
  ): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) {
        logger.error("RS-SDK service not found");
        if (callback) {
          await callback({
            text: "Cannot equip item - RS-SDK service not available.",
            action: "EQUIP_ITEM",
          });
        }
        return { success: false, error: "RS-SDK service not found" };
      }

      // Try to extract itemName from multiple sources
      let itemName: string | undefined;

      // Try options.parameters first
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.itemName && typeof params.itemName === "string") {
        itemName = params.itemName;
      }

      // Fallback: try to extract item name from the LLM's response
      if (!itemName) {
        const messageText = (message.content as { text?: string })?.text || "";
        const llmResponse = getCurrentLlmResponse();
        const textToSearch = `${llmResponse} ${messageText}`;
        
        const itemPatterns = [
          /equip\s+(?:my\s+|the\s+|a\s+|an\s+)?(\w+(?:\s+\w+)?(?:\s+\w+)?)/i,
          /wear\s+(?:my\s+|the\s+|a\s+|an\s+)?(\w+(?:\s+\w+)?(?:\s+\w+)?)/i,
          /wield\s+(?:my\s+|the\s+|a\s+|an\s+)?(\w+(?:\s+\w+)?(?:\s+\w+)?)/i,
          /put\s+on\s+(?:my\s+|the\s+|a\s+|an\s+)?(\w+(?:\s+\w+)?(?:\s+\w+)?)/i,
        ];

        for (const pattern of itemPatterns) {
          const match = textToSearch.match(pattern);
          if (match?.[1]) {
            itemName = match[1].trim();
            break;
          }
        }
      }

      // Final fallback: check for common equipment names in text (word boundary match)
      if (!itemName) {
        const textLower = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const commonItems = [
          "bronze sword", "bronze dagger", "bronze axe", "bronze pickaxe",
          "iron sword", "iron dagger", "iron axe", "iron pickaxe",
          "wooden shield", "bronze shield", "leather armor", "bronze armor",
          "shortbow", "longbow", "staff", "bronze scimitar", "iron scimitar"
        ];
        for (const item of commonItems) {
          const wordBoundary = new RegExp(`\\b${item}\\b`, "i");
          if (wordBoundary.test(textLower)) {
            itemName = item.split(" ").map(w => w.charAt(0).toUpperCase() + w.slice(1)).join(" ");
            break;
          }
        }
      }

      if (!itemName) {
        const errorMsg = "EQUIP_ITEM requires an itemName - please specify what to equip (e.g., 'equip Bronze dagger')";
        logger.warn({ params }, errorMsg);
        if (callback) {
          await callback({ text: errorMsg, action: "EQUIP_ITEM" });
        }
        return { success: false, error: errorMsg };
      }

      const result = await service.executeAction(runtime.agentId, "equipItem", { itemName });

      logger.info({ action: "EQUIP_ITEM", itemName, result }, "Executed equipItem action");

      if (callback) {
        await callback({
          text: result.success
            ? `Equipped ${itemName}. ${result.message || ""}`
            : `Failed to equip ${itemName}: ${result.message}`,
          action: "EQUIP_ITEM",
        });
      }

      return {
        success: result.success,
        text: result.message || (result.success ? `Equipped ${itemName}` : `Failed to equip ${itemName}`),
        data: { actionName: "equipItem", itemName, result },
      };
    } catch (error) {
      logger.error({ error }, "Failed to execute EQUIP_ITEM action");
      const errorMessage = error instanceof Error ? error.message : "Unknown error";
      if (callback) {
        await callback({
          text: `Error equipping item: ${errorMessage}`,
          action: "EQUIP_ITEM",
        });
      }
      return { success: false, error: errorMessage };
    }
  },
};

export default equipItemAction;
