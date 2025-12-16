#include "config.hpp"
#include "elizaos/core.hpp"
#include "types.hpp"
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

;
;
;
;
;
;
;
;

/**
 * Cleans model names by removing surrounding quotes if present
 */


// Re-for backwards compatibility
{ validateModelConfig } from './config';
{ getProviderRateLimits } from './config';
type { ModelConfig, ProviderRateLimits } from './types';

/**
 * Generates text embeddings using the configured provider
 * @param text The text to embed
 * @returns The embedding vector
 */
std::future<{ embedding: number[] }> generateTextEmbedding(IAgentRuntime runtime, const std::string& text);

  const config = validateModelConfig(runtime);
  const dimensions = config.EMBEDDING_DIMENSION;

  try {
    if (config.EMBEDDING_PROVIDER === 'openai') {
      return await generateOpenAIEmbedding(text, config, dimensions);
    } else if (config.EMBEDDING_PROVIDER === 'google') {
      return await generateGoogleEmbedding(text, config);
    } else if (config.EMBEDDING_PROVIDER === 'ollama') {
      return await generateOllamaEmbedding(text, config, dimensions);
    }

    throw new Error(`Unsupported embedding provider: ${config.EMBEDDING_PROVIDER}`);
  } catch (error) {
    logger.error(`[Document Processor] ${config.EMBEDDING_PROVIDER} embedding error:`, error);
    throw error;
  }
}

/**
 * Generates text embeddings in batches for improved performance
 * @param texts Array of texts to embed
 * @param batchSize Maximum number of texts to process in each batch (default: 20)
 * @returns Array of embedding results with success indicators
 */
async >> {
  const config = validateModelConfig(runtime);
  const results: Array<{
    embedding: number[] | null;
    success: boolean;
    error?: any;
    index: number;
  }> = [];

  logger.debug(
    `[Document Processor] Processing ${texts.length} embeddings in batches of ${batchSize}`
  );

  // Process texts in batches
  for (let i = 0; i < texts.length; i += batchSize) {
    const batch = texts.slice(i, i + batchSize);
    const batchStartIndex = i;

    logger.debug(
      `[Document Processor] Batch ${Math.floor(i / batchSize) + 1}/${Math.ceil(texts.length / batchSize)} (${batch.length} items)`
    );

    // Process batch in parallel
    const batchPromises = batch.map(async (text, batchIndex) => {
      const globalIndex = batchStartIndex + batchIndex;
      try {
        // Skip empty texts
        if (!text || text.trim().length === 0) {
          logger.warn(`[Document Processor] Skipping empty text at index ${globalIndex}`);
          return {
            embedding: null,
            success: false,
            error: new Error('Empty text provided'),
            index: globalIndex,
          };
        }

        const result = await generateTextEmbedding(runtime, text);
        return {
          embedding: result.embedding,
          success: true,
          index: globalIndex,
        };
      } catch (error) {
        logger.error(`[Document Processor] Embedding error for item ${globalIndex}:`, error);
        return {
          embedding: null,
          success: false,
          error,
          index: globalIndex,
        };
      }
    });

    const batchResults = await Promise.all(batchPromises);
    results.push(...batchResults);

    // Add a small delay between batches to respect rate limits
    if (i + batchSize < texts.length) {
      await new Promise((resolve) => setTimeout(resolve, 100));
    }
  }

  const successCount = results.filter((r) => r.success).length;
  const failureCount = results.length - successCount;

  logger.debug(
    `[Document Processor] Embedding batch complete: ${successCount} success, ${failureCount} failures`
  );

  return results;
}

/**
 * Generates an embedding using OpenAI
 */
std::future<{ embedding: number[] }> generateOpenAIEmbedding(const std::string& text, ModelConfig config, double dimensions););

  // Some OpenAI models support dimension parameter at initialization time
  const modelOptions: Record<string, any> = {};
  if (
    dimensions &&
    ['text-embedding-3-small', 'text-embedding-3-large'].includes(config.TEXT_EMBEDDING_MODEL)
  ) {
    modelOptions.dimensions = dimensions;
  }

  const modelInstance = openai.embedding(config.TEXT_EMBEDDING_MODEL, modelOptions);

  const { embedding, usage } = await embed({
    model: modelInstance,
    value: text,
  });

  const totalTokens = (usage as { totalTokens?: number })?.totalTokens;
  const usageMessage = totalTokens ? `${totalTokens} total tokens` : 'Usage details N/A';
  logger.debug(
    `[Document Processor] OpenAI embedding ${config.TEXT_EMBEDDING_MODEL}${modelOptions.dimensions ? ` (${modelOptions.dimensions}D)` : ''}: ${usageMessage}`
  );

  return { embedding };
}

