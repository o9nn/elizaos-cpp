#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_UTILS_PRICE_VALIDATOR_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_UTILS_PRICE_VALIDATOR_H
#include "core.hpp"
// External dependency removed
#include "./retry-cache.h"

class PriceValidationResult;
class CoinGeckoPriceResponse;

class PriceValidationResult : public object, public std::enable_shared_from_this<PriceValidationResult> {
public:
    using std::enable_shared_from_this<PriceValidationResult>::shared_from_this;
    boolean valid;

    string warning;

    double aggregatedPrice;

    double poolPrice;

    double divergencePercent;
};

class CoinGeckoPriceResponse : public object, public std::enable_shared_from_this<CoinGeckoPriceResponse> {
public:
    using std::enable_shared_from_this<CoinGeckoPriceResponse>::shared_from_this;
};

extern Record<string, string> COINGECKO_CHAIN_MAP;
extern double COINGECKO_CACHE_TTL_MS;
std::shared_ptr<Promise<std::shared_ptr<PriceValidationResult>>> checkPriceDivergence(string tokenAddress, std::shared_ptr<Chain> chain, double poolPriceUsd);

#endif
