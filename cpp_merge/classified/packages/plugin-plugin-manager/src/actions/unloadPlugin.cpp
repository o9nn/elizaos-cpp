#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-plugin-manager/src/actions/unloadPlugin.h"

std::shared_ptr<Action> unloadPluginAction = object{
    object::pair{std:("name"), std:("UNLOAD_PLUGIN")}, 
    object::pair{std:("similes"), array<string>{ std:("unload plugin"), std:("disable plugin"), std:("deactivate plugin"), std:("stop plugin"), std:("remove plugin") }}, 
    object::pair{std:("description"), std:("Unload a plugin that is currently loaded (except original plugins)")}, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("Autoliza")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I need to unload the example-plugin")}, 
            object::pair{std:("actions"), array<string>{ std:("UNLOAD_PLUGIN") }}
        }}
    }, object{
        object::pair{std:("name"), std:("Autoliza")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Unloading the example-plugin now.")}, 
            object::pair{std:("actions"), array<string>{ std:("UNLOAD_PLUGIN") }}, 
            object::pair{std:("simple"), true}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("Autoliza")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Disable the test plugin that is running")}, 
            object::pair{std:("actions"), array<string>{ std:("UNLOAD_PLUGIN") }}
        }}
    }, object{
        object::pair{std:("name"), std:("Autoliza")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll disable the test plugin for you.")}, 
            object::pair{std:("actions"), array<string>{ std:("UNLOAD_PLUGIN") }}, 
            object::pair{std:("simple"), true}
        }}
    } } }}, 
    , 
};

void Main(void)
{
}

MAIN
