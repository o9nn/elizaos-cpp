#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-BIRDEYE_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-BIRDEYE_SRC_INDEX_H
#include "core.h"
#include "@elizaos/core.h"
#include "./constants.h"
#include "./actions/token-search-address.h"
#include "./actions/token-search-symbol.h"
#include "./actions/wallet-search-address.h"
#include "./tasks/birdeye.h"
using Birdeye = _default;
#include "./service.h"
#include "./providers/agent-portfolio-provider.h"
#include "./providers/trending.h"
#include "./providers/wallet.h"

extern std::shared_ptr<Plugin> birdeyePlugin;
#endif
