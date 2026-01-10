#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-BIRDEYE_SRC_ACTIONS_TOKEN-SEARCH-SYMBOL_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-BIRDEYE_SRC_ACTIONS_TOKEN-SEARCH-SYMBOL_H
#include "core.h"
#include "@elizaos/core.h"
#include "../birdeye.h"
#include "../types/api/search.h"
#include "../utils.h"

extern string SYMBOL_SEARCH_MODE;
extern any tokenSearchSymbolAction;
extern std::function<string(string, double, array<std::shared_ptr<TokenResult>>)> formatTokenSummary;
#endif
