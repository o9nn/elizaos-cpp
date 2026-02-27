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
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Token Resolution Strategy (in priority order):
 * 
 * 1. **Hardcoded Addresses**: Fastest, most reliable for common tokens on each network
 *    - Used for tokens that may not be in CoinGecko yet (e.g., new chains like Base)
 *    - Verified addresses from official bridge/chain documentation
 * 
 * 2. **Known Coin IDs**: Well-known tokens resolved via CoinGecko coin ID
 *    - More reliable than search API
 *    - Direct lookup using stable coin identifiers
 * 
 * 3. **CoinGecko Search**: Fallback for unknown/new tokens
 *    - Search by symbol, then fetch platform address
 *    - May not find newer tokens or return multiple results
 * 
 * This multi-tier approach ensures maximum reliability for common tokens
 * while still supporting discovery of new tokens via CoinGecko.
 */

/**
 * Token metadata from CoinGecko
 */
struct TokenMetadata {
    std: symbol;
    std: name;
    std: address;
    double decimals;
    std: network;
};

/**
 * CoinGecko API response types
 */
struct CoinGeckoTokenResponse {
    std::optional<std:> symbol;
    std::optional<std:> name;
};

struct CoinGeckoSearchCoin {
    std: id;
    std: symbol;
    std: name;
};

struct CoinGeckoSearchResponse {
    std::optional<std::vector<CoinGeckoSearchCoin>> coins;
};

struct CoinGeckoCoinDetailResponse {
};

/**
 * CoinGecko platform IDs mapping from CDP network names
 */

/**
 * Hardcoded token addresses for networks
 * Fallback when CoinGecko doesn't have the data yet (e.g., new chains)
 * Format: network -> symbol -> address
 */

/**
 * In-memory cache for token metadata to avoid rate limits
 * Cache key format: "network:address"
 */

/**
 * Cache expiry time (5 minutes)
 */

/**
 * Get CoinGecko platform ID from network name
 */
std: getPlatformId(const std:& network);

/**
 * Get cache key for token
 */
std: getCacheKey(const std:& network, const std:& address);

/**
 * Check if cache entry is still valid
 */
bool isCacheValid(const std:& key);

/**
 * Fetch token metadata from CoinGecko API
 * 
 * @param address - Token contract address
 * @param network - Network name (e.g., "base", "ethereum")
 * @returns Token metadata or null if not found
 */

/**
 * Resolve token symbol to address for a given network
 * This uses CoinGecko's search API to find the token
 * 
 * @param symbol - Token symbol (e.g., "USDC", "WETH")
 * @param network - Network name
 * @returns Token address or null if not found
 */
std::future<std:> resolveTokenSymbol(const std:& symbol, const std:& network);

/**
 * Resolve token to address
 * Handles both symbols (e.g., "USDC") and addresses (0x...)
 * For native tokens (ETH/MATIC), returns zero address
 *
 * IMPORTANT: Always validates addresses with CoinGecko to prevent fake/invalid addresses.
 */
std::future<"0x> resolveTokenToAddress(const std:& token, const std:& network);" | null> {

  // For native tokens

  // If it looks like an address, validate it with CoinGecko to prevent fake addresses

  // Try to resolve symbol to address via CoinGecko

/**
 * Get token decimals (with fallback to common values)
 * 
 * @param address - Token address
 * @param network - Network name
 * @returns Number of decimals (defaults to 18 if not found)
 */
std::future<double> getTokenDecimals(const std:& address, const std:& network);

/**
 * Clear the token metadata cache
 */
void clearTokenCache();

/**
 * Get cache statistics
 */

/**
 * Add a hardcoded token address for a specific network
 * Useful for tokens not yet in CoinGecko or for faster resolution
 * 
 * @param network - Network name
 * @param symbol - Token symbol (will be converted to lowercase)
 * @param address - Token contract address
 */
void addHardcodedTokenAddress(const std:& network, const std:& symbol, const std:& address);

/**
 * Get hardcoded token addresses for a network
 */
std::unordered_map<std:, std:> getHardcodedTokens(const std:& network);


} // namespace elizaos
