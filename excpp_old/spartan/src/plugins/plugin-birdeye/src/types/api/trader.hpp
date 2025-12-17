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

// Trader Gainers Losers Types
struct GainersLosersParams {
    std::variant<"yesterday", "today", "1W"> type;
    "PnL" sort_by;
    std::variant<"asc", "desc"> sort_type;
    std::optional<double> offset;
    std::optional<double> limit;
};

struct GainersLosersResponse {
    bool success;
    std::optional<std::string> network;
    std::optional<std::string> address;
    std::optional<double> pnl;
    std::optional<double> trade_count;
    std::optional<double> volume;
};

// Trader Transactions Seek Types
struct TraderTransactionsSeekParams {
    std::string address;
    std::optional<double> offset;
    std::optional<double> limit;
    std::optional<std::variant<"swap", "add", "remove", "all">> tx_type;
    std::optional<double> before_time;
    std::optional<double> after_time;
};

struct TraderTransactionsSeekResponse {
    bool success;
    std::optional<std::string> symbol;
    std::optional<double> decimals;
    std::optional<std::string> address;
    std::optional<double> amount;
    std::optional<std::string> type;
    std::optional<std::variant<"from", "to">> type_swap;
    std::optional<double> ui_amount;
    std::optional<std::optional<double>> price;
    std::optional<double> nearest_price;
    std::optional<double> change_amount;
    std::optional<double> ui_change_amount;
    std::optional<std::string> symbol;
    std::optional<double> decimals;
    std::optional<std::string> address;
    std::optional<double> amount;
    std::optional<std::string> type;
    std::optional<std::variant<"from", "to">> type_swap;
    std::optional<std::optional<std::any>> fee_info;
    std::optional<double> ui_amount;
    std::optional<std::optional<double>> price;
    std::optional<double> nearest_price;
    std::optional<double> change_amount;
    std::optional<double> ui_change_amount;
    std::optional<std::optional<double>> base_price;
    std::optional<std::optional<double>> quote_price;
    std::optional<std::string> tx_hash;
    std::optional<std::string> source;
    std::optional<double> block_unix_time;
    std::optional<std::string> tx_type;
    std::optional<std::string> address;
    std::optional<std::string> owner;
    std::optional<bool> hasNext;
};


} // namespace elizaos
