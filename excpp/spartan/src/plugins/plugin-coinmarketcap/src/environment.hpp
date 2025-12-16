#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

import type { IAgentRuntime } from "@elizaos/core";
;

const coinmarketcapEnvSchema = z.object({
    COINMARKETCAP_API_KEY: z
        .string()
        .min(1, "CoinMarketCap API key is required"),
});

using CoinMarketCapConfig = z.infer<typeof coinmarketcapEnvSchema>;

std::future<CoinMarketCapConfig> validateCoinMarketCapConfig(IAgentRuntime runtime);;

        return coinmarketcapEnvSchema.parse(config);
    } catch (error) {
        if (error instanceof z.ZodError) {
            const errorMessages = error.errors
                .map((err) => `${err.path.join(".")}: ${err.message}`)
                .join("\n");
            throw new Error(
                `CoinMarketCap configuration validation failed:\n${errorMessages}`
            );
        }
        throw error;
    }
}

} // namespace elizaos
