#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "config.ts.hpp"
#include "ctx-embeddings.ts.hpp"
#include "elizaos/core.hpp"
#include "llm.ts.hpp"
#include "utils.ts.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Estimates token count for a text string (rough approximation)
 * Uses the common 4 characters per token rule
 */
double estimateTokens(const std::string& text);

/**
 * Gets CTX_KNOWLEDGE_ENABLED setting from runtime or environment
 * Ensures consistency with config.ts validation
 */
bool getCtxKnowledgeEnabled(std::optional<IAgentRuntime> runtime);

/**
 * Check if custom LLM should be used based on environment variables
 * Custom LLM is enabled when all three key variables are set:
 * - TEXT_PROVIDER
 * - TEXT_MODEL
 * - OPENROUTER_API_KEY (or provider-specific API key)
 */
bool shouldUseCustomLLM();

// MAIN DOCUMENT PROCESSING FUNCTIONS

/**
 * Process document fragments synchronously
 * This function:
 * 1. Splits the document text into chunks
 * 2. Enriches chunks with context if contextual Knowledge is enabled
 * 3. Generates embeddings for each chunk
 * 4. Stores fragments with embeddings in the database
 *
 * @param params Fragment parameters
 * @returns Number of fragments processed
 */
std::future<double> processFragmentsSynchronously(auto documentId, auto fullDocumentText, auto agentId, auto contentType, auto roomId, auto entityId, auto worldId, auto documentTitle);

// DOCUMENT EXTRACTION & MEMORY FUNCTIONS

/**
 * Extract text from document buffer based on content type
 * @param fileBuffer Document buffer
 * @param contentType MIME type of the document
 * @param originalFilename Original filename
 * @returns Extracted text
 */
std::future<std::string> extractTextFromDocument(const std::vector<uint8_t>& fileBuffer, const std::string& contentType, const std::string& originalFilename);

/**
 * Create a memory object for the main document
 * @param params Document parameters
 * @returns Memory object for the main document
 */
Memory createDocumentMemory(auto agentId, auto clientDocumentId, auto originalFilename, auto contentType, auto worldId, auto fileSize, auto documentId, auto customMetadata);

// CHUNKING AND FRAGMENT PROCESSING

/**
 * Split document text into chunks using standard parameters
 * @param documentText The full document text to split
 * @returns Array of text chunks
 */

/**
 * Process and save document fragments
 * @param params Processing parameters
 * @returns Object with counts of saved and failed fragments
 */

  // Process chunks in batches to respect concurrency limits

    // Process context generation in an optimized batch

    // Generate embeddings with rate limiting

    // Save fragments with embeddings

        // Log when all chunks for this document are processed

    // Add a small delay between batches to prevent overwhelming the API

/**
 * Generate embeddings for contextualized chunks
 * @param runtime IAgentRuntime
 * @param contextualizedChunks Array of contextualized chunks
 * @param rateLimiter Rate limiter function
 * @returns Array of embedding results
 */
  // Filter out failed chunks

  // Always use individual processing with ElizaOS runtime (keeping embeddings simple)

      // Apply rate limiting before embedding generation

// CONTEXTUAL ENRICHMENT FUNCTIONS

/**
 * Generate contextual chunks if contextual Knowledge is enabled
 */
std::future<std::vector> getContextualizedChunks(IAgentRuntime runtime, const std::string& fullDocumentText, const std::vector<std::string>& chunks, const std::string& contentType, const std::vector<double>& batchOriginalIndices, std::optional<std::string> documentTitle);>> {

  // Log configuration state once per document (not per batch)

  // Enhanced logging for contextual processing

  // If contextual Knowledge is disabled, prepare the chunks without modification

/**
 * Generate contexts for multiple chunks in a single batch
 */
std::future<std::vector> generateContextsInBatch(IAgentRuntime runtime, const std::string& fullDocumentText, const std::vector<std::string>& chunks, std::optional<std::string> contentType, std::optional<std::vector<double>> batchIndices, std::optional<std::string> documentTitle);>> {

  // Get active provider from validateModelConfig

  // Prepare prompts or system messages in parallel

  // Process valid prompts with rate limiting

      // Apply rate limiting before making API call

            // Use custom LLM with caching support
              // Use the newer caching approach with separate document
              // Original approach - document embedded in prompt
            // Fall back to runtime.useModel (original behavior)
              // Use the newer caching approach with separate document
              // Note: runtime.useModel doesn't support cacheDocument/cacheOptions
              // Original approach - document embedded in prompt

        // Track context generation progress without spam

/**
 * Prepare prompts for contextualization
 */
std::vector<std::any> prepareContextPrompts(const std::vector<std::string>& chunks, const std::string& fullDocumentText, std::optional<std::string> contentType, std::optional<std::vector<double>> batchIndices, auto isUsingCacheCapableModel);

// UTILITY FUNCTIONS

/**
 * Helper to generate embedding with proper error handling and validation
 */
    // Always use ElizaOS runtime for embeddings (keep it simple as requested)

    // Handle different embedding result formats consistently

    // Validate embedding

/**
 * Handle rate-limited API calls with automatic retry
 */
      // Handle rate limiting with exponential backoff

      // Try one more time

/**
 * Creates a comprehensive rate limiter that tracks both requests and tokens
 */
void createRateLimiter(double requestsPerMinute, std::optional<double> tokensPerMinute);

/**
 * Logs a comprehensive summary of the knowledge generation process
 */
void logKnowledgeGenerationSummary(auto savedCount, auto failedCount, auto ctxEnabled, auto providerLimits);

} // namespace elizaos
