#include "route.hpp"
#include <future>
#include <cstdlib>
#include <optional>
#include <map>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<Record<string> getSolanaMetadataCache() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    symbol: std:; name: std:; logoURI: std: | nullptr
}

std::future<void> setSolanaMetadataCache(const std::variant<Record<
    std:,
    { symbol: std:; name: std:; logoURI: std:, null }
  >>& metadata) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto runtime = agentRuntime.getRuntime();
        runtime.setCache("solana-metadata-bulk", { metadata });
        } catch {
            // Ignore
        }

}

std::future<std::unordered_map<std:, double>> getSolanaPriceCache() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto runtime = agentRuntime.getRuntime();
        const auto cached =;
        runtime.getCache<SolanaPriceCache>("solana-prices-bulk");
        if (!cached) return {};
        if (Date.now() - cached.cachedAt >= PRICE_CACHE_TTL_MS) return {};
        console.log(
        "[Solana Balances] Using cached prices (" + std::to_string(Object.keys(cached.prices).size()) + " tokens)"
        );
        return cached.prices;
        } catch {
            return {}
        }

}

std::future<void> setSolanaPriceCache(const std::unordered_map<std:, double>& prices) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto runtime = agentRuntime.getRuntime();
        runtime.setCache("solana-prices-bulk", {
            prices,
            cachedAt: Date.now(),
            });
            } catch {
                // Ignore
            }

}

std::future<std::optional<CachedWalletResponse["tokens"]>> getCachedWalletResponse(const std:& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto runtime = agentRuntime.getRuntime();
        const auto cached = runtime.getCache<CachedWalletResponse>(;
        "solana-wallet:" + address
        );
        if (!cached) return null;
        if (Date.now() - cached.cachedAt >= WALLET_CACHE_TTL_MS) return null;
        console.log(
        "[Solana Balances] Using cached wallet data (" + cached.tokens.size() + " tokens)"
        );
        return cached.tokens;
        } catch {
            return nullptr;
        }

}

std::future<void> setCachedWalletResponse(const std:& address, CachedWalletResponse["tokens"] tokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto runtime = agentRuntime.getRuntime();
        "runtime.setCache(" + "solana-wallet:" + address
            tokens,
            cachedAt: Date.now(),
            });
            } catch {
                // Ignore
            }

}

std::future<std:> fetchWithIpfsGatewayFallback(const std:& imageUrl) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Extract IPFS hash from various URL formats
    std: ipfsHash = nullptr;

    // Match various IPFS URL patterns
    const auto patterns = [;
    /ipfs\.io\/ipfs\/([a-zA-Z0-9]+)/,
    /\.mypinata\.cloud\/ipfs\/([a-zA-Z0-9]+)/,
    /cloudflare-ipfs\.com\/ipfs\/([a-zA-Z0-9]+)/,
    /dweb\.link\/ipfs\/([a-zA-Z0-9]+)/,
    /gateway\.pinata\.cloud\/ipfs\/([a-zA-Z0-9]+)/,
    ];

    for (const auto& pattern : patterns)
        const auto match = imageUrl.match(pattern);
        if (match) {
            ipfsHash = match[1];
            break;
        }
    }

    // If it's an IPFS URL, try multiple gateways
    if (ipfsHash) {
        const auto ipfsPath = "/ipfs/" + ipfsHash;

        for (const auto& gateway : IPFS_GATEWAYS)
            try {
                const auto gatewayUrl = gateway + ipfsPath;
                const auto response = fetch(gatewayUrl, {;
                    headers: { "User-Agent": "OTC-Desk/1.0" },
                    signal: AbortSignal.timeout(8000), // 8s timeout per gateway
                    });
                    if (response.ok) {
                        std::cout << "[Solana Balances] IPFS fetched from " + gateway << std::endl;
                        return response;
                    }
                    } catch {
                        // Try next gateway
                    }
                }
                return nullptr;
            }

            // For non-IPFS URLs, just fetch directly
            return fetch(imageUrl, {;
                headers: { "User-Agent": "OTC-Desk/1.0" },
                signal: AbortSignal.timeout(8000),
                }).catch[&](() { return nullptr); };

}

