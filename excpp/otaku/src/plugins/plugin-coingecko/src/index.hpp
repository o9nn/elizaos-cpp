#include "actions/getCategoriesList.action.hpp"
#include "actions/getCategoriesWithMarketData.action.hpp"
#include "actions/getHistoricalPrice.action.hpp"
#include "actions/getNFTCollectionStats.action.hpp"
#include "actions/getTokenMetadata.action.hpp"
#include "actions/getTokenPriceChart.action.hpp"
#include "actions/getTrendingSearch.action.hpp"
#include "actions/getTrendingTokens.action.hpp"
#include "services/coingecko.service.hpp"
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

import type { Plugin } from "@elizaos/core";
;
;
;
;
;
;
;
;
;

const coingeckoPlugin: Plugin = {
  name: "plugin-coingecko",
  description: "CoinGecko plugin exposing token metadata lookup, trending tokens, NFT collection stats, trending searches, price charts, historical prices, and coin categories",
  actions: [
    getTokenMetadataAction,
    getTrendingTokensAction,
    getNFTCollectionStatsAction,
    getTrendingSearchAction,
    getTokenPriceChartAction,
    getHistoricalPriceAction,
    getCategoriesListAction,
    getCategoriesWithMarketDataAction,
  ],
  services: [CoinGeckoService],
  evaluators: [],
  providers: [],
};

default coingeckoPlugin;

{
  CoinGeckoService,
  getTokenMetadataAction,
  getTrendingTokensAction,
  getNFTCollectionStatsAction,
  getTrendingSearchAction,
  getTokenPriceChartAction,
  getHistoricalPriceAction,
  getCategoriesListAction,
  getCategoriesWithMarketDataAction,
};



} // namespace elizaos
