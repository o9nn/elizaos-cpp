#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-COINMARKETCAP_SRC_ENVIRONMENT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-COINMARKETCAP_SRC_ENVIRONMENT_H
#include "core.h"
#include "@elizaos/core.h"
#include "zod.h"

typedef z::infer<coinmarketcapEnvSchema> CoinMarketCapConfig;


extern any coinmarketcapEnvSchema;
std::shared_ptr<Promise<CoinMarketCapConfig>> validateCoinMarketCapConfig(std::shared_ptr<IAgentRuntime> runtime);

#endif
