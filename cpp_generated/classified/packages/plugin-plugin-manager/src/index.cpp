#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-plugin-manager/src/index.h"

std::shared_ptr<Plugin> pluginManagerPlugin = object{
    object::pair{std::string("name"), std::string("plugin-manager")}, 
    object::pair{std::string("description"), std::string("Manages dynamic loading and unloading of plugins at runtime, with registry integration and configuration status checking")}, 
    object::pair{std::string("services"), array<PluginManagerService>{ PluginManagerService, PluginConfigurationService }}, 
    object::pair{std::string("actions"), array<any>{ loadPluginAction, unloadPluginAction, installPluginFromRegistryAction, searchPluginAction, getPluginDetailsAction, clonePluginAction, publishPluginAction }}, 
    object::pair{std::string("providers"), array<any>{ pluginStateProvider, pluginConfigurationStatusProvider, registryPluginsProvider, pluginKnowledgeProvider }}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("routes"), array<any>()}, 
    object::pair{std::string("init"), [=](auto config, auto runtime) mutable
    {
    }
    }
};

void Main(void)
{
}

MAIN
