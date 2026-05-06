#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-defillama/src/index.h"

std::shared_ptr<Plugin> defiLlamaPlugin = object{
    object::pair{std::string("name"), std::string("plugin-defillama")}, 
    object::pair{std::string("description"), std::string("DeFiLlama integration: protocol discovery, TVL lookups, yield opportunities, and historical trends")}, 
    object::pair{std::string("actions"), array<any>{ getProtocolSlugAction, getProtocolTvlAction, getProtocolTvlHistoryAction, getChainTvlHistoryAction, getYieldRatesAction, getYieldHistoryAction }}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("providers"), array<any>()}, 
    object::pair{std::string("services"), array<DefiLlamaService>{ DefiLlamaService }}
};

void Main(void)
{
}

MAIN
