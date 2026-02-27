#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Wallet balance cache TTL: 15 minutes

// Price cache TTL: 15 minutes

// Codex GraphQL endpoint and Solana network ID

// Bulk metadata cache for Solana tokens (permanent - metadata doesn't change)
struct SolanaMetadataCache {
    Record< metadata;
    std: name;
};


// Bulk price cache for Solana
struct SolanaPriceCache {
    double cachedAt;
};

std::future<void> setSolanaPriceCache(const std::unordered_map<std:, double>& prices);

struct CachedWalletResponse {
    std: mint;
    double amount;
    double decimals;
    std: symbol;
    std: name;
    std: logoURI;
    double priceUsd;
    double balanceUsd;
    double cachedAt;
};

std::future<void> setCachedWalletResponse(const std:& address, CachedWalletResponse["tokens"] tokens);

// Alternative IPFS gateways to try if main one fails

/**
 * Try to fetch image from multiple IPFS gateways
 */
std::future<std:> fetchWithIpfsGatewayFallback(const std:& imageUrl);

/**
 * Cache an image URL to Vercel Blob storage
 * Returns the cached blob URL, or null if caching fails (don't return broken IPFS URLs)
 */
std::future<std:> cacheImageToBlob(const std:& imageUrl);

std: getExtensionFromUrl(const std:& url);

/**
 * Codex GraphQL response types
 */
struct CodexBalanceItem {
    std: balance;
    std: balanceUsd;
    double shiftedBalance;
    std: tokenAddress;
    std: name;
    std: symbol;
    std: address;
    double decimals;
    double networkId;
    std: imageSmallUrl;
};

/**
 * Fetch balances from Codex API (faster, enriched data)
 */

/**
 * Fetch Solana token balances with cached metadata
 * Tries Codex first, falls back to Helius
 */
std::future<void> GET(NextRequest request);

} // namespace elizaos
