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
 * Validation service for Groq plugin
 * Provides runtime validation for API key availability
 */
class GroqValidationService {
public:
    GroqValidationService(IAgentRuntime runtime);
    std::future<GroqValidationService> start(IAgentRuntime runtime);
    std::future<void> stop();
    std::future<bool> isValid();
};

/**
 * Retrieves the Groq API base URL, using runtime settings or environment variables if available.
 *
 * @returns The resolved Groq API base URL.
 */
std::string getBaseURL(const std::any& runtime);

/**
 * Returns the appropriate Groq model name string for the specified model type.
 *
 * If environment variables for model names are set, they are used; otherwise, defaults are returned.
 *
 * @param model - The model type for which to retrieve the model name.
 * @returns The model name string corresponding to the given {@link model}.
 *
 * @remark If an error occurs, returns the default model name 'llama-3.1-8b-instant'.
 */
TiktokenModel findModelName(IAgentRuntime runtime, ModelTypeName model);

std::future<void> tokenizeText(IAgentRuntime runtime, ModelTypeName model, const std::string& prompt);

/**
 * Detokenize a sequence of tokens back into text using the specified model.
 *
 * @param {ModelTypeName} model - The type of model to use for detokenization.
 * @param {number[]} tokens - The sequence of tokens to detokenize.
 * @returns {string} The detokenized text.
 */
std::future<void> detokenizeText(IAgentRuntime runtime, ModelTypeName model, const std::vector<double>& tokens);

/**
 * Handles rate limit errors, waits for the appropriate delay, and retries the operation
 * @param error The error object from the failed request
 * @param retryFn The function to retry after waiting
 * @returns Result from the retry function
 */

      // Extract retry delay from error message if possible
        // Convert to milliseconds and add a small buffer

      // Wait for the suggested delay plus a small buffer

      // Retry the request

    // For other errors, log and rethrow

/**
 * Helper function to get tracer if instrumentation is enabled
 */
std::any getTracer(IAgentRuntime runtime);

/**
 * Helper function to start an LLM span
 */
    return fn(dummySpan);

  // Get active context to ensure proper nesting

/**
 * Generate text using Groq API with retry handling for rate limits
 */
std::future<void> generateGroqText(ReturnType<typeof createGroq> groq, const std::string& model, std::optional<std::any> params);

/**
 * Generate object using Groq API with consistent error handling
 */
std::future<void> generateGroqObject(ReturnType<typeof createGroq> groq, const std::string& model, ObjectGenerationParams params);

    // Register the validation service

      throw Error('Missing GROQ_API_KEY in environment variables');
        // Return empty array instead of crashing
        // Return empty string instead of crashing

        // Create a FormData instance

    // you can include elevenlabs before this plugin to prefer elevenlabs voice
              //...(instructions && { instructions }),

        // Return empty object instead of crashing

        // Return empty object instead of crashing
              // Encode the string into tokens first
              // Now decode tokens back into text

} // namespace elizaos
