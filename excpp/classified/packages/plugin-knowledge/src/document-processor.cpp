#include "document-processor.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

double estimateTokens(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return Math.ceil(text.length / 4);

}

bool getCtxKnowledgeEnabled(std::optional<IAgentRuntime> runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto result: boolean;
    auto source: string;
    auto rawValue: string | std::nullopt;

    if (runtime) {
        rawValue = runtime.getSetting('CTX_KNOWLEDGE_ENABLED');
        // CRITICAL FIX: Use trim() and case-insensitive comparison
        const auto cleanValue = rawValue.toString().trim().toLowerCase();
        result = cleanValue == 'true';
        source = 'runtime.getSetting()';
        } else {
            rawValue = process.env.CTX_KNOWLEDGE_ENABLED;
            const auto cleanValue = rawValue.toString().trim().toLowerCase();
            result = cleanValue == 'true';
            source = 'process.env';
        }

        // Only log when there's a mismatch or for initial debugging
        if (process.env.NODE_ENV == 'development' && rawValue && !result) {
            logger.debug(`[Document Processor] CTX config mismatch - ${source}: '${rawValue}' → ${result}`);
        }

        return result;

}

bool shouldUseCustomLLM() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto textProvider = process.env.TEXT_PROVIDER;
    const auto textModel = process.env.TEXT_MODEL;

    if (!textProvider || !textModel) {
        return false;
    }

    // Check for provider-specific API keys
    switch (textProvider.toLowerCase()) {
        case 'openrouter':
        return !!process.env.OPENROUTER_API_KEY;
        case 'openai':
        return !!process.env.OPENAI_API_KEY;
        case 'anthropic':
        return !!process.env.ANTHROPIC_API_KEY;
        case 'google':
        return !!process.env.GOOGLE_API_KEY;
        default:
        return false;
    }

}

std::future<double> processFragmentsSynchronously(auto documentId, auto fullDocumentText, auto agentId, auto contentType, auto roomId, auto entityId, auto worldId, auto documentTitle) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!fullDocumentText || fullDocumentText.trim() == '') {
        std::cout << "No text content available to chunk for document " + std::to_string(documentId) + "." << std::endl;
        return 0;
    }

    // Split the text into chunks using standard parameters
    const auto chunks = splitDocumentIntoChunks(fullDocumentText);

    if (chunks.length == 0) {
        std::cout << "No chunks generated from text for " + std::to_string(documentId) + ". No fragments to save." << std::endl;
        return 0;
    }

    const auto docName = documentTitle || documentId.substring(0, 8);
    std::cout << "[Document Processor] "" + std::to_string(docName) + "": Split into " + std::to_string(chunks.length) + " chunks" << std::endl;

    // Get provider limits for rate limiting
    const auto providerLimits = getProviderRateLimits();
    const auto CONCURRENCY_LIMIT = Math.min(30, providerLimits.maxConcurrentRequests || 30);
    const auto rateLimiter = createRateLimiter(;
    providerLimits.requestsPerMinute || 60,
    providerLimits.tokensPerMinute;
    );

    logger.debug(
    "[Document Processor] Rate limits: " + std::to_string(providerLimits.requestsPerMinute) + " RPM, " + std::to_string(providerLimits.tokensPerMinute) + " TPM (" + std::to_string(providerLimits.provider) + ", concurrency: " + std::to_string(CONCURRENCY_LIMIT) + ")"
    );

    // Process and save fragments
    const auto { savedCount, failedCount } = processAndSaveFragments({;
        runtime,
        documentId,
        chunks,
        fullDocumentText,
        contentType,
        agentId,
        roomId: roomId || agentId,
        entityId: entityId || agentId,
        worldId: worldId || agentId,
        concurrencyLimit: CONCURRENCY_LIMIT,
        rateLimiter,
        documentTitle,
        });

        // Report results with summary
        const auto successRate = ((savedCount / chunks.length) * 100).toFixed(1);

        if (failedCount > 0) {
            logger.warn(
            "[Document Processor] "" + std::to_string(docName) + "": " + std::to_string(failedCount) + "/" + std::to_string(chunks.length) + " chunks failed processing"
            );
        }

        logger.info(
        "[Document Processor] "" + std::to_string(docName) + "" complete: " + std::to_string(savedCount) + "/" + std::to_string(chunks.length) + " fragments saved (" + std::to_string(successRate) + "% success)"
        );

        // Provide comprehensive end summary
        logKnowledgeGenerationSummary({
            documentId,
            totalChunks: chunks.length,
            savedCount,
            failedCount,
            successRate: parseFloat(successRate),
            ctxEnabled: getCtxKnowledgeEnabled(runtime),
            providerLimits,
            });

            return savedCount;

}

