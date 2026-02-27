#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS_TRADER_STRATEGIES_STRATEGY_COPY_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS_TRADER_STRATEGIES_STRATEGY_COPY_H
#include "core.hpp"
// External dependency removed
#include "../utils.h"

void copyStrategy(std::shared_ptr<IAgentRuntime> runtime);

void onWalletEvent(any runtime, any strategyService, any hndl);

#endif