/**
 * Generates an embedding using Google
 */
std::future<{ embedding: number[] }> generateGoogleEmbedding(const std::string& text, ModelConfig config);

  // Google Embeddings API doesn't support dimension parameter at the AI SDK level yet
  const modelInstance = googleProvider.textEmbeddingModel(config.TEXT_EMBEDDING_MODEL);

  const { embedding, usage } = await embed({
    model: modelInstance,
    value: text,
  });

  const totalTokens = (usage as { totalTokens?: number })?.totalTokens;
  const usageMessage = totalTokens ? `${totalTokens} total tokens` : 'Usage details N/A';
  logger.debug(
    `[Document Processor] Google embedding ${config.TEXT_EMBEDDING_MODEL}: ${usageMessage}`
  );

  return { embedding };
}

/**
 * Generates an embedding using Ollama
 */
std::future<{ embedding: number[] }> generateOllamaEmbedding(const std::string& text, ModelConfig config, double dimensions); = await import('ollama-ai-provider');
    const { embed } = await import('ai');

    const baseURL = config.OLLAMA_BASE_URL || 'http://localhost:11434';
    const apiBase = baseURL.endsWith('/api') ? baseURL.slice(0, -4) : baseURL;

    const ollama = createOllama({
      baseURL: apiBase,
    });

    const modelName = cleanModelName(config.TEXT_EMBEDDING_MODEL || 'nomic-embed-text');
    logger.debug(`[Document Processor] Ollama embedding with model: ${modelName}`);

    const { embedding } = await embed({
      model: ollama.embedding(modelName),
      value: text,
    });

    logger.debug(
      `[Document Processor] Ollama embedding ${modelName}: ${embedding.length} dimensions`
    );

    return { embedding };
  } catch (error) {
    logger.error(`[Document Processor] Ollama embedding error:`, error);
    throw error;
  }
}

/**
 * Generates text using the configured provider
 * @param prompt The prompt text
 * @param system Optional system message
 * @param overrideConfig Optional configuration overrides
 * @returns The generated text result
 *
 * @example
 * // Regular text generation
 * const response = await generateText("Summarize this article: " + articleText);
 *
 * @example
 * // Text generation with system prompt
 * const response = await generateText(
 *   "Summarize this article: " + articleText,
 *   "You are a helpful assistant specializing in concise summaries."
 * );
 *
 * @example
 * // Using document caching with OpenRouter (available with Claude and Gemini models)
 * // This can reduce costs up to 90% when working with the same document repeatedly
 * const response = await generateText(
 *   "Extract key topics from this chunk: " + chunk,
 *   "You are a precision information extraction tool.",
 *   {
 *     cacheDocument: documentText,  // The full document to cache
 *     cacheOptions: { type: "ephemeral" }
 *   }
 * );
 */
