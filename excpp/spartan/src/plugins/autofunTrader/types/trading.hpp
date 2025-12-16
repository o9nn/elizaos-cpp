#include "elizaos/core.hpp"
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



struct TokenSignal {
    std::string address;
    std::string symbol;
    double marketCap;
    double volume24h;
    double price;
    double liquidity;
    double score;
    std::vector<std::string> reasons;
    std::optional<{> technicalSignals;
    double rsi;
    { macd;
    double value;
    double signal;
    double histogram;
    { volumeProfile;
    'increasing' | 'decreasing' | 'stable' trend;
    bool unusualActivity;
    double volatility;
    std::optional<{> socialMetrics;
    double mentionCount;
    double sentiment;
    double influencerMentions;
    std::optional<{> cmcMetrics;
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
    { intervals;
    double priceCheck;
    double walletSync;
    double performanceMonitor;
    { thresholds;
    double minLiquidity;
    double minVolume;
    double minScore;
    RiskLimits riskLimits;
    { slippageSettings;
    double baseSlippage;
    double maxSlippage;
    double liquidityMultiplier;
    double volumeMultiplier;
};

struct PortfolioStatus {
    double totalValue;
    { [tokenAddress: string]: { amount: number; value: number } } positions;

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
