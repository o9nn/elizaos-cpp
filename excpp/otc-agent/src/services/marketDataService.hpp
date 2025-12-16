#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "database.hpp"

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
    double value;
    double updateUnixTime;
    double liquidity;
    double volume24h;
    double priceChange24hPercent;
};

class MarketDataService {
public:
    MarketDataService();
    std::future<double> fetchTokenPrice(const std::string& tokenAddress, Chain chain);
    std::future<TokenMarketData> fetchMarketData(const std::string& tokenAddress, Chain chain);
    std::future<TokenMarketData> fetchEVMData(const std::string& tokenAddress, Chain chain);
    std::future<TokenMarketData> fetchSolanaData(const std::string& tokenAddress);
    std::future<void> refreshTokenData(const std::string& tokenId, const std::string& tokenAddress, Chain chain);
    std::future<void> refreshAllTokenData(const std::vector<std::any>& tokens);
};


} // namespace elizaos
