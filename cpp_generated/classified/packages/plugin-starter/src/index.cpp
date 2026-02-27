#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-starter/src/index.h"

std::shared_ptr<Plugin> plugin = object{
    object::pair{std::string("name"), std::string("plugin")}, 
    object::pair{std::string("description"), std::string("Plugin for ElizaOS")}, 
    object::pair{std::string("config"), configSchema}, 
    object::pair{std::string("services"), Services}, 
    object::pair{std::string("actions"), Actions}, 
    object::pair{std::string("providers"), Providers}, 
    object::pair{std::string("tests"), array<any>{ PluginTestSuite }}, 
    object::pair{std::string("dependencies"), array<any>()}
};

void Main(void)
{
}

MAIN
