#include "installPluginFromRegistry.hpp"

std::shared_ptr<Action> installPluginFromRegistryAction = object{
    object::pair{std:("name"), std:("installPluginFromRegistry")}, 
    object::pair{std:("description"), std:("Install a plugin from the ElizaOS plugin registry")}, 
    object::pair{std:("similes"), array<string>{ std:("install plugin from registry"), std:("add plugin from registry"), std:("download plugin"), std:("get plugin from registry") }}, 
    , 
};

void Main(void)
{
}

MAIN
