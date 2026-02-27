#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_COINGECKO_SRC_ACTIONS_GETHISTORICALPRICE_ACTION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_COINGECKO_SRC_ACTIONS_GETHISTORICALPRICE_ACTION_H
#include "core.hpp"
// External dependency removed
#include "../services/coingecko.service.h"

string formatMarketCap(double value);

string parseDateToApiFormat(string dateStr);

extern std::shared_ptr<Action> getHistoricalPriceAction;
#endif