async `);
    }
  } catch (error) {
    logger.error(`[Document Processor] ${provider} ${modelName} error:`, error);
    throw error;
  }
}

/**
 * Generates text using the Anthropic API with exponential backoff retry
 */
async );

  const modelInstance = anthropic(modelName);

  // Retry with exponential backoff for rate limit errors
  const maxRetries = 3;
  for (let attempt = 0; attempt < maxRetries; attempt++) {
    try {
      const result = await aiGenerateText({
        model: modelInstance,
        prompt: prompt,
        system: system,
        temperature: 0.3,
        maxTokens: maxTokens,
      });

      const totalTokens = result.usage.promptTokens + result.usage.completionTokens;
      logger.debug(
        `[Document Processor] ${modelName}: ${totalTokens} tokens (${result.usage.promptTokens}→${result.usage.completionTokens})`
      );

      return result;
    } catch (error: any) {
      // Check if it's a rate limit error (status 429)
      const isRateLimit =
        error?.status === 429 ||
        error?.message?.includes('rate limit') ||
        error?.message?.includes('429');

      if (isRateLimit && attempt < maxRetries - 1) {
        // Exponential backoff: 2^attempt seconds (2s, 4s, 8s)
        const delay = Math.pow(2, attempt + 1) * 1000;
        logger.warn(
          `[Document Processor] Rate limit hit (${modelName}): attempt ${attempt + 1}/${maxRetries}, retrying in ${Math.round(delay / 1000)}s`
        );
        await new Promise((resolve) => setTimeout(resolve, delay));
        continue;
      }

      // Re-throw error if not rate limit or max retries exceeded
      throw error;
    }
  }

  throw new Error('Max retries exceeded for Anthropic text generation');
}

/**
 * Generates text using the OpenAI API
 */
async );

  const modelInstance = openai.chat(modelName);

  const result = await aiGenerateText({
    model: modelInstance,
    prompt: prompt,
    system: system,
    temperature: 0.3,
    maxTokens: maxTokens,
  });

  const totalTokens = result.usage.promptTokens + result.usage.completionTokens;
  logger.debug(
    `[Document Processor] OpenAI ${modelName}: ${totalTokens} tokens (${result.usage.promptTokens}→${result.usage.completionTokens})`
  );

  return result;
}

/**
 * Generates text using Google's API
 */
async 

  // Create model instance directly from google provider
  const modelInstance = googleProvider(modelName);

  const result = await aiGenerateText({
    model: modelInstance,
    prompt: prompt,
    system: system,
    temperature: 0.3,
    maxTokens: maxTokens,
  });

  const totalTokens = result.usage.promptTokens + result.usage.completionTokens;
  logger.debug(
    `[Document Processor] Google ${modelName}: ${totalTokens} tokens (${result.usage.promptTokens}→${result.usage.completionTokens})`
  );

  return result;
}

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
async ,
  autoCacheContextualRetrieval = true
): Promise<GenerateTextResult<any, any>> {
  const openrouter = createOpenRouter({
    apiKey: config.OPENROUTER_API_KEY as string,
    baseURL: config.OPENROUTER_BASE_URL,
  });

  const modelInstance = openrouter.chat(modelName);

  // Determine if this is a Claude or Gemini model for caching
  const isClaudeModel = modelName.toLowerCase().includes('claude');
  const isGeminiModel = modelName.toLowerCase().includes('gemini');
  const isGemini25Model = modelName.toLowerCase().includes('gemini-2.5');
  const supportsCaching = isClaudeModel || isGeminiModel;

  // Extract document for caching from explicit param or auto-detect from prompt
  let documentForCaching: string | undefined = cacheDocument;

  if (!documentForCaching && autoCacheContextualRetrieval && supportsCaching) {
    // Try to extract document from the prompt if it contains document tags
    const docMatch = prompt.match(/<document>([\s\S]*?)<\/document>/);
    if (docMatch && docMatch[1]) {
      documentForCaching = docMatch[1].trim();
      logger.debug(
        `[Document Processor] Auto-detected document for caching (${documentForCaching.length} chars)`
      );
    }
  }

  // Only apply caching if we have a document to cache
  if (documentForCaching && supportsCaching) {
    // Define cache options
    const effectiveCacheOptions = cacheOptions || { type: 'ephemeral' };

    // Parse out the prompt part - if it's a contextual query, strip document tags
    let promptText = prompt;
    if (promptText.includes('<document>')) {
      promptText = promptText.replace(/<document>[\s\S]*?<\/document>/, '').trim();
    }

    if (isClaudeModel) {
      return await generateClaudeWithCaching(
        promptText,
        system,
        modelInstance,
        modelName,
        maxTokens,
        documentForCaching
      );
    } else if (isGeminiModel) {
      return await generateGeminiWithCaching(
        promptText,
        system,
        modelInstance,
        modelName,
        maxTokens,
        documentForCaching,
        isGemini25Model
      );
    }
  }

  // Standard request without caching
  logger.debug('[Document Processor] Using standard request without caching');
  return await generateStandardOpenRouterText(prompt, system, modelInstance, modelName, maxTokens);
}

/**
 * Generates text using Claude with caching via OpenRouter
 */
async `);

  // Structure for Claude models
  const messages = [
    // System message with cached document (if system is provided)
    system
      ? {
          role: 'system',
          content: [
            {
              type: 'text',
              text: system,
            },
            {
              type: 'text',
              text: documentForCaching,
              cache_control: {
                type: 'ephemeral',
              },
            },
          ],
        }
      : // User message with cached document (if no system message)
        {
          role: 'user',
          content: [
            {
              type: 'text',
              text: 'Document for context:',
            },
            {
              type: 'text',
              text: documentForCaching,
              cache_control: {
                type: 'ephemeral',
              },
            },
            {
              type: 'text',
              text: promptText,
            },
          ],
        },
    // Only add user message if system was provided (otherwise we included user above)
    system
      ? {
          role: 'user',
          content: [
            {
              type: 'text',
              text: promptText,
            },
          ],
        }
      : null,
  ].filter(Boolean);

  logger.debug('[Document Processor] Using Claude-specific caching structure');

  // Generate text with cache-enabled structured messages
  const result = await aiGenerateText({
    model: modelInstance,
    messages: messages as any,
    temperature: 0.3,
    maxTokens: maxTokens,
    providerOptions: {
      openrouter: {
        usage: {
          include: true,
        },
      },
    },
  });

  logCacheMetrics(result);
  const totalTokens = result.usage.promptTokens + result.usage.completionTokens;
  logger.debug(
    `[Document Processor] OpenRouter ${modelName}: ${totalTokens} tokens (${result.usage.promptTokens}→${result.usage.completionTokens})`
  );

  return result;
}

