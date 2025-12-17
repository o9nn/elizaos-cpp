#include "llm.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string cleanModelName(const std::string& modelName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return modelName.replace(/^[\"']|[\"']$/g, '');

}

std::future<> generateTextEmbedding(IAgentRuntime runtime, const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    embedding: number[]
}

std::future<std::vector> generateTextEmbeddingsBatch(IAgentRuntime runtime, const std::vector<std::string>& texts, number = 20 batchSize) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    embedding: number[] | nullptr; success: boolean; error?: any; index: number
}

std::future<> generateOpenAIEmbedding(const std::string& text, ModelConfig config, double dimensions) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    embedding: number[]
}

std::future<> generateGoogleEmbedding(const std::string& text, ModelConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    embedding: number[]
}

std::future<> generateOllamaEmbedding(const std::string& text, ModelConfig config, double dimensions) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    embedding: number[]
}

std::future<GenerateTextResult<any, any>> generateText(IAgentRuntime runtime, const std::string& prompt, std::optional<std::string> system, std::optional<TextGenerationOptions> overrideConfig) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto config = validateModelConfig(runtime);
        const auto provider = overrideConfig.provider || config.TEXT_PROVIDER;
        const auto modelName = overrideConfig.modelName || config.TEXT_MODEL;
        const auto maxTokens = overrideConfig.maxTokens || config.MAX_OUTPUT_TOKENS;

        // Auto-detect contextual retrieval prompts for caching - enabled by default
        const auto autoCacheContextualRetrieval = overrideConfig.autoCacheContextualRetrieval != false;

        try {
            switch (provider) {
                case 'anthropic':
                return generateAnthropicText(config, prompt, system, modelName!, maxTokens);
                case 'openai':
                return generateOpenAIText(config, prompt, system, modelName!, maxTokens);
                case 'openrouter':
                return generateOpenRouterText(;
                config,
                prompt,
                system,
                modelName!,
                maxTokens,
                overrideConfig.cacheDocument,
                overrideConfig.cacheOptions,
                autoCacheContextualRetrieval;
                );
                case 'google':
                return generateGoogleText(prompt, system, modelName!, maxTokens, config);
                case 'ollama':
                return generateOllamaText(config, prompt, system, modelName!, maxTokens);
                default:
                throw std::runtime_error(`Unsupported text provider: ${provider}`);
            }
            } catch (error) {
                std::cerr << "[Document Processor] " + std::to_string(provider) + " " + std::to_string(modelName) + " error:" << error << std::endl;
                throw;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<GenerateTextResult<any, any>> generateAnthropicText(ModelConfig config, const std::string& prompt, const std::string& system, const std::string& modelName, double maxTokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto anthropic = createAnthropic({;
            apiKey: config.ANTHROPIC_API_KEY,
            baseURL: config.ANTHROPIC_BASE_URL,
            });

            const auto modelInstance = anthropic(modelName);

            // Retry with exponential backoff for rate limit errors
            const auto maxRetries = 3;
            for (int attempt = 0; attempt < maxRetries; attempt++) {
                try {
                    const auto result = aiGenerateText({;
                        model: modelInstance,
                        prompt: prompt,
                        system: system,
                        temperature: 0.3,
                        maxTokens: maxTokens,
                        });

                        const auto totalTokens = result.usage.promptTokens + result.usage.completionTokens;
                        logger.debug(
                        "[Document Processor] " + std::to_string(modelName) + ": " + std::to_string(totalTokens) + " tokens (" + std::to_string(result.usage.promptTokens) + "→" + std::to_string(result.usage.completionTokens) + ")"
                        );

                        return result;
                        } catch (error: any) {
                            // Check if it's a rate limit error (status 429)
                            const auto isRateLimit =;
                            error.status == 429 ||;
                            error.message.includes('rate limit') ||;
                            error.message.includes('429');

                            if (isRateLimit && attempt < maxRetries - 1) {
                                // Exponential backoff: 2^attempt seconds (2s, 4s, 8s)
                                const auto delay = Math.pow(2, attempt + 1) * 1000;
                                logger.warn(
                                "[Document Processor] Rate limit hit (" + std::to_string(modelName) + "): attempt " + std::to_string(attempt + 1) + "/" + std::to_string(maxRetries) + ", retrying in " + std::to_string(Math.round(delay / 1000)) + "s"
                                );
                                new Promise((resolve) => setTimeout(resolve, delay));
                                continue;
                            }

                            // Re-throw error if not rate limit or max retries exceeded
                            throw;
                        }
                    }

                    throw std::runtime_error('Max retries exceeded for Anthropic text generation');

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<GenerateTextResult<any, any>> generateOpenAIText(ModelConfig config, const std::string& prompt, const std::string& system, const std::string& modelName, double maxTokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto openai = createOpenAI({;
        apiKey: config.OPENAI_API_KEY,
        baseURL: config.OPENAI_BASE_URL,
        });

        const auto modelInstance = openai.chat(modelName);

        const auto result = aiGenerateText({;
            model: modelInstance,
            prompt: prompt,
            system: system,
            temperature: 0.3,
            maxTokens: maxTokens,
            });

            const auto totalTokens = result.usage.promptTokens + result.usage.completionTokens;
            logger.debug(
            "[Document Processor] OpenAI " + std::to_string(modelName) + ": " + std::to_string(totalTokens) + " tokens (" + std::to_string(result.usage.promptTokens) + "→" + std::to_string(result.usage.completionTokens) + ")"
            );

            return result;

}

std::future<GenerateTextResult<any, any>> generateGoogleText(const std::string& prompt, const std::string& system, const std::string& modelName, double maxTokens, ModelConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Use the google provider directly
    const auto googleProvider = google;
    if (config.GOOGLE_API_KEY) {
        // Google provider uses env var GOOGLE_GENERATIVE_AI_API_KEY by default
        process.env.GOOGLE_GENERATIVE_AI_API_KEY = config.GOOGLE_API_KEY;
    }

    // Create model instance directly from google provider
    const auto modelInstance = googleProvider(modelName);

    const auto result = aiGenerateText({;
        model: modelInstance,
        prompt: prompt,
        system: system,
        temperature: 0.3,
        maxTokens: maxTokens,
        });

        const auto totalTokens = result.usage.promptTokens + result.usage.completionTokens;
        logger.debug(
        "[Document Processor] Google " + std::to_string(modelName) + ": " + std::to_string(totalTokens) + " tokens (" + std::to_string(result.usage.promptTokens) + "→" + std::to_string(result.usage.completionTokens) + ")"
        );

        return result;

}

std::future<GenerateTextResult<any, any>> generateOpenRouterText(ModelConfig config, const std::string& prompt, const std::string& system, const std::string& modelName, double maxTokens, std::optional<std::string> cacheDocument, std::optional<std::any> cacheOptions, auto autoCacheContextualRetrieval = true) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto openrouter = createOpenRouter({;
        apiKey: config.OPENROUTER_API_KEY,
        baseURL: config.OPENROUTER_BASE_URL,
        });

        const auto modelInstance = openrouter.chat(modelName);

        // Determine if this is a Claude or Gemini model for caching
        const auto isClaudeModel = modelName.toLowerCase().includes('claude');
        const auto isGeminiModel = modelName.toLowerCase().includes('gemini');
        const auto isGemini25Model = modelName.toLowerCase().includes('gemini-2.5');
        const auto supportsCaching = isClaudeModel || isGeminiModel;

        // Extract document for caching from explicit param or auto-detect from prompt
        std::string documentForCaching = cacheDocument;

        if (!documentForCaching && autoCacheContextualRetrieval && supportsCaching) {
            // Try to extract document from the prompt if it contains document tags
            const auto docMatch = prompt.match(/<document>([\s\S]*?)<\/document>/);
            if (docMatch && docMatch[1]) {
                documentForCaching = docMatch[1].trim();
                logger.debug(
                "[Document Processor] Auto-detected document for caching (" + std::to_string(documentForCaching.length) + " chars)";
                );
            }
        }

        // Only apply caching if we have a document to cache
        if (documentForCaching && supportsCaching) {
            // Define cache options
            const auto effectiveCacheOptions = cacheOptions || { type: 'ephemeral' };

            // Parse out the prompt part - if it's a contextual query, strip document tags
            auto promptText = prompt;
            if (promptText.includes('<document>')) {
                promptText = promptText.replace(/<document>[\s\S]*?<\/document>/, '').trim();
            }

            if (isClaudeModel) {
                return generateClaudeWithCaching(;
                promptText,
                system,
                modelInstance,
                modelName,
                maxTokens,
                documentForCaching;
                );
                } else if (isGeminiModel) {
                    return generateGeminiWithCaching(;
                    promptText,
                    system,
                    modelInstance,
                    modelName,
                    maxTokens,
                    documentForCaching,
                    isGemini25Model;
                    );
                }
            }

            // Standard request without caching
            logger.debug('[Document Processor] Using standard request without caching');
            return generateStandardOpenRouterText(prompt, system, modelInstance, modelName, maxTokens);

}

std::future<GenerateTextResult<any, any>> generateClaudeWithCaching(const std::string& promptText, const std::string& system, const std::any& modelInstance, const std::string& modelName, double maxTokens, const std::string& documentForCaching) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    logger.debug(`[Document Processor] Using explicit prompt caching with Claude ${modelName}`);

    // Structure for Claude models
    const auto messages = [;
    // System message with cached document (if system is provided)
    system;
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
                                    system;
                                    ? {
                                        role: 'user',
                                        content: [
                                        {
                                            type: 'text',
                                            text: promptText,
                                            },
                                            ],
                                        }
                                        : nullptr,
                                        ].filter(Boolean);

                                        logger.debug('[Document Processor] Using Claude-specific caching structure');

                                        // Generate text with cache-enabled structured messages
                                        const auto result = aiGenerateText({;
                                            model: modelInstance,
                                            messages: messages,
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
                                                        const auto totalTokens = result.usage.promptTokens + result.usage.completionTokens;
                                                        logger.debug(
                                                        "[Document Processor] OpenRouter " + std::to_string(modelName) + ": " + std::to_string(totalTokens) + " tokens (" + std::to_string(result.usage.promptTokens) + "→" + std::to_string(result.usage.completionTokens) + ")"
                                                        );

                                                        return result;

}

std::future<GenerateTextResult<any, any>> generateGeminiWithCaching(const std::string& promptText, const std::string& system, const std::any& modelInstance, const std::string& modelName, double maxTokens, const std::string& documentForCaching, bool isGemini25Model) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Gemini models support implicit caching as of 2.5 models
    const auto usingImplicitCaching = isGemini25Model;

    // Check if document is large enough for implicit caching
    // Gemini 2.5 Flash requires minimum 1028 tokens, Gemini 2.5 Pro requires 2048 tokens
    const auto estimatedDocTokens = Math.ceil(documentForCaching.length / 4); // Rough estimate of tokens;
    const auto minTokensForImplicitCache = modelName.toLowerCase().includes('flash') ? 1028 : 2048;
    const auto likelyTriggersCaching = estimatedDocTokens >= minTokensForImplicitCache;

    if (usingImplicitCaching) {
        logger.debug(`[Document Processor] Using Gemini 2.5 implicit caching with ${modelName}`);
        logger.debug(
        "[Document Processor] Gemini 2.5 models automatically cache large prompts (no cache_control needed)";
        );

        if (likelyTriggersCaching) {
            logger.debug(
            "[Document Processor] Document ~" + std::to_string(estimatedDocTokens) + " tokens exceeds " + std::to_string(minTokensForImplicitCache) + " token threshold for caching";
            );
            } else {
                logger.debug(
                "[Document Processor] Document ~" + std::to_string(estimatedDocTokens) + " tokens may not meet " + std::to_string(minTokensForImplicitCache) + " token threshold for caching";
                );
            }
            } else {
                logger.debug(`[Document Processor] Using standard prompt format with Gemini ${modelName}`);
                logger.debug(
                "[Document Processor] Note: Only Gemini 2.5 models support automatic implicit caching"
                );
            }

            // For Gemini models, we use a simpler format that works well with OpenRouter
            // The key for implicit caching is to keep the initial parts of the prompt consistent
            const auto geminiSystemPrefix = std::to_string(system) + "\n\n";

            // Format consistent with OpenRouter and Gemini expectations
            const auto geminiPrompt = std::to_string(geminiSystemPrefix) + std::to_string(documentForCaching) + "\n\n" + std::to_string(promptText);

            // Generate text with simple prompt structure to leverage implicit caching
            const auto result = aiGenerateText({;
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
                            const auto totalTokens = result.usage.promptTokens + result.usage.completionTokens;
                            const auto cachingType = usingImplicitCaching ? 'implicit' : 'standard';
                            logger.debug(
                            "[Document Processor] OpenRouter " + std::to_string(modelName) + " (" + std::to_string(cachingType) + " caching): " + std::to_string(totalTokens) + " tokens (" + std::to_string(result.usage.promptTokens) + "→" + std::to_string(result.usage.completionTokens) + ")"
                            );

                            return result;

}

std::future<GenerateTextResult<any, any>> generateStandardOpenRouterText(const std::string& prompt, const std::string& system, const std::any& modelInstance, const std::string& modelName, double maxTokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto result = aiGenerateText({;
        model: modelInstance,
        prompt: prompt,
        system: system,
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

                    const auto totalTokens = result.usage.promptTokens + result.usage.completionTokens;
                    logger.debug(
                    "[Document Processor] OpenRouter " + std::to_string(modelName) + ": " + std::to_string(totalTokens) + " tokens (" + std::to_string(result.usage.promptTokens) + "→" + std::to_string(result.usage.completionTokens) + ")"
                    );

                    return result;

}

void logCacheMetrics(GenerateTextResult<any result, auto any>) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (result.usage && (result.usage as any).cacheTokens) {
        logger.debug(
        "[Document Processor] Cache metrics - tokens: " + std::to_string((result.usage).cacheTokens) + ", discount: " + std::to_string((result.usage).cacheDiscount)
        );
    }

}

std::future<GenerateTextResult<any, any>> generateOllamaText(ModelConfig config, const std::string& prompt, const std::string& system, const std::string& modelName, double maxTokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Use the same approach as plugin-ollama
            const auto { createOllama } = import('ollama-ai-provider');
            const auto { generateText } = import('ai');

            const auto baseURL = config.OLLAMA_BASE_URL || 'http://localhost:11434';
            const auto apiBase = baseURL.endsWith('/api') ? baseURL.slice(0, -4) : baseURL;

            const auto ollama = createOllama({;
                baseURL: apiBase,
                });

                const auto model = cleanModelName(modelName || 'llama3.2:3b');
                logger.debug(`[Document Processor] Ollama text generation with model: ${model}`);

                const auto { text } = generateText({;
                    model: ollama(model),
                    prompt: prompt,
                    system: system,
                    temperature: 0.3,
                    maxTokens: maxTokens,
                    });

                    // Ollama doesn't provide token usage information, so we estimate
                    // Rough estimation: ~4 characters per token (common approximation)
                    const auto estimatedPromptTokens = Math.ceil(prompt.length / 4);
                    const auto estimatedCompletionTokens = Math.ceil(text.length / 4);
                    const auto estimatedTotalTokens = estimatedPromptTokens + estimatedCompletionTokens;

                    logger.debug(
                    "[Document Processor] Ollama " + std::to_string(model) + ": generated " + std::to_string(text.length) + " characters (estimated: " + std::to_string(estimatedTotalTokens) + " tokens)"
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
                            }<any, any>;
                            } catch (error) {
                                std::cerr << "[Document Processor] Ollama text generation error:" << error << std::endl;
                                throw;
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
