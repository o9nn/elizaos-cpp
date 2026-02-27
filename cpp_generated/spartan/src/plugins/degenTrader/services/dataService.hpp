#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_DATASERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_DATASERVICE_H
#include "core.h"
#include "@elizaos/core.h"
#include "../utils/cacheManager.h"
#include "../types/trading.h"
#include "../utils/wallet.h"
#include "./analyticsService.h"
#include "./calculation/birdeye.h"
#include "./calculation/technicalAnalysis.h"
#include "./calculation/scoring.h"
#include "./validation/tokenSecurity.h"
#include "./calculation/tradeCalculation.h"
#include "./walletService.h"

class DataService;

class DataService : public object, public std::enable_shared_from_this<DataService> {
public:
    using std::enable_shared_from_this<DataService>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    std::shared_ptr<WalletService> walletService;

    std::shared_ptr<CacheManager> cacheManager;

    std::shared_ptr<BirdeyeService> birdeyeService;

    std::shared_ptr<AnalyticsService> analyticsService;

    std::shared_ptr<TechnicalAnalysisService> technicalAnalysisService;

    std::shared_ptr<ScoringService> scoringService;

    std::shared_ptr<TokenSecurityService> tokenSecurityService;

    std::shared_ptr<TradeCalculationService> tradeCalculationService;

    DataService(std::shared_ptr<IAgentRuntime> runtime_, std::shared_ptr<WalletService> walletService_);
    virtual std::shared_ptr<Promise<void>> initialize();
    virtual std::shared_ptr<Promise<void>> stop();
    virtual std::shared_ptr<Promise<array<std::shared_ptr<TokenSignal>>>> getBirdeyeSignals();
    virtual std::shared_ptr<Promise<array<std::shared_ptr<TokenSignal>>>> getTwitterSignals();
    virtual std::shared_ptr<Promise<array<std::shared_ptr<TokenSignal>>>> getCMCSignals();
    virtual std::shared_ptr<Promise<object>> getTokenMarketData(string tokenAddress);
    virtual std::shared_ptr<Promise<any>> getTokensMarketData(array<string> tokenAddresses);
    virtual std::shared_ptr<Promise<array<string>>> getMonitoredTokens();
    virtual std::shared_ptr<Promise<array<any>>> getPositions();
    virtual any getDefaultRecommendation();
};

#endif
