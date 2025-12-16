#include "cache.hpp"
#include "raydium.hpp"
#include "tokenSupplyHelpers.hpp"
#include "util.hpp"
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



// Constants

// Monitoring metrics

/**
 * Get the current SOL price in USD
 * Prioritizes cache, then Pyth, then fallback APIs
 */
std::future<double> getSOLPrice();

/**
 * Fetch SOL/USD price directly from Pyth Network
 */
std::future<double> fetchSOLPriceFromPyth();

/**
 * Calculate token market data using SOL price
 */
std::future<std::any> calculateTokenMarketData(const std::any& token, double solPrice);

std::future<void> calculateRaydiumTokenMarketData(const std::any& token);

// Export metrics for monitoring - simplified without queue information
void getMarketDataMetrics();

} // namespace elizaos
