#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-manager/actions/unloadPlugin.h"

std::shared_ptr<Action> unloadPluginAction = object{
    object::pair{std::string("name"), std::string("UNLOAD_PLUGIN")}, 
    object::pair{std::string("similes"), array<string>{ std::string("unload plugin"), std::string("disable plugin"), std::string("deactivate plugin"), std::string("stop plugin"), std::string("remove plugin") }}, 
    object::pair{std::string("description"), std::string("Unload a plugin that is currently loaded (except original plugins)")}, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("Autoliza")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I need to unload the example-plugin")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UNLOAD_PLUGIN") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("Autoliza")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Unloading the example-plugin now.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UNLOAD_PLUGIN") }}, 
            object::pair{std::string("simple"), true}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("Autoliza")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Disable the test plugin that is running")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UNLOAD_PLUGIN") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("Autoliza")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll disable the test plugin for you.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UNLOAD_PLUGIN") }}, 
            object::pair{std::string("simple"), true}
        }}
    } } }}, 
    , 
};

void Main(void)
{
}

MAIN
