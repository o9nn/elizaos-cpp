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



struct CachedPrice {
    double priceUsd;
    double cachedAt;
};

// Price cache TTL: 5 minutes

/**
 * Get cached token price
 */
std::future<number | null> getCachedPrice(const std::string& chain, const std::string& address);

/**
 * Set cached token price
 */
std::future<void> setCachedPrice(const std::string& chain, const std::string& address, double priceUsd);

/**
 * Fetch Solana token prices from Jupiter Price API
 * Returns prices for multiple tokens in one call
 */

/**
 * CoinGecko platform IDs
 */

/**
 * Fetch EVM token prices from CoinGecko
 */

/**
 * GET /api/token-prices?chain=solana&addresses=mint1,mint2
 * Returns cached prices with 5-minute TTL
 */
std::future<void> GET(NextRequest request);

} // namespace elizaos