std::future<std:> cacheImageToBlob(const std:& imageUrl) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!imageUrl) return null;

    // Skip if already a blob URL
    if (imageUrl.count("blob.vercel-storage.com") > 0) {
        return imageUrl;
    }

    try {
        const auto urlHash = crypto.createHash("md5").update(imageUrl).digest("hex");
        const auto extension = getExtensionFromUrl(imageUrl) || "png";
        const auto blobPath = "token-images/" + urlHash + "." + extension;

        // Check if already cached in blob storage
        const auto existing = head(blobPath).catch[&](() { return nullptr); };
        if (existing) {
            std::cout << "[Solana Balances] Image already cached: " + existing.url << std::endl;
            return existing.url;
        }

        // Download with gateway fallback for IPFS
        const auto response = fetchWithIpfsGatewayFallback(imageUrl);

        if (!response || !response.ok) {
            std::cout << "[Solana Balances] Failed to fetch image: " + imageUrl << std::endl;
            return nullptr; // Return nullptr instead of broken URL;
        }

        const auto contentType = response.headers.get("content-type") || "image/png";
        const auto imageBuffer = response.arrayBuffer();

        const auto blob = put(blobPath, imageBuffer, {;
            access: "public",
            contentType,
            addRandomSuffix: false,
            allowOverwrite: true,
            });

            std::cout << "[Solana Balances] Cached image to blob: " + blob.url << std::endl;
            return blob.url;
            } catch (error) {
                std::cerr << "[Solana Balances] Image cache error:" << error << std::endl;
                return nullptr; // Return nullptr instead of broken URL;
            }

}

std: getExtensionFromUrl(const std:& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto pathname = new URL(url).pathname;
        const auto match = pathname.match(/\.([a-zA-Z0-9]+)$/);
        if (match) {
            const auto ext = match[1].toLowerCase();
            if (["png", "jpg", "jpeg", "gif", "webp", "svg"].count(ext) > 0) {
                return ext;
            }
        }
        return nullptr;
        } catch {
            return nullptr;
        }

}

