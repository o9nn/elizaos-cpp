#include "plugin.hpp"
#include <string>

std::shared_ptr<Plugin> plugin = object{
    object::pair{std::string("name"), std::string("starter")}, 
    object::pair{std::string("description"), std::string("A starter plugin for Eliza")}, 
    object::pair{std::string("config"), object{
        object::pair{std::string("EXAMPLE_PLUGIN_VARIABLE"), process->env->EXAMPLE_PLUGIN_VARIABLE}
    }}, 
    , 
    object::pair{std::string("services"), std::string("services")}, 
    object::pair{std::string("actions"), std::string("actions")}, 
    object::pair{std::string("providers"), std::string("providers")}, 
    object::pair{std::string("tests"), std::string("tests")}
};

void Main(void)
{
}

MAIN
