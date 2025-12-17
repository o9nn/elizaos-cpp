#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Multi-Provider AI API Implementation
 * Supports OpenAI, Groq, Anthropic, and other providers with real API endpoints
 */

/**
 * Provider configuration for different AI services
 */
struct ProviderConfig {
    std::string name;
    std::string baseURL;
    std::vector<ModelTypeName> supportedModels;
};

/**
 * Provider configurations
 */

      // Transform OpenAI-style to Anthropic-style
      // Transform Anthropic response to OpenAI-style

/**
 * Get provider configuration based on available API keys
 */
std::optional<ProviderConfig> getAvailableProvider(IAgentRuntime runtime, ModelTypeName modelType);

/**
 * Get API key for a specific provider
 */
std::string getProviderApiKey(IAgentRuntime runtime, const std::string& providerName);

/**
 * Make API request to provider with proper error handling
 */
std::future<std::any> makeProviderRequest(ProviderConfig provider, const std::string& apiKey, const std::string& endpoint, const std::any& payload);

/**
 * Emit usage event for cost tracking
 */
void emitProviderUsageEvent(IAgentRuntime runtime, const std::string& providerName, ModelTypeName modelType, const std::string& prompt, const std::any& usage);

} // namespace elizaos
