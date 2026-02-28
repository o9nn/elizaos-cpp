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

const openBankAction: Action = {
  name: "OPEN_BANK",
  similes: ["USE_BANK", "ACCESS_BANK", "BANK"],
  description:
    "Open the nearest bank booth or banker NPC to access your bank. Use when you need to deposit items (full inventory) or withdraw supplies. Do NOT use if no bank booth or banker is visible in nearby objects/NPCs. After opening, use DEPOSIT_ITEM or WITHDRAW_ITEM, then CLOSE_BANK when done. IMPORTANT: There is NO bank in Lumbridge in 2004scape! Nearest banks: Draynor Village (3092, 3243), Al Kharid (3269, 3167), Varrock West (3185, 3436).",
  examples: [
    [
      {
        name: "{{user1}}",
        content: { text: "Open the bank" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Opening the bank now.", action: "OPEN_BANK" },
      },
    ],
    [
      {
        name: "{{user1}}",
        content: { text: "I need to access my bank" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Heading to the bank booth.", action: "OPEN_BANK" },
      },
    ],
  ] as ActionExample[][],

  validate: async (runtime: IAgentRuntime): Promise<boolean> => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    // Note: we don't check for nearby bank because the bot might need to walk to one
    return true;
  },

  handler: async (
    runtime: IAgentRuntime,
    message: Memory,
    _state: State | undefined,
    _options?: HandlerOptions,
    callback?: HandlerCallback
  ): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) {
        logger.error("RS-SDK service not found");
        if (callback) {
          await callback({
            text: "Cannot open bank - RS-SDK service not available.",
            action: "OPEN_BANK",
          });
        }
        return { success: false, error: "RS-SDK service not found" };
      }

      const result = await service.executeAction(runtime.agentId, "openBank", {});

      logger.info({ action: "OPEN_BANK", result }, "Executed openBank action");

      if (callback) {
        await callback({
          text: result.success
            ? `Opened the bank. ${result.message || ""}`
            : `Failed to open bank: ${result.message}`,
          action: "OPEN_BANK",
        });
      }

      return {
        success: result.success,
        text: result.message || (result.success ? "Opened bank" : "Failed to open bank"),
        data: { actionName: "openBank", result },
      };
    } catch (error) {
      logger.error({ error }, "Failed to execute OPEN_BANK action");
      const errorMessage = error instanceof Error ? error.message : "Unknown error";
      if (callback) {
        await callback({
          text: `Error opening bank: ${errorMessage}`,
          action: "OPEN_BANK",
        });
      }
      return { success: false, error: errorMessage };
    }
  },
};

export default openBankAction;
