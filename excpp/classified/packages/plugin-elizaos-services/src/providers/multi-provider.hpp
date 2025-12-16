#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

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
    { models;
    (apiKey: string) => Record<string, string> headers;
    std::optional<(payload: any) => any> transformRequest;
    std::optional<(response: any) => any> transformResponse;
};

/**
 * Provider configurations
 */

      // Transform OpenAI-style to Anthropic-style
      // Transform Anthropic response to OpenAI-style

/**
 * Get provider configuration based on available API keys
 */

/**
 * Get API key for a specific provider
 */

/**
 * Make API request to provider with proper error handling
 */
std::future<std::any> makeProviderRequest(ProviderConfig provider, const std::string& apiKey, const std::string& endpoint, const std::any& payload);

/**
 * Emit usage event for cost tracking
 */
void emitProviderUsageEvent(IAgentRuntime runtime, const std::string& providerName, ModelTypeName modelType, const std::string& prompt, const std::any& usage);

} // namespace elizaos
