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



// Wallet balance cache TTL: 15 minutes

// Price cache TTL: 15 minutes

// Codex GraphQL endpoint and Solana network ID

// Bulk metadata cache for Solana tokens (permanent - metadata doesn't change)
struct SolanaMetadataCache {
    Record< metadata;
};

  Record<string> getSolanaMetadataCache();>

// Bulk price cache for Solana
struct SolanaPriceCache {
    std::unordered_map<std::string, double> prices;
    double cachedAt;
};

std::future<void> setSolanaPriceCache(Record<string prices, auto number>);

struct CachedWalletResponse {
    Array<{ tokens;
    std::string mint;
    double amount;
    double decimals;
    std::string symbol;
    std::string name;
    string | null logoURI;
    double priceUsd;
    double balanceUsd;
    double cachedAt;
};

std::future<CachedWalletResponse["tokens"] | null> getCachedWalletResponse(const std::string& address);

std::future<void> setCachedWalletResponse(const std::string& address, CachedWalletResponse["tokens"] tokens);

// Alternative IPFS gateways to try if main one fails

/**
 * Try to fetch image from multiple IPFS gateways
 */
std::future<Response | null> fetchWithIpfsGatewayFallback(const std::string& imageUrl);

/**
 * Cache an image URL to Vercel Blob storage
 * Returns the cached blob URL, or null if caching fails (don't return broken IPFS URLs)
 */
std::future<string | null> cacheImageToBlob(string | null imageUrl);

/**
 * Codex GraphQL response types
 */
struct CodexBalanceItem {
    std::string balance;
    string | null balanceUsd;
    double shiftedBalance;
    std::string tokenAddress;
    { token;
    std::string name;
    std::string symbol;
    std::string address;
    double decimals;
    double networkId;
    std::optional<{> info;
    string | null imageSmallUrl;
};

/**
 * Fetch balances from Codex API (faster, enriched data)
 */
std::future<CachedWalletResponse["tokens"] | null> fetchFromCodex(const std::string& walletAddress, const std::string& codexKey);

/**
 * Fetch Solana token balances with cached metadata
 * Tries Codex first, falls back to Helius
 */
std::future<void> GET(NextRequest request);
} // namespace elizaos
