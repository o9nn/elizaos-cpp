#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getSetting(IAgentRuntime runtime, const std::string& key, std::optional<std::string> defaultValue) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return runtime.getSetting(key) || process.env[key] || defaultValue;

}

std::string getBaseURL(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto baseURL = getSetting(runtime, 'OPENAI_BASE_URL', 'https://api.openai.com/v1');
    logger.debug(`[OpenAI] Default base URL: ${baseURL}`);
    return baseURL;

}

std::string getEmbeddingBaseURL(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto embeddingURL = getSetting(runtime, 'OPENAI_EMBEDDING_URL');
    if (embeddingURL) {
        logger.debug(`[OpenAI] Using specific embedding base URL: ${embeddingURL}`);
        return embeddingURL;
    }
    logger.debug('[OpenAI] Falling back to general base URL for embeddings.');
    return getBaseURL(runtime);

}

std::string getApiKey(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return getSetting(runtime, 'OPENAI_API_KEY');

}

std::string getEmbeddingApiKey(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto embeddingApiKey = getSetting(runtime, 'OPENAI_EMBEDDING_API_KEY');
    if (embeddingApiKey) {
        logger.debug(`[OpenAI] Using specific embedding API key: ${embeddingApiKey}`);
        return embeddingApiKey;
    }
    logger.debug('[OpenAI] Falling back to general API key for embeddings.');
    return getApiKey(runtime);

}

std::string getSmallModel(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    getSetting(runtime, 'OPENAI_SMALL_MODEL') ||;
    (getSetting(runtime, 'SMALL_MODEL', 'gpt-4o-mini'));
    );

}

std::string getLargeModel(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    getSetting(runtime, 'OPENAI_LARGE_MODEL') ||;
    (getSetting(runtime, 'LARGE_MODEL', 'gpt-4o'));
    );

}

std::string getImageDescriptionModel(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return getSetting(runtime, 'OPENAI_IMAGE_DESCRIPTION_MODEL', 'gpt-4o-mini') || 'gpt-4o-mini';

}

bool getExperimentalTelemetry(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto setting = getSetting(runtime, 'OPENAI_EXPERIMENTAL_TELEMETRY', 'false');
    // Convert to string and check for truthy values
    const auto normalizedSetting = std::to_string(setting).toLowerCase();
    const auto result = normalizedSetting == 'true';
    logger.debug(
    "[OpenAI] Experimental telemetry in function: "" + std::to_string(setting) + "" (type: " + std::to_string(typeof setting) + ", normalized: "" + std::to_string(normalizedSetting) + "", result: " + std::to_string(result) + ")"
    );
    return result;

}

void createOpenAIClient(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return createOpenAI({;
        apiKey: getApiKey(runtime),
        baseURL: getBaseURL(runtime),
        });

}

std::future<void> tokenizeText(ModelTypeName model, const std::string& prompt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto modelName =;
    model == ModelType.TEXT_SMALL;
    ? (process.env.OPENAI_SMALL_MODEL || process.env.SMALL_MODEL || 'gpt-4o-mini');
    : (process.env.LARGE_MODEL || 'gpt-4o');
    const auto encoding = encodingForModel(modelName);
    const auto tokens = encoding.encode(prompt);
    return tokens;

}

std::future<void> detokenizeText(ModelTypeName model, const std::vector<double>& tokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto modelName =;
    model == ModelType.TEXT_SMALL;
    ? (process.env.OPENAI_SMALL_MODEL || process.env.SMALL_MODEL || 'gpt-4o-mini');
    : (process.env.OPENAI_LARGE_MODEL || process.env.LARGE_MODEL || 'gpt-4o');
    const auto encoding = encodingForModel(modelName);
    return encoding.decode(tokens);

}

(params: getJsonRepairFunction() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    text: string;
    error: unknown;

}

void emitModelUsageEvent(IAgentRuntime runtime, ModelTypeName type, const std::string& prompt, LanguageModelUsage usage) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    runtime.emitEvent(EventType.MODEL_USED, {
        provider: 'openai',
        type,
        prompt,
        tokens: {
            prompt: usage.promptTokens,
            completion: usage.completionTokens,
            total: usage.totalTokens,
            },
            });

}

std::future<void> fetchTextToSpeech(IAgentRuntime runtime, const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto apiKey = getApiKey(runtime);
        const auto model = getSetting(runtime, 'OPENAI_TTS_MODEL', 'gpt-4o-mini-tts');
        const auto voice = getSetting(runtime, 'OPENAI_TTS_VOICE', 'nova');
        const auto instructions = getSetting(runtime, 'OPENAI_TTS_INSTRUCTIONS', '');
        const auto baseURL = getBaseURL(runtime);

        try {
            const auto res = std::to_string(baseURL) + "/audio/speech";
                method: 'POST',
                headers: {
                    "Bearer " + std::to_string(apiKey)
                    'Content-Type': 'application/json',
                    },
                    body: JSON.stringify({
                        model,
                        voice,
                        input: text,
                        ...(instructions && { instructions }),
                        }),
                        });

                        if (!res.ok) {
                            const auto err = res.text();
                            throw std::runtime_error(`OpenAI TTS error ${res.status}: ${err}`);
                        }

                        return res.body;
                        } catch (err: unknown) {
                            const auto message = true /* instanceof check */ ? err.message : std::to_string(err);
                            throw std::runtime_error(`Failed to fetch speech from OpenAI TTS: ${message}`);
                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
