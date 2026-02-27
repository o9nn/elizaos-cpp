import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";

const setCombatStyleAction: Action = {
  name: "SET_COMBAT_STYLE",
  similes: ["CHANGE_COMBAT_STYLE", "COMBAT_MODE"],
  description: "Change melee combat style to train a specific skill. Set BEFORE using ATTACK_NPC. Standard styles: 0=Accurate (trains Attack), 1=Aggressive (trains Strength), 2=Defensive (trains Defence), 3=Controlled (shared XP). NOTE: exact skill trained by style 2 may vary by weapon type. For balanced training, set to whichever of Attack/Strength/Defence is lowest, then switch after some kills. Do NOT use during active combat - set style first, then attack.",
  parameters: [
    { name: "style", description: "Combat style number: 0=Accurate(Attack), 1=Aggressive(Strength), 2=Defensive(Defence), 3=Controlled(shared)", required: true, schema: { type: "number" } },
  ],
  examples: [
    [{ name: "{{user1}}", content: { text: "Switch to aggressive style" } }, { name: "{{agentName}}", content: { text: "Setting combat to Aggressive for Strength XP.", action: "SET_COMBAT_STYLE" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => Boolean(runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME)),
  handler: async (runtime, message, _state, options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "SET_COMBAT_STYLE" }); return { success: false, error: "service not found" }; }

      let style: number | undefined;
      const params = options?.parameters as Record<string, number> | undefined;
      if (typeof params?.style === "number") style = params.style;

      // Fallback: extract from text
      if (style === undefined) {
        const text = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        if (text.includes("accurate") || text.includes("attack xp")) style = 0;
        else if (text.includes("aggressive") || text.includes("strength xp")) style = 1;
        else if (text.includes("defensive") || text.includes("defence xp")) style = 2;
        else if (text.includes("controlled") || text.includes("shared")) style = 3;
      }

      if (style === undefined || style < 0 || style > 3) {
        if (callback) await callback({ text: "SET_COMBAT_STYLE requires style 0-3 (0=Attack, 1=Strength, 2=Defence, 3=Controlled)", action: "SET_COMBAT_STYLE" });
        return { success: false, error: "Invalid style" };
      }

      const result = await service.executeAction(runtime.agentId, "setCombatStyle", { style });
      const styleNames = ["Accurate (Attack)", "Aggressive (Strength)", "Defensive (Defence)", "Controlled (shared)"];
      logger.info({ action: "SET_COMBAT_STYLE", style, result }, "Executed setCombatStyle");
      if (callback) await callback({ text: result.success ? `Combat style set to ${styleNames[style]}` : `Failed: ${result.message}`, action: "SET_COMBAT_STYLE" });
      return { success: result.success, text: result.message || styleNames[style] || "", data: { actionName: "setCombatStyle", style, result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "SET_COMBAT_STYLE" });
      return { success: false, error: msg };
    }
  },
};
export default setCombatStyleAction;
