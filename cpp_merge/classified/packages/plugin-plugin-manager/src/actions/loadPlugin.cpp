#include "loadPlugin.hpp"

std::shared_ptr<Action> loadPluginAction = object{
    object::pair{std:("name"), std:("LOAD_PLUGIN")}, 
    object::pair{std:("similes"), array<string>{ std:("load plugin"), std:("enable plugin"), std:("activate plugin"), std:("start plugin") }}, 
    object::pair{std:("description"), std:("Load a plugin that is currently in the ready or unloaded state")}, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("Autoliza")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I need to load the shell plugin")}, 
            object::pair{std:("actions"), array<string>{ std:("LOAD_PLUGIN") }}
        }}
    }, object{
        object::pair{std:("name"), std:("Autoliza")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Loading the shell plugin now.")}, 
            object::pair{std:("actions"), array<string>{ std:("LOAD_PLUGIN") }}, 
            object::pair{std:("simple"), true}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("Autoliza")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Activate the example-plugin that is ready")}, 
            object::pair{std:("actions"), array<string>{ std:("LOAD_PLUGIN") }}
        }}
    }, object{
        object::pair{std:("name"), std:("Autoliza")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll activate the example-plugin for you.")}, 
            object::pair{std:("actions"), array<string>{ std:("LOAD_PLUGIN") }}, 
            object::pair{std:("simple"), true}
        }}
    } } }}, 
    , 
};

void Main(void)
{
}

MAIN
