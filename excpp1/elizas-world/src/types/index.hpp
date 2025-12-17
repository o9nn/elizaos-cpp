#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TokenAmount {
    std::string amount;
    double decimals;
    double uiAmount;
};

struct ParsedTokenAccount {
    std::string mint;
    TokenAmount tokenAmount;
};

struct TokenBalance {
    std::string mint;
    double uiAmount;
    double decimals;
    std::string amount;
};

struct Social {
    std::string platform;
    std::string handle;
};

struct TokenInfo {
    std::optional<std::string> imageUrl;
    std::string type;
    std::string url;
};

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
    double h24;
    double h24;
    double pairCreatedAt;
    std::optional<double> score;
};

struct TokenHolding {
    std::string address;
    double balance;
    double decimals;
    double usdValue;
    double percentageOwned;
    double firstReceived;
    std::string address;
    std::string name;
    std::string symbol;
    std::string priceUsd;
    double m5;
    double h1;
    double h6;
    double h24;
    std::optional<double> fdv;
    double h24;
    double h24;
    std::optional<TokenInfo> info;
    double pairCreatedAt;
    std::optional<double> score;
};

struct TokenData {
    std::vector<TokenHolding> holdings;
    double totalValue;
    double totalHoldings;
    double significantPositions;
    double averagePosition;
    double topHoldingsValue;
    double topHoldingsPercentage;
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

struct DexScreenerResponse {
    std::string address;
    std::string name;
    std::string symbol;
    std::string priceUsd;
    double m5;
    double h1;
    double h6;
    double h24;
    std::optional<double> fdv;
    double h24;
    double h24;
    std::optional<TokenInfo> info;
    double pairCreatedAt;
};


} // namespace elizaos
