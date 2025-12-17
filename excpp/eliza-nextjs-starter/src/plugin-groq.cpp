#include "plugin-groq.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getBaseURL(const std::any& runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto defaultBaseURL =;
    runtime.getSetting("GROQ_BASE_URL") || "https://api.groq.com/openai/v1";
    // Simple implementation without getProviderBaseURL
    return runtime.getSetting("GROQ_BASE_URL") || defaultBaseURL;

}

TiktokenModel findModelName(ModelTypeName model) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto name =;
        model == ModelType.TEXT_SMALL;
        ? (process.env.SMALL_GROQ_MODEL || "llama-3.1-8b-instant");
        : (process.env.LARGE_GROQ_MODEL || "llama-3.2-90b-vision-preview");
        return name;
        } catch (error) {
            std::cerr << "Error in findModelName:" << error << std::endl;
            return "llama-3.1-8b-instant";
        }

}

std::future<void> tokenizeText(ModelTypeName model, const std::string& prompt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto encoding = encodingForModel(findModelName(model));
        const auto tokens = encoding.encode(prompt);
        return tokens;
        } catch (error) {
            std::cerr << "Error in tokenizeText:" << std::to_string(error) << std::endl;
            return [];
        }

}

std::future<void> detokenizeText(ModelTypeName model, const std::vector<double>& tokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto modelName = findModelName(model);
        const auto encoding = encodingForModel(modelName);
        return encoding.decode(tokens);
        } catch (error) {
            std::cerr << "Error in detokenizeText:" << std::to_string(error) << std::endl;
            return "";
        }

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
                                    std::cerr << "Final error in generateGroqText:" << std::to_string(retryError) << std::endl;
                                    return "Error generating text. Please try again later.";
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
            output: "no-schema",
            prompt: params.prompt,
            temperature: params.temperature,
            });
            return object;
            } catch (error: unknown) {
                std::cerr << "Error generating object:" << std::to_string(error) << std::endl;
                return {}
            }

}

} // namespace elizaos
