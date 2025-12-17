#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Prompt templates and utilities for generating contextual embeddings.
 * Based on Anthropic's contextual retrieval techniques:
 * https://www.anthropic.com/news/contextual-retrieval
 * https://github.com/anthropics/anthropic-cookbook/blob/main/skills/contextual-embeddings/guide.ipynb
 */

/**
 * Default token size settings for chunking and context generation.
 * These values have been adjusted based on research findings:
 * - Average chunk sizes of 400-600 tokens tend to work well for contextual embeddings
 * - Smaller chunks improve retrieval precision over larger ones
 * - Overlap should be meaningful to maintain context between chunks
 */

/**
 * Target context sizes for different document types.
 * Based on Anthropic's research, contextual enrichment typically adds 50-100 tokens.
 */

/**
 * Modern system prompt for contextual embeddings based on Anthropic's guidelines.
 * This system prompt is more concise and focused on the specific task.
 */

/**
 * System prompts optimized for different content types with caching support
 */

/**
 * Enhanced contextual embedding prompt template optimized for better retrieval performance.
 * Based on Anthropic's research showing significant improvements in retrieval accuracy.
 */

/**
 * Caching-optimized chunk prompt - separates document from instructions
 * This version doesn't include the document inline to support OpenRouter caching
 */

/**
 * Caching-optimized code chunk prompt
 */

/**
 * Caching-optimized math PDF chunk prompt
 */

/**
 * Caching-optimized technical documentation chunk prompt
 */

/**
 * Specialized prompt for PDF documents with mathematical content
 */

/**
 * Specialized prompt for code documents
 */

/**
 * Specialized prompt for technical documentation
 */

/**
 * Generates the full prompt string for requesting contextual enrichment from an LLM.
 *
 * @param docContent - The full content of the document.
 * @param chunkContent - The content of the specific chunk to be contextualized.
 * @param minTokens - Minimum target token length for the result.
 * @param maxTokens - Maximum target token length for the result.
 * @returns The formatted prompt string.
 */
std::string getContextualizationPrompt(const std::string& docContent, const std::string& chunkContent, auto minTokens = CONTEXT_TARGETS.DEFAULT.MIN_TOKENS, auto maxTokens = CONTEXT_TARGETS.DEFAULT.MAX_TOKENS, auto promptTemplate = CONTEXTUAL_CHUNK_ENRICHMENT_PROMPT_TEMPLATE);

/**
 * Generates a caching-compatible prompt string for contextual enrichment.
 * This separates the document from the chunk instructions to support OpenRouter caching.
 *
 * @param chunkContent - The content of the specific chunk to be contextualized.
 * @param contentType - Optional content type to determine specialized prompts.
 * @param minTokens - Minimum target token length for the result.
 * @param maxTokens - Maximum target token length for the result.
 * @returns Object containing the prompt and appropriate system message.
 */
void getCachingContextualizationPrompt(const std::string& chunkContent, std::optional<std::string> contentType, auto minTokens = CONTEXT_TARGETS.DEFAULT.MIN_TOKENS, auto maxTokens = CONTEXT_TARGETS.DEFAULT.MAX_TOKENS); {

  // Estimate if the chunk is already large relative to our target size

  // If the chunk is already large, adjust the target max tokens to avoid excessive growth
    // Allow for only ~30% growth for large chunks

  // Determine content type and corresponding templates

/**
 * Generates mime-type specific prompts with optimized parameters for different content types.
 *
 * @param mimeType - The MIME type of the document (e.g., 'application/pdf', 'text/markdown').
 * @param docContent - The full content of the document.
 * @param chunkContent - The content of the specific chunk.
 * @returns The formatted prompt string with mime-type specific settings.
 */
std::string getPromptForMimeType(const std::string& mimeType, const std::string& docContent, const std::string& chunkContent);

/**
 * Optimized version of getPromptForMimeType that separates document from prompt.
 * Returns structured data that supports OpenRouter caching.
 *
 * @param mimeType - The MIME type of the document.
 * @param chunkContent - The content of the specific chunk.
 * @returns Object containing prompt text and system message.
 */
void getCachingPromptForMimeType(const std::string& mimeType, const std::string& chunkContent); {

  // Determine appropriate token targets based on content type

  return getCachingContextualizationPrompt(chunkContent, mimeType, minTokens, maxTokens);

/**
 * Determines if a document likely contains mathematical content based on heuristics.
 *
 * @param content - The document content to analyze.
 * @returns True if the document appears to contain mathematical content.
 */
bool containsMathematicalContent(const std::string& content);
} // namespace elizaos
