#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

declare module '@elizaos/server' {
  import type { Character, IAgentRuntime, Plugin } from '@elizaos/core';

  class AgentServer {
    constructor(runtime?: IAgentRuntime);
    startAgent: (character: Character) => Promise<IAgentRuntime>;
    stopAgent: (runtime: IAgentRuntime) => Promise<void>;
    registerAgent: (runtime: IAgentRuntime) => void;
    unregisterAgent: (agentId: string) => void;
    initialize: (options: { dataDir?: string; postgresUrl?: string }) => Promise<void>;
    loadCharacterTryPath: typeof loadCharacterTryPath;
    jsonToCharacter: typeof jsonToCharacter;
    start(port?: number): Promise<void>;
    stop(): Promise<void>;
  }

   from '@elizaos/core';

  const plugin: Plugin;
}

} // namespace elizaos
