#include "actions/getPrice.hpp"
#include "elizaos/core.hpp"
#include "providers/trending.hpp"
#include "service.hpp"
#include "tasks/coinmarketcap.hpp"
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

;
;

//;
;

;

const coinmarketcapPlugin: Plugin = {
    name: "coinmarketcap",
    description: "CoinMarketCap Plugin for Eliza",
    actions: [getPrice],
    evaluators: [],
    providers: [trendingProvider],
    init: async (_, runtime: IAgentRuntime) => {
      const worldId = runtime.agentId; // this is global data for the agent

      // first, get all tasks with tags "queue", "repeat", "degen_intel" and delete them
      const tasks = await runtime.getTasks({
        tags: ['queue', 'repeat', 'plugin_coinmarketcap'],
      });

      for (const task of tasks) {
        await runtime.deleteTask(task.id);
      }

      runtime.registerTaskWorker({
        name: 'COINMARKETCAP_SYNC_TRENDING',
        validate: async (_runtime, _message, _state) => {
          return true; // TODO: validate after certain time
        },
        execute: async (runtime, _options, task) => {
          const cmc = new Coinmarketcap(runtime);
          try {
            await cmc.syncTokens();
            //await birdeye.syncTrendingTokens('base');
          } catch (error) {
            logger.error('Failed to sync trending tokens', error);
            // kill this task
            runtime.deleteTask(task.id);
          }
        },
      });

      runtime.createTask({
        name: 'COINMARKETCAP_SYNC_TRENDING',
        description: 'Sync trending tokens from Birdeye',
        worldId,
        metadata: {
          createdAt: Date.now(),
          updatedAt: Date.now(),
          updateInterval: 1000 * 60 * 60, // 1 hour
        },
        tags: ['queue', 'repeat', 'plugin_birdeye', 'immediate'],
      });

      /*
      const plugins = runtime.plugins.map((p) => p.name);
      let notUsed = true;

      // check for birdeeye key, if have then register provider
      if (runtime.getSetting('BIRDEYE_API_KEY')) {
        runtime.registerContextProvider(birdeyeTrendingProvider);
        runtime.registerContextProvider(birdeyeTradePortfolioProvider);
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

default coinmarketcapPlugin;

} // namespace elizaos
