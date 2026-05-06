#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "types.ts.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Validates the model configuration using runtime settings
 * @param runtime The agent runtime to get settings from
 * @returns The validated configuration or throws an error
 */
ModelConfig validateModelConfig(std::optional<IAgentRuntime> runtime);

/**
 * Validates the required API keys and configuration based on the selected mode
 * @param config The model configuration to validate
 * @param assumePluginOpenAI Whether we're assuming plugin-openai is being used
 * @throws Error if a required configuration value is missing
 */
void validateConfigRequirements(ModelConfig config, bool assumePluginOpenAI);

/**
 * Returns rate limit information for the configured providers
 * Checks BOTH TEXT_PROVIDER (for LLM calls) and EMBEDDING_PROVIDER
 *
 * @param runtime The agent runtime to get settings from
 * @returns Rate limit configuration for the current providers
 */
std::future<ProviderRateLimits> getProviderRateLimits(std::optional<IAgentRuntime> runtime);

} // namespace elizaos
