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

// pages/api/daoHoldings.ts

struct TokenInfo {
    std::string name;
    std::string symbol;
    double balance;
    double decimals;
    std::optional<{> price_info;
    double total_price;
};

struct HeliusResponse {
    { result;
    { items;
    TokenInfo token_info;
    std::optional<double> amount;
    { nativeBalance;
    double total_price;
};

struct DAOHolding {
    double rank;
    std::string name;
    std::string holdings;
    std::string value;
    std::string percentage;
    std::string imageUrl;
};

std::string formatCurrency(double amount);

std::string calculatePercentage(double amount, double total);

// Helper function to format token amounts
std::string formatTokenAmount(double amount, auto decimals = 18);
} // namespace elizaos
