#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-manager/actions/loadPlugin.h"

std::shared_ptr<Action> loadPluginAction = object{
    object::pair{std::string("name"), std::string("LOAD_PLUGIN")}, 
    object::pair{std::string("similes"), array<string>{ std::string("load plugin"), std::string("enable plugin"), std::string("activate plugin"), std::string("start plugin") }}, 
    object::pair{std::string("description"), std::string("Load a plugin that is currently in the ready or unloaded state")}, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("Autoliza")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I need to load the shell plugin")}, 
            object::pair{std::string("actions"), array<string>{ std::string("LOAD_PLUGIN") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("Autoliza")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Loading the shell plugin now.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("LOAD_PLUGIN") }}, 
            object::pair{std::string("simple"), true}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("Autoliza")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Activate the example-plugin that is ready")}, 
            object::pair{std::string("actions"), array<string>{ std::string("LOAD_PLUGIN") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("Autoliza")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll activate the example-plugin for you.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("LOAD_PLUGIN") }}, 
            object::pair{std::string("simple"), true}
        }}
    } } }}, 
    , 
};

void Main(void)
{
}

MAIN
