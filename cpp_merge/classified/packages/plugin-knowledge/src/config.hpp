#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_KNOWLEDGE_SRC_CONFIG_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_KNOWLEDGE_SRC_CONFIG_H
#include "core.hpp"
#include "./types.ts.h"
#include "zod.hpp"
// Using alias removed (invalid transpilation)
// External dependency removed

extern std::function<boolean(any)> parseBooleanEnv;
std::shared_ptr<ModelConfig> validateModelConfig(std::shared_ptr<IAgentRuntime> runtime = std::nullopt);

void validateConfigRequirements(std::shared_ptr<ModelConfig> config, boolean assumePluginOpenAI);

std::shared_ptr<Promise<std::shared_ptr<ProviderRateLimits>>> getProviderRateLimits(std::shared_ptr<IAgentRuntime> runtime = std::nullopt);

#endif