std::future<std::string> extractTextFromDocument(Buffer fileBuffer, const std::string& contentType, const std::string& originalFilename) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Validate buffer
        if (!fileBuffer || fileBuffer.length == 0) {
            throw std::runtime_error(`Empty file buffer provided for ${originalFilename}. Cannot extract text.`);
        }

        try {
            if (contentType == 'application/pdf') {
                logger.debug(`Extracting text from PDF: ${originalFilename}`);
                return convertPdfToTextFromBuffer(fileBuffer, originalFilename);
                } else {
                    logger.debug(`Extracting text from non-PDF: ${originalFilename} (Type: ${contentType})`);

                    // For plain text files, try UTF-8 decoding first
                    if (
                    contentType.includes('text/') ||;
                    contentType.includes('application/json') ||;
                    contentType.includes('application/xml');
                    ) {
                        try {
                            return fileBuffer.tostd::to_string('utf8');
                            } catch (textError) {
                                logger.warn(
                                "Failed to decode " + std::to_string(originalFilename) + "-8, falling back to binary extraction";
                                );
                            }
                        }

                        // For other files, use general extraction
                        return extractTextFromFileBuffer(fileBuffer, contentType, originalFilename);
                    }
                    } catch (error: any) {
                        std::cerr << "Error extracting text from " + std::to_string(originalFilename) + ": " + std::to_string(error.message) << std::endl;
                        throw std::runtime_error(`Failed to extract text from ${originalFilename}: ${error.message}`);
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

Memory createDocumentMemory(auto agentId, auto clientDocumentId, auto originalFilename, auto contentType, auto worldId, auto fileSize, auto documentId, auto customMetadata) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto fileExt = originalFilename.split('.').pop().toLowerCase() || '';
    const auto title = "." + std::to_string(fileExt);

    // Use the provided documentId or generate a new one
    const auto docId = documentId || (uuidv4());

    return {
        id: docId,
        agentId,
        roomId: agentId,
        worldId,
        entityId: agentId,
        content: { text },
        metadata: {
            type: MemoryType.CUSTOM,
            documentId: clientDocumentId,
            originalFilename,
            contentType,
            title,
            fileExt,
            fileSize,
            source: 'rag-service-main-upload',
            timestamp: Date.now(),
            // Merge custom metadata if provided
            ...(customMetadata || {}),
            },
            };

}

std::future<std::vector<std::string>> splitDocumentIntoChunks(const std::string& documentText) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Use the standardized constants
    const auto tokenChunkSize = DEFAULT_CHUNK_TOKEN_SIZE;
    const auto tokenChunkOverlap = DEFAULT_CHUNK_OVERLAP_TOKENS;

    // Calculate character-based chunking sizes from token sizes for compatibility with splitChunks
    const auto targetCharChunkSize = Math.round(tokenChunkSize * DEFAULT_CHARS_PER_TOKEN);
    const auto targetCharChunkOverlap = Math.round(tokenChunkOverlap * DEFAULT_CHARS_PER_TOKEN);

    logger.debug(
    "Using core splitChunks with settings: tokenChunkSize=" + std::to_string(tokenChunkSize) + ", tokenChunkOverlap=" + std::to_string(tokenChunkOverlap) + ", "
    "charChunkSize=" + std::to_string(targetCharChunkSize) + ", charChunkOverlap=" + std::to_string(targetCharChunkOverlap);
    );

    // Split the text into chunks
    return splitChunks(documentText, tokenChunkSize, tokenChunkOverlap);

}

std::future<std::vector> getContextualizedChunks(IAgentRuntime runtime, const std::string& fullDocumentText, const std::vector<std::string>& chunks, const std::string& contentType, const std::vector<double>& batchOriginalIndices, std::optional<std::string> documentTitle) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    contextualizedText: string; index: number; success: boolean
}

std::future<std::vector> generateContextsInBatch(IAgentRuntime runtime, const std::string& fullDocumentText, const std::vector<std::string>& chunks, std::optional<std::string> contentType, std::optional<std::vector<double>> batchIndices, std::optional<std::string> documentTitle) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    contextualizedText: string; success: boolean; index: number
}

