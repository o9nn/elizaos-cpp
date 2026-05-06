#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_SERVICES_PRICEPROTECTION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_SERVICES_PRICEPROTECTION_H
#include "core.h"
#include "./database.h"
#include "@/config/chains.h"
#include "./marketDataService.h"

class ValidationResult;
class PriceProtectionService;

class ValidationResult : public object, public std::enable_shared_from_this<ValidationResult> {
public:
    using std::enable_shared_from_this<ValidationResult>::shared_from_this;
    boolean isValid;

    double currentPrice;

    double priceAtQuote;

    double deviation;

    double deviationBps;

    double maxAllowedDeviationBps;

    string reason;
};

class PriceProtectionService : public object, public std::enable_shared_from_this<PriceProtectionService> {
public:
    using std::enable_shared_from_this<PriceProtectionService>::shared_from_this;
    std::shared_ptr<MarketDataService> marketDataService;

    PriceProtectionService();
    virtual std::shared_ptr<Promise<std::shared_ptr<ValidationResult>>> validateQuotePrice(string tokenId, string tokenAddress, std::shared_ptr<Chain> chain, double priceAtQuote, double maxDeviationBps);
};

#endif
