#include "apis.hpp"
#include "elizaos/core.hpp"
#include "providers/birdeyeTrending.hpp"
#include "providers/birdeyeWallet.hpp"
#include "providers/cmcMarket.hpp"
#include "providers/sentiment.hpp"
#include "services/srv_chain.hpp"
#include "services/srv_dataprovider.hpp"
#include "services/srv_strategy.hpp"
#include "tasks.hpp"
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

import type { IAgentRuntime, Plugin } from '@elizaos/core';
;
;
;

;
;
;
;
// INTEL_SYNC_WALLET provider? or solana handles this?

// Services
;
;
;

// create a new plugin
const degenIntelPlugin: Plugin = {
  name: 'spartan-intel',
  description: 'Spartan Intel plugin',
  routes,
  providers: [],
  services: [TradeChainService, TradeDataProviderService, TradeStrategyService],
  tests: [
    {
      name: 'test suite for intel',
      tests: [
        {
          name: 'test for intel',
          fn: async (runtime: IAgentRuntime) => {
            logger.info('test in intel working');
          },
        },
      ],
    },
  ],
  init: async (_, runtime: IAgentRuntime) => {
    console.log('intel init');

    await registerTasks(runtime);

    const plugins = runtime.plugins.map((p) => p.name);
    let notUsed = true;

    // check for cmc key, if have then register provider
    if (runtime.getSetting('COINMARKETCAP_API_KEY')) {
      runtime.registerProvider(cmcMarketProvider);
      notUsed = false;
    }

    // check for birdeeye key, if have then register provider
    if (runtime.getSetting('BIRDEYE_API_KEY')) {
      runtime.registerProvider(birdeyeTrendingProvider);
      runtime.registerProvider(birdeyeTradePortfolioProvider);
      notUsed = false;
    }

    // twitter for sentiment
    if (plugins.indexOf('twitter') !== -1) {
      runtime.registerProvider(sentimentProvider);
      notUsed = false;
    }

    if (notUsed) {
      logger.warn(
        'degen-intel plugin is included but not providing any value (COINMARKETCAP_API_KEY/BIRDEYE_API_KEY or twitter are suggested)'
      );
    }
  },
};

} // namespace elizaos
