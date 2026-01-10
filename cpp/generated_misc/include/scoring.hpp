#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_CALCULATION_SCORING_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_CALCULATION_SCORING_H
#include "core.h"
#include "../../types/trading.h"
#include "../base/BaseTradeService.h"
#include "@elizaos/core.h"

class ScoringService;

class ScoringService : public BaseTradeService, public std::enable_shared_from_this<ScoringService> {
public:
    using std::enable_shared_from_this<ScoringService>::shared_from_this;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<TokenSignal>>>> scoreTokenSignals(array<std::shared_ptr<TokenSignal>> signals);
    ScoringService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService);
};

#endif
