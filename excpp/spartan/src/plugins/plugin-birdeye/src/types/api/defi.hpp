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

import type { TimeInterval } from "./common";

// Network Types
struct DefiNetworksResponse {
    bool success;
    { data;
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
    { data;
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
    { data;
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
    "token" | "pair" address_type;
    TimeInterval type;
    std::optional<double> time_from;
    std::optional<double> time_to;
};


struct DefiHistoryPriceResponse {
    bool success;
    { data;
    Array<{ items;
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
    { data;
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


// Price Volume Types
struct PriceVolumeParams {
    std::string address;
    std::optional<TimeInterval> type;
};


struct PriceVolumeResponse {
    bool success;
    { data;
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
    { data;
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
    { data;
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
    std::optional<"swap" | "add" | "remove" | "all"> tx_type;
    std::optional<"asc" | "desc"> sort_type;
    std::optional<double> before_time;
    std::optional<double> after_time;
};


struct DefiTradesTokenInfo {
    std::string symbol;
    double decimals;
    std::string address;
    double amount;
    double uiAmount;
    number | null price;
    number | null nearestPrice;
    double changeAmount;
    double uiChangeAmount;
    std::optional<any | null> feeInfo;
};


struct DefiTradesTokenResponse {
    bool success;
    { data;
    Array<{ items;
    std::optional<DefiTradesTokenInfo> quote;
    std::optional<DefiTradesTokenInfo> base;
    std::optional<number | null> basePrice;
    std::optional<number | null> quotePrice;
    std::optional<std::string> txHash;
    std::optional<std::string> source;
    std::optional<double> blockUnixTime;
    std::optional<std::string> txType;
    std::optional<std::string> owner;
    std::optional<std::string> side;
    std::optional<string | null> alias;
    std::optional<double> pricePair;
    std::optional<DefiTradesTokenInfo> from;
    std::optional<DefiTradesTokenInfo> to;
    std::optional<number | null> tokenPrice;
    std::optional<std::string> poolId;
    std::optional<bool> hasNext;
};


} // namespace elizaos
