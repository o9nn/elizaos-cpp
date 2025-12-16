#include "actions/swap.hpp"
#include "actions/transfer.hpp"
#include "constants.hpp"
#include "providers/wallet.hpp"
#include "service.hpp"
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

import type { Plugin } from '@elizaos/core';
;
;
;
;
;

const solanaPlugin: Plugin = {
  name: SOLANA_SERVICE_NAME,
  description: 'Solana Plugin for Eliza',
  actions: [transferToken, executeSwap],
  evaluators: [],
  providers: [walletProvider],
  services: [SolanaService],
  init: async (_, runtime: IAgentRuntime) => {
    console.log('solana init');

    const asking = 'solana';
    const serviceType = 'TRADER_CHAIN';
    let traderChainService = runtime.getService(serviceType) as any;
    while (!traderChainService) {
      console.log(asking, 'waiting for', serviceType, 'service...');
      traderChainService = runtime.getService(serviceType) as any;
      if (!traderChainService) {
        await new Promise((waitResolve) => setTimeout(waitResolve, 1000));
      } else {
        console.log(asking, 'Acquired', serviceType, 'service...');
      }
    }

    const me = {
      name: 'Solana services',
    };
    traderChainService.registerChain(me);

    console.log('jupiter init done');
  },
};
default solanaPlugin;

} // namespace elizaos
