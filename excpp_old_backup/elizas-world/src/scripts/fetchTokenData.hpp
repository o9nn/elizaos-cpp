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



struct TokenAmount {
    std::string amount;
    double decimals;
    double uiAmount;
};

struct ParsedTokenAccount {
    { data;
    { parsed;
    { info;
    std::string mint;
    TokenAmount tokenAmount;
};

struct Social {
    std::string platform;
    std::string handle;
};

struct TokenInfo {
    std::optional<std::string> imageUrl;
    std::optional<std::vector<{ url: string }>> websites;

struct BaseToken {
    std::string address;
    std::string name;
    std::string symbol;
};

struct Liquidity {
    double usd;
    double base;
    double quote;
};

struct TokenPair {
    std::string chainId;
    std::string dexId;
    std::string url;
    std::string pairAddress;
    BaseToken baseToken;
    BaseToken quoteToken;
    std::string priceUsd;
    Liquidity liquidity;
    std::optional<double> fdv;
    std::optional<double> marketCap;
    std::optional<TokenInfo> info;
};

struct DexScreenerResponse {
    std::string schemaVersion;
    std::vector<TokenPair> pairs;
};

struct TokenHolding {
    std::string address;
    double balance;
    double decimals;
    double usdValue;
    double percentageOwned;
    TokenPair marketData;
};

struct TimeSeriesPoint {
    double timestamp;
    double price;
    double volume;
};

struct TokenAnalysis {
    double priceChange24h;
    std::optional<double> priceChange7d;
    double volumeChange24h;
    double volumeAvg24h;
    double txCount24h;
    std::optional<double> holders;
    std::vector<TimeSeriesPoint> timeSeries;
};

std::future<DexScreenerResponse> fetchDexScreenerData(const std::vector<std::string>& tokenAddresses);

std::future<void> getTokenBalances(Connection connection, PublicKey walletPubkey);

std::future<TokenAnalysis | null> fetchTokenAnalysis(const std::string& address);

std::future<void> main();

} // namespace elizaos
