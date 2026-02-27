#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/index.h"

std::shared_ptr<Plugin> degenTraderPlugin = object{
    object::pair{std::string("name"), std::string("Degen Trader Plugin")}, 
    object::pair{std::string("description"), std::string("Autonomous trading agent plugin")}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("providers"), array<any>()}, 
    object::pair{std::string("actions"), array<any>()}, 
    object::pair{std::string("services"), array<DegenTradingService>{ DegenTradingService }}
};

void Main(void)
{
}

MAIN
