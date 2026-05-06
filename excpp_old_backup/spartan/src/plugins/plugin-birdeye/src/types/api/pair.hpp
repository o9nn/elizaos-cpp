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


// Pair Trades Types
struct PairTradesParams {
    std::string pair;
    std::optional<double> limit;
    std::optional<double> offset;
};

struct PairTradesResponse {
    bool success;
    { data;
    Array<{ items;
    std::optional<std::string> signature;
    std::optional<double> blockNumber;
    std::optional<double> unixTime;
    std::optional<"buy" | "sell"> type;
    std::optional<std::string> tokenAddress;
    std::optional<double> tokenAmount;
    std::optional<double> tokenAmountUI;
    std::optional<std::string> tokenSymbol;
    std::optional<double> tokenDecimals;
    std::optional<double> priceUsd;
    std::optional<double> volumeUsd;
    std::optional<std::string> maker;
    std::optional<std::string> taker;
    std::optional<std::string> txType;
    std::optional<std::string> poolAddress;
    std::optional<std::string> poolName;
    std::optional<std::string> dex;
};

// OHLCV Pair Types
struct OHLCVPairParams {
    std::string address;
    std::optional<TimeInterval> type;
    std::optional<double> time_from;
    std::optional<double> time_to;
};

struct OHLCVPairResponse {
    bool success;
    { data;
    Array<{ items;
    std::optional<double> unixTime;
    std::optional<std::string> address;
    std::optional<TimeInterval> type;
    std::optional<double> o;
    std::optional<double> h;
    std::optional<double> l;
    std::optional<double> c;
    std::optional<double> v;
};

// Pair Overview Types
struct PairOverviewMultiParams {
    std::string list_address;
    std::optional<double> before_time;
};

struct PairOverviewSingleParams {
    std::string address;
};

struct PairOverviewData {
    std::string address;
    std::string name;
    { base;
    std::string address;
    double decimals;
    std::string icon;
    std::string symbol;
    { quote;
    std::string address;
    double decimals;
    std::string icon;
    std::string symbol;
    std::string created_at;
    std::string source;
    double liquidity;
    number | null liquidity_change_percentage_24h;
    double price;
    double volume_24h;
    number | null volume_24h_change_percentage_24h;
    double trade_24h;
    double trade_24h_change_percent;
    double unique_wallet_24h;
    number | null unique_wallet_24h_change_percent;
    double trade_30m;
    double trade_1h;
    double trade_2h;
    double trade_4h;
    double trade_8h;
    double trade_12h;
    double trade_30m_change_percent;
    double trade_1h_change_percent;
    double trade_2h_change_percent;
    double trade_4h_change_percent;
    double trade_8h_change_percent;
    double trade_12h_change_percent;
    double volume_30m;
    double volume_1h;
    double volume_2h;
    double volume_4h;
    double volume_8h;
    double volume_12h;
    double volume_30m_quote;
    double volume_1h_quote;
    double volume_2h_quote;
    double volume_4h_quote;
    double volume_8h_quote;
    double volume_12h_quote;
    double volume_30m_base;
    double volume_1h_base;
    double volume_2h_base;
    double volume_4h_base;
    double volume_8h_base;
    double volume_12h_base;
};

struct PairOverviewSingleResponse {
    bool success;
    { data;
    std::optional<std::string> address;
    std::optional<std::string> name;
    std::optional<{> base;
    std::optional<std::string> address;
    std::optional<double> decimals;
    std::optional<std::string> icon;
    std::optional<std::string> symbol;
    std::optional<{> quote;
    std::optional<std::string> address;
    std::optional<double> decimals;
    std::optional<std::string> icon;
    std::optional<std::string> symbol;
    std::optional<std::string> created_at;
    std::optional<std::string> source;
    std::optional<double> liquidity;
    std::optional<number | null> liquidity_change_percentage_24h;
    std::optional<double> price;
    std::optional<double> volume_24h;
    std::optional<number | null> volume_24h_change_percentage_24h;
    std::optional<double> trade_24h;
    std::optional<double> trade_24h_change_percent;
    std::optional<double> unique_wallet_24h;
    std::optional<number | null> unique_wallet_24h_change_percent;
    std::optional<double> trade_30m;
    std::optional<double> trade_1h;
    std::optional<double> trade_2h;
    std::optional<double> trade_4h;
    std::optional<double> trade_8h;
    std::optional<double> trade_12h;
    std::optional<double> trade_30m_change_percent;
    std::optional<double> trade_1h_change_percent;
    std::optional<double> trade_2h_change_percent;
    std::optional<double> trade_4h_change_percent;
    std::optional<double> trade_8h_change_percent;
    std::optional<double> trade_12h_change_percent;
    std::optional<double> volume_30m;
    std::optional<double> volume_1h;
    std::optional<double> volume_2h;
    std::optional<double> volume_4h;
    std::optional<double> volume_8h;
    std::optional<double> volume_12h;
    std::optional<double> volume_30m_quote;
    std::optional<double> volume_1h_quote;
    std::optional<double> volume_2h_quote;
    std::optional<double> volume_4h_quote;
    std::optional<double> volume_8h_quote;
    std::optional<double> volume_12h_quote;
    std::optional<double> volume_30m_base;
    std::optional<double> volume_1h_base;
    std::optional<double> volume_2h_base;
    std::optional<double> volume_4h_base;
    std::optional<double> volume_8h_base;
    std::optional<double> volume_12h_base;
};

struct PairOverviewMultiResponse {
    bool success;
    { data;
};


} // namespace elizaos
