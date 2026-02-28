import type { IAgentRuntime, Plugin } from "@elizaos/core";
import { rsSdkActions } from "./actions";
import { rsSdkProviders } from "./providers";
import { RsSdkService } from "./services/RsSdkService";

export const rsSdkPlugin: Plugin = {
  name: "rs-sdk",
  description: "RuneScape SDK plugin for controlling bots through Eliza",
  services: [RsSdkService],
  actions: rsSdkActions,
  providers: rsSdkProviders,

  init: async (_config: Record<string, string>, runtime: IAgentRuntime) => {
    runtime.logger.info("RS-SDK plugin initialized");
  },
};

// Export actions array for external use
export { rsSdkActions };

export default rsSdkPlugin;
