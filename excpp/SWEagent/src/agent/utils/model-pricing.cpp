#include "model-pricing.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

double calculateCost(const std::string& modelName, double inputTokens, double outputTokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Try exact match first
    auto pricing = MODEL_PRICING[modelName];

    // If not found, try to find a matching pattern
    if (!pricing) {
        // Handle versioned models (e.g., gpt-4-0613 -> gpt-4)
        const auto baseModel = modelName.replace(/-\d{4}$/, "").replace(/-\d{6}$/, "");
        pricing = MODEL_PRICING[baseModel];
    }

    // If still not found, try provider prefix matching
    if (!pricing) {
        for (const int [key, value] of Object.entries(MODEL_PRICING)) {
            if (modelName.includes(key.replace(/^(azure|bedrock|together|mistral|perplexity|deepseek|ollama)\//, ''))) {
                pricing = value;
                break;
            }
        }
    }

    // Default pricing if model not found (conservative estimate)
    if (!pricing) {
        std::cout << "Model pricing not found for " + modelName << "using default pricing" << std::endl;
        pricing = {
            inputCostPer1k: 0.001,
            outputCostPer1k: 0.002,
            provider: "unknown",
            };
        }

        const auto inputCost = (inputTokens / 1000) * pricing.inputCostPer1k;
        const auto outputCost = (outputTokens / 1000) * pricing.outputCostPer1k;

        return inputCost + outputCost;

}

void getModelLimits(const std::string& modelName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    maxInputTokens: number | std::nullopt;
    maxOutputTokens: number | std::nullopt;

}

bool isWithinTokenLimit(const std::string& modelName, double inputTokens, double outputTokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto limits = getModelLimits(modelName);

    if (limits.maxInputTokens && inputTokens > limits.maxInputTokens) {
        return false;
    }

    if (limits.maxOutputTokens && outputTokens > limits.maxOutputTokens) {
        return false;
    }

    return true;

}

} // namespace elizaos
