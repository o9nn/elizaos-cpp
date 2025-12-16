#include "actions.hpp"
#include "services/relay.service.hpp"
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

import type { Plugin } from "@elizaos/core";
;
;

const relayPlugin: Plugin = {
  name: "relay",
  description:
    "Relay Link integration for cross-chain bridging, swapping, and execution using the Relay protocol",
  actions: [relayQuoteAction, relayBridgeAction, relayStatusAction],
  services: [RelayService],
  evaluators: [],
  providers: [],
};

default relayPlugin;

// Re-types for external use
* from "./types";
{ RelayService } from "./services/relay.service";

} // namespace elizaos
