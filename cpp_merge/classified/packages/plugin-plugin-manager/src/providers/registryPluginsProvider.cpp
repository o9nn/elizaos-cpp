#include "registryPluginsProvider.hpp"
#include <string>

std::shared_ptr<Provider> registryPluginsProvider = object{
    object::pair{std::string("name"), std::string("registryPlugins")}, 
    object::pair{std::string("description"), std::string("Provides list of available plugins from the ElizaOS registry using cached data")}, 
};

void Main(void)
{
}

MAIN
