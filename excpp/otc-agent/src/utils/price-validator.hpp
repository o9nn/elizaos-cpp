#include "retry-cache.hpp"
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

;
;

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
const COINGECKO_CHAIN_MAP: Record<string, string> = {
  base: "base",
  solana: "solana",
  bsc: "binance-smart-chain",
  ethereum: "ethereum",
};

// Cache TTL for CoinGecko prices (30 seconds)
const COINGECKO_CACHE_TTL_MS = 30_000;

/**
 * Check if the pool price diverges significantly (>10%) from the aggregated off-chain price.
 * Uses CoinGecko public API with exponential retry and 30-second caching.
 */
std::future<PriceValidationResult> checkPriceDivergence(const std::string& tokenAddress, Chain chain, double poolPriceUsd);; // Cannot validate if pool price is invalid
  }

  const platformId = COINGECKO_CHAIN_MAP[chain];
  if (!platformId) {
    return { valid: true }; // Unsupported chain for validation
  }

  try {
    // Fetch price from CoinGecko with retry and caching
    const url = `https://api.coingecko.com/api/v3/simple/token_price/${platformId}?contract_addresses=${tokenAddress}&vs_currencies=usd`;
    const cacheKey = `coingecko:${platformId}:${tokenAddress.toLowerCase()}`;

    const data = await fetchJsonWithRetryAndCache<CoinGeckoPriceResponse>(
      url,
      { headers: { Accept: "application/json" } },
      {
        cacheTtlMs: COINGECKO_CACHE_TTL_MS,
        cacheKey,
        maxRetries: 3,
      },
    );

    const tokenData = data[tokenAddress.toLowerCase()];

    if (!tokenData || !tokenData.usd) {
      return { valid: true }; // No price found off-chain
    }

    const aggregatedPrice = tokenData.usd;

    // Calculate divergence
    const diff = Math.abs(poolPriceUsd - aggregatedPrice);
    const divergence = diff / aggregatedPrice;
    const divergencePercent = divergence * 100;

    if (divergencePercent > 10) {
      return {
        valid: false,
        warning: `Price Warning: Pool price ($${poolPriceUsd.toFixed(4)}) diverges by ${divergencePercent.toFixed(1)}% from market price ($${aggregatedPrice.toFixed(4)}).`,
        aggregatedPrice,
        poolPrice: poolPriceUsd,
        divergencePercent,
      };
    }

    return {
      valid: true,
      aggregatedPrice,
      poolPrice: poolPriceUsd,
      divergencePercent,
    };
  } catch (error) {
    console.warn("Price validation failed:", error);
    return { valid: true }; // Fail open
  }
}

} // namespace elizaos
