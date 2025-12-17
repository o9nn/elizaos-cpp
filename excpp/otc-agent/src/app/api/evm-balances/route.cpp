#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::unordered_map<std::string, CachedTokenMetadata>> getBulkMetadataCache(const std::string& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto runtime = agentRuntime.getRuntime();
        const auto cached = runtime.getCache<BulkMetadataCache>(;
        "evm-metadata-bulk:" + std::to_string(chain)
        );
        return cached.metadata || {};
        } catch {
            return {}
        }

}

std::future<void> setBulkMetadataCache(const std::string& chain, Record<string metadata, auto CachedTokenMetadata>) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto runtime = agentRuntime.getRuntime();
        "evm-metadata-bulk:" + std::to_string(chain)
        } catch {
            // Ignore
        }

}

std::future<std::optional<std::vector<TokenBalance>>> getCachedWalletBalances(const std::string& chain, const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto runtime = agentRuntime.getRuntime();
        const auto cached = runtime.getCache<CachedWalletBalances>(;
        "evm-wallet:" + std::to_string(chain) + ":" + std::to_string(address.toLowerCase())
        );
        if (!cached) return null;
        if (Date.now() - cached.cachedAt >= WALLET_CACHE_TTL_MS) return null;
        console.log(
        "[EVM Balances] Using cached wallet data (" + std::to_string(cached.tokens.length) + " tokens)"
        );
        return cached.tokens;
        } catch {
            return nullptr;
        }

}

std::future<void> setCachedWalletBalances(const std::string& chain, const std::string& address, const std::vector<TokenBalance>& tokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto runtime = agentRuntime.getRuntime();
        "evm-wallet:" + std::to_string(chain) + ":" + std::to_string(address.toLowerCase())
            tokens,
            cachedAt: Date.now(),
            });
            } catch {
                // Ignore
            }

}

std::future<std::optional<std::string>> cacheImageToBlob(const std::optional<std::string>& imageUrl) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!imageUrl) return null;

    // Skip if already a blob URL
    if (imageUrl.includes("blob.vercel-storage.com")) {
        return imageUrl;
    }

    try {
        const auto urlHash = crypto.createHash("md5").update(imageUrl).digest("hex");
        const auto extension = getExtensionFromUrl(imageUrl) || "png";
        const auto blobPath = "token-images/" + std::to_string(urlHash) + "." + std::to_string(extension);

        // Check if already cached in blob storage
        const auto existing = head(blobPath).catch(() => nullptr);
        if (existing) {
            return existing.url;
        }

        // Download image
        const auto response = fetch(imageUrl, {;
            headers: { "User-Agent": "OTC-Desk/1.0" },
            signal: AbortSignal.timeout(5000),
            });

            if (!response.ok) {
                return nullptr;
            }

            const auto contentType = response.headers.get("content-type") || "image/png";
            const auto imageBuffer = response.arrayBuffer();

            const auto blob = put(blobPath, imageBuffer, {;
                access: "public",
                contentType,
                addRandomSuffix: false,
                allowOverwrite: true,
                });

                return blob.url;
                } catch {
                    return nullptr;
                }

}

std::optional<std::string> getExtensionFromUrl(const std::string& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto pathname = new URL(url).pathname;
        const auto match = pathname.match(/\.([a-zA-Z0-9]+)$/);
        if (match) {
            const auto ext = match[1].toLowerCase();
            if (["png", "jpg", "jpeg", "gif", "webp", "svg"].includes(ext)) {
                return ext;
            }
        }
        return nullptr;
        } catch {
            return nullptr;
        }

}

std::future<std::unordered_map<std::string, double>> getBulkPriceCache(const std::string& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto runtime = agentRuntime.getRuntime();
        const auto cached = runtime.getCache<BulkPriceCache>(;
        "evm-prices-bulk:" + std::to_string(chain)
        );
        if (!cached) return {};
        if (Date.now() - cached.cachedAt >= PRICE_CACHE_TTL_MS) return {};
        console.log(
        "[EVM Balances] Using cached prices (" + std::to_string(Object.keys(cached.prices).length) + " tokens)"
        );
        return cached.prices;
        } catch {
            return {}
        }

}

