#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTOFUNTRADER_UTILS_ANALYZETRADE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTOFUNTRADER_UTILS_ANALYZETRADE_H
#include "core.hpp"
#include <string>
#include "../types.h"
// External dependency removed

extern string tradeAnalysisTemplate;
template <typename RET>
RET assessMarketCondition(std::shared_ptr<IAgentRuntime> runtime);

double calculateVolatility(array<double> priceHistory);

double calculateDynamicSlippage(string amount, any quoteData);

template <typename RET>
RET assessMarketCondition(std::shared_ptr<IAgentRuntime> runtime)
{
    try
    {
        auto tradeService = as<any>(runtime->getService(ServiceTypes::AUTOFUN_TRADING));
        auto solData = std::async([=]() { tradeService["dataService"]["getTokenMarketData"](std::string("So11111111111111111111111111111111111111112")); });
        if (OR((!solData["priceHistory"]), (solData["priceHistory"]["length"] < 24))) {
            return std::string("neutral");
        }
        auto currentPrice = solData["price"];
        auto previousPrice = const_(solData["priceHistory"])[0];
        auto priceChange = ((currentPrice - previousPrice) / previousPrice) * 100;
        if (priceChange > 5) return std::string("bullish");
        if (priceChange < -5) return std::string("bearish");
        return std::string("neutral");
    }
    catch (const any& error)
    {
        console->log(std::string("Error assessing market condition:"), error);
        return std::string("neutral");
    }
};


#endif
