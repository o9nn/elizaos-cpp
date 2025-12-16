#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "codex.hpp"
#include "db.hpp"
#include "mcap.hpp"
#include "redis.hpp"
#include "util.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define interface for the API response types
struct DexScreenerPair {
    std::string pairAddress;
    std::string priceUsd;
    std::string usd;
};

struct DexScreenerResponse {
    std::optional<std::vector<DexScreenerPair>> pairs;
};

struct PriceCandle {
    double time;
    std::string open;
    std::string high;
    std::string low;
    std::string close;
    std::string volume;
};

struct ChartResponse {
    std::optional<std::vector<PriceCandle>> priceCandles;
};

// For devnet testing - placeholder token address for locked tokens since there are none in devnet

struct PriceFeedInfo {
    double price;
    Date timestamp;
    double volume;
    std::optional<double> priceUsd;
};

using CandlePrice = {

std::future<void> getLatestCandle(const std::string& tokenMint, const std::any& swap, std::optional<std::any> tokenInfo);

std::future<void> fetchPriceChartData(double start, double end, double range, const std::string& tokenMint);

      // default candle period

  // Convert price feed to candle price data

  // Keep track of the last valid candle to fill gaps

      // Break new candle data starts

      // We have data for this time period
      // No data for this time period, but we have a previous candle
      // Fill with the last known price (close price of the last candle)
    // If we don't have a lastValidCandle, we skip this period (no data yet)

/**
 * Fetch price chart data for locked tokens using Codex API
 * @param token The token mint address
 * @param start Start time in milliseconds
 * @param end End time in milliseconds
 * @param range Time range in minutes for each candle
 * @param env Cloudflare worker environment
 * @returns Array of OHLC candle data
 */

// Define interface for candle data
struct Candle {
    double open;
    double high;
    double low;
    double close;
    double volume;
    double time;
};

/**
 * Group candles by the specified time range
 * @param candles Original candle data
 * @param rangeMinutes Time range in minutes
 * @returns Grouped candle data
 */
std::vector<Candle> groupCandlesByRange(const std::vector<Candle>& candles, double rangeMinutes);

/**
 * Create a single candle from a group of candles
 * @param group Group of candles
 * @param rangeStart Start time for the group
 * @returns Consolidated candle
 */
Candle createCandleFromGroup(const std::vector<Candle>& group, double rangeStart);

} // namespace elizaos
