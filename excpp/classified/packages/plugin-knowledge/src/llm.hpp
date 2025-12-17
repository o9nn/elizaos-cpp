#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "config.hpp"
#include "elizaos/core.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Cleans model names by removing surrounding quotes if present
 */
std::string cleanModelName(const std::string& modelName);

// Re-for backwards compatibility

/**
 * Generates text embeddings using the configured provider
 * @param text The text to embed
 * @returns The embedding vector
 */
  // Validate input text

/**
 * Generates text embeddings in batches for improved performance
 * @param texts Array of texts to embed
 * @param batchSize Maximum number of texts to process in each batch (default: 20)
 * @returns Array of embedding results with success indicators
 */
std::future<std::vector> generateTextEmbeddingsBatch(IAgentRuntime runtime, const std::vector<std::string>& texts, double batchSize = 20);>> {

  // Process texts in batches

    // Process batch in parallel
        // Skip empty texts

    // Add a small delay between batches to respect rate limits

/**
 * Generates an embedding using OpenAI
 */

  // Some OpenAI models support dimension parameter at initialization time

/**
 * Generates an embedding using Google
 */
  // Create the provider instance with API key config

  // Google Embeddings API doesn't support dimension parameter at the AI SDK level yet

/**
 * Generates an embedding using Ollama
 */
    // Use the same approach as plugin-ollama

/**
 * Generates text using the configured provider
 * @param prompt The prompt text
 * @param system Optional system message
 * @param overrideConfig Optional configuration overrides
 * @returns The generated text result
 *
 * @example
 * // Regular text generation
 *
 * @example
 * // Text generation with system prompt
 *   "Summarize this article: " + articleText,
 *   "You are a helpful assistant specializing in concise summaries."
 * );
 *
 * @example
 * // Using document caching with OpenRouter (available with Claude and Gemini models)
 * // This can reduce costs up to 90% when working with the same document repeatedly
 *   "Extract key topics from this chunk: " + chunk,
 *   "You are a precision information extraction tool.",
 *   {
 *     cacheDocument: documentText,  // The full document to cache
 *     cacheOptions: { type: "ephemeral" }
 *   }
 * );
 */

/**
 * Generates text using the Anthropic API with exponential backoff retry
 */

/**
 * Generates text using the OpenAI API
 */

/**
 * Generates text using Google's API
 */

/**
 * Generates text using OpenRouter with optional document caching
 *
 * Document caching is a powerful feature for RAG applications that can significantly
 * reduce token costs when working with the same document repeatedly. It works by:
 *
 * 1. For Claude models: Explicitly caching the document with Claude's cache_control API
 * 2. For Gemini 2.5+ models: Leveraging implicit caching through consistent prompt structure
 *
 * Caching can reduce costs by up to 90% for subsequent queries on the same document.
 * This is especially valuable for contextual RAG applications.
 *
 * Requirements:
 * - Claude models: Require explicit cache_control API
 * - Gemini 2.5 models: Require minimum document size (2048 tokens for Pro, 1028 for Flash)
 *
 * @private
 */

/**
 * Generates text using Claude with caching via OpenRouter
 */

/**
 * Generates text using Gemini with caching via OpenRouter
 */

/**
 * Generates text using standard OpenRouter API (no caching)
 */

/**
 * Logs cache metrics if available in the result
 */
void logCacheMetrics(GenerateTextResult<any, any> result);

/**
 * Generates text using Ollama
 */

} // namespace elizaos
