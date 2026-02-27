#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_BIRDEYE_SRC_ACTIONS_TOKEN_SEARCH_SYMBOL_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_BIRDEYE_SRC_ACTIONS_TOKEN_SEARCH_SYMBOL_H
#include "core.hpp"
// External dependency removed
#include "../birdeye.h"
#include "../types/api/search.h"
#include "../utils.h"

extern string SYMBOL_SEARCH_MODE;
extern any tokenSearchSymbolAction;
extern std::function<string(string, double, array<std::shared_ptr<TokenResult>>)> formatTokenSummary;
#endif
