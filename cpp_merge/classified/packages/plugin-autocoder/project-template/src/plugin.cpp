#include "plugin.hpp"

std::shared_ptr<Plugin> plugin = object{
    object::pair{std:("name"), std:("starter")}, 
    object::pair{std:("description"), std:("A starter plugin for Eliza")}, 
    object::pair{std:("config"), object{
        object::pair{std:("EXAMPLE_PLUGIN_VARIABLE"), process->env->EXAMPLE_PLUGIN_VARIABLE}
    }}, 
    , 
    object::pair{std:("services"), std:("services")}, 
    object::pair{std:("actions"), std:("actions")}, 
    object::pair{std:("providers"), std:("providers")}, 
    object::pair{std:("tests"), std:("tests")}
};

void Main(void)
{
}

MAIN
