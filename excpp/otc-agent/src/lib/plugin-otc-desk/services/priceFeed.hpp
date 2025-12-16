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

// Price feed service for fetching real-time token prices
// For multi-token support, use MarketDataService

struct PriceCache {
    double price;
    double timestamp;
};


const CACHE_TTL = 60 * 1000; // 60 seconds

/**
 * Get cached price from runtime storage
 */
std::future<PriceCache | null> getCachedPrice(const std::string& key); = await import("../../agent-runtime");
  const runtime = await agentRuntime.getRuntime();
  return (await runtime.getCache<PriceCache>(`price:${key}`)) ?? null;
}

/**
 * Set cached price in runtime storage
 */
std::future<void> setCachedPrice(const std::string& key, PriceCache value); = await import("../../agent-runtime");
  const runtime = await agentRuntime.getRuntime();
  await runtime.setCache(`price:${key}`, value);
}

/**
 * Get ETH price in USD
 */
std::future<double> getEthPriceUsd();

  // Fetch from CoinGecko
  const response = await fetch(
    "https://api.coingecko.com/api/v3/simple/price?ids=ethereum&vs_currencies=usd",
    {
      headers: {
        Accept: "application/json",
      },
    },
  );

  if (response.ok) {
    const data = await response.json();
    const price = data.ethereum?.usd;

    if (typeof price === "number") {
      await setCachedPrice(cacheKey, {
        price,
        timestamp: Date.now(),
      });
      return price;
    }
  }

  throw new Error("Failed to fetch ETH price");
}

/**
 * Get SOL price in USD
 */
std::future<double> getSolPriceUsd();

  // Fetch from CoinGecko
  const response = await fetch(
    "https://api.coingecko.com/api/v3/simple/price?ids=solana&vs_currencies=usd",
    {
      headers: {
        Accept: "application/json",
      },
    },
  );

  if (response.ok) {
    const data = await response.json();
    const price = data.solana?.usd;

    if (typeof price === "number") {
      await setCachedPrice(cacheKey, {
        price,
        timestamp: Date.now(),
      });
      return price;
    }
  }

  throw new Error("Failed to fetch SOL price");
}

/**
 * Format token amount with proper display (K, M, B suffixes)
 */
B`;
  } else if (num >= 1000000) {
    return `${(num / 1000000).toFixed(2)}M`;
  } else if (num >= 1000) {
    return `${(num / 1000).toFixed(2)}K`;
  } else {
    return num.toLocaleString();
  }
}

} // namespace elizaos
