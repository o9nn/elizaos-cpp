#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-coingecko/src/index.h"

std::shared_ptr<Plugin> coingeckoPlugin = object{
    object::pair{std::string("name"), std::string("plugin-coingecko")}, 
    object::pair{std::string("description"), std::string("CoinGecko plugin exposing token metadata lookup, trending tokens, NFT collection stats, trending searches, price charts, historical prices, and coin categories")}, 
    object::pair{std::string("actions"), array<any>{ getTokenMetadataAction, getTrendingTokensAction, getNFTCollectionStatsAction, getTrendingSearchAction, getTokenPriceChartAction, getHistoricalPriceAction, getCategoriesListAction, getCategoriesWithMarketDataAction }}, 
    object::pair{std::string("services"), array<CoinGeckoService>{ CoinGeckoService }}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("providers"), array<any>()}
};

void Main(void)
{
}

MAIN
