#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-manager/index.h"

std::shared_ptr<Plugin> pluginManagerPlugin = object{
    object::pair{std::string("name"), std::string("plugin-manager")}, 
    object::pair{std::string("description"), std::string("Manages dynamic loading and unloading of plugins at runtime, including registry installation and configuration management")}, 
    object::pair{std::string("services"), array<PluginManagerService>{ PluginManagerService, PluginConfigurationService, PluginUserInteractionService }}, 
    object::pair{std::string("actions"), array<any>{ loadPluginAction, unloadPluginAction, startPluginConfigurationAction, installPluginFromRegistryAction }}, 
    object::pair{std::string("providers"), array<any>{ pluginStateProvider, pluginConfigurationStatusProvider, registryPluginsProvider }}, 
    object::pair{std::string("evaluators"), array<any>{ pluginConfigurationEvaluator }}, 
    object::pair{std::string("init"), [=](auto config, auto runtime) mutable
    {
    }
    }
};

void Main(void)
{
}

MAIN
