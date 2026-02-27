#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-COINMARKETCAP_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-COINMARKETCAP_SRC_INDEX_H
#include "core.h"
#include "@elizaos/core.h"
#include "./actions/getPrice.h"
using getPrice = _default;
#include "./tasks/coinmarketcap.h"
using Coinmarketcap = _default;
#include "./providers/trending.h"

extern std::shared_ptr<Plugin> coinmarketcapPlugin;
#endif
