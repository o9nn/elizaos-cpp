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

const attackNpcAction: Action = {
  name: "ATTACK_NPC",
  similes: ["FIGHT_NPC", "KILL_NPC", "ATTACK", "FIGHT"],
  description:
    "Attack a nearby NPC by name to train combat skills (Attack/Strength/Defence depending on combat style). Use SET_COMBAT_STYLE first to choose which skill to train. Do NOT use on NPCs you cannot see in the Nearby NPCs list. Do NOT use if HP is critically low (eat food first with EAT_FOOD). Specify the exact NPC name from the nearby list. If attack fails with 'already under attack' or times out, try a different target. After killing an NPC, pick up loot with PICKUP_ITEM (bones for Prayer XP via USE_ITEM, hides for selling). For cows/chickens, you may need to OPEN_DOOR on the gate first.",
  parameters: [
    {
      name: "npcName",
      description: "The name of the NPC to attack (e.g., 'Goblin', 'Rat', 'Guard')",
      required: true,
      schema: { type: "string" },
    },
  ],
  examples: [
    [
      {
        name: "{{user1}}",
        content: { text: "Attack the goblin" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Attacking the goblin!", action: "ATTACK_NPC" },
      },
    ],
    [
      {
        name: "{{user1}}",
        content: { text: "Kill the rat nearby" },
      },
      {
        name: "{{agentName}}",
        content: { text: "Fighting the rat now.", action: "ATTACK_NPC" },
      },
    ],
  ] as ActionExample[][],

  validate: async (runtime: IAgentRuntime): Promise<boolean> => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    // Must have NPCs nearby and not be critically low HP
    const hasNpcs = state.nearbyNpcs.length > 0;
    const hpSkill = state.skills.find(s => s.name === "Hitpoints");
    const hpOk = !hpSkill || hpSkill.level > 3;
    return hasNpcs && hpOk;
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
            text: "Cannot attack - RS-SDK service not available.",
            action: "ATTACK_NPC",
          });
        }
        return { success: false, error: "RS-SDK service not found" };
      }

      // Try to extract npcName from multiple sources:
      // 1. options.parameters (from Eliza's parameter extraction)
      // 2. Parse from message/state text (fallback)
      let npcName: string | undefined;

      // Try options.parameters first
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.npcName && typeof params.npcName === "string") {
        npcName = params.npcName;
      }

      // Best fallback: match against actual nearby NPCs from game state (most reliable)
      if (!npcName) {
        const combinedText = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const service2 = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
        if (service2) {
          const botState = await service2.getBotState(runtime.agentId);
          if (botState?.nearbyNpcs) {
            for (const npc of botState.nearbyNpcs) {
              if (combinedText.includes(npc.name.toLowerCase())) {
                npcName = npc.name;
                break;
              }
            }
          }
        }
      }

      if (!npcName) {
        const errorMsg = "ATTACK_NPC requires an npcName - please specify what NPC to attack (e.g., 'attack the Rat')";
        logger.warn({ params }, errorMsg);
        if (callback) {
          await callback({ text: errorMsg, action: "ATTACK_NPC" });
        }
        return { success: false, error: errorMsg };
      }

      const result = await service.executeAction(runtime.agentId, "attackNpc", { npcName });

      logger.info({ action: "ATTACK_NPC", npcName, result }, "Executed attackNpc action");

      if (callback) {
        await callback({
          text: result.success
            ? `Attacking ${npcName}! ${result.message || ""}`
            : `Failed to attack ${npcName}: ${result.message}`,
          action: "ATTACK_NPC",
        });
      }

      return {
        success: result.success,
        text: result.message || (result.success ? `Attacking ${npcName}` : `Failed to attack ${npcName}`),
        data: { actionName: "attackNpc", npcName, result },
      };
    } catch (error) {
      logger.error({ error }, "Failed to execute ATTACK_NPC action");
      const errorMessage = error instanceof Error ? error.message : "Unknown error";
      if (callback) {
        await callback({
          text: `Error attacking NPC: ${errorMessage}`,
          action: "ATTACK_NPC",
        });
      }
      return { success: false, error: errorMessage };
    }
  },
};

export default attackNpcAction;
