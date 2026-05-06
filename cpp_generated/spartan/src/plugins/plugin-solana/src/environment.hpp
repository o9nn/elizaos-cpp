#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-SOLANA_SRC_ENVIRONMENT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-SOLANA_SRC_ENVIRONMENT_H
#include "core.h"
#include "@elizaos/core.h"
#include "zod.h"

typedef z::infer<solanaEnvSchema> SolanaConfig;


extern any solanaEnvSchema;
std::shared_ptr<Promise<SolanaConfig>> validateSolanaConfig(std::shared_ptr<IAgentRuntime> runtime);

#endif
