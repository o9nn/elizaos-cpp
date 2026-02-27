#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-plugin-manager/src/providers/registryPluginsProvider.h"

std::shared_ptr<Provider> registryPluginsProvider = object{
    object::pair{std:("name"), std:("registryPlugins")}, 
    object::pair{std:("description"), std:("Provides list of available plugins from the ElizaOS registry using cached data")}, 
};

void Main(void)
{
}

MAIN
