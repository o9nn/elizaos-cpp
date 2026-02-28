import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";

const talkToNpcAction: Action = {
  name: "TALK_TO_NPC",
  similes: ["SPEAK_TO_NPC", "CHAT_WITH_NPC", "TALK_NPC"],
  description: "Talk to a nearby NPC to start a conversation or quest dialog. Use for quests, getting information, or initiating trades. Do NOT use for combat (use ATTACK_NPC), for shops (use OPEN_SHOP), or for banking (use OPEN_BANK). The NPC must be visible in the nearby NPCs list.",
  parameters: [
    { name: "npcName", description: "The name of the NPC to talk to (e.g., 'Cook', 'Hans', 'Shop keeper')", required: true, schema: { type: "string" } },
  ],
  examples: [
    [{ name: "{{user1}}", content: { text: "Talk to the cook" } }, { name: "{{agentName}}", content: { text: "Talking to the Cook.", action: "TALK_TO_NPC" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => Boolean(runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME)),
  handler: async (runtime, message, _state, options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "TALK_TO_NPC" }); return { success: false, error: "RS-SDK service not found" }; }

      let npcName: string | undefined;
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.npcName && typeof params.npcName === "string") npcName = params.npcName;

      if (!npcName) {
        const combinedText = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const botState = await service.getBotState(runtime.agentId);
        if (botState?.nearbyNpcs) {
          for (const npc of botState.nearbyNpcs) {
            if (combinedText.includes(npc.name.toLowerCase())) { npcName = npc.name; break; }
          }
        }
      }

      if (!npcName) { if (callback) await callback({ text: "TALK_TO_NPC requires an npcName", action: "TALK_TO_NPC" }); return { success: false, error: "Missing npcName" }; }

      const result = await service.executeAction(runtime.agentId, "talkToNpc", { npcName });
      logger.info({ action: "TALK_TO_NPC", npcName, result }, "Executed talkToNpc");
      if (callback) await callback({ text: result.success ? `Talking to ${npcName}. ${result.message || ""}` : `Failed: ${result.message}`, action: "TALK_TO_NPC" });
      return { success: result.success, text: result.message || "", data: { actionName: "talkToNpc", npcName, result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "TALK_TO_NPC" });
      return { success: false, error: msg };
    }
  },
};
export default talkToNpcAction;
