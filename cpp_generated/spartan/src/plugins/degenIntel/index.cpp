#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenIntel/index.h"

std::shared_ptr<Plugin> degenIntelPlugin = object{
    object::pair{std::string("name"), std::string("spartan-intel")}, 
    object::pair{std::string("description"), std::string("Spartan Intel plugin")}, 
    object::pair{std::string("routes"), std::string("routes")}, 
    object::pair{std::string("providers"), array<any>()}, 
    object::pair{std::string("services"), array<TradeChainService>{ TradeChainService, TradeDataProviderService, TradeStrategyService }}, 
    object::pair{std::string("tests"), array<object>{ object{
        object::pair{std::string("name"), std::string("test suite for intel")}, 
        object::pair{std::string("tests"), array<object>{ object{
            object::pair{std::string("name"), std::string("test for intel")}, 
            object::pair{std::string("fn"), [=](auto runtime) mutable
            {
                logger->info(std::string("test in intel working"));
            }
            }
        } }}
    } }}, 
    object::pair{std::string("init"), [=](auto _, auto runtime) mutable
    {
        console->log(std::string("intel init"));
        std::async([=]() { registerTasks(runtime); });
        auto plugins = runtime->plugins->map([=](auto p) mutable
        {
            return p["name"];
        }
        );
        auto notUsed = true;
        if (runtime->getSetting(std::string("COINMARKETCAP_API_KEY"))) {
            runtime->registerProvider(cmcMarketProvider);
            notUsed = false;
        }
        if (runtime->getSetting(std::string("BIRDEYE_API_KEY"))) {
            runtime->registerProvider(birdeyeTrendingProvider);
            runtime->registerProvider(birdeyeTradePortfolioProvider);
            notUsed = false;
        }
        if (plugins->indexOf(std::string("twitter")) != -1) {
            runtime->registerProvider(sentimentProvider);
            notUsed = false;
        }
        if (notUsed) {
            logger->warn(std::string("degen-intel plugin is included but not providing any value (COINMARKETCAP_API_KEY/BIRDEYE_API_KEY or twitter are suggested)"));
        }
    }
    }
};

void Main(void)
{
}

MAIN
