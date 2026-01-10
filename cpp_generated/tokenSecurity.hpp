#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_VALIDATION_TOKENSECURITY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_VALIDATION_TOKENSECURITY_H
#include "core.h"
#include "../base/BaseTradeService.h"
#include "@elizaos/core.h"

class TokenSecurityService;

class TokenSecurityService : public BaseTradeService, public std::enable_shared_from_this<TokenSecurityService> {
public:
    using std::enable_shared_from_this<TokenSecurityService>::shared_from_this;
    virtual std::shared_ptr<Promise<object>> validateTokenForTrading(string tokenAddress);
    virtual std::shared_ptr<Promise<object>> fetchTokenMetadata(string tokenAddress);
    TokenSecurityService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService);
};

#endif
