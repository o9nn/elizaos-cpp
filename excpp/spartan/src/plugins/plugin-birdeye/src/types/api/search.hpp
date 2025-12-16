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

import type { BirdeyeSupportedChain } from "../shared";

// Search Types
struct TokenMarketSearchParams {
    std::optional<BirdeyeSupportedChain | "all"> chain;
    std::optional<std::string> keyword;
    std::optional<"token" | "market" | "all"> target;
    std::optional<"asc" | "desc"> sort_type;
    std::optional<bool> verify_token;
    std::optional<std::string> markets;
    std::optional<double> offset;
    std::optional<double> limit;
};


struct TokenMarketSearchResponse {
    bool success;
    { data;
    Array<{ items;
    std::optional<"token" | "market"> type;
    std::optional<std::vector<TokenResult | MarketResult>> result;
};


struct TokenResult {
    std::optional<std::string> name;
    std::optional<std::string> symbol;
    std::optional<std::string> address;
    std::optional<std::string> network;
    std::optional<double> fdv;
    std::optional<double> market_cap;
    std::optional<double> liquidity;
    std::optional<double> volume_24h_change_percent;
    std::optional<double> price;
    std::optional<double> price_change_24h_percent;
    std::optional<double> buy_24h;
    std::optional<double> buy_24h_change_percent;
    std::optional<double> sell_24h;
    std::optional<double> sell_24h_change_percent;
    std::optional<double> trade_24h;
    std::optional<double> trade_24h_change_percent;
    std::optional<double> unique_wallet_24h;
    std::optional<double> unique_view_24h_change_percent;
    std::optional<std::string> last_trade_human_time;
    std::optional<double> last_trade_unix_time;
    std::optional<std::string> creation_time;
    std::optional<double> volume_24h_usd;
    std::optional<std::string> logo_uri;
};


struct MarketResult {
    std::string name;
    std::string address;
    double liquidity;
    std::string source;
    double trade_24h;
    double trade_24h_change_percent;
    double unique_wallet_24h;
    double unique_wallet_24h_change_percent;
    std::string last_trade_human_time;
    double last_trade_unix_time;
    std::string base_mint;
    std::string quote_mint;
    double amount_base;
    number; // Note: typo in API response amout_quote;
    std::string creation_time;
    double volume_24h_usd;
};


} // namespace elizaos
