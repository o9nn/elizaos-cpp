#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-COINGECKO_SRC_ACTIONS_GETTOKENPRICECHART_ACTION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-COINGECKO_SRC_ACTIONS_GETTOKENPRICECHART_ACTION_H
#include "core.h"
#include "@elizaos/core.h"
#include "../services/coingecko.service.h"

string formatMarketCap(double value);

extern std::shared_ptr<Action> getTokenPriceChartAction;
#endif
