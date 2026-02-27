#include "index.hpp"

std::shared_ptr<Plugin> coingeckoPlugin = object{
    object::pair{std:("name"), std:("plugin-coingecko")}, 
    object::pair{std:("description"), std:("CoinGecko plugin exposing token metadata lookup, trending tokens, NFT collection stats, trending searches, price charts, historical prices, and coin categories")}, 
    object::pair{std:("actions"), array<any>{ getTokenMetadataAction, getTrendingTokensAction, getNFTCollectionStatsAction, getTrendingSearchAction, getTokenPriceChartAction, getHistoricalPriceAction, getCategoriesListAction, getCategoriesWithMarketDataAction }}, 
    object::pair{std:("services"), array<CoinGeckoService>{ CoinGeckoService }}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("providers"), array<any>()}
};

void Main(void)
{
}

MAIN
