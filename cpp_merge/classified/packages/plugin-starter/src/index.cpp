#include "index.hpp"

std::shared_ptr<Plugin> plugin = object{
    object::pair{std:("name"), std:("plugin")}, 
    object::pair{std:("description"), std:("Plugin for ElizaOS")}, 
    object::pair{std:("config"), configSchema}, 
    object::pair{std:("services"), Services}, 
    object::pair{std:("actions"), Actions}, 
    object::pair{std:("providers"), Providers}, 
    object::pair{std:("tests"), array<any>{ PluginTestSuite }}, 
    object::pair{std:("dependencies"), array<any>()}
};

void Main(void)
{
}

MAIN
