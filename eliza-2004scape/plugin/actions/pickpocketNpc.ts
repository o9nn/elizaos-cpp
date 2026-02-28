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

const pickpocketNpcAction: Action = {
  name: "PICKPOCKET_NPC",
  similes: ["PICKPOCKET", "STEAL_FROM", "THIEVE"],
  description:
    "Pickpocket a nearby NPC to train Thieving and earn coins. The NPC must have a 'Pickpocket' option (Men, Women, Farmers, Guards, etc.). You will be stunned briefly on failure (takes ~5 seconds). Do NOT use if HP is critically low (stun damage can kill you). Do NOT use on NPCs that don't appear in the Nearby NPCs list. Higher Thieving level unlocks more NPCs and reduces failure rate.",
  parameters: [
    {
      name: "npcName",
      description: "The name of the NPC to pickpocket (e.g., 'Man', 'Woman', 'Farmer', 'Guard')",
      required: true,
      schema: { type: "string" },
    },
  ],
  examples: [
    [
      { name: "{{user1}}", content: { text: "Pickpocket the man" } },
      { name: "{{agentName}}", content: { text: "Pickpocketing the man.", action: "PICKPOCKET_NPC" } },
    ],
    [
      { name: "{{user1}}", content: { text: "Steal from the farmer" } },
      { name: "{{agentName}}", content: { text: "Attempting to pickpocket the farmer.", action: "PICKPOCKET_NPC" } },
    ],
    [
      { name: "{{user1}}", content: { text: "Train thieving on the woman" } },
      { name: "{{agentName}}", content: { text: "Pickpocketing the woman for Thieving XP.", action: "PICKPOCKET_NPC" } },
    ],
  ] as ActionExample[][],

  validate: async (runtime: IAgentRuntime): Promise<boolean> => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true; // allow if can't check state yet
    // Must have NPCs nearby and HP not critically low (stun deals damage)
    const hasNpcs = state.nearbyNpcs.length > 0;
    const hpSkill = state.skills.find(s => s.name === "Hitpoints");
    const hpOk = !hpSkill || hpSkill.level > 3;
    return hasNpcs && hpOk;
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
        if (callback) await callback({ text: "RS-SDK service not available.", action: "PICKPOCKET_NPC" });
        return { success: false, error: "RS-SDK service not found" };
      }

      // Extract npcName from parameters or LLM text
      let npcName: string | undefined;

      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.npcName && typeof params.npcName === "string") {
        npcName = params.npcName;
      }

      // Fallback: match against actual nearby NPCs from game state
      if (!npcName) {
        const combinedText = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const botState = await service.getBotState(runtime.agentId);
        if (botState?.nearbyNpcs) {
          for (const npc of botState.nearbyNpcs) {
            if (combinedText.includes(npc.name.toLowerCase())) {
              npcName = npc.name;
              break;
            }
          }
        }
      }

      // Second fallback: common pickpocket targets
      if (!npcName) {
        const textLower = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const pickpocketTargets = ["man", "woman", "farmer", "guard", "knight", "master farmer"];
        for (const target of pickpocketTargets) {
          if (new RegExp(`\\b${target}\\b`, "i").test(textLower)) {
            npcName = target.split(" ").map(w => w.charAt(0).toUpperCase() + w.slice(1)).join(" ");
            break;
          }
        }
      }

      if (!npcName) {
        const errorMsg = "PICKPOCKET_NPC requires an npcName - specify who to pickpocket (e.g., 'pickpocket the Man')";
        if (callback) await callback({ text: errorMsg, action: "PICKPOCKET_NPC" });
        return { success: false, error: errorMsg };
      }

      const result = await service.executeAction(runtime.agentId, "pickpocketNpc", { npcName });

      logger.info({ action: "PICKPOCKET_NPC", npcName, result }, "Executed pickpocketNpc");

      if (callback) {
        await callback({
          text: result.success
            ? `Pickpocketed ${npcName}! ${result.message || ""}`
            : `Failed to pickpocket ${npcName}: ${result.message}`,
          action: "PICKPOCKET_NPC",
        });
      }

      return {
        success: result.success,
        text: result.message || (result.success ? `Pickpocketed ${npcName}` : `Failed to pickpocket ${npcName}`),
        data: { actionName: "pickpocketNpc", npcName, result },
      };
    } catch (error) {
      const errorMessage = error instanceof Error ? error.message : "Unknown error";
      logger.error({ error }, "Failed to execute PICKPOCKET_NPC action");
      if (callback) await callback({ text: `Error pickpocketing: ${errorMessage}`, action: "PICKPOCKET_NPC" });
      return { success: false, error: errorMessage };
    }
  },
};

export default pickpocketNpcAction;