std::future<void> setBulkPriceCache(const std::string& chain, Record<string prices, auto number>) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto runtime = agentRuntime.getRuntime();
        "evm-prices-bulk:" + std::to_string(chain)
            prices,
            cachedAt: Date.now(),
            });
            } catch {
                // Ignore
            }

}

std::future<std::vector<TokenBalance>> fetchAlchemyBalances(const std::string& address, const std::string& chain, const std::string& apiKey) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = CHAIN_CONFIG[chain];
    if (!config) return [];

    const auto url = "https://" + std::to_string(config.alchemyNetwork) + ".g.alchemy.com/v2/" + std::to_string(apiKey);

    try {
        // Step 1: Get all token balances (fast, single call)
        const auto balancesResponse = fetch(url, {;
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({
                jsonrpc: "2.0",
                id: 1,
                method: "alchemy_getTokenBalances",
                params: [address, "erc20"],
                }),
                signal: AbortSignal.timeout(10000),
                });

                if (!balancesResponse.ok) {
                    console.error(
                    "[EVM Balances] getTokenBalances failed:",
                    balancesResponse.status,
                    );
                    return [];
                }

                const auto balancesData = balancesResponse.json();

                if (balancesData.error) {
                    console.error(
                    "[EVM Balances] Alchemy error:",
                    balancesData.error.message,
                    );
                    return [];
                }

                const auto tokenBalances = balancesData.result.tokenBalances || [];

                // Filter non-zero balances
                const auto nonZeroBalances = tokenBalances.filter(;
                [&](t: { tokenBalance: string }) {
                    const auto bal = t.tokenBalance;
                    return bal && bal != "0x0" && bal != "0x" && BigInt(bal) > 0n;
                    },
                    );

                    console.log(
                    "[EVM Balances] Found " + std::to_string(nonZeroBalances.length) + " tokens with balance > 0"
                    );

                    if (nonZeroBalances.length == 0) return [];

                    // Step 2: Get bulk metadata cache (single fast lookup)
                    const auto bulkCache = getBulkMetadataCache(chain);
                    const std::unordered_map<std::string, CachedTokenMetadata> cachedMetadata = {;
                        ...bulkCache,
                        };
                        const std::vector<std::string> needsMetadata = [];

                        for (const auto& t : nonZeroBalances)
                            const auto addr = (;
                        t as { contractAddress: string }
                        ).contractAddress.toLowerCase();
                        if (!cachedMetadata[addr]) {
                            needsMetadata.push(addr);
                        }
                    }

                    console.log(
                    "[EVM Balances] " + std::to_string(Object.keys(cachedMetadata).length) + " cached, " + std::to_string(needsMetadata.length) + " need metadata"
                    );

                    // Step 3: Fetch metadata for uncached tokens (parallel, fast)
                    if (needsMetadata.length > 0) {
                        const auto metadataResults = Promise.all(;
                        needsMetadata.map(async (contractAddress) => {
                            try {
                                const auto metaRes = fetch(url, {;
                                    method: "POST",
                                    headers: { "Content-Type": "application/json" },
                                    body: JSON.stringify({
                                        jsonrpc: "2.0",
                                        id: 1,
                                        method: "alchemy_getTokenMetadata",
                                        params: [contractAddress],
                                        }),
                                        signal: AbortSignal.timeout(5000),
                                        });

                                        if (metaRes.ok) {
                                            const auto metaData = metaRes.json();
                                            const auto result = metaData.result || {};

                                            const CachedTokenMetadata metadata = {;
                                                symbol: result.symbol || "ERC20",
                                                name: result.name || "Unknown Token",
                                                decimals: result.decimals || 18,
                                                logoUrl: result.logo || std::nullopt,
                                                };

                                                // Fire-and-forget image caching
                                                if (result.logo) {
                                                    cacheImageToBlob(result.logo).catch(() => {
                                                        // Ignore image caching failures - non-critical
                                                        });
                                                    }

                                                    return { contractAddress, metadata }
                                                }
                                                } catch {
                                                    // Network/timeout errors when fetching metadata - use fallback
                                                }

                                                return {
                                                    contractAddress,
                                                    metadata: { symbol: "ERC20", name: "Unknown Token", decimals: 18 },
                                                    };
                                                    }),
                                                    );

                                                    for (const int { contractAddress, metadata } of metadataResults) {
                                                        cachedMetadata[contractAddress] = metadata;
                                                    }

                                                    // Update bulk cache with new metadata (merge with existing to handle concurrent requests)
                                                    getBulkMetadataCache(chain);
                                                    .then((existing) => {
                                                        const auto merged = { ...existing, ...cachedMetadata };
                                                        setBulkMetadataCache(chain, merged).catch(() => {
                                                            // Cache write failures are non-critical
                                                            });
                                                            });
                                                            .catch(() => {
                                                                // Cache read failures are non-critical - proceed without merging
                                                                });
                                                            }

                                                            // Step 4: Build token list
                                                            const std::vector<TokenBalance> tokens = nonZeroBalances.map(;
                                                            [&](tokenData: { contractAddress: string; tokenBalance: string }) {
                                                                const auto contractAddress = tokenData.contractAddress.toLowerCase();
                                                                const auto balance = BigInt(tokenData.tokenBalance).toString();
                                                                const auto metadata = cachedMetadata[contractAddress] || {;
                                                                    symbol: "ERC20",
                                                                    name: "Unknown",
                                                                    decimals: 18,
                                                                    };

                                                                    return {
                                                                        contractAddress,
                                                                        symbol: metadata.symbol,
                                                                        name: metadata.name,
                                                                        decimals: metadata.decimals,
                                                                        balance,
                                                                        logoUrl: metadata.logoUrl,
                                                                        };
                                                                        },
                                                                        );

                                                                        return tokens;
                                                                        } catch (error) {
                                                                            std::cerr << "[EVM Balances] Alchemy error:" << error << std::endl;
                                                                            return [];
                                                                        }

}

