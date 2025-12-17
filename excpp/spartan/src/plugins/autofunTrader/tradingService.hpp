#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "services/analyticsService.hpp"
#include "services/dataService.hpp"
#include "services/execution/buyService.hpp"
#include "services/execution/sellService.hpp"
#include "services/execution/tradeExecutionService.hpp"
#include "services/monitoringService.hpp"
#include "services/taskService.hpp"
#include "services/tradeMemoryService.hpp"
#include "services/walletService.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Combined DegenTradingService that integrates all functionality

// Service imports

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

class DegenTradingService {
public:
    DegenTradingService();
    static void start(IAgentRuntime runtime);
    static void stop(IAgentRuntime runtime);
    bool isServiceRunning();

private:
    std::string processId_;
    BuyService buyService_;
    SellService sellService_;
    DataService dataService_;
    AnalyticsService analyticsService_;
    MonitoringService monitoringService_;
    TaskService taskService_;
    WalletService walletService_;
    TradeExecutionService tradeExecutionService_;
    TradeMemoryService tradeMemoryService_;
};


} // namespace elizaos
