#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-robot/index.h"

std::shared_ptr<Plugin> robotPlugin = object{
    object::pair{std::string("name"), std::string("plugin-robot")}, 
    object::pair{std::string("description"), std::string("Control screen using robotjs and provide screen context")}, 
    object::pair{std::string("actions"), array<any>{ performScreenAction }}, 
    object::pair{std::string("providers"), array<any>{ screenProvider }}, 
    object::pair{std::string("services"), array<any>{ RobotService }}
};

void Main(void)
{
}

MAIN