std::future<std::unordered_map<std::string, double>> fetchDeFiLlamaPrices(const std::string& chain, const std::vector<std::string>& addresses) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (addresses.length == 0) return {};

    // DeFiLlama chain identifiers
    const auto llamaChain =;
    chain == "base" ? "base" : chain == "bsc" ? "bsc" : chain;

    try {
        // DeFiLlama accepts comma-separated list of chain:address
        const auto coins = std::to_string(llamaChain) + ":" + std::to_string(a);
        const auto url = "https://coins.llama.fi/prices/current/" + std::to_string(coins);

        const auto response = fetch(url, { signal: AbortSignal.timeout(10000) });

        if (!response.ok) {
            std::cout << "[EVM Balances] DeFiLlama API error:" << response.status << std::endl;
            return {}
        }

        const auto data = response.json();
        const std::unordered_map<std::string, double> prices = {};

        // Response format: { coins: { "chain:address": { price: number, ... } } }
        if (data.coins) {
            for (const int [key, priceData] of Object.entries(data.coins)) {
                const auto address = key.split(":")[1].toLowerCase();
                const auto price = (priceData as { price?: number }).price;
                if (address && typeof price == "number" && price > 0) {
                    prices[address] = price;
                }
            }
        }

        console.log(
        "[EVM Balances] DeFiLlama returned " + std::to_string(Object.keys(prices).length) + " prices"
        );
        return prices;
        } catch (error) {
            std::cerr << "[EVM Balances] DeFiLlama error:" << error << std::endl;
            return {}
        }

}

