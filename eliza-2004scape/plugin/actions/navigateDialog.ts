import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";

const navigateDialogAction: Action = {
  name: "NAVIGATE_DIALOG",
  similes: ["ANSWER_DIALOG", "DIALOG_CHOICE", "QUEST_DIALOG", "CLICK_DIALOG"],
  description: "Click through dialog pages or choose a specific dialog option. Use ONLY when a dialog box is currently open on screen (after TALK_TO_NPC, during a quest, or to dismiss level-up messages). IMPORTANT: optionIndex 0 = special 'click here to continue' action for progressing through dialog text. optionIndex 1-5 = actual choice buttons when the dialog presents numbered options. Always dismiss blocking dialogs (level-up congratulations) with option 0 before attempting other actions. For multi-step NPC conversations (like paying Al Kharid toll or buying kebabs from Karim), use 0 to continue until you see specific choices, then select the right option number.",
  parameters: [
    { name: "optionIndex", description: "0 = 'click to continue' (advance dialog text). 1-5 = select a specific choice button. Use 0 when dialog shows 'Click here to continue' or to dismiss level-up messages. Use 1-5 for actual choices like 'Yes'/'No'.", required: false, schema: { type: "number" } },
  ],
  examples: [
    [{ name: "{{user1}}", content: { text: "Click through the dialog" } }, { name: "{{agentName}}", content: { text: "Clicking through the dialog.", action: "NAVIGATE_DIALOG" } }],
    [{ name: "{{user1}}", content: { text: "Choose option 2 in the dialog" } }, { name: "{{agentName}}", content: { text: "Selecting option 2.", action: "NAVIGATE_DIALOG" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => Boolean(runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME)),
  handler: async (runtime, message, _state, options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "NAVIGATE_DIALOG" }); return { success: false, error: "service not found" }; }

      let optionIndex = -1;
      const params = options?.parameters as Record<string, number> | undefined;
      if (typeof params?.optionIndex === "number") optionIndex = params.optionIndex;

      // Try to extract option number from LLM response
      if (optionIndex === -1) {
        const text = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`;
        const match = text.match(/option\s*(\d+)/i) || text.match(/choice\s*(\d+)/i) || text.match(/select\s*(\d+)/i);
        if (match?.[1]) optionIndex = parseInt(match[1], 10);
      }

      const result = await service.executeAction(runtime.agentId, "navigateDialog", { optionIndex });
      logger.info({ action: "NAVIGATE_DIALOG", optionIndex, result }, "Executed navigateDialog");
      if (callback) await callback({ text: result.success ? `${result.message || "Dialog navigated."}` : `Failed: ${result.message}`, action: "NAVIGATE_DIALOG" });
      return { success: result.success, text: result.message || "", data: { actionName: "navigateDialog", result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "NAVIGATE_DIALOG" });
      return { success: false, error: msg };
    }
  },
};
export default navigateDialogAction;
