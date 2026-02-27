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

const useItemOnObjectAction: Action = {
  name: "USE_ITEM_ON_OBJECT",
  similes: ["ITEM_ON_OBJECT", "USE_ON_LOC", "SMELT_ORE", "USE_ON"],
  description:
    "Use an inventory item on a nearby world object. Essential for: smelting ore at a Furnace, using items on Anvils, using items on Spinning wheels, filling containers at Wells/Fountains, or any action that combines an inventory item with a world object. Do NOT use for cooking (use COOK_FOOD), smithing bars (use SMITH_AT_ANVIL), or combining two inventory items (use USE_ITEM_ON_ITEM). The item must be in your inventory and the object must be in the Nearby Objects list.",
  parameters: [
    {
      name: "itemName",
      description: "The name of the inventory item to use (e.g., 'Copper ore', 'Bronze bar', 'Wool')",
      required: true,
      schema: { type: "string" },
    },
    {
      name: "objectName",
      description: "The name of the world object to use the item on (e.g., 'Furnace', 'Spinning wheel', 'Well')",
      required: true,
      schema: { type: "string" },
    },
  ],
  examples: [
    [
      { name: "{{user1}}", content: { text: "Use the copper ore on the furnace" } },
      { name: "{{agentName}}", content: { text: "Using copper ore on the furnace to smelt.", action: "USE_ITEM_ON_OBJECT" } },
    ],
    [
      { name: "{{user1}}", content: { text: "Smelt the ore at the furnace" } },
      { name: "{{agentName}}", content: { text: "Smelting ore at the furnace.", action: "USE_ITEM_ON_OBJECT" } },
    ],
    [
      { name: "{{user1}}", content: { text: "Spin the wool on the spinning wheel" } },
      { name: "{{agentName}}", content: { text: "Using wool on the spinning wheel.", action: "USE_ITEM_ON_OBJECT" } },
    ],
  ] as ActionExample[][],

  validate: async (runtime: IAgentRuntime): Promise<boolean> => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true; // allow if can't check state yet
    // Must have items in inventory and objects nearby
    const hasItems = state.inventory.length > 0;
    const hasObjects = state.nearbyLocs.length > 0;
    return hasItems && hasObjects;
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
        if (callback) await callback({ text: "RS-SDK service not available.", action: "USE_ITEM_ON_OBJECT" });
        return { success: false, error: "RS-SDK service not found" };
      }

      let itemName: string | undefined;
      let objectName: string | undefined;

      // Try parameters first
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.itemName && typeof params.itemName === "string") itemName = params.itemName;
      if (params?.objectName && typeof params.objectName === "string") objectName = params.objectName;

      // Fallback: match against actual inventory items and nearby objects
      if (!itemName || !objectName) {
        const combinedText = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const botState = await service.getBotState(runtime.agentId);

        if (!itemName && botState?.inventory) {
          for (const invItem of botState.inventory) {
            if (combinedText.includes(invItem.name.toLowerCase())) {
              itemName = invItem.name;
              break;
            }
          }
        }

        if (!objectName && botState?.nearbyLocs) {
          for (const loc of botState.nearbyLocs) {
            if (combinedText.includes(loc.name.toLowerCase())) {
              objectName = loc.name;
              break;
            }
          }
        }
      }

      // Second fallback: common smelting patterns
      if (!itemName || !objectName) {
        const textLower = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        if (!objectName) {
          const objects = ["furnace", "spinning wheel", "well", "fountain", "anvil", "range", "pottery wheel", "potter's wheel"];
          for (const obj of objects) {
            if (new RegExp(`\\b${obj}\\b`, "i").test(textLower)) {
              objectName = obj.split(" ").map(w => w.charAt(0).toUpperCase() + w.slice(1)).join(" ");
              break;
            }
          }
        }
        if (!itemName) {
          const ores = ["copper ore", "tin ore", "iron ore", "coal", "gold ore", "mithril ore", "adamantite ore", "runite ore", "bronze bar", "iron bar", "wool", "clay", "bucket"];
          for (const ore of ores) {
            if (new RegExp(`\\b${ore}\\b`, "i").test(textLower)) {
              itemName = ore.split(" ").map(w => w.charAt(0).toUpperCase() + w.slice(1)).join(" ");
              break;
            }
          }
        }
      }

      if (!itemName) {
        const errorMsg = "USE_ITEM_ON_OBJECT requires itemName - specify which item to use (e.g., 'use Copper ore on Furnace')";
        if (callback) await callback({ text: errorMsg, action: "USE_ITEM_ON_OBJECT" });
        return { success: false, error: errorMsg };
      }
      if (!objectName) {
        const errorMsg = "USE_ITEM_ON_OBJECT requires objectName - specify which object to use item on (e.g., 'use Copper ore on Furnace')";
        if (callback) await callback({ text: errorMsg, action: "USE_ITEM_ON_OBJECT" });
        return { success: false, error: errorMsg };
      }

      const result = await service.executeAction(runtime.agentId, "useItemOnObject", { itemName, objectName });

      logger.info({ action: "USE_ITEM_ON_OBJECT", itemName, objectName, result }, "Executed useItemOnObject");

      if (callback) {
        await callback({
          text: result.success
            ? `Used ${itemName} on ${objectName}. ${result.message || ""}`
            : `Failed: ${result.message}`,
          action: "USE_ITEM_ON_OBJECT",
        });
      }

      return {
        success: result.success,
        text: result.message || "",
        data: { actionName: "useItemOnObject", itemName, objectName, result },
      };
    } catch (error) {
      const errorMessage = error instanceof Error ? error.message : "Unknown error";
      logger.error({ error }, "Failed to execute USE_ITEM_ON_OBJECT action");
      if (callback) await callback({ text: `Error: ${errorMessage}`, action: "USE_ITEM_ON_OBJECT" });
      return { success: false, error: errorMessage };
    }
  },
};

export default useItemOnObjectAction;
