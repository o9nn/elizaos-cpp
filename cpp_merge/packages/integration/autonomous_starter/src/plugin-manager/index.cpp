#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-manager/index.h"

std::shared_ptr<Plugin> pluginManagerPlugin = object{
    object::pair{std:("name"), std:("plugin-manager")}, 
    object::pair{std:("description"), std:("Manages dynamic loading and unloading of plugins at runtime, including registry installation and configuration management")}, 
    object::pair{std:("services"), array<PluginManagerService>{ PluginManagerService, PluginConfigurationService, PluginUserInteractionService }}, 
    object::pair{std:("actions"), array<any>{ loadPluginAction, unloadPluginAction, startPluginConfigurationAction, installPluginFromRegistryAction }}, 
    object::pair{std:("providers"), array<any>{ pluginStateProvider, pluginConfigurationStatusProvider, registryPluginsProvider }}, 
    object::pair{std:("evaluators"), array<any>{ pluginConfigurationEvaluator }}, 
    object::pair{std:("init"), [=](auto config, auto runtime) mutable
    {
    }
    }
};

void Main(void)
{
}

MAIN
