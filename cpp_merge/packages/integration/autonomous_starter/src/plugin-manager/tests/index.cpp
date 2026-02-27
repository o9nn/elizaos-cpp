#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-manager/tests/index.test.h"

void Main(void)
{
    describe(std::string("Plugin Manager Index"), [=]() mutable
    {
        it(std::string("should pluginManagerPlugin with correct definitions"), [=]() mutable
        {
            expect(pluginManagerPlugin->name)->toBe(std::string("plugin-manager"));
            expect(pluginManagerPlugin->description)->toBe(std::string("Manages dynamic loading and unloading of plugins at runtime, including registry installation and configuration management"));
            expect(pluginManagerPlugin->services)->toEqual(array<PluginManagerService>{ PluginManagerService, PluginConfigurationService, PluginUserInteractionService });
            expect(pluginManagerPlugin->providers)->toEqual(array<any>{ pluginStateProvider, pluginConfigurationStatusProvider, registryPluginsProvider });
            expect(pluginManagerPlugin->actions)->toEqual(array<any>{ loadPluginAction, unloadPluginAction, startPluginConfigurationAction, installPluginFromRegistryAction });
            expect(pluginManagerPlugin->init)->toBeInstanceOf(Function);
        }
        );
    }
    );
}

MAIN
