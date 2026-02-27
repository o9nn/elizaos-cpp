#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_CALCULATION_BIRDEYE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_CALCULATION_BIRDEYE_H
#include "core.h"
#include "@elizaos/core.h"
#include "../../config.h"

class BirdeyeService;

class BirdeyeService : public object, public std::enable_shared_from_this<BirdeyeService> {
public:
    using std::enable_shared_from_this<BirdeyeService>::shared_from_this;
    string apiKey;

    BirdeyeService(string apiKey_);
    virtual any getBirdeyeFetchOptions();
    virtual std::shared_ptr<Promise<object>> getTokenMarketData(string tokenAddress);
    virtual std::shared_ptr<Promise<any>> getTokensMarketData(array<string> tokenAddresses);
    virtual any getEmptyMarketData();
};

#endif
