#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_COINGECKO_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_COINGECKO_SRC_INDEX_H
#include "core.hpp"
// External dependency removed
#include "./services/coingecko.service.h"
#include "./actions/getTokenMetadata.action.h"
#include "./actions/getTrendingTokens.action.h"
#include "./actions/getNFTCollectionStats.action.h"
#include "./actions/getTrendingSearch.action.h"
#include "./actions/getTokenPriceChart.action.h"
#include "./actions/getCategoriesList.action.h"
#include "./actions/getCategoriesWithMarketData.action.h"
#include "./actions/getHistoricalPrice.action.h"

extern std::shared_ptr<Plugin> coingeckoPlugin;
#endif
