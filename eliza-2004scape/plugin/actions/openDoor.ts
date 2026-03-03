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

const openDoorAction: Action = {
  name: "OPEN_DOOR",
  similes: ["USE_DOOR", "OPEN_GATE", "ENTER_DOOR"],
  description:
    "Open the nearest closed door or gate, walking to it if needed. Use when a door is blocking your path and you need to pass through. Do NOT use if there is no door nearby or if the door is already open. The bot auto-detects the nearest door/gate.",
  examples: [
    [
      {
        name: "{{user1}}",
        content: { text: "Open that door" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Opening the nearby door.", action: "OPEN_DOOR" },
      },
    ],
    [
      {
        name: "{{user1}}",
        content: { text: "I need to get through the gate" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Opening the gate to pass through.", action: "OPEN_DOOR" },
      },
    ],
  ] as ActionExample[][],

  validate: async (runtime: IAgentRuntime): Promise<boolean> => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    // Check if any door or gate is in nearby objects
    return state.nearbyLocs.some(l => /door|gate/i.test(l.name));
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
            text: "Cannot open door - RS-SDK service not available.",
            action: "OPEN_DOOR",
          });
        }
        return { success: false, error: "RS-SDK service not found" };
      }

      const result = await service.executeAction(runtime.agentId, "openDoor", {});

      logger.info({ action: "OPEN_DOOR", result }, "Executed openDoor action");

      if (callback) {
        await callback({
          text: result.success
            ? `${result.message || "Opened the door."}`
            : `Failed to open door: ${result.message}`,
          action: "OPEN_DOOR",
        });
      }

      return {
        success: result.success,
        text: result.message || (result.success ? "Opened door" : "Failed to open door"),
        data: { actionName: "openDoor", result },
      };
    } catch (error) {
      logger.error({ error }, "Failed to execute OPEN_DOOR action");
      const errorMessage = error instanceof Error ? error.message : "Unknown error";
      if (callback) {
        await callback({
          text: `Error opening door: ${errorMessage}`,
          action: "OPEN_DOOR",
        });
      }
      return { success: false, error: errorMessage };
    }
  },
};

export default openDoorAction;
