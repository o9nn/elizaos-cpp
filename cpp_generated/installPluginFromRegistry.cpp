#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-manager/actions/installPluginFromRegistry.h"

std::shared_ptr<Action> installPluginFromRegistryAction = object{
    object::pair{std::string("name"), std::string("installPluginFromRegistry")}, 
    object::pair{std::string("description"), std::string("Install a plugin from the ElizaOS plugin registry")}, 
    object::pair{std::string("similes"), array<string>{ std::string("install plugin from registry"), std::string("add plugin from registry"), std::string("download plugin"), std::string("get plugin from registry") }}, 
    , 
    object::pair{std::string("validate"), [=](auto runtime) mutable
    {
        auto pluginManagerService = runtime->getService(std::string("PLUGIN_MANAGER"));
        return !!pluginManagerService;
    }
    }
};

void Main(void)
{
}

MAIN
