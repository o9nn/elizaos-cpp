#include "pluginStateProvider.hpp"
#include <string>

std::shared_ptr<Provider> pluginStateProvider = object{
    object::pair{std::string("name"), std::string("pluginState")}, 
    object::pair{std::string("description"), std::string("Provides information about the current state of all plugins including loaded status, missing environment variables, and errors")}, 
};

void Main(void)
{
}

MAIN
