#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_EXECUTION_SELLSERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_EXECUTION_SELLSERVICE_H
#include "core.h"
#include "@elizaos/core.h"
#include "../base/BaseTradeService.h"
#include "../validation/TokenValidationService.h"
#include "../calculation/tradeCalculation.h"
#include "../../types.h"
#include "uuid.h"
using uuidv4 = v4;
#include "../../utils/bignumber.h"
#include "../../utils/wallet.h"
#include "../tradeMemoryService.h"
#include "../walletService.h"
#include "../dataService.h"
#include "../analyticsService.h"
#include "../../utils/wallet.h"

class SellService;

class SellService : public BaseTradeService, public std::enable_shared_from_this<SellService> {
public:
    using std::enable_shared_from_this<SellService>::shared_from_this;
    object pendingSells = object{};

    std::shared_ptr<TokenValidationService> validationService;

    std::shared_ptr<TradeCalculationService> calculationService;

    std::shared_ptr<TradeMemoryService> tradeMemoryService;

    SellService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService, std::shared_ptr<TradeMemoryService> tradeMemoryService);
    virtual std::shared_ptr<Promise<void>> initialize();
    virtual std::shared_ptr<Promise<void>> stop();
    virtual std::shared_ptr<Promise<void>> handleSellSignal(any params);
    virtual std::shared_ptr<Promise<void>> updateExpectedOutAmount(any signal);
    virtual std::shared_ptr<Promise<object>> executeSell(any signal);
    SellService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService);
};

#endif
