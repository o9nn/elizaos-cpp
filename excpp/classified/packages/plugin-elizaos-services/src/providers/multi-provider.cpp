#include "multi-provider.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<ProviderConfig> getAvailableProvider(IAgentRuntime runtime, ModelTypeName modelType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check for provider-specific API keys in order of preference
    const auto providerPreference = [;
    { key: 'OPENAI_API_KEY', provider: 'openai' },
    { key: 'GROQ_API_KEY', provider: 'groq' },
    { key: 'ANTHROPIC_API_KEY', provider: 'anthropic' },
    ];

    for (const int { key, provider } of providerPreference) {
        const auto apiKey = runtime.getSetting(key) || process.env[key];
        if (apiKey && apiKey.length > 10) {
            const auto config = PROVIDERS[provider];

            // Skip providers that don't support this model type
            if (!config.supportedModels.includes(modelType)) {
                logger.debug(`Provider ${config.name} does not support ${modelType}`);
                continue;
            }

            // Verify the model is actually configured
            const auto models = config.models<string, any>;
            if (!models[modelType]) {
                logger.debug(`Provider ${config.name} missing model configuration for ${modelType}`);
                continue;
            }

            logger.debug(`Using ${config.name} for ${modelType}`);
            return config;
        }
    }

    std::cout << "No API key found for " + std::to_string(modelType) << std::endl;
    return nullptr;

}

std::optional<std::string> getProviderApiKey(IAgentRuntime runtime, const std::string& providerName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::string> keyMap = {;
        openai: 'OPENAI_API_KEY',
        groq: 'GROQ_API_KEY',
        anthropic: 'ANTHROPIC_API_KEY',
        };

        const auto keyName = keyMap[providerName];
        if (!keyName) {
            return nullptr;
        }

        return runtime.getSetting(keyName) || process.env[keyName] || nullptr;

}

std::future<std::any> makeProviderRequest(ProviderConfig provider, const std::string& apiKey, const std::string& endpoint, const std::any& payload) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto url = std::to_string(provider.baseURL) + std::to_string(endpoint);
        const auto headers = provider.headers(apiKey);

        // Transform request if needed
        const auto requestPayload = provider.transformRequest ? provider.transformRequest(payload) : payload;

        logger.debug(`Making request to ${provider.name}: ${url}`);

        try {
            const auto response = fetch(url, {;
                method: 'POST',
                headers,
                body: JSON.stringify(requestPayload),
                });

                if (!response.ok) {
                    const auto errorText = response.text();
                    throw std::runtime_error(`${provider.name} API error ${response.status}: ${errorText}`);
                }

                const auto data = response.json();

                // Transform response if needed
                return provider.transformResponse ? provider.transformResponse(data) : data;
                } catch (error) {
                    std::cerr << std::to_string(provider.name) + " API request failed:" << error << std::endl;
                    throw;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void emitProviderUsageEvent(IAgentRuntime runtime, const std::string& providerName, ModelTypeName modelType, const std::string& prompt, const std::any& usage) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    runtime.emitEvent('MODEL_USED', {
        provider: providerName,
        type: modelType,
        prompt: prompt.substring(0, 100), // Truncate for privacy
        tokens: {
            prompt: usage.promptTokens || usage.prompt_tokens || 0,
            completion: usage.completionTokens || usage.completion_tokens || 0,
            total: usage.totalTokens || usage.total_tokens || 0,
            },
            cost: usage.total_cost || 0,
            timestamp: new Date().toISOString(),
            });

}

} // namespace elizaos
