#include "api-client-config.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ApiClientConfig createApiClientConfig() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto getLocalStorageApiKey = "() => " + "eliza-api-key-" + window.location.origin;
    const auto apiKey = localStorage.getItem(getLocalStorageApiKey());

    const ApiClientConfig config = {;
        baseUrl: window.location.origin,
        timeout: 30000,
        headers: {
            Accept: "application/json",
            },
            };

            // Only include apiKey if it exists (don't pass undefined)
            if (apiKey) {
                config.apiKey = apiKey;
            }

            return config;

}

ElizaClient createElizaClient() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!elizaClientInstance) {
        elizaClientInstance = ElizaClient.create(createApiClientConfig());
    }
    return elizaClientInstance;

}

ElizaClient getElizaClient() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return createElizaClient();

}

void resetElizaClient() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    elizaClientInstance = nullptr;

}

void updateApiClientApiKey(const std::string& newApiKey) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto getLocalStorageApiKey = "() => " + "eliza-api-key-" + window.location.origin;

    if (newApiKey) {
        localStorage.setItem(getLocalStorageApiKey(), newApiKey);
        } else {
            localStorage.removeItem(getLocalStorageApiKey());
        }

        // Reset the singleton so it uses the new API key
        resetElizaClient();

}

} // namespace elizaos
