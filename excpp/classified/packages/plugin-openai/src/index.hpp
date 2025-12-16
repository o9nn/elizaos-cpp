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
 * Validation service for OpenAI plugin
 * Provides runtime validation for API key availability
 */
class OpenAIValidationService extends Service {
  static override serviceType: ServiceTypeName = 'openai-validation' as ServiceTypeName;
  static serviceName = 'openai-validation';

  override capabilityDescription = 'Validates OpenAI plugin configuration and API availability';

  constructor(runtime: IAgentRuntime) {
    super(runtime);
  }

    // No cleanup needed

  /**
   * Check if OpenAI is properly configured and available
   */

/**
 * Retrieves a configuration setting from the runtime, falling back to environment variables or a default value if not found.
 *
 * @param key - The name of the setting to retrieve.
 * @param defaultValue - The value to return if the setting is not found in the runtime or environment.
 * @returns The resolved setting value, or {@link defaultValue} if not found.
 */

/**
 * Retrieves the OpenAI API base URL from runtime settings, environment variables, or defaults, using provider-aware resolution.
 *
 * @returns The resolved base URL for OpenAI API requests.
 */
std::string getBaseURL(IAgentRuntime runtime);

/**
 * Retrieves the OpenAI API base URL for embeddings, falling back to the general base URL.
 *
 * @returns The resolved base URL for OpenAI embedding requests.
 */
std::string getEmbeddingBaseURL(IAgentRuntime runtime);

/**
 * Helper function to get the API key for OpenAI
 *
 * @param runtime The runtime context
 * @returns The configured API key
 */

/**
 * Helper function to get the embedding API key for OpenAI, falling back to the general API key if not set.
 *
 * @param runtime The runtime context
 * @returns The configured API key
 */

/**
 * Helper function to get the small model name with fallbacks
 *
 * @param runtime The runtime context
 * @returns The configured small model name
 */
std::string getSmallModel(IAgentRuntime runtime);

/**
 * Helper function to get the large model name with fallbacks
 *
 * @param runtime The runtime context
 * @returns The configured large model name
 */
std::string getLargeModel(IAgentRuntime runtime);

/**
 * Helper function to get the image description model name with fallbacks
 *
 * @param runtime The runtime context
 * @returns The configured image description model name
 */
std::string getImageDescriptionModel(IAgentRuntime runtime);

/**
 * Helper function to get experimental telemetry setting
 *
 * @param runtime The runtime context
 * @returns Whether experimental telemetry is enabled
 */
bool getExperimentalTelemetry(IAgentRuntime runtime);

/**
 * Create an OpenAI client with proper configuration
 *
 * @param runtime The runtime context
 * @returns Configured OpenAI client
 */
void createOpenAIClient(IAgentRuntime runtime);

/**
 * Asynchronously tokenizes the given text based on the specified model and prompt.
 *
 * @param {ModelTypeName} model - The type of model to use for tokenization.
 * @param {string} prompt - The text prompt to tokenize.
 * @returns {number[]} - An array of tokens representing the encoded prompt.
 */
std::future<void> tokenizeText(ModelTypeName model, const std::string& prompt);

/**
 * Detokenize a sequence of tokens back into text using the specified model.
 *
 * @param {ModelTypeName} model - The type of model to use for detokenization.
 * @param {number[]} tokens - The sequence of tokens to detokenize.
 * @returns {string} The detokenized text.
 */
std::future<void> detokenizeText(ModelTypeName model, const std::vector<double>& tokens);

/**
 * Helper function to generate objects using specified model type
 */

/**
 * Returns a function to repair JSON text
 */

/**
 * Emits a model usage event
 * @param runtime The runtime context
 * @param type The model type
 * @param prompt The prompt used
 * @param usage The LLM usage data
 */
void emitModelUsageEvent(IAgentRuntime runtime, ModelTypeName type, const std::string& prompt, LanguageModelUsage usage);

/**
 * function for text-to-speech
 */
std::future<void> fetchTextToSpeech(IAgentRuntime runtime, const std::string& text);

/**
 * Defines the OpenAI plugin with its name, description, and configuration options.
 * @type {Plugin}
 */
      // Register the validation service

      // If API key is not set, we'll show a warning but continue
        // Return early without throwing an error
      // Convert to warning instead of error

        // Add timeout to prevent hanging

        using OpenAIResponseType = {

        // Check if a custom prompt was provided (not the default prompt)

        // If custom prompt is used, return the raw content

        // Otherwise, maintain backwards compatibility with object return

      return generateObjectByModelType(runtime, params, ModelType.OBJECT_SMALL, getSmallModel);
      return generateObjectByModelType(runtime, params, ModelType.OBJECT_LARGE, getLargeModel);
      // Use o1-preview or similar research model when available

        // Fallback to TEXT_LARGE if research model is not available


} // namespace elizaos
