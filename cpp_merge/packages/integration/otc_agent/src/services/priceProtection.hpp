#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_SERVICES_PRICEPROTECTION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_SERVICES_PRICEPROTECTION_H
#include "core.hpp"
#include "./database.h"
// External dependency removed
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
