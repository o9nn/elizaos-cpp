#include "index.hpp"

std::shared_ptr<Plugin> degenTraderPlugin = object{
    object::pair{std:("name"), std:("Degen Trader Plugin")}, 
    object::pair{std:("description"), std:("Autonomous trading agent plugin")}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("providers"), array<any>()}, 
    object::pair{std:("actions"), array<any>()}, 
    object::pair{std:("services"), array<DegenTradingService>{ DegenTradingService }}
};

void Main(void)
{
}

MAIN
