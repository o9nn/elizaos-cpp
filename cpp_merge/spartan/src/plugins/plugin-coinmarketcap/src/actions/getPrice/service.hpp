#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-COINMARKETCAP_SRC_ACTIONS_GETPRICE_SERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-COINMARKETCAP_SRC_ACTIONS_GETPRICE_SERVICE_H
#include "core.h"
#include "axios.h"
using axios = _default;
#include "./types.h"

extern string BASE_URL;
extern std::function<object(string)> createPriceService;
#endif
