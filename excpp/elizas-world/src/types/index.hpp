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

struct TokenBalance {
    std::string mint;
    { tokenAmount;
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
    std::optional<std::vector<{ label: string; url: string }>> websites;

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
    std::optional<{> volume;
    double h24;
    std::optional<{> txns;
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
    { marketData;
    { baseToken;
    std::string address;
    std::string name;
    std::string symbol;
    std::string priceUsd;
    { priceChange;
    double m5;
    double h1;
    double h6;
    double h24;
    std::optional<double> fdv;
    std::optional<{> volume;
    double h24;
    std::optional<{> txns;
    double h24;
    std::optional<TokenInfo> info;
    double pairCreatedAt;
    std::optional<double> score;
};

struct TokenData {
    std::vector<TokenHolding> holdings;
    { summary;
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
    Array<{ pairs;
    { baseToken;
    std::string address;
    std::string name;
    std::string symbol;
    std::string priceUsd;
    { priceChange;
    double m5;
    double h1;
    double h6;
    double h24;
    std::optional<double> fdv;
    std::optional<{> volume;
    double h24;
    std::optional<{> txns;
    double h24;
    std::optional<TokenInfo> info;
    double pairCreatedAt;
};

type {
    TokenAmount,
    ParsedTokenAccount,
    TokenBalance,
    Social,
    TokenInfo,
    BaseToken,
    Liquidity,
    TokenPair,
    TokenHolding,
    TokenData,
    TimeSeriesPoint,
    TokenAnalysis,
    DexScreenerResponse,
    AlchemyTokenBalance
};
} // namespace elizaos
