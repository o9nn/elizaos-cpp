#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTOFUNTRADER_SERVICES_CALCULATION_BIRDEYE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTOFUNTRADER_SERVICES_CALCULATION_BIRDEYE_H
#include "core.hpp"
// External dependency removed
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
