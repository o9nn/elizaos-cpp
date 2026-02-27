#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTOFUNTRADER_SERVICES_CALCULATION_SIGNALCALCULATION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTOFUNTRADER_SERVICES_CALCULATION_SIGNALCALCULATION_H
#include "core.hpp"
#include "../../types/trading.h"
#include "../base/BaseTradeService.h"
// External dependency removed

class SignalCalculationService;

class SignalCalculationService : public BaseTradeService, public std::enable_shared_from_this<SignalCalculationService> {
public:
    using std::enable_shared_from_this<SignalCalculationService>::shared_from_this;
    virtual any calculateTechnicalSignals(any marketData);
    virtual std::shared_ptr<Promise<array<std::shared_ptr<TokenSignal>>>> scoreTokenSignals(array<std::shared_ptr<TokenSignal>> signals);
    virtual std::shared_ptr<Promise<double>> calculateDrawdown(object portfolio);
    virtual std::shared_ptr<Promise<double>> getHighWaterMark();
    virtual std::shared_ptr<Promise<void>> updateHighWaterMark(double value);
    SignalCalculationService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService);
};

#endif
