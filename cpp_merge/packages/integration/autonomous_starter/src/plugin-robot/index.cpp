#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-robot/index.h"

std::shared_ptr<Plugin> robotPlugin = object{
    object::pair{std:("name"), std:("plugin-robot")}, 
    object::pair{std:("description"), std:("Control screen using robotjs and provide screen context")}, 
    object::pair{std:("actions"), array<any>{ performScreenAction }}, 
    object::pair{std:("providers"), array<any>{ screenProvider }}, 
    object::pair{std:("services"), array<any>{ RobotService }}
};

void Main(void)
{
}

MAIN
