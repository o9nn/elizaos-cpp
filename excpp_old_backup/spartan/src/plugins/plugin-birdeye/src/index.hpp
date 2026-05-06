#include "actions/token-search-address.hpp"
#include "actions/token-search-symbol.hpp"
#include "actions/wallet-search-address.hpp"
#include "constants.hpp"
#include "elizaos/core.hpp"
#include "providers/agent-portfolio-provider.hpp"
#include "providers/trending.hpp"
#include "providers/wallet.hpp"
#include "service.hpp"
#include "tasks/birdeye.hpp"
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



// SYNC_WALLET provider? or solana handles this?

// create a new plugin
      // testAllEndpointsAction, // this action can be used to optionally test all endpoints

    // is plugin-trader active

    //
    // trader support
    //

    // FIXME: only if plugin-trader is active
      // don't block init from finishing
        // FIXME: maybe a max retry?

    //
    // tasks
    //

    // first, get all tasks with tags "queue", "repeat", "degen_intel" and delete them

      // how do we turn this on and off?
      // I guess we can check for the plugin-trader service
            //await birdeye.syncTrendingTokens('base');
            // kill this task

    // shouldn't plugin-solana and plugin-evm handle this?
          // kill this task

    /*

    // check for birdeeye key, if have then register provider

    */

} // namespace elizaos
