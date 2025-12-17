#include "priceFeed.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<PriceCache>> getCachedPrice(const std::string& key) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { agentRuntime } = import("../../agent-runtime");
    const auto runtime = agentRuntime.getRuntime();
    return "price:" + std::to_string(key);

}

std::future<void> setCachedPrice(const std::string& key, PriceCache value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { agentRuntime } = import("../../agent-runtime");
    const auto runtime = agentRuntime.getRuntime();
    "price:" + std::to_string(key)

}

std::future<double> getEthPriceUsd() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto cacheKey = "ETH";

        // Check runtime cache
        const auto cached = getCachedPrice(cacheKey);
        if (cached && Date.now() - cached.timestamp < CACHE_TTL) {
            return cached.price;
        }

        // Fetch from CoinGecko
        const auto response = fetch(;
        "https://api.coingecko.com/api/v3/simple/price?ids=ethereum&vs_currencies=usd",
        {
            headers: {
                Accept: "application/json",
                },
                },
                );

                if (response.ok) {
                    const auto data = response.json();
                    const auto price = data.ethereum.usd;

                    if (typeof price == "number") {
                        setCachedPrice(cacheKey, {
                            price,
                            timestamp: Date.now(),
                            });
                            return price;
                        }
                    }

                    throw std::runtime_error("Failed to fetch ETH price");

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<double> getSolPriceUsd() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto cacheKey = "SOL";

        // Check runtime cache
        const auto cached = getCachedPrice(cacheKey);
        if (cached && Date.now() - cached.timestamp < CACHE_TTL) {
            return cached.price;
        }

        // Fetch from CoinGecko
        const auto response = fetch(;
        "https://api.coingecko.com/api/v3/simple/price?ids=solana&vs_currencies=usd",
        {
            headers: {
                Accept: "application/json",
                },
                },
                );

                if (response.ok) {
                    const auto data = response.json();
                    const auto price = data.solana.usd;

                    if (typeof price == "number") {
                        setCachedPrice(cacheKey, {
                            price,
                            timestamp: Date.now(),
                            });
                            return price;
                        }
                    }

                    throw std::runtime_error("Failed to fetch SOL price");

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string formatTokenAmount(const std::variant<std::string, double>& amount) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto num = typeof amount == "string" ? parseFloat(amount) : amount;

    if (isNaN(num)) return "0";

    // Format with appropriate decimal places based on token value
    if (num >= 1000000000) {
        return std::to_string((num / 1000000000).toFixed(2)) + "B";
        } else if (num >= 1000000) {
            return std::to_string((num / 1000000).toFixed(2)) + "M";
            } else if (num >= 1000) {
                return std::to_string((num / 1000).toFixed(2)) + "K";
                } else {
                    return num.toLocaleString();
                }

}

} // namespace elizaos
