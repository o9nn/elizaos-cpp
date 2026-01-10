#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/agent/utils/model-pricing.h"

double calculateCost(string modelName, double inputTokens, double outputTokens)
{
    auto pricing = const_(MODEL_PRICING)[modelName];
    if (!pricing) {
        auto baseModel = modelName->replace((new RegExp(std::string("-\d{4}"))), string_empty)->replace((new RegExp(std::string("-\d{6}"))), string_empty);
        pricing = const_(MODEL_PRICING)[baseModel];
    }
    if (!pricing) {
        for (auto& [key, value] : Object->entries(MODEL_PRICING))
        {
            if (modelName->includes(key->replace((new RegExp(std::string("^(azure|bedrock|together|mistral|perplexity|deepseek|ollama)\"))), string_empty))) {
                pricing = value;
                break;
            }
        }
    }
    if (!pricing) {
        console->warn(std::string("Model pricing not found for ") + modelName + std::string(", using default pricing"));
        pricing = object{
            object::pair{std::string("inputCostPer1k"), 0.001}, 
            object::pair{std::string("outputCostPer1k"), 0.002}, 
            object::pair{std::string("provider"), std::string("unknown")}
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
        auto baseModel = modelName->replace((new RegExp(std::string("-\d{4}"))), string_empty)->replace((new RegExp(std::string("-\d{6}"))), string_empty);
        auto basePricing = const_(MODEL_PRICING)[baseModel];
        if (basePricing) {
            return object{
                object::pair{std::string("maxInputTokens"), basePricing->maxInputTokens}, 
                object::pair{std::string("maxOutputTokens"), basePricing->maxOutputTokens}
            };
        }
    }
    return object{
        object::pair{std::string("maxInputTokens"), pricing->maxInputTokens}, 
        object::pair{std::string("maxOutputTokens"), pricing->maxOutputTokens}
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
    object::pair{std::string("gpt-4o"), object{
        object::pair{std::string("inputCostPer1k"), 0.005}, 
        object::pair{std::string("outputCostPer1k"), 0.015}, 
        object::pair{std::string("maxInputTokens"), 128000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("openai")}
    }}, 
    object::pair{std::string("gpt-4o-mini"), object{
        object::pair{std::string("inputCostPer1k"), 0.00015}, 
        object::pair{std::string("outputCostPer1k"), 0.0006}, 
        object::pair{std::string("maxInputTokens"), 128000}, 
        object::pair{std::string("maxOutputTokens"), 16384}, 
        object::pair{std::string("provider"), std::string("openai")}
    }}, 
    object::pair{std::string("gpt-4-turbo"), object{
        object::pair{std::string("inputCostPer1k"), 0.01}, 
        object::pair{std::string("outputCostPer1k"), 0.03}, 
        object::pair{std::string("maxInputTokens"), 128000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("openai")}
    }}, 
    object::pair{std::string("gpt-4"), object{
        object::pair{std::string("inputCostPer1k"), 0.03}, 
        object::pair{std::string("outputCostPer1k"), 0.06}, 
        object::pair{std::string("maxInputTokens"), 8192}, 
        object::pair{std::string("maxOutputTokens"), 8192}, 
        object::pair{std::string("provider"), std::string("openai")}
    }}, 
    object::pair{std::string("gpt-3.5-turbo"), object{
        object::pair{std::string("inputCostPer1k"), 0.0005}, 
        object::pair{std::string("outputCostPer1k"), 0.0015}, 
        object::pair{std::string("maxInputTokens"), 16385}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("openai")}
    }}, 
    object::pair{std::string("gpt-5"), object{
        object::pair{std::string("inputCostPer1k"), 1.25}, 
        object::pair{std::string("outputCostPer1k"), 10}, 
        object::pair{std::string("maxInputTokens"), 128000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("openai")}
    }}, 
    object::pair{std::string("gpt-5-mini"), object{
        object::pair{std::string("inputCostPer1k"), 0.25}, 
        object::pair{std::string("outputCostPer1k"), 2}, 
        object::pair{std::string("maxInputTokens"), 128000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("openai")}
    }}, 
    object::pair{std::string("gpt-5-nano"), object{
        object::pair{std::string("inputCostPer1k"), 0.05}, 
        object::pair{std::string("outputCostPer1k"), 0.4}, 
        object::pair{std::string("maxInputTokens"), 128000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("openai")}
    }}, 
    object::pair{std::string("claude-4-sonnet"), object{
        object::pair{std::string("inputCostPer1k"), 0.003}, 
        object::pair{std::string("outputCostPer1k"), 0.015}, 
        object::pair{std::string("maxInputTokens"), 200000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("anthropic")}
    }}, 
    object::pair{std::string("claude-4-1-opus"), object{
        object::pair{std::string("inputCostPer1k"), 0.015}, 
        object::pair{std::string("outputCostPer1k"), 0.075}, 
        object::pair{std::string("maxInputTokens"), 200000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("anthropic")}
    }}, 
    object::pair{std::string("claude-4-opus"), object{
        object::pair{std::string("inputCostPer1k"), 0.015}, 
        object::pair{std::string("outputCostPer1k"), 0.075}, 
        object::pair{std::string("maxInputTokens"), 200000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("anthropic")}
    }}, 
    object::pair{std::string("claude-3-5-haiku"), object{
        object::pair{std::string("inputCostPer1k"), 0.00025}, 
        object::pair{std::string("outputCostPer1k"), 0.00125}, 
        object::pair{std::string("maxInputTokens"), 200000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("anthropic")}
    }}, 
    object::pair{std::string("gemini-pro"), object{
        object::pair{std::string("inputCostPer1k"), 0.00025}, 
        object::pair{std::string("outputCostPer1k"), 0.0005}, 
        object::pair{std::string("maxInputTokens"), 32760}, 
        object::pair{std::string("maxOutputTokens"), 8192}, 
        object::pair{std::string("provider"), std::string("google")}
    }}, 
    object::pair{std::string("gemini-1.5-pro"), object{
        object::pair{std::string("inputCostPer1k"), 0.00125}, 
        object::pair{std::string("outputCostPer1k"), 0.005}, 
        object::pair{std::string("maxInputTokens"), 2097152}, 
        object::pair{std::string("maxOutputTokens"), 8192}, 
        object::pair{std::string("provider"), std::string("google")}
    }}, 
    object::pair{std::string("gemini-1.5-flash"), object{
        object::pair{std::string("inputCostPer1k"), 0.000075}, 
        object::pair{std::string("outputCostPer1k"), 0.0003}, 
        object::pair{std::string("maxInputTokens"), 1048576}, 
        object::pair{std::string("maxOutputTokens"), 8192}, 
        object::pair{std::string("provider"), std::string("google")}
    }}, 
    object::pair{std::string("command-r"), object{
        object::pair{std::string("inputCostPer1k"), 0.0005}, 
        object::pair{std::string("outputCostPer1k"), 0.0015}, 
        object::pair{std::string("maxInputTokens"), 128000}, 
        object::pair{std::string("maxOutputTokens"), 4000}, 
        object::pair{std::string("provider"), std::string("cohere")}
    }}, 
    object::pair{std::string("command-r-plus"), object{
        object::pair{std::string("inputCostPer1k"), 0.003}, 
        object::pair{std::string("outputCostPer1k"), 0.015}, 
        object::pair{std::string("maxInputTokens"), 128000}, 
        object::pair{std::string("maxOutputTokens"), 4000}, 
        object::pair{std::string("provider"), std::string("cohere")}
    }}, 
    object::pair{std::string("bedrock/claude-3-opus"), object{
        object::pair{std::string("inputCostPer1k"), 0.015}, 
        object::pair{std::string("outputCostPer1k"), 0.075}, 
        object::pair{std::string("maxInputTokens"), 200000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("bedrock")}
    }}, 
    object::pair{std::string("bedrock/claude-3-sonnet"), object{
        object::pair{std::string("inputCostPer1k"), 0.003}, 
        object::pair{std::string("outputCostPer1k"), 0.015}, 
        object::pair{std::string("maxInputTokens"), 200000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("bedrock")}
    }}, 
    object::pair{std::string("bedrock/claude-3-haiku"), object{
        object::pair{std::string("inputCostPer1k"), 0.00025}, 
        object::pair{std::string("outputCostPer1k"), 0.00125}, 
        object::pair{std::string("maxInputTokens"), 200000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("bedrock")}
    }}, 
    object::pair{std::string("azure/gpt-4o"), object{
        object::pair{std::string("inputCostPer1k"), 0.005}, 
        object::pair{std::string("outputCostPer1k"), 0.015}, 
        object::pair{std::string("maxInputTokens"), 128000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("azure")}
    }}, 
    object::pair{std::string("azure/gpt-4o-mini"), object{
        object::pair{std::string("inputCostPer1k"), 0.00015}, 
        object::pair{std::string("outputCostPer1k"), 0.0006}, 
        object::pair{std::string("maxInputTokens"), 128000}, 
        object::pair{std::string("maxOutputTokens"), 16384}, 
        object::pair{std::string("provider"), std::string("azure")}
    }}, 
    object::pair{std::string("azure/gpt-4"), object{
        object::pair{std::string("inputCostPer1k"), 0.03}, 
        object::pair{std::string("outputCostPer1k"), 0.06}, 
        object::pair{std::string("maxInputTokens"), 8192}, 
        object::pair{std::string("maxOutputTokens"), 8192}, 
        object::pair{std::string("provider"), std::string("azure")}
    }}, 
    object::pair{std::string("azure/gpt-4-turbo"), object{
        object::pair{std::string("inputCostPer1k"), 0.01}, 
        object::pair{std::string("outputCostPer1k"), 0.03}, 
        object::pair{std::string("maxInputTokens"), 128000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("azure")}
    }}, 
    object::pair{std::string("azure/gpt-35-turbo"), object{
        object::pair{std::string("inputCostPer1k"), 0.0005}, 
        object::pair{std::string("outputCostPer1k"), 0.0015}, 
        object::pair{std::string("maxInputTokens"), 16385}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("azure")}
    }}, 
    object::pair{std::string("together/llama-3-70b"), object{
        object::pair{std::string("inputCostPer1k"), 0.0009}, 
        object::pair{std::string("outputCostPer1k"), 0.0009}, 
        object::pair{std::string("maxInputTokens"), 8192}, 
        object::pair{std::string("maxOutputTokens"), 8192}, 
        object::pair{std::string("provider"), std::string("together")}
    }}, 
    object::pair{std::string("together/llama-3-8b"), object{
        object::pair{std::string("inputCostPer1k"), 0.0002}, 
        object::pair{std::string("outputCostPer1k"), 0.0002}, 
        object::pair{std::string("maxInputTokens"), 8192}, 
        object::pair{std::string("maxOutputTokens"), 8192}, 
        object::pair{std::string("provider"), std::string("together")}
    }}, 
    object::pair{std::string("together/mixtral-8x7b"), object{
        object::pair{std::string("inputCostPer1k"), 0.0006}, 
        object::pair{std::string("outputCostPer1k"), 0.0006}, 
        object::pair{std::string("maxInputTokens"), 32768}, 
        object::pair{std::string("maxOutputTokens"), 32768}, 
        object::pair{std::string("provider"), std::string("together")}
    }}, 
    object::pair{std::string("mistral/mistral-large"), object{
        object::pair{std::string("inputCostPer1k"), 0.008}, 
        object::pair{std::string("outputCostPer1k"), 0.024}, 
        object::pair{std::string("maxInputTokens"), 32000}, 
        object::pair{std::string("maxOutputTokens"), 32000}, 
        object::pair{std::string("provider"), std::string("mistral")}
    }}, 
    object::pair{std::string("mistral/mistral-medium"), object{
        object::pair{std::string("inputCostPer1k"), 0.0027}, 
        object::pair{std::string("outputCostPer1k"), 0.0081}, 
        object::pair{std::string("maxInputTokens"), 32000}, 
        object::pair{std::string("maxOutputTokens"), 32000}, 
        object::pair{std::string("provider"), std::string("mistral")}
    }}, 
    object::pair{std::string("mistral/mistral-small"), object{
        object::pair{std::string("inputCostPer1k"), 0.002}, 
        object::pair{std::string("outputCostPer1k"), 0.006}, 
        object::pair{std::string("maxInputTokens"), 32000}, 
        object::pair{std::string("maxOutputTokens"), 32000}, 
        object::pair{std::string("provider"), std::string("mistral")}
    }}, 
    object::pair{std::string("perplexity/llama-3-sonar-large"), object{
        object::pair{std::string("inputCostPer1k"), 0.001}, 
        object::pair{std::string("outputCostPer1k"), 0.001}, 
        object::pair{std::string("maxInputTokens"), 32768}, 
        object::pair{std::string("maxOutputTokens"), 32768}, 
        object::pair{std::string("provider"), std::string("perplexity")}
    }}, 
    object::pair{std::string("perplexity/llama-3-sonar-small"), object{
        object::pair{std::string("inputCostPer1k"), 0.0002}, 
        object::pair{std::string("outputCostPer1k"), 0.0002}, 
        object::pair{std::string("maxInputTokens"), 32768}, 
        object::pair{std::string("maxOutputTokens"), 32768}, 
        object::pair{std::string("provider"), std::string("perplexity")}
    }}, 
    object::pair{std::string("deepseek/deepseek-coder"), object{
        object::pair{std::string("inputCostPer1k"), 0.00014}, 
        object::pair{std::string("outputCostPer1k"), 0.00028}, 
        object::pair{std::string("maxInputTokens"), 128000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("deepseek")}
    }}, 
    object::pair{std::string("deepseek/deepseek-chat"), object{
        object::pair{std::string("inputCostPer1k"), 0.00014}, 
        object::pair{std::string("outputCostPer1k"), 0.00028}, 
        object::pair{std::string("maxInputTokens"), 128000}, 
        object::pair{std::string("maxOutputTokens"), 4096}, 
        object::pair{std::string("provider"), std::string("deepseek")}
    }}, 
    object::pair{std::string("ollama/llama3"), object{
        object::pair{std::string("inputCostPer1k"), 0}, 
        object::pair{std::string("outputCostPer1k"), 0}, 
        object::pair{std::string("maxInputTokens"), 8192}, 
        object::pair{std::string("maxOutputTokens"), 8192}, 
        object::pair{std::string("provider"), std::string("ollama")}
    }}, 
    object::pair{std::string("ollama/codellama"), object{
        object::pair{std::string("inputCostPer1k"), 0}, 
        object::pair{std::string("outputCostPer1k"), 0}, 
        object::pair{std::string("maxInputTokens"), 16384}, 
        object::pair{std::string("maxOutputTokens"), 16384}, 
        object::pair{std::string("provider"), std::string("ollama")}
    }}, 
    object::pair{std::string("ollama/mistral"), object{
        object::pair{std::string("inputCostPer1k"), 0}, 
        object::pair{std::string("outputCostPer1k"), 0}, 
        object::pair{std::string("maxInputTokens"), 32768}, 
        object::pair{std::string("maxOutputTokens"), 32768}, 
        object::pair{std::string("provider"), std::string("ollama")}
    }}
};

void Main(void)
{
}

MAIN
