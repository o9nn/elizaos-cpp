#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


// Network Types
struct DefiNetworksResponse {
    bool success;
    std::vector<std::string> chains;
};

// Price Types
struct DefiPriceParams {
    std::string address;
    std::optional<double> check_liquidity;
    std::optional<bool> include_liquidity;
};

struct DefiPriceResponse {
    bool success;
    double value;
    std::optional<double> updateUnixTime;
    std::optional<std::string> updateHumanTime;
    std::optional<double> liquidity;
};

// Multi Price Types
struct DefiMultiPriceParams {
    std::string list_address;
    std::optional<double> check_liquidity;
    std::optional<bool> include_liquidity;
};

struct DefiMultiPriceResponse {
    bool success;
    std::optional<double> value;
    std::optional<double> updateUnixTime;
    std::optional<std::string> updateHumanTime;
    std::optional<double> priceChange24h;
};

// Multi Price Types POST
struct DefiMultiPriceParamsPOST {
    std::optional<double> check_liquidity;
    std::optional<bool> include_liquidity;
    std::string list_address;
};

// History Price Types
struct DefiHistoryPriceParams {
    std::string address;
    std::variant<"token", "pair"> address_type;
    TimeInterval type;
    std::optional<double> time_from;
    std::optional<double> time_to;
};

struct DefiHistoryPriceResponse {
    bool success;
    std::optional<double> unixTime;
    std::optional<double> value;
};

// Historical Price Unix Types
struct HistoricalPriceUnixParams {
    std::string address;
    std::optional<double> unixtime;
};

struct HistoricalPriceUnixResponse {
    bool success;
    std::optional<double> value;
    std::optional<double> updateUnixTime;
    std::optional<std::string> priceChange24h;
};

// OHLCV Types
struct OHLCVParams {
    std::string address;
    std::optional<TimeInterval> type;
    std::optional<double> time_from;
    std::optional<double> time_to;
};

struct OHLCVResponse {
    bool success;
    std::optional<double> unixTime;
    std::optional<std::string> address;
    std::optional<TimeInterval> type;
    std::optional<double> o;
    std::optional<double> h;
    std::optional<double> l;
    std::optional<double> c;
    std::optional<double> v;
};

// Price Volume Types
struct PriceVolumeParams {
    std::string address;
    std::optional<TimeInterval> type;
};

struct PriceVolumeResponse {
    bool success;
    std::optional<double> price;
    std::optional<double> updateUnixTime;
    std::optional<std::string> updateHumanTime;
    std::optional<double> volumeUSD;
    std::optional<double> volumeChangePercent;
    std::optional<double> priceChangePercent;
};

// Multi Price Volume Types
struct MultiPriceVolumeParams {
    std::string list_address;
    std::optional<TimeInterval> type;
};

struct MultiPriceVolumeResponse {
    bool success;
    std::optional<double> price;
    std::optional<double> updateUnixTime;
    std::optional<std::string> updateHumanTime;
    std::optional<double> volumeUSD;
    std::optional<double> volumeChangePercent;
    std::optional<double> priceChangePercent;
};

// Base Quote Types
struct BaseQuoteParams {
    std::string base_address;
    std::string quote_address;
    std::optional<TimeInterval> type;
    std::optional<double> time_from;
    std::optional<double> time_to;
};

struct BaseQuoteResponse {
    bool success;
    std::optional<double> unixTime;
    std::optional<double> vBase;
    std::optional<double> vQuote;
    std::optional<double> o;
    std::optional<double> h;
    std::optional<double> l;
    std::optional<double> c;
};

// Token Trades Types
struct DefiTradesTokenParams {
    std::string address;
    std::optional<double> limit;
    std::optional<double> offset;
    std::optional<std::variant<"swap", "add", "remove", "all">> tx_type;
    std::optional<std::variant<"asc", "desc">> sort_type;
    std::optional<double> before_time;
    std::optional<double> after_time;
};

struct DefiTradesTokenInfo {
    std::string symbol;
    double decimals;
    std::string address;
    double amount;
    double uiAmount;
    std::optional<double> price;
    std::optional<double> nearestPrice;
    double changeAmount;
    double uiChangeAmount;
    std::optional<std::optional<std::any>> feeInfo;
};

struct DefiTradesTokenResponse {
    bool success;
    std::optional<DefiTradesTokenInfo> quote;
    std::optional<DefiTradesTokenInfo> base;
    std::optional<std::optional<double>> basePrice;
    std::optional<std::optional<double>> quotePrice;
    std::optional<std::string> txHash;
    std::optional<std::string> source;
    std::optional<double> blockUnixTime;
    std::optional<std::string> txType;
    std::optional<std::string> owner;
    std::optional<std::string> side;
    std::optional<std::string> alias;
    std::optional<double> pricePair;
    std::optional<DefiTradesTokenInfo> from;
    std::optional<DefiTradesTokenInfo> to;
    std::optional<std::optional<double>> tokenPrice;
    std::optional<std::string> poolId;
    std::optional<bool> hasNext;
};


} // namespace elizaos
