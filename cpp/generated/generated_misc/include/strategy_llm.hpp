#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS-TRADER_STRATEGIES_STRATEGY_LLM_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS-TRADER_STRATEGIES_STRATEGY_LLM_H
#include "core.h"
#include "@elizaos/core.h"
#include "../utils.h"

extern std::string buyTemplate;
void llmStrategy(std::shared_ptr<IAgentRuntime> runtime);

void generateBuySignal(std::any runtime, std::any strategyService, std::any hndl);

void onPriceDelta();

void onSentimentDelta();

void onVol24hDelta();

void onLiquidDelta();

#endif
