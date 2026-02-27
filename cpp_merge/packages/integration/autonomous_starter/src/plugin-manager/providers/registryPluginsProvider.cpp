#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-manager/providers/registryPluginsProvider.h"

std::shared_ptr<Provider> registryPluginsProvider = object{
    object::pair{std:("name"), std:("registryPlugins")}, 
    object::pair{std:("description"), std:("Provides list of available plugins from the ElizaOS registry")}, 
};

void Main(void)
{
}

MAIN
