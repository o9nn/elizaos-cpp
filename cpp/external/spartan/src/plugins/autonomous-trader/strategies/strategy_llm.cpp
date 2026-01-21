#include "strategy_llm.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> llmStrategy(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto service = acquireService(runtime, "TRADER_STRATEGY", "llm trading strategy");
    const auto infoService = acquireService(runtime, "TRADER_DATAPROVIDER", "llm trading info");
    //const solanaService = await acquireService(runtime, 'CHAIN_SOLANA', 'solana service info');

    const auto me = {;
        name: "LLM trading strategy",
        };
        const auto hndl = service.register_strategy(me);
        // we want trending
        infoService.interested_trending(async (results) => {
            std::cout << "LLM trading strategy" << results << std::endl;
            // update our cache?

            // temp hack
            generateBuySignal(runtime, service, hndl);
            });
            // sentiment update

            // after we have trending and sentiment
            // then ask the LLM to generate any buy signals

            // priceDeltas? maybe only for open positions
            //

}

std::future<void> generateBuySignal(auto runtime, auto strategyService, auto hndl) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto sentimentsData = (runtime.getCache<Sentiment[]>("sentiments")) || [];
    const auto trendingData = (runtime.getCache<IToken[]>("tokens_solana")) || [];

    auto sentiments = "";

    auto idx = 1;
    for (const auto& sentiment : sentimentsData)
        if (!sentiment.occuringTokens.length) continue;
        // FIXME: which chain
        "sentiments += " + "ENTRY " + idx + "\nTIME: " + sentiment.timeslot + "\nTOKEN ANALYSIS:\n"
        for (const auto& token : sentiment.occuringTokens)
            "sentiments += " + token.token + " - Sentiment: " + token.sentiment + "\n" + token.reason + "\n"
        }

        sentiments += "\n-------------------\n";
        idx++;
    }

    // Get all trending tokens
    auto tokens = "";
    if (!trendingData.length) {
        std::cout << "No trending tokens found in cache" << std::endl;
        } else {
            auto index = 1;
            for (const auto& token : trendingData)
                // FIXME: which chain
                "tokens += " + "ENTRY " + index + "\n\nTOKEN SYMBOL: " + token.name + "\nTOKEN ADDRESS: " + token.address + "\nPRICE: " + token.price + "\n24H CHANGE: " + token.price24hChangePercent + "\nLIQUIDITY: " + token.liquidity
                tokens += "\n-------------------\n";
                index++;
            }
        }

        const auto prompt = buyTemplate;
        .replace("{{sentiment}}", sentiments);
        .replace("{{trending_tokens}}", tokens);

        // FIXME: chain?
        const auto requiredFields = ["recommend_buy", "reason", "recommend_buy_address"];
        const auto response = askLlmObject(;
        runtime,
        { prompt, system: "You are a buy signal analyzer." },
        requiredFields;
        );
        std::cout << "response" << response << std::endl;

        // verify address for this chain (plugin-solana)
        if (response.recommend_buy_chain != 'solana') {
            // abort
            return;
        }
        const auto solanaService = acquireService(runtime, "chain_solana", "llm trading strategy");
        if (!solanaService.validateAddress(response.recommend_buy_address)) {
            // handle failure
            // maybe just recall itself
        }

        // if looks good, get token(s) info (birdeye?) (infoService)
        const auto infoService = acquireService(runtime, "TRADER_DATAPROVIDER", "llm trading info");
        const auto token = infoService.getToken(;
        response.recommend_buy_chain,
        response.recommend_buy_address;
        );

        // validateTokenForTrading (look at liquidity/volume/suspicious atts)

        // now it's a signal

        // phase 1 in parallel (fetch wallets/balance)
        // assess response, figure what wallet are buying based on balance
        // list of wallets WITH this strategy ODI
        // individualize
        // get balance of each ODI
        // and scale amount for each wallet based on available balance
        function scaleAmount(walletKeypair, balance, signal) {
            // NEO write this
        }

        // phase 2 in parallel buy everything (eventually prioritize premium over non) NEO
        // create promise and that create tasks
        // execute buys on each of wallet
        // calculateOptimalBuyAmount
        // wallet.swap (wallet slippage cfg: 2.5%)
        // wallet.quote
        // calculateDynamicSlippage (require quote)
        // wallet.buy
        // we just need the outAmount
        // calc fee/slippage => position

        // open position ODI
        // set up exit conditions
        //await strategyService.open_position(hndl, pos)

}

std::future<void> onPriceDelta() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // per token
    // get all positions with this chain/token
    // filter positions, which position change about this price change
    // may trigger some exit/close position action (might not)
    // exit position: wallet.swap, strategyService.close_position(hndl, pos)
    // sell
    // swap/quote/sell

}

std::future<void> onSentimentDelta() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // get all positions with this chain/token
    // is this wallet/position sentiment delta trigger

}

std::future<void> onVol24hDelta() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // per token
    // get all positions with this chain/token
    // filter positions, which position change about this vol change
    // may trigger some exit/close position action (might not)
    // exit position: wallet.swap, strategyService.close_position(hndl, pos)

}

std::future<void> onLiquidDelta() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // per token
    // get all positions with this chain/token
    // filter positions, which position change about this liq change
    // may trigger some exit/close position action (might not)
    // exit position: wallet.swap, strategyService.close_position(hndl, pos)

}

} // namespace elizaos
