#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_BASE_BASETRADESERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_BASE_BASETRADESERVICE_H
#include "core.h"
#include "@elizaos/core.h"
#include "../walletService.h"
#include "../dataService.h"
#include "../analyticsService.h"
#include "../../types/trading.h"
#include "../../config/trading.h"

class BaseTradeService;

class BaseTradeService : public object, public std::enable_shared_from_this<BaseTradeService> {
public:
    using std::enable_shared_from_this<BaseTradeService>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    std::shared_ptr<WalletService> walletService;

    std::shared_ptr<DataService> dataService;

    std::shared_ptr<AnalyticsService> analyticsService;

    std::shared_ptr<TradingConfig> tradingConfig;

    BaseTradeService(std::shared_ptr<IAgentRuntime> runtime_, std::shared_ptr<WalletService> walletService_, std::shared_ptr<DataService> dataService_, std::shared_ptr<AnalyticsService> analyticsService_);
    virtual any getWalletService();
    virtual any getDataService();
    virtual any getAnalyticsService();
};

#endif
