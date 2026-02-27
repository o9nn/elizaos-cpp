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

const dropItemAction: Action = {
  name: "DROP_ITEM",
  similes: ["DROP", "DISCARD_ITEM", "THROW_AWAY"],
  description:
    "Drop an item from inventory onto the ground to free up space. Use when inventory is full and you need room for new items. Do NOT drop tools you need (axe, pickaxe, tinderbox). Prefer banking over dropping if a bank is nearby. Specify the exact item name from your inventory.",
  parameters: [
    {
      name: "itemName",
      description: "The name of the item to drop (e.g., 'Logs', 'Bones', 'Bronze dagger')",
      required: true,
      schema: { type: "string" },
    },
  ],
  examples: [
    [
      {
        name: "{{user1}}",
        content: { text: "Drop the logs" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Dropping the logs from inventory.", action: "DROP_ITEM" },
      },
    ],
    [
      {
        name: "{{user1}}",
        content: { text: "Discard the bones" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Dropping bones to free up space.", action: "DROP_ITEM" },
      },
    ],
  ] as ActionExample[][],

  validate: async (runtime: IAgentRuntime): Promise<boolean> => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    return state.inventory.length > 0; // must have items to drop
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
          await callback({ text: "Cannot drop item - RS-SDK service not available.", action: "DROP_ITEM" });
        }
        return { success: false, error: "RS-SDK service not found" };
      }

      let itemName: string | undefined;

      // Try options.parameters first
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.itemName && typeof params.itemName === "string") {
        itemName = params.itemName;
      }

      // Fallback: search LLM response for known inventory items (most reliable)
      if (!itemName) {
        const llmResponse = getCurrentLlmResponse();
        const combinedText = `${llmResponse} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();

        // First try: match known droppable items from inventory via the service
        const service2 = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
        if (service2) {
          const botState = await service2.getBotState(runtime.agentId);
          if (botState?.inventory) {
            // Check if the LLM mentions any actual inventory item by name
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

      // Second fallback: common droppable items by word boundary
      if (!itemName) {
        const textLower = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const droppableItems = ["logs", "bones", "ashes", "feather", "raw shrimps", "jug", "pot", "bucket", "bread"];
        for (const item of droppableItems) {
          const wordBoundary = new RegExp(`\\b${item}\\b`, "i");
          if (wordBoundary.test(textLower)) {
            itemName = item.split(" ").map(w => w.charAt(0).toUpperCase() + w.slice(1)).join(" ");
            break;
          }
        }
      }

      if (!itemName) {
        const errorMsg = "DROP_ITEM requires an itemName - specify what to drop (e.g., 'drop Logs')";
        logger.warn({ params }, errorMsg);
        if (callback) {
          await callback({ text: errorMsg, action: "DROP_ITEM" });
        }
        return { success: false, error: errorMsg };
      }

      const result = await service.executeAction(runtime.agentId, "dropItem", { itemName });

      logger.info({ action: "DROP_ITEM", itemName, result }, "Executed dropItem action");

      if (callback) {
        await callback({
          text: result.success
            ? `Dropped ${itemName}. ${result.message || ""}`
            : `Failed to drop ${itemName}: ${result.message}`,
          action: "DROP_ITEM",
        });
      }

      return {
        success: result.success,
        text: result.message || (result.success ? `Dropped ${itemName}` : `Failed to drop ${itemName}`),
        data: { actionName: "dropItem", itemName, result },
      };
    } catch (error) {
      logger.error({ error }, "Failed to execute DROP_ITEM action");
      const errorMessage = error instanceof Error ? error.message : "Unknown error";
      if (callback) {
        await callback({ text: `Error dropping item: ${errorMessage}`, action: "DROP_ITEM" });
      }
      return { success: false, error: errorMessage };
    }
  },
};

export default dropItemAction;
