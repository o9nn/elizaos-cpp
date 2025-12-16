#include "actions/getChainTvlHistory.action.hpp"
#include "actions/getProtocolSlug.action.hpp"
#include "actions/getProtocolTvl.action.hpp"
#include "actions/getProtocolTvlHistory.action.hpp"
#include "actions/getYieldHistory.action.hpp"
#include "actions/getYieldRates.action.hpp"
#include "services/defillama.service.hpp"
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
;
;
;
;
;

const defiLlamaPlugin: Plugin = {
  name: "plugin-defillama",
  description: "DeFiLlama integration: protocol discovery, TVL lookups, yield opportunities, and historical trends",
  actions: [
    getProtocolSlugAction,
    getProtocolTvlAction,
    getProtocolTvlHistoryAction,
    getChainTvlHistoryAction,
    getYieldRatesAction,
    getYieldHistoryAction,
  ],
  evaluators: [],
  providers: [],
  services: [DefiLlamaService],
};

default defiLlamaPlugin;
{
  DefiLlamaService,
  getProtocolSlugAction,
  getProtocolTvlAction,
  getProtocolTvlHistoryAction,
  getChainTvlHistoryAction,
  getYieldRatesAction,
  getYieldHistoryAction,
};



} // namespace elizaos
