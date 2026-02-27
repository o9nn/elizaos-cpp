#include "index.hpp"

std::shared_ptr<Plugin> degenIntelPlugin = object{
    object::pair{std:("name"), std:("spartan-intel")}, 
    object::pair{std:("description"), std:("Spartan Intel plugin")}, 
    object::pair{std:("routes"), std:("routes")}, 
    object::pair{std:("providers"), array<any>()}, 
    object::pair{std:("services"), array<TradeChainService>{ TradeChainService, TradeDataProviderService, TradeStrategyService }}, 
    object::pair{std:("tests"), array<object>{ object{
        object::pair{std:("name"), std:("test suite for intel")}, 
        object::pair{std:("tests"), array<object>{ object{
            object::pair{std:("name"), std:("test for intel")}, 
            object::pair{std:("fn"), [=](auto runtime) mutable
            {
                logger->info(std:("test in intel working"));
            }
            }
        } }}
    } }}, 
    object::pair{std:("init"), [=](auto _, auto runtime) mutable
    {
        console->log(std:("intel init"));
        std::async([=]() { registerTasks(runtime); });
        auto plugins = runtime->plugins->map([=](auto p) mutable
        {
            return p["name"];
        }
        );
        auto notUsed = true;
        if (runtime->getSetting(std:("COINMARKETCAP_API_KEY"))) {
            runtime->registerProvider(cmcMarketProvider);
            notUsed = false;
        }
        if (runtime->getSetting(std:("BIRDEYE_API_KEY"))) {
            runtime->registerProvider(birdeyeTrendingProvider);
            runtime->registerProvider(birdeyeTradePortfolioProvider);
            notUsed = false;
        }
        if (plugins->indexOf(std:("twitter")) != -1) {
            runtime->registerProvider(sentimentProvider);
            notUsed = false;
        }
        if (notUsed) {
            logger->warn(std:("degen-intel plugin is included but not providing any value (COINMARKETCAP_API_KEY/BIRDEYE_API_KEY or twitter are suggested)"));
        }
    }
    }
};

void Main(void)
{
}

MAIN
