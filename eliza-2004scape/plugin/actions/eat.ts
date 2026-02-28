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

const eatAction: Action = {
  name: "EAT_FOOD",
  similes: ["EAT", "CONSUME_FOOD", "HEAL"],
  description:
    "Eat food from inventory to restore hitpoints. Use when HP is low or during combat to heal. REQUIRES: edible food in inventory (Bread, Shrimp, Trout, etc.). Do NOT use if inventory has no food. Do NOT use if HP is already full. For raw food, use COOK_FOOD first.",
  parameters: [
    {
      name: "foodName",
      description: "The name of the food to eat (e.g., 'Shrimp', 'Bread', 'Lobster')",
      required: true,
      schema: { type: "string" },
    },
  ],
  examples: [
    [
      {
        name: "{{user1}}",
        content: { text: "Eat some food to heal" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Eating food to restore health.", action: "EAT_FOOD" },
      },
    ],
    [
      {
        name: "{{user1}}",
        content: { text: "Consume a lobster" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Eating the lobster.", action: "EAT_FOOD" },
      },
    ],
  ] as ActionExample[][],

  validate: async (runtime: IAgentRuntime): Promise<boolean> => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    const knownFoods = ["shrimp", "anchovies", "bread", "cooked", "trout", "salmon", "tuna", "lobster", "swordfish", "shark", "cake", "pie", "meat"];
    const hasFood = state.inventory.some(i => knownFoods.some(f => i.name.toLowerCase().includes(f)));
    return hasFood;
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
            text: "Cannot eat - RS-SDK service not available.",
            action: "EAT_FOOD",
          });
        }
        return { success: false, error: "RS-SDK service not found" };
      }

      // Try to extract foodName from multiple sources
      let foodName: string | undefined;

      // Try options.parameters first
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.foodName && typeof params.foodName === "string") {
        foodName = params.foodName;
      }

      // Fallback: try to extract food name from the LLM's response
      if (!foodName) {
        const messageText = (message.content as { text?: string })?.text || "";
        const llmResponse = getCurrentLlmResponse();
        const textToSearch = `${llmResponse} ${messageText}`;
        
        const foodPatterns = [
          /eat\s+(?:a\s+|an\s+|the\s+|some\s+|my\s+)?(\w+(?:\s+\w+)?)/i,
          /consume\s+(?:a\s+|an\s+|the\s+|some\s+|my\s+)?(\w+(?:\s+\w+)?)/i,
          /heal\s+with\s+(?:a\s+|an\s+|the\s+|some\s+|my\s+)?(\w+(?:\s+\w+)?)/i,
        ];

        for (const pattern of foodPatterns) {
          const match = textToSearch.match(pattern);
          if (match?.[1]) {
            foodName = match[1].trim();
            break;
          }
        }
      }

      // Final fallback: check for common food names in text (word boundary match)
      if (!foodName) {
        const textLower = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const commonFoods = [
          "shrimp", "anchovies", "bread", "cooked meat", "cooked chicken",
          "trout", "salmon", "tuna", "lobster", "swordfish", "shark",
          "cake", "apple", "banana", "cabbage", "potato"
        ];
        for (const food of commonFoods) {
          const wordBoundary = new RegExp(`\\b${food}\\b`, "i");
          if (wordBoundary.test(textLower)) {
            foodName = food.split(" ").map(w => w.charAt(0).toUpperCase() + w.slice(1)).join(" ");
            break;
          }
        }
      }

      if (!foodName) {
        const errorMsg = "EAT_FOOD requires a foodName - please specify what to eat (e.g., 'eat Shrimp')";
        logger.warn({ params }, errorMsg);
        if (callback) {
          await callback({ text: errorMsg, action: "EAT_FOOD" });
        }
        return { success: false, error: errorMsg };
      }

      const result = await service.executeAction(runtime.agentId, "eat", { foodName });

      logger.info({ action: "EAT_FOOD", foodName, result }, "Executed eat action");

      if (callback) {
        await callback({
          text: result.success
            ? `Ate ${foodName}. ${result.message || ""}`
            : `Failed to eat ${foodName}: ${result.message}`,
          action: "EAT_FOOD",
        });
      }

      return {
        success: result.success,
        text: result.message || (result.success ? `Ate ${foodName}` : `Failed to eat ${foodName}`),
        data: { actionName: "eat", foodName, result },
      };
    } catch (error) {
      logger.error({ error }, "Failed to execute EAT_FOOD action");
      const errorMessage = error instanceof Error ? error.message : "Unknown error";
      if (callback) {
        await callback({
          text: `Error eating food: ${errorMessage}`,
          action: "EAT_FOOD",
        });
      }
      return { success: false, error: errorMessage };
    }
  },
};

export default eatAction;