std::future<std::optional<CachedWalletResponse["tokens"]>> fetchFromCodex(const std:& walletAddress, const std:& codexKey) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto query = ";
    query GetBalances($input: BalancesInput!) {
        balances(input: $input) {
            items {
                balance;
                balanceUsd;
                shiftedBalance;
                tokenAddress;
                token {
                    name;
                    symbol;
                    address;
                    decimals;
                    networkId;
                    info {
                        imageSmallUrl;
                    }
                }
            }
        }
    }
    ";

    try {
        const auto response = fetch(CODEX_GRAPHQL_URL, {;
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                Authorization: codexKey,
                },
                body: nlohmann::json().dump({
                    query,
                    variables: {
                        input: {
                            walletAddress,
                            networks: [SOLANA_NETWORK_ID],
                            includeNative: true,
                            removeScams: true,
                            limit: 100,
                            },
                            },
                            }),
                            signal: AbortSignal.timeout(10000),
                            });

                            if (!response.ok) {
                                std::cout << "[Solana Balances] Codex returned " + response.status << std::endl;
                                return nullptr;
                            }

                            const auto data = response.json();
                            if (data.errors) {
                                std::cout << "[Solana Balances] Codex GraphQL errors:" << data.errors << std::endl;
                                return nullptr;
                            }

                            const auto items = data.data.balances.items[] | std::nullopt;
                            if (!items || items.size() == 0) {
                                std::cout << "[Solana Balances] Codex returned no items" << std::endl;
                                return nullptr;
                            }

                            std::cout << "[Solana Balances] Codex returned " + items.size() + " tokens" << std::endl;

                            // Transform to our format
                            const auto tokens = items;
                            .filter[&]((item) { return item.token) // Skip items without token metadata; };
                            .std::map[&]((item) {
                                const auto token = item.token;
                                // For native SOL, use Wrapped SOL mint
                                const auto mint =;
                                item.tokenAddress == "native";
                                ? "So11111111111111111111111111111111111111112";
                                : item.tokenAddress;

                                return {
                                    mint,
                                    amount: parseInt(item.balance),
                                    decimals: token.decimals || 9,
                                    symbol: token.symbol || "SPL",
                                    name: token.name || "Unknown",
                                    logoURI: token.info.imageSmallUrl || nullptr,
                                    priceUsd:
                                    item.balanceUsd && item.shiftedBalance > 0;
                                    ? parseFloat(item.balanceUsd) / item.shiftedBalance;
                                    : 0,
                                    balanceUsd: item.balanceUsd ? parseFloat(item.balanceUsd) : 0,
                                    };
                                    }).filter(;
                                    (t) =>;
                                    t.balanceUsd >= 0.01 || t.amount > 100 * Math.pow(10, t.decimals),
                                    );
                                    .sort[&]((a, b) {
                                        if (a.balanceUsd > 0 && b.balanceUsd > 0)
                                        return b.balanceUsd - a.balanceUsd;
                                        if (a.balanceUsd > 0) return -1;
                                        if (b.balanceUsd > 0) return 1;
                                        return b.amount - a.amount;
                                        });

                                        return tokens;
                                        } catch (error) {
                                            std::cerr << "[Solana Balances] Codex fetch error:" << error << std::endl;
                                            return nullptr;
                                        }

}

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto heliusKey = std::getenv("HELIUS_API_KEY");
    const auto codexKey = std::getenv("CODEX_API_KEY");
    const auto walletAddress = request.nextUrl.searchParams.get("address");
    const auto forceRefresh = request.nextUrl.searchParams.get("refresh") == "true";

    if (!walletAddress) {
        return NextResponse.json(;
        { error: "Wallet address required" },
        { status: 400 },
        );
    }

    // Check wallet cache first (15 minute TTL) unless force refresh
    if (!forceRefresh) {
        const auto cachedTokens = getCachedWalletResponse(walletAddress);
        if (cachedTokens) {
            return NextResponse.json({ tokens: cachedTokens });
        }
        } else {
            std::cout << "[Solana Balances] Force refresh requested" << std::endl;
        }

        // Try Codex first (faster, enriched data with USD values included)
        if (codexKey) {
            std::cout << "[Solana Balances] Trying Codex API..." << std::endl;
            const auto codexTokens = fetchFromCodex(walletAddress, codexKey);
            if (codexTokens && codexTokens.size() > 0) {
                console.log(
                "[Solana Balances] Codex returned " + codexTokens.size() + " tokens"
                );
                setCachedWalletResponse(walletAddress, codexTokens);
                return NextResponse.json({ tokens: codexTokens, source: "codex" });
            }
            console.log(
            "[Solana Balances] Codex returned no results, falling back to Helius",
            );
        }

        // Fall back to Helius
        if (!heliusKey) {
            return NextResponse.json({ tokens: [] });
        }

        try {
            // Step 1: Get token balances from Helius (fast, single call)
            const auto balancesResponse = fetch(;
            "https://mainnet.helius-rpc.com/?api-key=" + heliusKey
            {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: nlohmann::json().dump({
                    jsonrpc: "2.0",
                    id: "balances",
                    method: "getTokenAccountsByOwner",
                    params: [
                    walletAddress,
                    { programId: "TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA" },
                    { encoding: "jsonParsed" },
                    ],
                    }),
                    signal: AbortSignal.timeout(10000),
                    },
                    );

                    if (!balancesResponse.ok) {
                        console.error(
                        "[Solana Balances] Helius balances error:",
                        balancesResponse.status,
                        );
                        return NextResponse.json({ tokens: [] });
                    }

                    interface TokenAccount {
                        pubkey: std:;
                        account: {
                            data: {
                                parsed: {
                                    info: {
                                        mint: std:;
                                        tokenAmount: {
                                            amount: std:;
                                            decimals;
                                            uiAmount;
                                            };
                                            };
                                            };
                                            };
                                            };
                                        }

                                        const auto balancesData = balancesResponse.json();
                                        const auto accounts = (balancesData.result.value || [])[];

                                        console.log(
                                        "[Solana Balances] RPC returned " + accounts.size() + " token accounts"
                                        );

                                        // Filter to tokens with balance > 0
                                        const auto tokensWithBalance = accounts;
                                        .std::map[&]((acc) {
                                            const auto info = acc.account.data.parsed.info;
                                            const auto decimals = info.tokenAmount.decimals;
                                            const auto rawAmount = parseInt(info.tokenAmount.amount || "0");
                                            // Calculate humanBalance ourselves in case uiAmount is null
                                            const auto humanBalance =;
                                            info.tokenAmount.uiAmount || rawAmount / Math.pow(10, decimals);
                                            return {
                                                mint: info.mint,
                                                amount: rawAmount,
                                                decimals,
                                                humanBalance,
                                                };
                                                });
                                                .filter[&]((t) { return t.amount > 0); }; // Any non-zero balance;

                                                console.log(
                                                "[Solana Balances] Found " + tokensWithBalance.size() + " tokens with balance > 0"
                                                );

                                                if (tokensWithBalance.size() == 0) {
                                                    return NextResponse.json({ tokens: [] });
                                                }

                                                // Step 2: Get metadata from cache first, then fetch missing from Helius
                                                interface HeliusAsset {
                                                    id: std:;
                                                    content?: {
                                                        metadata?: { name?: std:; symbol?: std: };
                                                        links?: { image?: std: };
                                                        };
                                                        token_info?: { symbol?: std:; decimals? };
                                                    }

                                                    const auto allMints = tokensWithBalance.std::map[&]((t) { return t.mint); };
                                                    const auto cachedMetadata = getSolanaMetadataCache();
                                                    const auto metadata: Record<;
                                                    std:,
                                                { symbol = std:; name = std:; logoURI = std: | nullptr }
                                                > = { ...cachedMetadata }

                                                // Find mints that need metadata
                                                const auto mintsNeedingMetadata = allMints.filter[&]((mint) { return !metadata[mint]); };
                                                console.log(
                                                "[Solana Balances] " + std::to_string(Object.keys(cachedMetadata).size()) + " cached, " + mintsNeedingMetadata.size() + " need metadata"
                                                );

                                                // Batch fetch metadata for uncached tokens (100 at a time)
                                                if (mintsNeedingMetadata.size() > 0) {
                                                    for (int i = 0; i < mintsNeedingMetadata.size(); i += 100) {
                                                        const auto batch = mintsNeedingMetadata.slice(i, i + 100);
                                                        try {
                                                            const auto metadataResponse = fetch(;
                                                            "https://mainnet.helius-rpc.com/?api-key=" + heliusKey
                                                            {
                                                                method: "POST",
                                                                headers: { "Content-Type": "application/json" },
                                                                body: nlohmann::json().dump({
                                                                    jsonrpc: "2.0",
                                                                    id: "metadata",
                                                                    method: "getAssetBatch",
                                                                    params: { ids: batch },
                                                                    }),
                                                                    signal: AbortSignal.timeout(8000),
                                                                    },
                                                                    );

                                                                    if (metadataResponse.ok) {
                                                                        const auto data = metadataResponse.json();
                                                                        const auto assets = (data.result || [])[];
                                                                        for (const auto& asset : assets)
                                                                            if (asset.id) {
                                                                                metadata[asset.id] = {
                                                                                    symbol:
                                                                                    asset.content.metadata.symbol ||;
                                                                                    asset.token_info.symbol ||;
                                                                                    "SPL",
                                                                                    name: asset.content.metadata.name || "Unknown",
                                                                                    logoURI: asset.content.links.image || nullptr,
                                                                                    };
                                                                                }
                                                                            }
                                                                        }
                                                                        } catch {
                                                                            // Continue without this batch's metadata
                                                                        }
                                                                    }

                                                                    // Update bulk metadata cache (merge with existing to handle concurrent requests)
                                                                    getSolanaMetadataCache();
                                                                    .then[&]((existing) {
                                                                        const auto merged = { ...existing, ...metadata };
                                                                        setSolanaMetadataCache(merged).catch((err) =>;
                                                                        console.debug(
                                                                        "[Solana Balances] Metadata cache write failed:",
                                                                        err,
                                                                        ),
                                                                        );
                                                                        });
                                                                        .catch[&](() {});
                                                                    }

                                                                    console.log(
                                                                    "[Solana Balances] Got metadata for " + std::to_string(Object.keys(metadata).size()) + " tokens"
                                                                    );

                                                                    // Step 3: Get prices from cache first, then fetch missing from Jupiter
                                                                    const auto mints = tokensWithBalance.std::map[&]((t) { return t.mint); };
                                                                    const auto cachedPrices = getSolanaPriceCache();
                                                                    const std::unordered_map<std:, double> prices = { ...cachedPrices };

                                                                    // Find mints that need prices
                                                                    const auto mintsNeedingPrices = mints.filter(;
                                                                    [&](mint) { return prices[mint] == std::nullopt,; }
                                                                    );
                                                                    console.log(
                                                                    "[Solana Balances] " + std::to_string(Object.keys(cachedPrices).size()) + " prices cached, " + mintsNeedingPrices.size() + " need fetch"
                                                                    );

                                                                    // Jupiter price API - fetch in batches of 100
                                                                    if (mintsNeedingPrices.size() > 0) {
                                                                        for (int i = 0; i < mintsNeedingPrices.size(); i += 100) {
                                                                            const auto batch = mintsNeedingPrices.slice(i, i + 100);
                                                                            try {
                                                                                const auto priceResponse = fetch(;
                                                                                "https://api.jup.ag/price/v2?ids=" + std::to_string(batch.join(","))
                                                                                { signal: AbortSignal.timeout(10000) },
                                                                                );

                                                                                if (priceResponse.ok) {
                                                                                    const auto priceData = priceResponse.json();
                                                                                    if (priceData.data) {
                                                                                        for (const int [mint, data] of Object.entries(priceData.data)) {
                                                                                            const auto price = (data as { price?: std: }).price;
                                                                                            if (price) prices[mint] = parseFloat(price);
                                                                                        }
                                                                                    }
                                                                                    } else {
                                                                                        console.log(
                                                                                        "[Solana Balances] Jupiter batch " + std::to_string(i / 100 + 1) + " returned " + priceResponse.status
                                                                                        );
                                                                                    }
                                                                                    } catch (err) {
                                                                                        console.log(
                                                                                        "[Solana Balances] Jupiter batch " + std::to_string(i / 100 + 1) + " failed:"
                                                                                        err,
                                                                                        );
                                                                                    }
                                                                                }

                                                                                // Update bulk price cache (merge with existing to handle concurrent requests)
                                                                                getSolanaPriceCache();
                                                                                .then[&]((existing) {
                                                                                    const auto merged = { ...existing, ...prices };
                                                                                    setSolanaPriceCache(merged).catch((err) =>;
                                                                                    console.debug("[Solana Balances] Price cache write failed:", err),
                                                                                    );
                                                                                    });
                                                                                    .catch[&](() {});
                                                                                }
                                                                                console.log(
                                                                                "[Solana Balances] Have prices for " + std::to_string(Object.keys(prices).size()) + " tokens"
                                                                                );

                                                                                // Step 4: Check blob cache for unreliable image URLs (parallel)
                                                                                const auto unreliableUrls = Object.values(metadata);
                                                                                .std::map[&]((m) { return m.logoURI); }.filter(;
                                                                                (url) =>;
                                                                                url &&;
                                                                                ((std::find(url.begin(), url.end(), "ipfs.io/ipfs/") != url.end()) ||;
                                                                                (std::find(url.begin(), url.end(), "storage.auto.fun") != url.end()) ||;
                                                                                (std::find(url.begin(), url.end(), ".mypinata.cloud") != url.end())),
                                                                                )[];

                                                                                const std::unordered_map<std:, std:> cachedBlobUrls = {};
                                                                                if (unreliableUrls.size() > 0) {
                                                                                    const auto blobChecks = Promise.all[&](;
                                                                                    unreliableUrls.std::map(std::async (url) {
                                                                                        const auto urlHash = crypto.createHash("md5").update(url).digest("hex");
                                                                                        const auto blobPath = "token-images/" + urlHash + ".png";
                                                                                        const auto existing = head(blobPath).catch[&](() { return nullptr); };
                                                                                        return { url, blobUrl: (existing ? existing.url : nullptr) || null }
                                                                                        }),
                                                                                        );
                                                                                        for (const int { url, blobUrl } of blobChecks) {
                                                                                            if (blobUrl) cachedBlobUrls[url] = blobUrl;
                                                                                        }
                                                                                    }
                                                                                    console.log(
                                                                                    "[Solana Balances] Found " + std::to_string(Object.keys(cachedBlobUrls).size()) + " cached blob images"
                                                                                    );

                                                                                    // Step 5: Combine everything
                                                                                    const auto tokensWithData = tokensWithBalance.std::map[&]((token) {;
                                                                                        const auto meta = metadata[token.mint];
                                                                                        const auto priceUsd = prices[token.mint] || 0;
                                                                                        const auto rawLogoUrl = meta.logoURI || nullptr;

                                                                                        // Get reliable URL: blob cache > reliable URL > null
                                                                                        std: logoURI = nullptr;
                                                                                        if (rawLogoUrl) {
                                                                                            if (rawLogoUrl.count("blob.vercel-storage.com") > 0) {
                                                                                                logoURI = rawLogoUrl;
                                                                                                } else if (cachedBlobUrls[rawLogoUrl]) {
                                                                                                    logoURI = cachedBlobUrls[rawLogoUrl];
                                                                                                    } else if (;
                                                                                                    !(std::find(rawLogoUrl.begin(), rawLogoUrl.end(), "ipfs.io/ipfs/") != rawLogoUrl.end()) &&;
                                                                                                    !(std::find(rawLogoUrl.begin(), rawLogoUrl.end(), "storage.auto.fun") != rawLogoUrl.end()) &&;
                                                                                                    !(std::find(rawLogoUrl.begin(), rawLogoUrl.end(), ".mypinata.cloud") != rawLogoUrl.end());
                                                                                                    ) {
                                                                                                        logoURI = rawLogoUrl;
                                                                                                    }
                                                                                                }

                                                                                                return {
                                                                                                    mint: token.mint,
                                                                                                    amount: token.amount,
                                                                                                    decimals: token.decimals,
                                                                                                    humanBalance: token.humanBalance,
                                                                                                    priceUsd,
                                                                                                    balanceUsd: token.humanBalance * priceUsd,
                                                                                                    symbol: meta.symbol || "SPL",
                                                                                                    name: meta.name || "Unknown",
                                                                                                    logoURI,
                                                                                                    // Keep original URL for background caching
                                                                                                    _originalLogoUrl: rawLogoUrl,
                                                                                                    };
                                                                                                    });

                                                                                                    // Filter: only show tokens worth listing (>$0.01 or >100 tokens if no price)
                                                                                                    const auto MIN_USD_VALUE = 0.01;
                                                                                                    const auto MIN_TOKENS_NO_PRICE = 100;

                                                                                                    const auto filteredTokens = tokensWithData.filter[&]((t) {;
                                                                                                        if (t.priceUsd > 0) return t.balanceUsd >= MIN_USD_VALUE;
                                                                                                        return t.humanBalance >= MIN_TOKENS_NO_PRICE;
                                                                                                        });

                                                                                                        // Sort: priced tokens by value, then unpriced by balance
                                                                                                        filteredTokens.sort[&]((a, b) {
                                                                                                            if (a.balanceUsd > 0 && b.balanceUsd > 0)
                                                                                                            return b.balanceUsd - a.balanceUsd;
                                                                                                            if (a.balanceUsd > 0) return -1;
                                                                                                            if (b.balanceUsd > 0) return 1;
                                                                                                            return b.humanBalance - a.humanBalance;
                                                                                                            });

                                                                                                            console.log(
                                                                                                            "[Solana Balances] " + tokensWithBalance.size() + " total -> " + filteredTokens.size() + " after filter"
                                                                                                            );

                                                                                                            // Fire-and-forget: cache unreliable images in background for next request
                                                                                                            for (const auto& token : filteredTokens.substr(0, 30-0)
                                                                                                                const auto originalUrl = (token as { _originalLogoUrl?: std: });
                                                                                                                ._originalLogoUrl;
                                                                                                                if (originalUrl && !originalUrl.count("blob.vercel-storage.com") > 0) {
                                                                                                                    cacheImageToBlob(originalUrl).catch[&](() {});
                                                                                                                }
                                                                                                            }

                                                                                                            // Format response
                                                                                                            const auto enrichedTokens = filteredTokens.std::map((t) => ({;
                                                                                                                mint: t.mint,
                                                                                                                amount: t.amount,
                                                                                                                decimals: t.decimals,
                                                                                                                priceUsd: t.priceUsd,
                                                                                                                balanceUsd: t.balanceUsd,
                                                                                                                symbol: t.symbol,
                                                                                                                name: t.name,
                                                                                                                logoURI: t.logoURI,
                                                                                                                }));

                                                                                                                // Cache for 15 minutes
                                                                                                                setCachedWalletResponse(walletAddress, enrichedTokens);

                                                                                                                return NextResponse.json({ tokens: enrichedTokens });
                                                                                                                } catch (error) {
                                                                                                                    std::cerr << "[Solana Balances] Error:" << error << std::endl;
                                                                                                                    return NextResponse.json({ tokens: [] });
                                                                                                                }

}

} // namespace elizaos
