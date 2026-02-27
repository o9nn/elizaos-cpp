#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_SOLANA_SRC_ENVIRONMENT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_SOLANA_SRC_ENVIRONMENT_H
#include "core.hpp"
// External dependency removed
#include "zod.hpp"

typedef z::infer<solanaEnvSchema> SolanaConfig;


extern any solanaEnvSchema;
std::shared_ptr<Promise<SolanaConfig>> validateSolanaConfig(std::shared_ptr<IAgentRuntime> runtime);

#endif
