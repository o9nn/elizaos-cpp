#include "pluginStateProvider.hpp"

std::shared_ptr<Provider> pluginStateProvider = object{
    object::pair{std:("name"), std:("pluginState")}, 
    object::pair{std:("description"), std:("Provides information about the current state of all plugins including loaded status, missing environment variables, and errors")}, 
};

void Main(void)
{
}

MAIN
