#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_CALCULATION_TECHNICALANALYSIS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_CALCULATION_TECHNICALANALYSIS_H
#include "core.hpp"
#include "../base/BaseTradeService.h"
// External dependency removed

class TechnicalAnalysisService;

class TechnicalAnalysisService : public BaseTradeService, public std::enable_shared_from_this<TechnicalAnalysisService> {
public:
    using std::enable_shared_from_this<TechnicalAnalysisService>::shared_from_this;
    virtual any calculateTechnicalSignals(any marketData);
    TechnicalAnalysisService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService);
};

#endif
