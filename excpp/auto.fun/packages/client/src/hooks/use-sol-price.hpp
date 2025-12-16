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

struct SolPriceResponse {
    double price;
};


struct CoinGeckoResponse {
    { solana;
    double usd;
};


struct BinanceResponse {
    std::string price;
};


/**
 * Fetches the current SOL price in USD
 * Uses multiple sources with fallbacks (CoinGecko, Binance)
 */
const fetchSolPrice = async (): Promise<number> => {
  try {
    // First try the backend API endpoint
    try {
      const response = await fetch(`${env.apiUrl}/api/sol-price`);
      if (response.ok) {
        const data = (await response.json()) as SolPriceResponse;
        if (data && data.price) {
          return Number(data.price);
        }
      }
    } catch (error) {
      console.error("Error fetching SOL price from API:", error);
    }

    // If API fails, try CoinGecko
    try {
      const response = await fetch(
        "https://api.coingecko.com/api/v3/simple/price?ids=solana&vs_currencies=usd",
      );
      const data = (await response.json()) as CoinGeckoResponse;
      if (data && data.solana && data.solana.usd) {
        return Number(data.solana.usd);
      }
    } catch (error) {
      console.error("Error fetching SOL price from CoinGecko:", error);
    }

    // If CoinGecko fails, try Binance
    try {
      const response = await fetch(
        "https://api.binance.com/api/v3/ticker/price?symbol=SOLUSDT",
      );
      const data = (await response.json()) as BinanceResponse;
      if (data && data.price) {
        return Number(data.price);
      }
    } catch (error) {
      console.error("Error fetching SOL price from Binance:", error);
    }

    // Fallback to a default value if all else fails
    return 135.0;
  } catch (error) {
    console.error("Error fetching SOL price:", error);
    return 135.0; // Fallback price
  }
};

/**
 * React hook to get and cache the current SOL price in USD
 * The price is automatically refreshed every minute
 */
);
}

} // namespace elizaos
