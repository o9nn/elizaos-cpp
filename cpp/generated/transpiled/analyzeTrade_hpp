#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_UTILS_ANALYZETRADE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_UTILS_ANALYZETRADE_H
#include "core.h"
#include "../types.h"
#include "@elizaos/core.h"

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
        auto tradeService = as<any>(runtime->getService(ServiceTypes["DEGEN_TRADING"]));
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
