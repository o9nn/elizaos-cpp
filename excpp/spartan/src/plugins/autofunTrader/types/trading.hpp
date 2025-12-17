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
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



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
    std::string trend;
    bool unusualActivity;
    double volatility;
    double mentionCount;
    double sentiment;
    double influencerMentions;
    double rank;
    double priceChange24h;
    double volumeChange24h;
};

struct RiskLimits {
    double maxPositionSize;
    double maxDrawdown;
    double stopLossPercentage;
    double takeProfitPercentage;
};

struct TradingConfig {
    double priceCheck;
    double walletSync;
    double performanceMonitor;
    double minLiquidity;
    double minVolume;
    double minScore;
    RiskLimits riskLimits;
    double baseSlippage;
    double maxSlippage;
    double liquidityMultiplier;
    double volumeMultiplier;
};

struct PortfolioStatus {
    double totalValue;
    double solBalance;
    double drawdown;
};

struct SellSignalMessage {
    UUID positionId;
    std::string tokenAddress;
    std::string amount;
    std::string entityId;
    std::optional<std::string> expectedOutAmount;
};

using WalletPortfolioItem = {
using WalletPortfolio = {

} // namespace elizaos
