#include "index.hpp"

std::shared_ptr<Plugin> pluginManagerPlugin = object{
    object::pair{std:("name"), std:("plugin-manager")}, 
    object::pair{std:("description"), std:("Manages dynamic loading and unloading of plugins at runtime, with registry integration and configuration status checking")}, 
    object::pair{std:("services"), array<PluginManagerService>{ PluginManagerService, PluginConfigurationService }}, 
    object::pair{std:("actions"), array<any>{ loadPluginAction, unloadPluginAction, installPluginFromRegistryAction, searchPluginAction, getPluginDetailsAction, clonePluginAction, publishPluginAction }}, 
    object::pair{std:("providers"), array<any>{ pluginStateProvider, pluginConfigurationStatusProvider, registryPluginsProvider, pluginKnowledgeProvider }}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("routes"), array<any>()}, 
    object::pair{std:("init"), [=](auto config, auto runtime) mutable
    {
    }
    }
};

void Main(void)
{
}

MAIN
