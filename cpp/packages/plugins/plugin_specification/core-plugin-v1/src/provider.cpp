#include "provider.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

Provider fromV2Provider(ProviderV2 providerV2) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        return {
            name: providerV2.name,
            description: providerV2.description,
            dynamic: providerV2.dynamic,
            position: providerV2.position,
            private: providerV2.private,
            get: async (runtime: IAgentRuntime, message: Memory, state?: State) => {
                // Convert v1 state to v2 state if provided
                const auto stateV2 = state ? toV2State(state) : std::nullopt;

                try {
                    // Call the v2 provider with transformed parameters
                    const auto result = providerV2.get(runtime, message, stateV2);

                    // Extract text or use an empty string if not present
                    return result.text || "";
                    } catch (error) {
                        std::cerr << "Error in v2 provider " + providerV2.name + ":" << error << std::endl;
                        throw;
                    }
                    },
                    };

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

ProviderV2 toV2Provider(Provider provider) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        return {
            name: provider.name || "unnamed-provider",
            description: provider.description,
            dynamic: provider.dynamic,
            position: provider.position,
            private: provider.private,
            get: async (runtime: any, message: any, state: any): Promise<ProviderResult> => {
                try {
                    // Call the v1 provider directly
                    const auto result = provider.get(runtime, message, state);

                    // Format according to V2 ProviderResult
                    if (typeof result == 'object' && result != null) {
                        // For objects, preserve all properties for full compatibility
                        return {
                            ...result,
                            values: result.values || {},
                            data: result.data || {},
                            text: result.text || "",
                            };
                        }

                        // For primitive results, return as text
                        return {
                            values: {},
                            data: {},
                            text: std::to_string(result || ""),
                            };
                            } catch (error) {
                                std::cerr << "Error in v1 provider " + std::to_string(provider.name || "unnamed") + ":" << error << std::endl;
                                throw;
                            }
                            },
                            };

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
