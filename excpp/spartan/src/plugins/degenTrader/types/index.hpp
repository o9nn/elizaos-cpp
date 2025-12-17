#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



enum ServiceTypes {
}

struct TokenSignal {
    std::string address;
    std::string symbol;
    double marketCap;
    double volume24h;
    double price;
    double liquidity;
    double score;
    std::vector<std::string> reasons;
    double rsi;
    double value;
    double signal;
    double histogram;
    std::variant<'increasing', 'decreasing', 'stable'> trend;
    bool unusualActivity;
    double volatility;
    double mentionCount;
    double sentiment;
    double influencerMentions;
    double rank;
    double priceChange24h;
    double volumeChange24h;
};

struct BuySignalMessage {
    UUID positionId;
    std::string tokenAddress;
    std::string entityId;
    std::string expectedOutAmount;
};

struct SellSignalMessage {
    UUID positionId;
    std::string tokenAddress;
    std::string amount;
    std::string entityId;
    std::optional<std::string> expectedOutAmount;
    std::string currentBalance;
    std::string walletAddress;
    bool isSimulation;
    std::string sellRecommenderId;
    std::string reason;
};

struct PriceSignalMessage {
    std::string tokenAddress;
    double initialPrice;
    double currentPrice;
    double priceChange;
};

struct PortfolioStatus {
    double totalValue;
    double solBalance;
    double drawdown;
};

struct TradePerformanceData {
    std::string token_address;
    double buy_price;
    std::string buy_timeStamp;
    double buy_amount;
    double buy_value_usd;
    double buy_market_cap;
    double buy_liquidity;
    double profit_usd;
    double profit_percent;
    bool rapidDump;
};


} // namespace elizaos
