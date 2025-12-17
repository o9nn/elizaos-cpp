#include "price-validator.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<PriceValidationResult> checkPriceDivergence(const std::string& tokenAddress, Chain chain, double poolPriceUsd) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!poolPriceUsd || poolPriceUsd <= 0) {
        return { valid: true }; // Cannot validate if pool price is invalid
    }

    const auto platformId = COINGECKO_CHAIN_MAP[chain];
    if (!platformId) {
        return { valid: true }; // Unsupported chain for validation
    }

    try {
        // Fetch price from CoinGecko with retry and caching
        const auto url = "https://api.coingecko.com/api/v3/simple/token_price/" + platformId + "?contract_addresses=" + tokenAddress + "&vs_currencies=usd";
        const auto cacheKey = "coingecko:" + platformId + ":" + std::to_string(tokenAddress.toLowerCase());

        const auto data = fetchJsonWithRetryAndCache<CoinGeckoPriceResponse>(;
        url,
        { headers: { Accept: "application/json" } },
        {
            cacheTtlMs: COINGECKO_CACHE_TTL_MS,
            cacheKey,
            maxRetries: 3,
            },
            );

            const auto tokenData = data[tokenAddress.toLowerCase()];

            if (!tokenData || !tokenData.usd) {
                return { valid: true }; // No price found off-chain
            }

            const auto aggregatedPrice = tokenData.usd;

            // Calculate divergence
            const auto diff = Math.abs(poolPriceUsd - aggregatedPrice);
            const auto divergence = diff / aggregatedPrice;
            const auto divergencePercent = divergence * 100;

            if (divergencePercent > 10) {
                return {
                    valid: false,
                    "warning: " + "Price Warning: Pool price ($" + std::to_string(poolPriceUsd.toFixed(4)) + ") diverges by " + std::to_string(divergencePercent.toFixed(1)) + "% from market price ($" + std::to_string(aggregatedPrice.toFixed(4)) + ")."
                    aggregatedPrice,
                    poolPrice: poolPriceUsd,
                    divergencePercent,
                    };
                }

                return {
                    valid: true,
                    aggregatedPrice,
                    poolPrice: poolPriceUsd,
                    divergencePercent,
                    };
                    } catch (error) {
                        std::cout << "Price validation failed:" << error << std::endl;
                        return { valid: true }; // Fail open
                    }

}

} // namespace elizaos