std::future<std::unordered_map<std::string, double>> fetchCoinGeckoPrices(const std::string& chain, const std::vector<std::string>& addresses) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (addresses.length == 0) return {};

    const auto config = CHAIN_CONFIG[chain];
    if (!config) return {};

    try {
        const auto addressList = addresses.join(",");
        const auto apiKey = process.env.COINGECKO_API_KEY;

        const auto url = apiKey;
        "https://pro-api.coingecko.com/api/v3/simple/token_price/" + std::to_string(config.coingeckoPlatform) + "?contract_addresses=" + std::to_string(addressList) + "&vs_currencies=usd"
        "https://api.coingecko.com/api/v3/simple/token_price/" + std::to_string(config.coingeckoPlatform) + "?contract_addresses=" + std::to_string(addressList) + "&vs_currencies=usd"

        const HeadersInit headers = {};
        if (apiKey) {
            headers["X-Cg-Pro-Api-Key"] = apiKey;
        }

        const auto response = fetch(url, {;
            headers,
            signal: AbortSignal.timeout(10000),
            });

            if (!response.ok) return {};

            const auto data = response.json();
            const std::unordered_map<std::string, double> prices = {};

            for (const int [address, priceData] of Object.entries(data)) {
                const auto usd = (priceData as { usd?: number }).usd;
                if (typeof usd == "number") {
                    prices[address.toLowerCase()] = usd;
                }
            }

            return prices;
            } catch {
                return {}
            }

}

