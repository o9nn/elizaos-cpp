#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/strategies/strategy_copy.h"

void copyStrategy(std::shared_ptr<IAgentRuntime> runtime)
{
    auto service = std::async([=]() { acquireService(runtime, std::string("TRADER_STRATEGY"), std::string("copy trading strategy")); });
    auto infoService = std::async([=]() { acquireService(runtime, std::string("TRADER_DATAPROVIDER"), std::string("copy trading info")); });
    auto me = object{
        object::pair{std::string("name"), std::string("Copy trading strategy")}
    };
    auto hndl = std::async([=]() { service["register_strategy"](me); });
};


void onWalletEvent(any runtime, any strategyService, any hndl)
{
};