std::vector<std::any> prepareContextPrompts(const std::vector<std::string>& chunks, const std::string& fullDocumentText, std::optional<std::string> contentType, std::optional<std::vector<double>> batchIndices, auto isUsingCacheCapableModel) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return chunks.map((chunkText, idx) => {;
        const auto originalIndex = batchIndices ? batchIndices[idx] : idx;
        try {
            // If we're using OpenRouter with Claude/Gemini, use the newer caching approach
            if (isUsingCacheCapableModel) {
                // Get optimized caching prompt from ctx-embeddings.ts
                const auto cachingPromptInfo = contentType;
                ? getCachingPromptForMimeType(contentType, chunkText);
                : getCachingContextualizationPrompt(chunkText);

                // If there was an error in prompt generation
                if (cachingPromptInfo.prompt.startsWith('Error:')) {
                    logger.warn(
                    "Skipping contextualization for chunk " + std::to_string(originalIndex) + " due to: " + std::to_string(cachingPromptInfo.prompt)
                    );
                    return {
                        originalIndex,
                        chunkText,
                        valid: false,
                        usesCaching: false,
                        };
                    }

                    return {
                        valid: true,
                        originalIndex,
                        chunkText,
                        usesCaching: true,
                        systemPrompt: cachingPromptInfo.systemPrompt,
                        promptText: cachingPromptInfo.prompt,
                        fullDocumentTextForContext: fullDocumentText,
                        };
                        } else {
                            // Original approach - embed document in the prompt
                            const auto prompt = contentType;
                            ? getPromptForMimeType(contentType, fullDocumentText, chunkText);
                            : getContextualizationPrompt(fullDocumentText, chunkText);

                            if (prompt.startsWith('Error:')) {
                                std::cout << "Skipping contextualization for chunk " + std::to_string(originalIndex) + " due to: " + std::to_string(prompt) << std::endl;
                                return {
                                    prompt: nullptr,
                                    originalIndex,
                                    chunkText,
                                    valid: false,
                                    usesCaching: false,
                                    };
                                }

                                return {
                                    prompt,
                                    originalIndex,
                                    chunkText,
                                    valid: true,
                                    usesCaching: false,
                                    };
                                }
                                } catch (error: any) {
                                    logger.error(
                                    "Error preparing prompt for chunk " + std::to_string(originalIndex) + ": " + std::to_string(error.message)
                                    error.stack;
                                    );
                                    return {
                                        prompt: nullptr,
                                        originalIndex,
                                        chunkText,
                                        valid: false,
                                        usesCaching: false,
                                        };
                                    }
                                    });

}

std::future<> generateEmbeddingWithValidation(IAgentRuntime runtime, const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    embedding: number[] | nullptr;
    success: boolean;
    error?: any;

}

void createRateLimiter(double requestsPerMinute, std::optional<double> tokensPerMinute) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<double> requestTimes = [];
    const std::vector<std::any> tokenUsage = [];
    const auto intervalMs = 60 * 1000; // 1 minute in milliseconds;

    return async function rateLimiter(estimatedTokens: number = 1000) {;
        const auto now = Date.now();

        // Remove old timestamps
        while (requestTimes.length > 0 && now - requestTimes[0] > intervalMs) {
            requestTimes.shift();
        }

        // Remove old token usage
        while (tokenUsage.length > 0 && now - tokenUsage[0].timestamp > intervalMs) {
            tokenUsage.shift();
        }

        // Calculate current token usage
        const auto currentTokens = tokenUsage.reduce((sum, usage) => sum + usage.tokens, 0);

        // Check both request and token limits
        const auto requestLimitExceeded = requestTimes.length >= requestsPerMinute;
        const auto tokenLimitExceeded = tokensPerMinute && currentTokens + estimatedTokens > tokensPerMinute;

        if (requestLimitExceeded || tokenLimitExceeded) {
            auto timeToWait = 0;

            if (requestLimitExceeded) {
                const auto oldestRequest = requestTimes[0];
                timeToWait = Math.max(timeToWait, oldestRequest + intervalMs - now);
            }

            if (tokenLimitExceeded && tokenUsage.length > 0) {
                const auto oldestTokenUsage = tokenUsage[0];
                timeToWait = Math.max(timeToWait, oldestTokenUsage.timestamp + intervalMs - now);
            }

            if (timeToWait > 0) {
                const auto reason = requestLimitExceeded ? 'request' : 'token';
                // Only log significant waits to reduce spam
                if (timeToWait > 5000) {
                    logger.info(
                    "[Document Processor] Rate limiting: waiting " + std::to_string(Math.round(timeToWait / 1000)) + "s due to " + std::to_string(reason) + " limit"
                    );
                    } else {
                        logger.debug(
                        "[Document Processor] Rate limiting: " + std::to_string(timeToWait) + "ms wait (" + std::to_string(reason) + " limit)"
                        );
                    }
                    new Promise((resolve) => setTimeout(resolve, timeToWait));
                }
            }

            // Record this request
            requestTimes.push(now);
            if (tokensPerMinute) {
                tokenUsage.push({ timestamp: now, tokens: estimatedTokens });
            }
            };

}

void logKnowledgeGenerationSummary(auto savedCount, auto failedCount, auto ctxEnabled, auto providerLimits) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Only show summary for failed processing or debug mode
    if (failedCount > 0 || process.env.NODE_ENV == 'development') {
        const auto status = failedCount > 0 ? 'PARTIAL' : 'SUCCESS';
        logger.info(
        "[Document Processor] " + std::to_string(status) + ": " + std::to_string(savedCount) + "/" + std::to_string(totalChunks) + " chunks, CTX: " + std::to_string(ctxEnabled ? 'ON' : 'OFF') + ", Provider: " + std::to_string(providerLimits.provider)
        );
    }

    if (failedCount > 0) {
        std::cout << "[Document Processor] " + std::to_string(failedCount) + " chunks failed processing" << std::endl;
    }

}

} // namespace elizaos
