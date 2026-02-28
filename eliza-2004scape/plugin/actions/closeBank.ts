import {
  type Action, type ActionExample, type ActionResult, type HandlerCallback,
  type HandlerOptions, type IAgentRuntime, logger, type Memory, type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";

const closeBankAction: Action = {
  name: "CLOSE_BANK",
  similes: ["EXIT_BANK", "LEAVE_BANK"],
  description: "Close the bank interface. Use ONLY after you are done depositing/withdrawing items. Do NOT use if bank is not currently open. Always close the bank when finished to resume normal gameplay.",
  examples: [
    [{ name: "{{user1}}", content: { text: "Close the bank" } }, { name: "{{agentName}}", content: { text: "Closing the bank.", action: "CLOSE_BANK" } }],
  ] as ActionExample[][],
  validate: async (runtime: IAgentRuntime) => Boolean(runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME)),
  handler: async (runtime, _message, _state, _options, callback): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) { if (callback) await callback({ text: "RS-SDK service not available.", action: "CLOSE_BANK" }); return { success: false, error: "service not found" }; }
      const result = await service.executeAction(runtime.agentId, "closeBank", {});
      if (callback) await callback({ text: result.success ? `${result.message}` : `Failed: ${result.message}`, action: "CLOSE_BANK" });
      return { success: result.success, text: result.message || "", data: { actionName: "closeBank", result } };
    } catch (error) {
      const msg = error instanceof Error ? error.message : "Unknown error";
      if (callback) await callback({ text: `Error: ${msg}`, action: "CLOSE_BANK" });
      return { success: false, error: msg };
    }
  },
};
export default closeBankAction;
