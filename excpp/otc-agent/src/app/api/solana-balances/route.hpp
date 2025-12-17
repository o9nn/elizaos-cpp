#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
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
    std::string name;
};


// Bulk price cache for Solana
struct SolanaPriceCache {
    double cachedAt;
};

std::future<void> setSolanaPriceCache(const std::unordered_map<std::string, double>& prices);

struct CachedWalletResponse {
    std::string mint;
    double amount;
    double decimals;
    std::string symbol;
    std::string name;
    std::string logoURI;
    double priceUsd;
    double balanceUsd;
    double cachedAt;
};

std::future<void> setCachedWalletResponse(const std::string& address, CachedWalletResponse["tokens"] tokens);

// Alternative IPFS gateways to try if main one fails

/**
 * Try to fetch image from multiple IPFS gateways
 */

/**
 * Cache an image URL to Vercel Blob storage
 * Returns the cached blob URL, or null if caching fails (don't return broken IPFS URLs)
 */
std::future<std::string> cacheImageToBlob(const std::string& imageUrl);

std::string getExtensionFromUrl(const std::string& url);

/**
 * Codex GraphQL response types
 */
struct CodexBalanceItem {
    std::string balance;
    std::string balanceUsd;
    double shiftedBalance;
    std::string tokenAddress;
    std::string name;
    std::string symbol;
    std::string address;
    double decimals;
    double networkId;
    std::string imageSmallUrl;
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