/**
 * Generates text using Gemini with caching via OpenRouter
 */
async `);
    logger.debug(
      `[Document Processor] Gemini 2.5 models automatically cache large prompts (no cache_control needed)`
    );

    if (likelyTriggersCaching) {
      logger.debug(
        `[Document Processor] Document ~${estimatedDocTokens} tokens exceeds ${minTokensForImplicitCache} token threshold for caching`
      );
    } else {
      logger.debug(
        `[Document Processor] Document ~${estimatedDocTokens} tokens may not meet ${minTokensForImplicitCache} token threshold for caching`
      );
    }
  } else {
    logger.debug(`[Document Processor] Using standard prompt format with Gemini ${modelName}`);
    logger.debug(
      `[Document Processor] Note: Only Gemini 2.5 models support automatic implicit caching`
    );
  }

  // For Gemini models, we use a simpler format that works well with OpenRouter
  // The key for implicit caching is to keep the initial parts of the prompt consistent
  const geminiSystemPrefix = system ? `${system}\n\n` : '';

  // Format consistent with OpenRouter and Gemini expectations
  const geminiPrompt = `${geminiSystemPrefix}${documentForCaching}\n\n${promptText}`;

  // Generate text with simple prompt structure to leverage implicit caching
  const result = await aiGenerateText({
    model: modelInstance,
    prompt: geminiPrompt,
    temperature: 0.3,
    maxTokens: maxTokens,
    providerOptions: {
      openrouter: {
        usage: {
          include: true, // Include usage info to see cache metrics
        },
      },
    },
  });

  logCacheMetrics(result);
  const totalTokens = result.usage.promptTokens + result.usage.completionTokens;
  const cachingType = usingImplicitCaching ? 'implicit' : 'standard';
  logger.debug(
    `[Document Processor] OpenRouter ${modelName} (${cachingType} caching): ${totalTokens} tokens (${result.usage.promptTokens}→${result.usage.completionTokens})`
  );

  return result;
}

/**
 * Generates text using standard OpenRouter API (no caching)
 */
async ,
      },
    },
  });

  const totalTokens = result.usage.promptTokens + result.usage.completionTokens;
  logger.debug(
    `[Document Processor] OpenRouter ${modelName}: ${totalTokens} tokens (${result.usage.promptTokens}→${result.usage.completionTokens})`
  );

  return result;
}

/**
 * Logs cache metrics if available in the result
 */
, discount: ${(result.usage as any).cacheDiscount}`
    );
  }
}

/**
 * Generates text using Ollama
 */
async  = await import('ollama-ai-provider');
    const { generateText } = await import('ai');

    const baseURL = config.OLLAMA_BASE_URL || 'http://localhost:11434';
    const apiBase = baseURL.endsWith('/api') ? baseURL.slice(0, -4) : baseURL;

    const ollama = createOllama({
      baseURL: apiBase,
    });

    const model = cleanModelName(modelName || 'llama3.2:3b');
    logger.debug(`[Document Processor] Ollama text generation with model: ${model}`);

    const { text } = await generateText({
      model: ollama(model),
      prompt: prompt,
      system: system,
      temperature: 0.3,
      maxTokens: maxTokens,
    });

    // Ollama doesn't provide token usage information, so we estimate
    // Rough estimation: ~4 characters per token (common approximation)
    const estimatedPromptTokens = Math.ceil(prompt.length / 4);
    const estimatedCompletionTokens = Math.ceil(text.length / 4);
    const estimatedTotalTokens = estimatedPromptTokens + estimatedCompletionTokens;

    logger.debug(
      `[Document Processor] Ollama ${model}: generated ${text.length} characters (estimated: ${estimatedTotalTokens} tokens)`
    );

    // Create proper GenerateTextResult format with estimated usage
    // Note: These are rough estimates since Ollama doesn't provide actual token counts
    return {
      text,
      usage: {
        promptTokens: estimatedPromptTokens,
        completionTokens: estimatedCompletionTokens,
        totalTokens: estimatedTotalTokens,
      },
    } as GenerateTextResult<any, any>;
  } catch (error) {
    logger.error(`[Document Processor] Ollama text generation error:`, error);
    throw error;
  }
}

} // namespace elizaos