std::future<std::unordered_map<std::string, double>> fetchPrices(const std::string& chain, const std::vector<std::string>& addresses) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (addresses.length == 0) return {};

    // Try DeFiLlama first (better coverage for newer tokens)
    const auto llamaPrices = fetchDeFiLlamaPrices(chain, addresses);

    // Find addresses still missing prices
    const auto missingAddresses = addresses.filter(;
    [&](a) { return !llamaPrices[a.toLowerCase()],; }
    );

    if (missingAddresses.length == 0) {
        return llamaPrices;
    }

    // Try CoinGecko for remaining
    const auto geckoprices = fetchCoinGeckoPrices(chain, missingAddresses);

    return { ...llamaPrices, ...geckoprices }

}

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto address = request.nextUrl.searchParams.get("address");
    const auto chain = request.nextUrl.searchParams.get("chain") || "base";
    const auto forceRefresh = request.nextUrl.searchParams.get("refresh") == "true";

    if (!address) {
        return NextResponse.json({ error: "Address required" }, { status: 400 });
    }

    if (!CHAIN_CONFIG[chain]) {
        return NextResponse.json({ error: "Unsupported chain" }, { status: 400 });
    }

    try {
        // Check wallet cache first (15 minute TTL) unless force refresh
        if (!forceRefresh) {
            const auto cachedTokens = getCachedWalletBalances(chain, address);
            if (cachedTokens) {
                return NextResponse.json({ tokens: cachedTokens });
            }
            } else {
                std::cout << "[EVM Balances] Force refresh requested" << std::endl;
            }

            const auto alchemyKey =;
            process.env.ALCHEMY_API_KEY || process.env.NEXT_PUBLIC_ALCHEMY_API_KEY;

            if (!alchemyKey) {
                std::cerr << "[EVM Balances] ALCHEMY_API_KEY is required - add to .env" << std::endl;
                return NextResponse.json({;
                    tokens: [],
                    error: "ALCHEMY_API_KEY required",
                    });
                }

                std::cout << "[EVM Balances] Using Alchemy API" << std::endl;
                const auto tokens = fetchAlchemyBalances(address, chain, alchemyKey);

                if (tokens.length == 0) {
                    return NextResponse.json({ tokens: [] });
                }

                // Get bulk price cache (single fast lookup)
                const auto cachedPrices = getBulkPriceCache(chain);
                const auto tokensNeedingPrices = tokens.filter((t) => !t.priceUsd);
                const std::vector<std::string> uncachedAddresses = [];

                // Apply cached prices first
                for (const auto& token : tokensNeedingPrices)
                    const auto cachedPrice = cachedPrices[token.contractAddress.toLowerCase()];
                    if (cachedPrice != undefined) {
                        token.priceUsd = cachedPrice;
                        } else {
                            uncachedAddresses.push(token.contractAddress);
                        }
                    }

                    console.log(
                    "[EVM Balances] " + std::to_string(Object.keys(cachedPrices).length) + " prices cached, " + std::to_string(uncachedAddresses.length) + " need fetch"
                    );

                    // Fetch uncached prices (DeFiLlama + CoinGecko)
                    if (uncachedAddresses.length > 0) {
                        const auto newPrices = fetchPrices(chain, uncachedAddresses);
                        for (const auto& token : tokensNeedingPrices)
                            if (!token.priceUsd) {
                                const auto price = newPrices[token.contractAddress.toLowerCase()] || 0;
                                token.priceUsd = price;
                            }
                        }

                        // Merge new prices with cached and save (fire-and-forget)
                        const auto allPrices = { ...cachedPrices };
                        for (const int [addr, price] of Object.entries(newPrices)) {
                            if (price > 0) {
                                allPrices[addr.toLowerCase()] = price;
                            }
                        }
                        // Merge with existing to handle concurrent requests
                        getBulkPriceCache(chain);
                        .then((existing) => {
                            const auto merged = { ...existing, ...allPrices };
                            setBulkPriceCache(chain, merged).catch((err) =>;
                            console.debug("[EVM Balances] Price cache write failed:", err),
                            );
                            });
                            .catch(() => {});
                        }

                        // Calculate USD values
                        for (const auto& token : tokens)
                            if (!token.balanceUsd && token.priceUsd) {
                                const auto humanBalance =;
                                Number(BigInt(token.balance)) / Math.pow(10, token.decimals);
                                token.balanceUsd = humanBalance * token.priceUsd;
                            }
                        }

                        // Filter only obvious dust - show tokens without prices too
                        const auto filteredTokens = tokens.filter((t) => {;
                            const auto humanBalance = Number(BigInt(t.balance)) / Math.pow(10, t.decimals);
                            const auto balanceUsd = t.balanceUsd || 0;
                            const auto hasPrice = t.priceUsd && t.priceUsd > 0;

                            // If we have a price, use minimal USD filter
                            if (hasPrice && balanceUsd < MIN_VALUE_USD) {
                                return false;
                            }
                            // Always require at least 1 token
                            return humanBalance >= MIN_TOKEN_BALANCE;
                            });

                            // Sort: priced tokens first (by USD value), then unpriced tokens (by balance)
                            filteredTokens.sort((a, b) => {
                                const auto aHasPrice = a.priceUsd && a.priceUsd > 0;
                                const auto bHasPrice = b.priceUsd && b.priceUsd > 0;

                                // Priced tokens come first
                                if (aHasPrice && !bHasPrice) return -1;
                                if (!aHasPrice && bHasPrice) return 1;

                                // Both priced: sort by USD value
                                if (aHasPrice && bHasPrice) {
                                    return (b.balanceUsd || 0) - (a.balanceUsd || 0);
                                }

                                // Both unpriced: sort by token balance
                                const auto aBalance = Number(BigInt(a.balance)) / Math.pow(10, a.decimals);
                                const auto bBalance = Number(BigInt(b.balance)) / Math.pow(10, b.decimals);
                                return bBalance - aBalance;
                                });

                                console.log(
                                "[EVM Balances] " + std::to_string(tokens.length) + " total -> " + std::to_string(filteredTokens.length) + " after dust filter"
                                );

                                // Cache the result for 15 minutes
                                setCachedWalletBalances(chain, address, filteredTokens);

                                return NextResponse.json({ tokens: filteredTokens });
                                } catch (error) {
                                    std::cerr << "[EVM Balances] Error:" << error << std::endl;
                                    return NextResponse.json({ tokens: [] });
                                }

}

} // namespace elizaos
