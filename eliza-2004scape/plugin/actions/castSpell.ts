import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";

const castSpellAction: Action = {
  name: "CAST_SPELL",
  similes: ["MAGIC_ATTACK", "SPELL", "CAST"],
  description: "Cast a combat spell on a nearby NPC to train Magic. REQUIRES: appropriate runes in inventory (Air rune + Mind rune for Wind Strike, etc.). Do NOT use if you have no runes. Do NOT use on NPCs not in the nearby list. Supported spells: Wind/Water/Earth/Fire Strike and Bolt.",
  parameters: [
    { name: "npcName", description: "The NPC to cast the spell on (e.g., 'Goblin', 'Rat')", required: true, schema: { type: "string" } },
    { name: "spellName", description: "The spell to cast (e.g., 'Wind Strike', 'Water Strike', 'Earth Strike', 'Fire Strike')", required: false, schema: { type: "string" } },
  ],
  examples: [
    [{ name: "{{user1}}", content: { text: "Cast Wind Strike on the goblin" } }, { name: "{{agentName}}", content: { text: "Casting Wind Strike on Goblin.", action: "CAST_SPELL" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    // Check for runes
    const hasRunes = state.inventory.some(i => /rune/i.test(i.name));
    return hasRunes;
  },
  handler: async (runtime, message, _state, options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "CAST_SPELL" }); return { success: false, error: "service not found" }; }

      let npcName: string | undefined;
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.npcName) npcName = params.npcName;
      if (!npcName) {
        const combinedText = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const botState = await service.getBotState(runtime.agentId);
        if (botState?.nearbyNpcs) {
          for (const npc of botState.nearbyNpcs) {
            if (combinedText.includes(npc.name.toLowerCase())) { npcName = npc.name; break; }
          }
        }
      }
      if (!npcName) { if (callback) await callback({ text: "CAST_SPELL requires npcName", action: "CAST_SPELL" }); return { success: false, error: "Missing npcName" }; }

      // Map spell names to component IDs (standard spellbook)
      const spellName = (params?.spellName || "Wind Strike").toLowerCase();
      const spellComponents: Record<string, number> = {
        "wind strike": 14286848, "water strike": 14286849, "earth strike": 14286850, "fire strike": 14286851,
        "wind bolt": 14286852, "water bolt": 14286853, "earth bolt": 14286854, "fire bolt": 14286855,
      };
      const spellComponent = spellComponents[spellName] || spellComponents["wind strike"];

      const result = await service.executeAction(runtime.agentId, "castSpell", { npcName, spellComponent });
      logger.info({ action: "CAST_SPELL", npcName, spellName, result }, "Executed castSpell");
      if (callback) await callback({ text: result.success ? `Cast ${spellName} on ${npcName}. ${result.message || ""}` : `Failed: ${result.message}`, action: "CAST_SPELL" });
      return { success: result.success, text: result.message || "", data: { actionName: "castSpell", npcName, spellName, result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "CAST_SPELL" });
      return { success: false, error: msg };
    }
  },
};
export default castSpellAction;
