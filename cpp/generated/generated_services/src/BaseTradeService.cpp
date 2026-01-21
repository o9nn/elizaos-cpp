#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/services/base/BaseTradeService.h"

BaseTradeService::BaseTradeService(std::shared_ptr<IAgentRuntime> runtime_, std::shared_ptr<WalletService> walletService_, std::shared_ptr<DataService> dataService_, std::shared_ptr<AnalyticsService> analyticsService_) : runtime(runtime_), walletService(walletService_), dataService(dataService_), analyticsService(analyticsService_)  {
    this->tradingConfig = DEFAULT_CONFIG;
}

any BaseTradeService::getWalletService()
{
    return this->walletService;
}

any BaseTradeService::getDataService()
{
    return this->dataService;
}

any BaseTradeService::getAnalyticsService()
{
    return this->analyticsService;
}

