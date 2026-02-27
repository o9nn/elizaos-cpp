#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTOFUNTRADER_SERVICES_CALCULATION_SCORING_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTOFUNTRADER_SERVICES_CALCULATION_SCORING_H
#include "core.hpp"
#include "../../types/trading.h"
#include "../base/BaseTradeService.h"
// External dependency removed

class ScoringService;

class ScoringService : public BaseTradeService, public std::enable_shared_from_this<ScoringService> {
public:
    using std::enable_shared_from_this<ScoringService>::shared_from_this;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<TokenSignal>>>> scoreTokenSignals(array<std::shared_ptr<TokenSignal>> signals);
    ScoringService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService);
};

#endif
