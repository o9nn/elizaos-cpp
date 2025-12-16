#include ".constants.js.hpp"
#include ".hyperfy/src/core/extras/playerEmotes.js.hpp"
#include ".service.js.hpp"
#include ".utils.hpp"
#include "elizaos/core.hpp"
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



class EmoteManager {
  private emoteHashMap: Map<string, string>
  private currentEmoteTimeout: NodeJS.Timeout | null
  private movementCheckInterval: NodeJS.Timeout | null = null;
  private runtime: IAgentRuntime;

  constructor(runtime) {
    this.runtime = runtime;
    this.emoteHashMap = new Map()
    this.currentEmoteTimeout = null
  }

    // Get duration from EMOTES_LIST


} // namespace elizaos
