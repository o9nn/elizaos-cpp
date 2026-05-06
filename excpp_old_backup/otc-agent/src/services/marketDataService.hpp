#include "database.hpp"
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



struct CoinGeckoPrice {
    double usd;
    double usd_market_cap;
    double usd_24h_vol;
    double usd_24h_change;
};

struct BirdeyeResponse {
    { data;
    double value;
    double updateUnixTime;
    double liquidity;
    double volume24h;
    double priceChange24hPercent;
};

class MarketDataService {
  private coingeckoApiKey?: string;
  private birdeyeApiKey?: string;

  constructor() {
    this.coingeckoApiKey = process.env.COINGECKO_API_KEY;
    this.birdeyeApiKey = process.env.BIRDEYE_API_KEY;
  }

    // Map chain to CoinGecko platform ID

    // Solana addresses are Base58 encoded and case-sensitive - preserve original case
    // In local development without Birdeye, return mock data
    // The actual price comes from on-chain (desk.token_usd_price_8d)

        // Return placeholder data for localnet - actual price from on-chain


} // namespace elizaos
