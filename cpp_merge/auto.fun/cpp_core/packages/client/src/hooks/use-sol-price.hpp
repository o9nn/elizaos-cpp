#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SolPriceResponse {
    double price;
};

struct CoinGeckoResponse {
    double usd;
};

struct BinanceResponse {
    std::string price;
};

/**
 * Fetches the current SOL price in USD
 * Uses multiple sources with fallbacks (CoinGecko, Binance)
 */
    // First try the backend API endpoint
          return Number(data.price);

    // If API fails, try CoinGecko
        return Number(data.solana.usd);

    // If CoinGecko fails, try Binance
        return Number(data.price);

    // Fallback to a default value if all else fails

/**
 * React hook to get and cache the current SOL price in USD
 * The price is automatically refreshed every minute
 */
void useSolPrice();

} // namespace elizaos
