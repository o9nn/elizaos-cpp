#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS_TRADER_STRATEGIES_STRATEGY_LLM_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS_TRADER_STRATEGIES_STRATEGY_LLM_H
#include "core.hpp"
// External dependency removed
#include "../utils.h"

extern string buyTemplate;
void llmStrategy(std::shared_ptr<IAgentRuntime> runtime);

void generateBuySignal(any runtime, any strategyService, any hndl);

void onPriceDelta();

void onSentimentDelta();

void onVol24hDelta();

void onLiquidDelta();

#endif
