#include "plugin-groq.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getBaseURL(const std::any& runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    runtime.getSetting("GROQ_BASE_URL") || "https://api.groq.com/openai/v1"
    );

}

TiktokenModel findModelName(ModelTypeName model) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto name =;
    model == ModelType.TEXT_SMALL;
    ? (process.env.SMALL_GROQ_MODEL || "llama-3.1-8b-instant");
    : (process.env.LARGE_GROQ_MODEL || "llama-3.3-70b-versatile");
    return name;

}

std::future<void> tokenizeText(ModelTypeName model, const std::string& prompt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto encoding = encodingForModel(findModelName(model));
    const auto tokens = encoding.encode(prompt);
    return tokens;

}

std::future<void> detokenizeText(ModelTypeName model, const std::vector<double>& tokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto modelName = findModelName(model);
    const auto encoding = encodingForModel(modelName);
    return encoding.decode(tokens);

}

std::future<void> generateGroqText(ReturnType<typeof createGroq> groq, const std::string& model, std::optional<std::any> params) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { text: groqResponse } = generateText({;
        model: groq.languageModel(model),
        prompt: params.prompt,
        system: params.system,
        temperature: params.temperature,
        maxOutputTokens: params.maxTokens,
        frequencyPenalty: params.frequencyPenalty,
        presencePenalty: params.presencePenalty,
        stopSequences: params.stopSequences,
        });
        return groqResponse;

}

std::future<void> generateGroqObject(ReturnType<typeof createGroq> groq, const std::string& model, ObjectGenerationParams params) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { object } = generateObject({;
        model: groq.languageModel(model),
        output: "no-schema",
        prompt: params.prompt,
        temperature: params.temperature,
        });
        return object;

}

} // namespace elizaos
