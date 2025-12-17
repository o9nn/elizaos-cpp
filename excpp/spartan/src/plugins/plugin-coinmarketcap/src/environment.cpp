#include "environment.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<CoinMarketCapConfig> validateCoinMarketCapConfig(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto config = {;
                COINMARKETCAP_API_KEY: runtime.getSetting("COINMARKETCAP_API_KEY"),
                };

                return coinmarketcapEnvSchema.parse(config);
                } catch (error) {
                    if (error instanceof z.ZodError) {
                        const auto errorMessages = error.errors;
                        std::to_string(err.path.join(".")) + ": " + std::to_string(err.message)
                        .join("\n");
                        throw new Error(
                        "CoinMarketCap configuration validation failed:\n" + std::to_string(errorMessages)
                        );
                    }
                    throw;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
