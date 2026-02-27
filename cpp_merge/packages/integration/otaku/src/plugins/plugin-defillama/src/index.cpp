#include "index.hpp"

std::shared_ptr<Plugin> defiLlamaPlugin = object{
    object::pair{std:("name"), std:("plugin-defillama")}, 
    object::pair{std:("description"), std:("DeFiLlama integration: protocol discovery, TVL lookups, yield opportunities, and historical trends")}, 
    object::pair{std:("actions"), array<any>{ getProtocolSlugAction, getProtocolTvlAction, getProtocolTvlHistoryAction, getChainTvlHistoryAction, getYieldRatesAction, getYieldHistoryAction }}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("providers"), array<any>()}, 
    object::pair{std:("services"), array<DefiLlamaService>{ DefiLlamaService }}
};

void Main(void)
{
}

MAIN
