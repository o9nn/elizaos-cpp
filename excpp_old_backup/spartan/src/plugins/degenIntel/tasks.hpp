#include "elizaos/core.hpp"
#include "tasks/birdeye.hpp"
#include "tasks/buySignal.hpp"
#include "tasks/sellSignal.hpp"
#include "tasks/twitter.hpp"
#include "tasks/twitterParser.hpp"
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



// let's not make it a dependency
//import type { ITradeService } from '../../degenTrader/types';

/**
 * Registers tasks for the agent to perform various Intel-related actions.
 * * @param { IAgentRuntime } runtime - The agent runtime object.
 * @param { UUID } [worldId] - The optional world ID to associate with the tasks.
 * @returns {Promise<void>} - A promise that resolves once tasks are registered.
 */

  // first, get all tasks with tags "queue", "repeat", "degen_intel" and delete them

  /*
          //await birdeye.syncTrendingTokens('base');
          // kill this task

          // kill this task
          //await runtime.deleteTask(task.id);

  */

  // shouldn't plugin-solana and plugin-evm handle this?
        // kill this task
        //await runtime.deleteTask(task.id);

  // Only create the Twitter sync task if the Twitter service exists
  //const twitterService = runtime.getService('twitter');
        // Check if Twitter service exists and return false if it doesn't
          // Log only once when we'll be removing the task

          // Get all tasks of this type

          // Delete all these tasks

        // Check if Twitter service exists and return false if it doesn't
          // The main task handler above will take care of removing all Twitter tasks

  // enable trading stuff only if we need to
  //const tradeService = runtime.getService(ServiceTypes.DEGEN_TRADING) as unknown; //  as ITradeService
  // has to be included after degenTrader
  //if (plugins.indexOf('degenTrader') !== -1) {
        // Check if we have some sentiment data before proceeding
          // Log the error but don't delete the task

        // Check if we have some sentiment data before proceeding
          // Log the error but don't delete the task


} // namespace elizaos
