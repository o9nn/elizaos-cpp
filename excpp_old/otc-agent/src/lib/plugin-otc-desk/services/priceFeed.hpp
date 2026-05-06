#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Price feed service for fetching real-time token prices
// For multi-token support, use MarketDataService

struct PriceCache {
    double price;
    double timestamp;
};

/**
 * Get cached price from runtime storage
 */

/**
 * Set cached price in runtime storage
 */
std::future<void> setCachedPrice(const std::string& key, PriceCache value);

/**
 * Get ETH price in USD
 */
std::future<double> getEthPriceUsd();

/**
 * Get SOL price in USD
 */
std::future<double> getSolPriceUsd();

/**
 * Format token amount with proper display (K, M, B suffixes)
 */
std::string formatTokenAmount(const std::variant<std::string, double>& amount);

} // namespace elizaos
