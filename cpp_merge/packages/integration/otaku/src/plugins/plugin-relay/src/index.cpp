#include "index.hpp"
#include <string>

std::shared_ptr<Plugin> relayPlugin = object{
    object::pair{std::string("name"), std::string("relay")}, 
    object::pair{std::string("description"), std::string("Relay Link integration for cross-chain bridging, swapping, and execution using the Relay protocol")}, 
    object::pair{std::string("actions"), array<any>{ relayQuoteAction, relayBridgeAction, relayStatusAction }}, 
    object::pair{std::string("services"), array<RelayService>{ RelayService }}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("providers"), array<any>()}
};

void Main(void)
{
}

MAIN
