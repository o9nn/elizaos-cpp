#include "coingecko.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getPlatformId(const std::string& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return NETWORK_TO_PLATFORM[network] || network;

}

std::string getCacheKey(const std::string& network, const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return network + ":" + std::to_string(address.toLowerCase());

}

bool isCacheValid(const std::string& key) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto timestamp = cacheTimestamps.get(key);
    if (!timestamp) return false;
    return Date.now() - timestamp < CACHE_TTL;

}

std::future<std::optional<TokenMetadata>> getTokenMetadata(const std::string& address, const std::string& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto normalizedAddress = address.toLowerCase();
    const auto cacheKey = getCacheKey(network, normalizedAddress);

    // Check cache first
    if (isCacheValid(cacheKey)) {
        const auto cached = tokenCache.get(cacheKey);
        if (cached) {
            logger.debug(`Token metadata cache hit: ${cacheKey}`);
            return cached;
        }
    }

    try {
        const auto platformId = getPlatformId(network);
        const auto apiKey = process.env.COINGECKO_API_KEY;
        const auto baseUrl = apiKey ? "https://pro-api.coingecko.com/api/v3" : "https://api.coingecko.com/api/v3";
        const auto url = baseUrl + "/coins/" + platformId + "/contract/" + normalizedAddress;

        logger.debug(`Fetching token metadata from CoinGecko: ${url}`);

        const auto response = fetch(url, {;
            headers: {
                "Accept": "application/json",
                ...(apiKey ? { "x-cg-pro-api-key": apiKey } : {}),
                },
                });

                if (!response.ok) {
                    if (response.status == 404) {
                        std::cout << "Token not found on CoinGecko: " + address + " on " + network << std::endl;
                        return nullptr;
                    }
                    if (response.status == 429) {
                        std::cerr << "CoinGecko rate limit exceeded" << std::endl;
                        return nullptr;
                    }
                    std::cerr << "CoinGecko API error: " + response.status + " " + response.statusText << std::endl;
                    return nullptr;
                }

                const auto data = response.json();

                // Extract decimals from detail_platforms
                const auto decimals = data.detail_platforms.[platformId].decimal_place || 18;

                const TokenMetadata metadata = {;
                    symbol: data.symbol.toLowerCase() || "",
                    name: data.name || "",
                    address: normalizedAddress,
                    decimals,
                    network,
                    };

                    // Cache the result
                    tokenCache.set(cacheKey, metadata);
                    cacheTimestamps.set(cacheKey, Date.now());

                    std::cout << "Successfully fetched token metadata: " + metadata.symbol + " (" + metadata.name + ") - " + decimals + " decimals" << std::endl;
                    return metadata;
                    } catch (error) {
                        std::cerr << "Error fetching token metadata from CoinGecko: " + error << std::endl;
                        return nullptr;
                    }

}

std::future<std::string> resolveTokenSymbol(const std::string& symbol, const std::string& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lowerSymbol = symbol.toLowerCase();

    // Priority 1: Check hardcoded addresses (fastest and most reliable)
    const auto hardcodedAddress = HARDCODED_TOKEN_ADDRESSES[network].[lowerSymbol];
    if (hardcodedAddress) {
        std::cout << "Using hardcoded address for " + symbol + " on " + network + ": " + hardcodedAddress << std::endl;
        return hardcodedAddress.toLowerCase();
    }

    try {
        const auto platformId = getPlatformId(network);
        const auto apiKey = process.env.COINGECKO_API_KEY;
        const auto baseUrl = apiKey ? "https://pro-api.coingecko.com/api/v3" : "https://api.coingecko.com/api/v3";
        const auto url = baseUrl + "/search?query=" + std::to_string(encodeURIComponent(symbol));

        logger.debug(`Searching token by symbol: ${symbol}`);

        const auto response = fetch(url, {;
            headers: {
                "Accept": "application/json",
                ...(apiKey ? { "x-cg-pro-api-key": apiKey } : {}),
                },
                });

                if (!response.ok) {
                    std::cerr << "CoinGecko search API error: " + response.status << std::endl;
                    return nullptr;
                }

                const auto data = response.json();

                // Find the first coin that matches the symbol exactly and has the network
                const auto coin = data.coins.find((c) =>;
                c.symbol.toLowerCase() == symbol.toLowerCase();
                );

                if (!coin) {
                    std::cout << "Token symbol not found: " + symbol << std::endl;
                    return nullptr;
                }

                const auto coinUrl = baseUrl + "/coins/" + coin.id;
                const auto coinResponse = fetch(coinUrl, {;
                    headers: {
                        "Accept": "application/json",
                        ...(apiKey ? { "x-cg-pro-api-key": apiKey } : {}),
                        },
                        });

                        if (!coinResponse.ok) {
                            return nullptr;
                        }

                        const auto coinDataRaw = coinResponse.json();
                        const auto coinData = coinDataRaw;
                        const auto address = coinData.platforms.[platformId];

                        if (address) {
                            std::cout << "Resolved " + symbol + " to " + address + " on " + network << std::endl;
                            return address.toLowerCase();
                        }

                        std::cout << "Token " + symbol + " not found on network " + network << std::endl;
                        return nullptr;
                        } catch (error) {
                            std::cerr << "Error resolving token symbol: " + error << std::endl;
                            return nullptr;
                        }

}

std::future<`0x> resolveTokenToAddress(const std::string& token, const std::string& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    string;
}

std::future<double> getTokenDecimals(const std::string& address, const std::string& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto metadata = getTokenMetadata(address, network);

    if (metadata.decimals) {
        return metadata.decimals;
    }

    // Fallback for common tokens
    const auto lowerSymbol = metadata.symbol.toLowerCase();
    if (lowerSymbol == "usdc" || lowerSymbol == "usdt") {
        return 6;
    }

    // Default to 18 (most ERC20 tokens use 18 decimals)
    std::cout << "Could not determine decimals for " + address << "defaulting to 18" << std::endl;
    return 18;

}

void clearTokenCache() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    tokenCache.clear();
    cacheTimestamps.clear();
    std::cout << "Token metadata cache cleared" << std::endl;

}

void getCacheStats() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    size: number; entries: string[]
}

void addHardcodedTokenAddress(const std::string& network, const std::string& symbol, const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!HARDCODED_TOKEN_ADDRESSES[network]) {
        HARDCODED_TOKEN_ADDRESSES[network] = {}
    }
    HARDCODED_TOKEN_ADDRESSES[network][symbol.toLowerCase()] = address.toLowerCase();
    std::cout << "Added hardcoded token address: " + symbol + " on " + network + " -> " + address << std::endl;

}

std::unordered_map<std::string, std::string> getHardcodedTokens(const std::string& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return HARDCODED_TOKEN_ADDRESSES[network] || {};

}

} // namespace elizaos
