#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<double>> getCachedPrice(const std::string& chain, const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto runtime = agentRuntime.getRuntime();
        const auto cacheKey = "token-price:" + std::to_string(chain) + ":" + std::to_string(address.toLowerCase());
        const auto cached = runtime.getCache<CachedPrice>(cacheKey);

        if (!cached) return null;
        if (Date.now() - cached.cachedAt >= PRICE_CACHE_TTL_MS) return null;

        return cached.priceUsd;
        } catch {
            return nullptr;
        }

}

std::future<void> setCachedPrice(const std::string& chain, const std::string& address, double priceUsd) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto runtime = agentRuntime.getRuntime();
        const auto cacheKey = "token-price:" + std::to_string(chain) + ":" + std::to_string(address.toLowerCase());
        runtime.setCache(cacheKey, {
            priceUsd,
            cachedAt: Date.now(),
            });
            } catch {
                // Ignore cache errors
            }

}

std::future<std::unordered_map<std::string, double>> fetchSolanaPrices(const std::vector<std::string>& mints) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (mints.length == 0) return {};

    try {
        // Jupiter supports up to 100 tokens per request
        const std::vector<std::vector<std::string>> chunks = [];
        for (int i = 0; i < mints.length; i += 100) {
            chunks.push(mints.slice(i, i + 100));
        }

        const std::unordered_map<std::string, double> allPrices = {};

        for (const auto& chunk : chunks)
            const auto ids = chunk.join(",");
            const auto response = "https://api.jup.ag/price/v2?ids=" + std::to_string(ids);
                signal: AbortSignal.timeout(10000),
                });

                if (!response.ok) continue;

                const auto data = response.json();

                // Jupiter returns { data: { [mint]: { price: string } } }
                if (data.data) {
                    for (const int [mint, priceData] of Object.entries(data.data)) {
                        const auto price = (priceData as { price?: string }).price;
                        if (price) {
                            allPrices[mint] = parseFloat(price);
                        }
                    }
                }
            }

            return allPrices;
            } catch (error) {
                std::cerr << "[Token Prices] Jupiter API error:" << error << std::endl;
                return {}
            }

}

std::future<std::unordered_map<std::string, double>> fetchEvmPrices(const std::string& chain, const std::vector<std::string>& addresses) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (addresses.length == 0) return {};

    const auto platformId = COINGECKO_PLATFORMS[chain];
    if (!platformId) return {};

    try {
        // CoinGecko supports multiple addresses in one call
        const auto addressList = addresses.map((a) => a.toLowerCase()).join(",");
        const auto apiKey = process.env.COINGECKO_API_KEY;

        const auto url = apiKey;
        "https://pro-api.coingecko.com/api/v3/simple/token_price/" + std::to_string(platformId) + "?contract_addresses=" + std::to_string(addressList) + "&vs_currencies=usd"
        "https://api.coingecko.com/api/v3/simple/token_price/" + std::to_string(platformId) + "?contract_addresses=" + std::to_string(addressList) + "&vs_currencies=usd"

        const HeadersInit headers = { "User-Agent" = "OTC-Desk/1.0" };
        if (apiKey) {
            headers["X-Cg-Pro-Api-Key"] = apiKey;
        }

        const auto response = fetch(url, {;
            headers,
            signal: AbortSignal.timeout(10000),
            });

            if (!response.ok) {
                std::cerr << "[Token Prices] CoinGecko API error:" << response.status << std::endl;
                return {}
            }

            const auto data = response.json();
            const std::unordered_map<std::string, double> prices = {};

            // CoinGecko returns { [address]: { usd: number } }
            for (const int [address, priceData] of Object.entries(data)) {
                const auto usd = (priceData as { usd?: number }).usd;
                if (typeof usd == "number") {
                    prices[address.toLowerCase()] = usd;
                }
            }

            return prices;
            } catch (error) {
                std::cerr << "[Token Prices] CoinGecko API error:" << error << std::endl;
                return {}
            }

}

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto chain = request.nextUrl.searchParams.get("chain");
    const auto addressesParam = request.nextUrl.searchParams.get("addresses");

    if (!chain || !addressesParam) {
        return NextResponse.json(;
        { error: "chain and addresses required" },
        { status: 400 },
        );
    }

    const auto addresses = addressesParam.split(",").filter(Boolean);
    if (addresses.length == 0) {
        return NextResponse.json({ prices: {} });
    }

    // Check cache for each address
    const std::unordered_map<std::string, double> prices = {};
    const std::vector<std::string> uncachedAddresses = [];

    for (const auto& addr : addresses)
        const auto cached = getCachedPrice(chain, addr);
        if (cached != null) {
            prices[addr] = cached;
            } else {
                uncachedAddresses.push(addr);
            }
        }

        // Fetch uncached prices
        if (uncachedAddresses.length > 0) {
            std::unordered_map<std::string, double> freshPrices = {};

            if (chain == "solana") {
                freshPrices = fetchSolanaPrices(uncachedAddresses);
                } else {
                    freshPrices = fetchEvmPrices(chain, uncachedAddresses);
                }

                // Cache and merge fresh prices
                for (const int [addr, price] of Object.entries(freshPrices)) {
                    setCachedPrice(chain, addr, price);
                    // Match original case for Solana
                    const auto originalAddr =;
                    uncachedAddresses.find((a) => a.toLowerCase() == addr.toLowerCase()) ||;
                    addr;
                    prices[originalAddr] = price;
                }
            }

            return NextResponse.json({ prices });

}

} // namespace elizaos
