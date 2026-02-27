#include "index.hpp"

std::shared_ptr<Plugin> relayPlugin = object{
    object::pair{std:("name"), std:("relay")}, 
    object::pair{std:("description"), std:("Relay Link integration for cross-chain bridging, swapping, and execution using the Relay protocol")}, 
    object::pair{std:("actions"), array<any>{ relayQuoteAction, relayBridgeAction, relayStatusAction }}, 
    object::pair{std:("services"), array<RelayService>{ RelayService }}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("providers"), array<any>()}
};

void Main(void)
{
}

MAIN
