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
#include "retry-cache.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct PriceValidationResult {
    bool valid;
    std::optional<std::string> warning;
    std::optional<double> aggregatedPrice;
    std::optional<double> poolPrice;
    std::optional<double> divergencePercent;
};

struct CoinGeckoPriceResponse {
    std::optional<double> usd;
};

// CoinGecko platform IDs for supported chains
// See: https://www.coingecko.com/en/api/documentation (asset_platforms endpoint)

// Cache TTL for CoinGecko prices (30 seconds)

/**
 * Check if the pool price diverges significantly (>10%) from the aggregated off-chain price.
 * Uses CoinGecko public API with exponential retry and 30-second caching.
 */
std::future<PriceValidationResult> checkPriceDivergence(const std::string& tokenAddress, Chain chain, double poolPriceUsd);

} // namespace elizaos
