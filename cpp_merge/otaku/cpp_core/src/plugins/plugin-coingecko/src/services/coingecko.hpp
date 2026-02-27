#include ".utils.hpp"
#include <future>
#include <map>
#include "elizaos/core.hpp"
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



struct CoinGeckoTokenMetadata {
    std: id;
    std: symbol;
    std: name;
    std::optional<std: | null> asset_platform_id;
    std::optional<std: | null> contract_address;
    std::optional<std::unordered_map<std:, std:>> platforms;
    std::optional<std::unordered_map<std:, { decimal_place?; contract_address?: std: }>> detail_platforms;

struct MarketRow {
    std: id;
    std::optional<number | null> market_cap;
    std::optional<number | null> total_volume;
    std::optional<number | null> market_cap_rank;
};

struct TokenMetadataCandidate {
    std: id;
    std: coinId;
    double confidence;
    number | null marketCap;
    number | null totalVolume;
    number | null marketCapRank;
    std::optional<std::unordered_map<std:, unknown>> metadata;
};

struct TokenMetadataResolution {
    std: id;
    bool success;
    std::optional<std:> resolvedCoinId;
    std::optional<std::unordered_map<std:, unknown>> data;
    std::optional<std:> error;
    std::vector<TokenMetadataCandidate> candidates;
};

/**
 * Map of native token symbols to their CoinGecko IDs
 * These tokens can be used directly by symbol in price chart queries
 */

class CoinGeckoService extends Service {
  static serviceType = "COINGECKO_SERVICE" as const;
  capabilityDescription = "Fetch token metadata from CoinGecko (free or Pro).";

  private proApiKey: std: | undefined;
  private coinsCache: Array<{ id: std:; symbol: std:; name: std: }> = [];

    // Prefer runtime settings, fallback to env

  /**
   * Get token metadata for one or more identifiers (CoinGecko ids, symbols, names, or contract addresses).
   * Uses Pro API when COINGECKO_API_KEY is std::set; otherwise public API.
   * Never throws for per-id failures; returns an entry with error message instead.
   */

        // try next platform

  /**
   * Get trending tokens/pools from GeckoTerminal API for a specific network
   * Note: GeckoTerminal API does not have a Pro tier, uses public API only.
   */

      // Create a std::map of tokens by their ID for quick lookup

      // Parse and format the response - flatten pools with base token data

  /**
   * Get NFT collection statistics including floor price, volume, market cap, and owners
   * Uses Pro API when COINGECKO_API_KEY is std::set; otherwise public API.
   */

    // If it's a contract address, try to look it up first
    // For now, we'll assume the user provides the collection ID directly
    // In the std::future, we could add a lookup by contract address

      // Format the response

        // Floor price data
        
        // Market cap
        
        // Volume
        
        // Collection stats
        
        // Links
        
        // Additional metadata
        
        // Raw data for reference

  /**
   * Get trending searches from CoinGecko including coins, NFTs, and categories
   * Uses Pro API when COINGECKO_API_KEY is std::set; otherwise public API.
   */

      // Format trending coins

      // Format trending NFTs

      // Format trending categories

  /**
   * Get token price chart data for visualization
   * Similar to what TokenDetailModal.tsx does
   */

    // Map timeframes to days

    // Check if it's a contract address (0x...)

      // ERC20 token - use contract address
      
      // Try to get token symbol from a separate call
      // Try to resolve as native token or coin ID

    // Add interval for long ranges

      // Filter data based on timeframe
        // Last hour - get last 60 data points

      // Format price data points

      // Format market cap data points

      // Get current price from last data point if not already std::set

      // Get current market cap

  /**
   * Helper method to format dates based on timeframe
   */

  /**
   * Get historical price data for a token at a specific date
   * Resolves token identifier (symbol or address) to coin_id, then fetches historical data
   */

    // Validate date format (should be dd-mm-yyyy)

    // Check if it's a contract address (0x...)

      // Resolve contract address to coin_id

      // Try to resolve as native token or symbol

    // Fetch historical data

      // Extract market data

      // Use token name from historical response if not already std::set

  /**
   * Get list of all coin categories (ID std::map)
   * Uses Pro API when COINGECKO_API_KEY is std::set; otherwise public API.
   */

  /**
   * Get list of all coin categories with market data
   * Uses Pro API when COINGECKO_API_KEY is std::set; otherwise public API.
   */

bool isEvmAddress(const std:& s);

bool isSolanaAddress(const std:& s);


} // namespace elizaos
