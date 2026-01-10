#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_ANALYTICSSERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_ANALYTICSSERVICE_H
#include "core.h"
#include "@elizaos/core.h"
#include "../types/index.h"
#include "uuid.h"
using uuidv4 = v4;

class AnalyticsService;

class AnalyticsService : public object, public std::enable_shared_from_this<AnalyticsService> {
public:
    using std::enable_shared_from_this<AnalyticsService>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    AnalyticsService(std::shared_ptr<IAgentRuntime> runtime_);
    virtual std::shared_ptr<Promise<void>> initialize();
    virtual std::shared_ptr<Promise<void>> stop();
    virtual std::shared_ptr<Promise<double>> scoreTechnicalSignals(any signals);
    virtual std::shared_ptr<Promise<double>> scoreSocialMetrics(any metrics);
    virtual std::shared_ptr<Promise<double>> scoreMarketMetrics(object metrics);
    virtual std::shared_ptr<Promise<void>> trackSlippageImpact(string tokenAddress, string expectedAmount, string actualAmount, double slippageBps, boolean isSell);
    virtual double calculateRSI(array<double> prices, double period);
    virtual object calculateMACD(array<double> prices);
    virtual double calculateEMA(array<double> prices, double period);
    virtual std::shared_ptr<Promise<void>> trackTradeExecution(object data);
    virtual std::shared_ptr<Promise<any>> addTradePerformance(std::shared_ptr<TradePerformanceData> data, boolean isSimulation);
    virtual std::shared_ptr<Promise<void>> updateTokenStatistics(string tokenAddress, object data);
};

#endif
