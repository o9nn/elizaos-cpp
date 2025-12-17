#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "claude-proxy.hpp"
#include "elizaos/core.hpp"
#include "elizaos/plugin-anthropic.hpp"
#include "elizaos/plugin-elizaos-services.hpp"
#include "elizaos/plugin-groq.hpp"
#include "elizaos/plugin-local-embedding.hpp"
#include "elizaos/plugin-ollama.hpp"
#include "elizaos/plugin-openai.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Import all LLM provider plugins
// @ts-ignore - workspace dependency

// @ts-ignore - workspace dependency

// @ts-ignore - workspace dependency

// @ts-ignore - workspace dependency

// @ts-ignore - workspace dependency

// @ts-ignore - workspace dependency

/**
 * Enum for supported inference providers
 */
enum InferenceProvider {
}

// Provider configuration
struct ProviderInfo {
    std::optional<std::string> name;
    Plugin plugin;
    std::string displayName;
    std::string status;
    std::string message;
};

// Inference plugin state
class InferenceState {
public:
};

/**
 * Reset state for testing purposes
 * @internal
 */
void resetInferenceState();

/**
 * Initialize provider plugins and check their validity
 */
std::future<void> initializeProviders(IAgentRuntime runtime);

// Define interface for validation service
struct ValidationService {
};

/**
 * Check if a provider is valid and configured
 */
std::future<bool> checkProviderValidity(IAgentRuntime runtime, InferenceProvider providerName, ProviderInfo providerInfo);

/**
 * Get configuration message for provider
 */
std::string getProviderConfigMessage(InferenceProvider provider);

/**
 * Get the active provider based on selection and availability
 */

  // If a specific provider is selected, try to use it first

  // Otherwise, use the first available provider from preferences

/**
 * Route model calls to the active provider
 */
  // For TEXT_EMBEDDING, always try to use the local FastEmbed plugin first

    // Try local embedding first

  // For all other model types, use the normal provider selection logic

  // Check if the provider supports this model type
    // Try to find another provider that supports this model type

    // Search through all available providers for one that supports this model type

    // If no provider supports this model type, throw an error

/**
 * Get provider status for all providers
 */

/**
 * Set the selected provider
 */
std::future<void> setSelectedProvider(IAgentRuntime runtime, const std::string& provider);

/**
 * Set provider preferences
 */
std::future<void> setProviderPreferences(IAgentRuntime runtime, const std::vector<std::string>& preferences);

/**
 * Defines the inference plugin with dynamic provider routing
 */

    // Ensure local embedding is initialized for embedding support

      return routeToProvider(runtime, ModelType.TEXT_EMBEDDING, params);

      return routeToProvider(runtime, ModelType.TEXT_TOKENIZER_ENCODE, params);

      return routeToProvider(runtime, ModelType.TEXT_TOKENIZER_DECODE, params);

      return routeToProvider(runtime, ModelType.TEXT_SMALL, params);

      return routeToProvider(runtime, ModelType.TEXT_LARGE, params);

      return routeToProvider(runtime, ModelType.IMAGE, params);

      return routeToProvider(runtime, ModelType.IMAGE_DESCRIPTION, params);

      return routeToProvider(runtime, ModelType.TRANSCRIPTION, audioBuffer);

      return routeToProvider(runtime, ModelType.TEXT_TO_SPEECH, text);

      return routeToProvider(runtime, ModelType.OBJECT_SMALL, params);

      return routeToProvider(runtime, ModelType.OBJECT_LARGE, params);

// Export Claude handler for external use

} // namespace elizaos
