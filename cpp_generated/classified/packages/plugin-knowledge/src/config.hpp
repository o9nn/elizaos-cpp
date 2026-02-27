#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-KNOWLEDGE_SRC_CONFIG_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-KNOWLEDGE_SRC_CONFIG_H
#include "core.h"
#include "./types.ts.h"
#include "zod.h"
using z = _default;
#include "@elizaos/core.h"

extern std::function<boolean(any)> parseBooleanEnv;
std::shared_ptr<ModelConfig> validateModelConfig(std::shared_ptr<IAgentRuntime> runtime = undefined);

void validateConfigRequirements(std::shared_ptr<ModelConfig> config, boolean assumePluginOpenAI);

std::shared_ptr<Promise<std::shared_ptr<ProviderRateLimits>>> getProviderRateLimits(std::shared_ptr<IAgentRuntime> runtime = undefined);

#endif
