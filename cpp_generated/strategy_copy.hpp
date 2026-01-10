#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS-TRADER_STRATEGIES_STRATEGY_COPY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS-TRADER_STRATEGIES_STRATEGY_COPY_H
#include "core.h"
#include "@elizaos/core.h"
#include "../utils.h"

void copyStrategy(std::shared_ptr<IAgentRuntime> runtime);

void onWalletEvent(any runtime, any strategyService, any hndl);

#endif
