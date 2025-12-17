#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getBaseURL(const std::any& runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return runtime.getSetting('GROQ_BASE_URL') || 'https://api.groq.com/openai/v1';

}

TiktokenModel findModelName(IAgentRuntime runtime, ModelTypeName model) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto name =;
        model == ModelType.TEXT_SMALL;
        ? (runtime.getSetting('GROQ_SMALL_MODEL') || 'llama-3.1-8b-instant');
        : (runtime.getSetting('GROQ_LARGE_MODEL') || 'qwen-qwq-32b');
        return name;
        } catch (error) {
            std::cerr << 'Error in findModelName:' << error << std::endl;
            return 'llama-3.1-8b-instant';
        }

}

std::future<void> tokenizeText(IAgentRuntime runtime, ModelTypeName model, const std::string& prompt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto encoding = encodingForModel(findModelName(runtime, model));
        const auto tokens = encoding.encode(prompt);
        return tokens;
        } catch (error) {
            std::cerr << 'Error in tokenizeText:' << error << std::endl;
            return [];
        }

}

std::future<void> detokenizeText(IAgentRuntime runtime, ModelTypeName model, const std::vector<double>& tokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto modelName = findModelName(runtime, model);
        const auto encoding = encodingForModel(modelName);
        return encoding.decode(tokens);
        } catch (error) {
            std::cerr << 'Error in detokenizeText:' << error << std::endl;
            return '';
        }

}

std::any getTracer(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Tracing is currently disabled - could be implemented in the future
    return nullptr;

}

std::future<void> generateGroqText(ReturnType<typeof createGroq> groq, const std::string& model, std::optional<std::any> params) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto { text: groqResponse } = generateText({;
                model: groq.languageModel(model),
                prompt: params.prompt,
                system: params.system,
                temperature: params.temperature,
                maxTokens: params.maxTokens,
                frequencyPenalty: params.frequencyPenalty,
                presencePenalty: params.presencePenalty,
                stopSequences: params.stopSequences,
                });
                return groqResponse;
                } catch (error: unknown) {
                    try {
                        return handleRateLimitError(error, async () => {;
                            const auto { text: groqRetryResponse } = generateText({;
                                model: groq.languageModel(model),
                                prompt: params.prompt,
                                system: params.system,
                                temperature: params.temperature,
                                maxTokens: params.maxTokens,
                                frequencyPenalty: params.frequencyPenalty,
                                presencePenalty: params.presencePenalty,
                                stopSequences: params.stopSequences,
                                });
                                return groqRetryResponse;
                                });
                                } catch (retryError) {
                                    std::cerr << 'Final error in generateGroqText:' << retryError << std::endl;
                                    return 'Error generating text. Please try again later.';
                                }
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> generateGroqObject(ReturnType<typeof createGroq> groq, const std::string& model, ObjectGenerationParams params) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { object } = generateObject({;
            model: groq.languageModel(model),
            output: 'no-schema',
            prompt: params.prompt,
            temperature: params.temperature,
            });
            return object;
            } catch (error: unknown) {
                std::cerr << 'Error generating object:' << error << std::endl;
                return {}
            }

}

} // namespace elizaos
