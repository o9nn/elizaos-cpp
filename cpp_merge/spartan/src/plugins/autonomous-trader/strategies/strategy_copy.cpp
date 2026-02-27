#include "strategy_copy.hpp"

void copyStrategy(std::shared_ptr<IAgentRuntime> runtime)
{
    auto service = std::async([=]() { acquireService(runtime, std:("TRADER_STRATEGY"), std:("copy trading strategy")); });
    auto infoService = std::async([=]() { acquireService(runtime, std:("TRADER_DATAPROVIDER"), std:("copy trading info")); });
    auto me = object{
        object::pair{std:("name"), std:("Copy trading strategy")}
    };
    auto hndl = std::async([=]() { service["register_strategy"](me); });
};


void onWalletEvent(any runtime, any strategyService, any hndl)
{
};


