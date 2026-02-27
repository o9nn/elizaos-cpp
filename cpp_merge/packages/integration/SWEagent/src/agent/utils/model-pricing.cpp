#include "model-pricing.h"

double calculateCost(string modelName, double inputTokens, double outputTokens)
{
    auto pricing = const_(MODEL_PRICING)[modelName];
    if (!pricing) {
        auto baseModel = modelName->replace((new RegExp(std:("-\d{4}"))), string_empty)->replace((new RegExp(std:("-\d{6}"))), string_empty);
        pricing = const_(MODEL_PRICING)[baseModel];
    }
    if (!pricing) {
        for (auto& [key, value] : Object->entries(MODEL_PRICING))
        {
            if (modelName->includes(key->replace((new RegExp(std:("^(azure|bedrock|together|mistral|perplexity|deepseek|ollama)\"))), string_empty))) {
                pricing = value;
                break;
            }
        }
    }
    if (!pricing) {
        console->warn(std:("Model pricing not found for ") + modelName + std:(", using default pricing"));
        pricing = object{
            object::pair{std:("inputCostPer1k"), 0.001}, 
            object::pair{std:("outputCostPer1k"), 0.002}, 
            object::pair{std:("provider"), std:("unknown")}
        };
    }
    auto inputCost = (inputTokens / 1000) * pricing->inputCostPer1k;
    auto outputCost = (outputTokens / 1000) * pricing->outputCostPer1k;
    return inputCost + outputCost;
};


object getModelLimits(string modelName)
{
    auto pricing = const_(MODEL_PRICING)[modelName];
    if (!pricing) {
        auto baseModel = modelName->replace((new RegExp(std:("-\d{4}"))), string_empty)->replace((new RegExp(std:("-\d{6}"))), string_empty);
        auto basePricing = const_(MODEL_PRICING)[baseModel];
        if (basePricing) {
            return object{
                object::pair{std:("maxInputTokens"), basePricing->maxInputTokens}, 
                object::pair{std:("maxOutputTokens"), basePricing->maxOutputTokens}
            };
        }
    }
    return object{
        object::pair{std:("maxInputTokens"), pricing->maxInputTokens}, 
        object::pair{std:("maxOutputTokens"), pricing->maxOutputTokens}
    };
};


boolean isWithinTokenLimit(string modelName, double inputTokens, double outputTokens)
{
    auto limits = getModelLimits(modelName);
    if (AND((limits["maxInputTokens"]), (inputTokens > limits["maxInputTokens"]))) {
        return false;
    }
    if (AND((limits["maxOutputTokens"]), (outputTokens > limits["maxOutputTokens"]))) {
        return false;
    }
    return true;
};


Record<string, std::shared_ptr<ModelPricing>> MODEL_PRICING = object{
    object::pair{std:("gpt-4o"), object{
        object::pair{std:("inputCostPer1k"), 0.005}, 
        object::pair{std:("outputCostPer1k"), 0.015}, 
        object::pair{std:("maxInputTokens"), 128000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("openai")}
    }}, 
    object::pair{std:("gpt-4o-mini"), object{
        object::pair{std:("inputCostPer1k"), 0.00015}, 
        object::pair{std:("outputCostPer1k"), 0.0006}, 
        object::pair{std:("maxInputTokens"), 128000}, 
        object::pair{std:("maxOutputTokens"), 16384}, 
        object::pair{std:("provider"), std:("openai")}
    }}, 
    object::pair{std:("gpt-4-turbo"), object{
        object::pair{std:("inputCostPer1k"), 0.01}, 
        object::pair{std:("outputCostPer1k"), 0.03}, 
        object::pair{std:("maxInputTokens"), 128000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("openai")}
    }}, 
    object::pair{std:("gpt-4"), object{
        object::pair{std:("inputCostPer1k"), 0.03}, 
        object::pair{std:("outputCostPer1k"), 0.06}, 
        object::pair{std:("maxInputTokens"), 8192}, 
        object::pair{std:("maxOutputTokens"), 8192}, 
        object::pair{std:("provider"), std:("openai")}
    }}, 
    object::pair{std:("gpt-3.5-turbo"), object{
        object::pair{std:("inputCostPer1k"), 0.0005}, 
        object::pair{std:("outputCostPer1k"), 0.0015}, 
        object::pair{std:("maxInputTokens"), 16385}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("openai")}
    }}, 
    object::pair{std:("gpt-5"), object{
        object::pair{std:("inputCostPer1k"), 1.25}, 
        object::pair{std:("outputCostPer1k"), 10}, 
        object::pair{std:("maxInputTokens"), 128000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("openai")}
    }}, 
    object::pair{std:("gpt-5-mini"), object{
        object::pair{std:("inputCostPer1k"), 0.25}, 
        object::pair{std:("outputCostPer1k"), 2}, 
        object::pair{std:("maxInputTokens"), 128000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("openai")}
    }}, 
    object::pair{std:("gpt-5-nano"), object{
        object::pair{std:("inputCostPer1k"), 0.05}, 
        object::pair{std:("outputCostPer1k"), 0.4}, 
        object::pair{std:("maxInputTokens"), 128000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("openai")}
    }}, 
    object::pair{std:("claude-4-sonnet"), object{
        object::pair{std:("inputCostPer1k"), 0.003}, 
        object::pair{std:("outputCostPer1k"), 0.015}, 
        object::pair{std:("maxInputTokens"), 200000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("anthropic")}
    }}, 
    object::pair{std:("claude-4-1-opus"), object{
        object::pair{std:("inputCostPer1k"), 0.015}, 
        object::pair{std:("outputCostPer1k"), 0.075}, 
        object::pair{std:("maxInputTokens"), 200000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("anthropic")}
    }}, 
    object::pair{std:("claude-4-opus"), object{
        object::pair{std:("inputCostPer1k"), 0.015}, 
        object::pair{std:("outputCostPer1k"), 0.075}, 
        object::pair{std:("maxInputTokens"), 200000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("anthropic")}
    }}, 
    object::pair{std:("claude-3-5-haiku"), object{
        object::pair{std:("inputCostPer1k"), 0.00025}, 
        object::pair{std:("outputCostPer1k"), 0.00125}, 
        object::pair{std:("maxInputTokens"), 200000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("anthropic")}
    }}, 
    object::pair{std:("gemini-pro"), object{
        object::pair{std:("inputCostPer1k"), 0.00025}, 
        object::pair{std:("outputCostPer1k"), 0.0005}, 
        object::pair{std:("maxInputTokens"), 32760}, 
        object::pair{std:("maxOutputTokens"), 8192}, 
        object::pair{std:("provider"), std:("google")}
    }}, 
    object::pair{std:("gemini-1.5-pro"), object{
        object::pair{std:("inputCostPer1k"), 0.00125}, 
        object::pair{std:("outputCostPer1k"), 0.005}, 
        object::pair{std:("maxInputTokens"), 2097152}, 
        object::pair{std:("maxOutputTokens"), 8192}, 
        object::pair{std:("provider"), std:("google")}
    }}, 
    object::pair{std:("gemini-1.5-flash"), object{
        object::pair{std:("inputCostPer1k"), 0.000075}, 
        object::pair{std:("outputCostPer1k"), 0.0003}, 
        object::pair{std:("maxInputTokens"), 1048576}, 
        object::pair{std:("maxOutputTokens"), 8192}, 
        object::pair{std:("provider"), std:("google")}
    }}, 
    object::pair{std:("command-r"), object{
        object::pair{std:("inputCostPer1k"), 0.0005}, 
        object::pair{std:("outputCostPer1k"), 0.0015}, 
        object::pair{std:("maxInputTokens"), 128000}, 
        object::pair{std:("maxOutputTokens"), 4000}, 
        object::pair{std:("provider"), std:("cohere")}
    }}, 
    object::pair{std:("command-r-plus"), object{
        object::pair{std:("inputCostPer1k"), 0.003}, 
        object::pair{std:("outputCostPer1k"), 0.015}, 
        object::pair{std:("maxInputTokens"), 128000}, 
        object::pair{std:("maxOutputTokens"), 4000}, 
        object::pair{std:("provider"), std:("cohere")}
    }}, 
    object::pair{std:("bedrock/claude-3-opus"), object{
        object::pair{std:("inputCostPer1k"), 0.015}, 
        object::pair{std:("outputCostPer1k"), 0.075}, 
        object::pair{std:("maxInputTokens"), 200000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("bedrock")}
    }}, 
    object::pair{std:("bedrock/claude-3-sonnet"), object{
        object::pair{std:("inputCostPer1k"), 0.003}, 
        object::pair{std:("outputCostPer1k"), 0.015}, 
        object::pair{std:("maxInputTokens"), 200000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("bedrock")}
    }}, 
    object::pair{std:("bedrock/claude-3-haiku"), object{
        object::pair{std:("inputCostPer1k"), 0.00025}, 
        object::pair{std:("outputCostPer1k"), 0.00125}, 
        object::pair{std:("maxInputTokens"), 200000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("bedrock")}
    }}, 
    object::pair{std:("azure/gpt-4o"), object{
        object::pair{std:("inputCostPer1k"), 0.005}, 
        object::pair{std:("outputCostPer1k"), 0.015}, 
        object::pair{std:("maxInputTokens"), 128000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("azure")}
    }}, 
    object::pair{std:("azure/gpt-4o-mini"), object{
        object::pair{std:("inputCostPer1k"), 0.00015}, 
        object::pair{std:("outputCostPer1k"), 0.0006}, 
        object::pair{std:("maxInputTokens"), 128000}, 
        object::pair{std:("maxOutputTokens"), 16384}, 
        object::pair{std:("provider"), std:("azure")}
    }}, 
    object::pair{std:("azure/gpt-4"), object{
        object::pair{std:("inputCostPer1k"), 0.03}, 
        object::pair{std:("outputCostPer1k"), 0.06}, 
        object::pair{std:("maxInputTokens"), 8192}, 
        object::pair{std:("maxOutputTokens"), 8192}, 
        object::pair{std:("provider"), std:("azure")}
    }}, 
    object::pair{std:("azure/gpt-4-turbo"), object{
        object::pair{std:("inputCostPer1k"), 0.01}, 
        object::pair{std:("outputCostPer1k"), 0.03}, 
        object::pair{std:("maxInputTokens"), 128000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("azure")}
    }}, 
    object::pair{std:("azure/gpt-35-turbo"), object{
        object::pair{std:("inputCostPer1k"), 0.0005}, 
        object::pair{std:("outputCostPer1k"), 0.0015}, 
        object::pair{std:("maxInputTokens"), 16385}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("azure")}
    }}, 
    object::pair{std:("together/llama-3-70b"), object{
        object::pair{std:("inputCostPer1k"), 0.0009}, 
        object::pair{std:("outputCostPer1k"), 0.0009}, 
        object::pair{std:("maxInputTokens"), 8192}, 
        object::pair{std:("maxOutputTokens"), 8192}, 
        object::pair{std:("provider"), std:("together")}
    }}, 
    object::pair{std:("together/llama-3-8b"), object{
        object::pair{std:("inputCostPer1k"), 0.0002}, 
        object::pair{std:("outputCostPer1k"), 0.0002}, 
        object::pair{std:("maxInputTokens"), 8192}, 
        object::pair{std:("maxOutputTokens"), 8192}, 
        object::pair{std:("provider"), std:("together")}
    }}, 
    object::pair{std:("together/mixtral-8x7b"), object{
        object::pair{std:("inputCostPer1k"), 0.0006}, 
        object::pair{std:("outputCostPer1k"), 0.0006}, 
        object::pair{std:("maxInputTokens"), 32768}, 
        object::pair{std:("maxOutputTokens"), 32768}, 
        object::pair{std:("provider"), std:("together")}
    }}, 
    object::pair{std:("mistral/mistral-large"), object{
        object::pair{std:("inputCostPer1k"), 0.008}, 
        object::pair{std:("outputCostPer1k"), 0.024}, 
        object::pair{std:("maxInputTokens"), 32000}, 
        object::pair{std:("maxOutputTokens"), 32000}, 
        object::pair{std:("provider"), std:("mistral")}
    }}, 
    object::pair{std:("mistral/mistral-medium"), object{
        object::pair{std:("inputCostPer1k"), 0.0027}, 
        object::pair{std:("outputCostPer1k"), 0.0081}, 
        object::pair{std:("maxInputTokens"), 32000}, 
        object::pair{std:("maxOutputTokens"), 32000}, 
        object::pair{std:("provider"), std:("mistral")}
    }}, 
    object::pair{std:("mistral/mistral-small"), object{
        object::pair{std:("inputCostPer1k"), 0.002}, 
        object::pair{std:("outputCostPer1k"), 0.006}, 
        object::pair{std:("maxInputTokens"), 32000}, 
        object::pair{std:("maxOutputTokens"), 32000}, 
        object::pair{std:("provider"), std:("mistral")}
    }}, 
    object::pair{std:("perplexity/llama-3-sonar-large"), object{
        object::pair{std:("inputCostPer1k"), 0.001}, 
        object::pair{std:("outputCostPer1k"), 0.001}, 
        object::pair{std:("maxInputTokens"), 32768}, 
        object::pair{std:("maxOutputTokens"), 32768}, 
        object::pair{std:("provider"), std:("perplexity")}
    }}, 
    object::pair{std:("perplexity/llama-3-sonar-small"), object{
        object::pair{std:("inputCostPer1k"), 0.0002}, 
        object::pair{std:("outputCostPer1k"), 0.0002}, 
        object::pair{std:("maxInputTokens"), 32768}, 
        object::pair{std:("maxOutputTokens"), 32768}, 
        object::pair{std:("provider"), std:("perplexity")}
    }}, 
    object::pair{std:("deepseek/deepseek-coder"), object{
        object::pair{std:("inputCostPer1k"), 0.00014}, 
        object::pair{std:("outputCostPer1k"), 0.00028}, 
        object::pair{std:("maxInputTokens"), 128000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("deepseek")}
    }}, 
    object::pair{std:("deepseek/deepseek-chat"), object{
        object::pair{std:("inputCostPer1k"), 0.00014}, 
        object::pair{std:("outputCostPer1k"), 0.00028}, 
        object::pair{std:("maxInputTokens"), 128000}, 
        object::pair{std:("maxOutputTokens"), 4096}, 
        object::pair{std:("provider"), std:("deepseek")}
    }}, 
    object::pair{std:("ollama/llama3"), object{
        object::pair{std:("inputCostPer1k"), 0}, 
        object::pair{std:("outputCostPer1k"), 0}, 
        object::pair{std:("maxInputTokens"), 8192}, 
        object::pair{std:("maxOutputTokens"), 8192}, 
        object::pair{std:("provider"), std:("ollama")}
    }}, 
    object::pair{std:("ollama/codellama"), object{
        object::pair{std:("inputCostPer1k"), 0}, 
        object::pair{std:("outputCostPer1k"), 0}, 
        object::pair{std:("maxInputTokens"), 16384}, 
        object::pair{std:("maxOutputTokens"), 16384}, 
        object::pair{std:("provider"), std:("ollama")}
    }}, 
    object::pair{std:("ollama/mistral"), object{
        object::pair{std:("inputCostPer1k"), 0}, 
        object::pair{std:("outputCostPer1k"), 0}, 
        object::pair{std:("maxInputTokens"), 32768}, 
        object::pair{std:("maxOutputTokens"), 32768}, 
        object::pair{std:("provider"), std:("ollama")}
    }}
};

void Main(void)
{
}

MAIN
