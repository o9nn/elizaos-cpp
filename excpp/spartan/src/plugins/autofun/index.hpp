#include "elizaos/core.hpp"
#include "providers/autofun.hpp"
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

// create a new plugin
const autofunPlugin: Plugin = {
  name: 'autofun',
  description: 'Autofun plugin',
  routes: [],
  providers: [autofunProvider],
  tests: [
    {
      name: 'test suite for degen-intel',
      tests: [
        {
          name: 'test for degen-intel',
          fn: async (runtime: IAgentRuntime) => {
            logger.info('test in degen-intel working');
          },
        },
      ],
    },
  ],
  // FIXME: make a service
  services: [],
  init: async (_, runtime: IAgentRuntime) => {
    // is plugin-degenTrader active
    let hasPluginTrader = true;

    //
    // trader support
    //

    // FIXME: only if plugin-degenTrader is active
    if (hasPluginTrader) {
      // don't block init from finishing
      new Promise<void>(async (resolve) => {
        resolve();
        console.log('autofunStartIn');
        let service = runtime.getService('TRADER_DATAPROVIDER') as any;
        // FIXME: maybe a max retry?
        while (!service) {
          console.log('autofun waiting for Trading info service...');
          service = runtime.getService('TRADER_DATAPROVIDER') as any;
          if (!service) {
            await new Promise((waitResolve) => setTimeout(waitResolve, 1000));
          } else {
            console.log('autofun Acquired trading chain service...');
          }
        }
        const me = {
          name: 'Autofun',
          trendingService: 'AUTOFUN',
        };
        await service.registerDataProvder(me);

        console.log('autofunStart done');
      });
    }

    /*
    await registerTasks(runtime);

    //const res = autofunProvider.get(runtime, {}, {})
    //console.log('autofunProvider', res)

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
    */
  },
};

} // namespace elizaos
