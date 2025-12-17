#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// pages/api/daoHoldings.ts

struct TokenInfo {
    std::string name;
    std::string symbol;
    double balance;
    double decimals;
    double total_price;
};

struct HeliusResponse {
    TokenInfo token_info;
    std::optional<double> amount;
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
std::string formatTokenAmount(double amount, auto decimals);
} // namespace elizaos
