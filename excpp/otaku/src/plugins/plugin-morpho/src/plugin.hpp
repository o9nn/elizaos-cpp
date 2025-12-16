#include "actions.hpp"
#include "elizaos/core.hpp"
#include "services.hpp"
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

import type { Plugin, IAgentRuntime } from "@elizaos/core";
;

// Import services
;

// Import actions
;

const morphoPlugin: Plugin = {
  name: "plugin-morpho",
  description:
    "Morpho protocol integration for optimized lending and borrowing through peer-to-peer matching",

  async init(): Promise<void> {
    logger.info("Initializing Morpho plugin...");
  },

  // Services that manage state and external integrations
  services: [MorphoService],

  // Actions that handle user commands
  actions: [
    marketInfoAction,
    marketPositionsAction,
    marketTransferAction,
    vaultPositionsAction,
    vaultInfoAction,
    vaultTransferAction,
  ],

  // Providers that supply read-only context
  providers: [],

  // Evaluators for post-interaction analysis
  evaluators: [],
};

default morphoPlugin;

} // namespace elizaos
