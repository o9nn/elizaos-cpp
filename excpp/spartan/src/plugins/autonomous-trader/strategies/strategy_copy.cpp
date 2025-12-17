#include "strategy_copy.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> copyStrategy(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto service = acquireService(runtime, 'TRADER_STRATEGY', 'copy trading strategy');
    const auto infoService = acquireService(runtime, 'TRADER_DATAPROVIDER', 'copy trading info');

    const auto me = {;
        name: 'Copy trading strategy',
        };
        const auto hndl = service.register_strategy(me);

        // ok which wallets do we need to set up listeners on?

}

std::future<void> onWalletEvent(auto runtime, auto strategyService, auto hndl) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // what wallets are using this strategy
    // who's following this wallet
    // if no one, deregister event
    // scale amount for each specific interested wallet
    // buy: (maybe based on available balance)
    // sell: can be scaled based on position info, as long we record the OG wallet amount (and have our amount)
    // execute trade, open/close positions:
    // verify address for this chain
    // if looks good, get token(s) info (birdeye?)
    // validateTokenForTrading (look at liquidity/volume/suspicious atts)
    // now it's a signal
    // assess response, figure what wallet are buying based on balance
    // and scale amount for each wallet based on available balance
    // execute buys on each of wallet
    // calculateOptimalBuyAmount
    // wallet.swap (wallet slippage cfg: 2.5%)
    // wallet.quote
    // calculateDynamicSlippage (require quote)
    // wallet.buy
    // open position
    // set up exit conditions
    //await strategyService.open_position(hndl, pos)

}

} // namespace elizaos
