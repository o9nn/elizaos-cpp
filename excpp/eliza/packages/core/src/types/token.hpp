#include "service.hpp"
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



/**
 * A standardized representation of a token holding.
 */
struct TokenBalance {
    string; // Token mint address, or a native identifier like 'SOL' or 'ETH' address;
    string; // Raw balance as a string to handle large numbers with precision balance;
    double decimals;
    std::optional<number; // User-friendly balance, adjusted for decimals> uiAmount;
    std::optional<std::string> name;
    std::optional<std::string> symbol;
    std::optional<std::string> logoURI;
};

/**
 * Generic representation of token data that can be provided by various services.
 */
struct TokenData {
    string; // Unique identifier (e.g., contract address or a composite ID) id;
    std::string symbol;
    std::string name;
    string; // Contract address address;
    string; // e.g., 'solana', 'ethereum', 'base' chain;
    string; // e.g., 'birdeye', 'coinmarketcap' sourceProvider;
    std::optional<double> price;
    std::optional<double> priceChange24hPercent;
    std::optional<number; // Absolute change> priceChange24hUSD;
    std::optional<double> volume24hUSD;
    std::optional<double> marketCapUSD;
    std::optional<double> liquidity;
    std::optional<double> holders;
    std::optional<std::string> logoURI;
    std::optional<double> decimals;
    std::optional<Date; // When this specific data point was last updated from the source> lastUpdatedAt;
    std::optional<std::any> raw;
};

/**
 * Interface for a generic service that provides token data.
 */

  /**
   * Fetches detailed information for a single token.
   * @param address The token's contract address.
   * @param chain The blockchain the token resides on.
   * @returns A Promise resolving to TokenData or null if not found.
   */

  /**
   * Fetches a list of trending tokens.
   * @param chain Optional: Filter by a specific blockchain.
   * @param limit Optional: Number of tokens to return. Defaults to a service-specific value.
   * @param timePeriod Optional: Time period for trending data (e.g., '24h', '7d'). Defaults to service-specific.
   * @returns A Promise resolving to an array of TokenData.
   */

  /**
   * Searches for tokens based on a query string.
   * @param query The search query (e.g., symbol, name, address).
   * @param chain Optional: Filter by a specific blockchain.
   * @param limit Optional: Number of results to return.
   * @returns A Promise resolving to an array of TokenData.
   */

  /**
   * Fetches data for multiple tokens by their addresses on a specific chain.
   * @param addresses Array of token contract addresses.
   * @param chain The blockchain the tokens reside on.
   * @returns A Promise resolving to an array of TokenData. May not include all requested if some are not found.
   */

  // Future potential methods:
  // getHistoricalPriceData(address: string, chain: string, timeFrame: string): Promise<any[]>;
  // getTokenMarketChart(address: string, chain: string, days: number): Promise<any[]>;

} // namespace elizaos
