import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";

const interactObjectAction: Action = {
  name: "INTERACT_OBJECT",
  similes: ["USE_OBJECT", "CLICK_OBJECT", "INTERACT_LOC"],
  description: "Interact with a nearby world object using a specific option or its first option (e.g., climb Ladder, pray at Altar, walk-across Log balance, squeeze-through Obstacle pipe). Use for objects not covered by specific actions. Do NOT use for trees (use CHOP_TREE), rocks (use MINE_ROCK), doors (use OPEN_DOOR), NPCs (use TALK_TO_NPC/ATTACK_NPC), or using items on objects (use USE_ITEM_ON_OBJECT). The object must appear in the Nearby Objects list. Optionally specify the exact option text when the object has multiple options (e.g., 'Climb' vs 'Climb-down' on a Tree branch).",
  parameters: [
    { name: "objectName", description: "Name of the object to interact with (e.g., 'Ladder', 'Staircase', 'Log balance', 'Obstacle net')", required: true, schema: { type: "string" } },
    { name: "optionText", description: "Optional: specific option to use if the object has multiple options (e.g., 'Climb-down', 'Walk-across', 'Squeeze-through'). If omitted, uses the first available option.", required: false, schema: { type: "string" } },
  ],
  examples: [
    [{ name: "{{user1}}", content: { text: "Climb the ladder" } }, { name: "{{agentName}}", content: { text: "Climbing the ladder.", action: "INTERACT_OBJECT" } }],
    [{ name: "{{user1}}", content: { text: "Use the spinning wheel" } }, { name: "{{agentName}}", content: { text: "Using the spinning wheel.", action: "INTERACT_OBJECT" } }],
    [{ name: "{{user1}}", content: { text: "Walk across the log balance" } }, { name: "{{agentName}}", content: { text: "Walking across the log balance.", action: "INTERACT_OBJECT" } }],
    [{ name: "{{user1}}", content: { text: "Climb down the tree branch" } }, { name: "{{agentName}}", content: { text: "Climbing down the tree branch.", action: "INTERACT_OBJECT" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    return state.nearbyLocs.length > 0; // must have objects nearby
  },
  handler: async (runtime, message, _state, options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "INTERACT_OBJECT" }); return { success: false, error: "service not found" }; }

      let objectName: string | undefined;
      let optionText: string | undefined;
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.objectName) objectName = params.objectName;
      if (params?.optionText) optionText = params.optionText;

      if (!objectName) {
        const combinedText = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const botState = await service.getBotState(runtime.agentId);
        if (botState?.nearbyLocs) {
          for (const loc of botState.nearbyLocs) {
            if (combinedText.includes(loc.name.toLowerCase())) { objectName = loc.name; break; }
          }
        }
      }

      // Try to extract option text from LLM response if not provided
      if (!optionText) {
        const combinedText = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const knownOptions = ["climb-down", "climb-over", "climb-up", "climb", "walk-across", "walk-on", "squeeze-through", "cross", "jump", "balance", "pray", "search", "open", "close"];
        for (const opt of knownOptions) {
          if (combinedText.includes(opt)) { optionText = opt; break; }
        }
      }

      if (!objectName) { if (callback) await callback({ text: "INTERACT_OBJECT requires objectName", action: "INTERACT_OBJECT" }); return { success: false, error: "Missing objectName" }; }

      const actionParams: Record<string, string> = { objectName };
      if (optionText) actionParams.optionText = optionText;
      const result = await service.executeAction(runtime.agentId, "interactObject", actionParams);
      logger.info({ action: "INTERACT_OBJECT", objectName, result }, "Executed interactObject");
      if (callback) await callback({ text: result.success ? `Interacted with ${objectName}. ${result.message || ""}` : `Failed: ${result.message}`, action: "INTERACT_OBJECT" });
      return { success: result.success, text: result.message || "", data: { actionName: "interactObject", objectName, result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "INTERACT_OBJECT" });
      return { success: false, error: msg };
    }
  },
};
export default interactObjectAction;
