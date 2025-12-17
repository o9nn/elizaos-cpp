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



struct TokenBalance {
    std::string contractAddress;
    std::string symbol;
    std::string name;
    double decimals;
    std::string balance;
    std::optional<std::string> logoUrl;
    std::optional<double> priceUsd;
    std::optional<double> balanceUsd;
};

// Metadata cache (permanent - token metadata doesn't change)
struct CachedTokenMetadata {
    std::string symbol;
    std::string name;
    double decimals;
    std::optional<std::string> logoUrl;
};

// Bulk metadata cache - stores all known metadata per chain in one key
struct BulkMetadataCache {
};

std::future<void> setBulkMetadataCache(const std::string& chain, const std::unordered_map<std::string, CachedTokenMetadata>& metadata);

// Price cache TTL: 15 minutes

// Wallet balance cache TTL: 15 minutes

struct CachedWalletBalances {
    std::vector<TokenBalance> tokens;
    double cachedAt;
};

std::future<void> setCachedWalletBalances(const std::string& chain, const std::string& address, const std::vector<TokenBalance>& tokens);

/**
 * Cache an image URL to Vercel Blob storage
 * Returns the cached blob URL, or null if caching fails
 */
std::future<std::string> cacheImageToBlob(const std::string& imageUrl);

std::string getExtensionFromUrl(const std::string& url);

// Minimum thresholds to filter obvious dust
// Very permissive - we want to show new tokens without prices

// Chain configs

// Bulk price cache - stores all prices per chain in one key
struct BulkPriceCache {
    double cachedAt;
};

std::future<void> setBulkPriceCache(const std::string& chain, const std::unordered_map<std::string, double>& prices);

/**
 * Fetch token balances using Alchemy's getTokenBalances + cached metadata
 */

/**
 * Fetch prices from DeFiLlama (free, good coverage)
 */

/**
 * Fetch prices from CoinGecko (fallback)
 */

/**
 * Fetch prices - try DeFiLlama first, then CoinGecko
 */

/**
 * GET /api/evm-balances?address=0x...&chain=base&refresh=true
 */
std::future<void> GET(NextRequest request);

} // namespace elizaos
