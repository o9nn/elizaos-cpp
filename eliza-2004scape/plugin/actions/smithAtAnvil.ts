import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";

const smithAtAnvilAction: Action = {
  name: "SMITH_AT_ANVIL",
  similes: ["SMITH", "FORGE", "MAKE_WEAPON"],
  description: "Smith metal bars into weapons or armour at a nearby anvil to train Smithing. REQUIRES: Hammer AND metal bars (Bronze bar, Iron bar, etc.) in inventory, AND an Anvil in Nearby Objects. Do NOT use if missing hammer or bars. Do NOT use if no anvil nearby. Specify product name (dagger, sword, etc.).",
  parameters: [
    { name: "product", description: "What to smith (e.g., 'dagger', 'sword', 'platebody'). Defaults to 'dagger' if not specified.", required: false, schema: { type: "string" } },
  ],
  examples: [
    [{ name: "{{user1}}", content: { text: "Smith a bronze dagger" } }, { name: "{{agentName}}", content: { text: "Smithing a bronze dagger at the anvil.", action: "SMITH_AT_ANVIL" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    if (!service) return false;
    const state = await service.getBotState(runtime.agentId);
    if (!state) return true;
    const hasHammer = state.inventory.some(i => /hammer/i.test(i.name));
    const hasBars = state.inventory.some(i => /bar/i.test(i.name));
    return hasHammer && hasBars;
  },
  handler: async (runtime, message, _state, options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "SMITH_AT_ANVIL" }); return { success: false, error: "service not found" }; }
      let product = "dagger";
      const params = options?.parameters as Record<string, string> | undefined;
      if (params?.product && typeof params.product === "string") product = params.product;
      else {
        const text = `${getCurrentLlmResponse()} ${(message.content as { text?: string })?.text || ""}`.toLowerCase();
        const products = ["dagger", "sword", "scimitar", "mace", "axe", "pickaxe", "med helm", "full helm", "platelegs", "plateskirt", "platebody", "chainbody", "sq shield", "kiteshield", "nails", "dart tip", "arrowtips", "knife"];
        for (const p of products) { if (text.includes(p)) { product = p; break; } }
      }
      const result = await service.executeAction(runtime.agentId, "smithAtAnvil", { product });
      logger.info({ action: "SMITH_AT_ANVIL", product, result }, "Executed smithAtAnvil");
      if (callback) await callback({ text: result.success ? `${result.message}` : `Failed: ${result.message}`, action: "SMITH_AT_ANVIL" });
      return { success: result.success, text: result.message || "", data: { actionName: "smithAtAnvil", product, result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "SMITH_AT_ANVIL" });
      return { success: false, error: msg };
    }
  },
};
export default smithAtAnvilAction;
