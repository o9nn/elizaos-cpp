#include "index.hpp"
#include <string>

namespace global {
    namespace NodeJS {
    }
}
std::shared_ptr<Plugin> autoPlugin = object{
    object::pair{std::string("name"), std::string("auto")}, 
    object::pair{std::string("description"), std::string("Auto plugin")}, 
    object::pair{std::string("events"), events}, 
    object::pair{std::string("actions"), array<any>{ reflectAction }}, 
    object::pair{std::string("services"), array<AutonomousService>{ AutonomousService }}, 
    object::pair{std::string("providers"), array<any>{ autonomousFeedProvider }}
};

void Main(void)
{
}

MAIN
