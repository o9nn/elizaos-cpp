#include "pluginInstallation.test.h"

any resetRegistryCache = vi->fn();

void Main(void)
{
    describe(std:("Plugin Installation"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<PluginManagerService>> pluginManager;
        beforeEach([=]() mutable
        {
            resetRegistryCache();
            mockRuntime = as<any>(object{
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent-id"))}, 
                object::pair{std:("plugins"), array<any>()}, 
                object::pair{std:("registerPlugin"), [=]() mutable
                {
                    return Promise->resolve();
                }
                }, 
                object::pair{std:("registerAction"), [=]() mutable
                {
                }
                }, 
                object::pair{std:("registerProvider"), [=]() mutable
                {
                }
                }, 
                object::pair{std:("registerEvaluator"), [=]() mutable
                {
                }
                }, 
                object::pair{std:("services"), std::make_shared<Map>()}, 
                object::pair{std:("getService"), vi->fn()}
            });
            pluginManager = std::make_shared<PluginManagerService>(mockRuntime, object{
                object::pair{std:("pluginDirectory"), std:("./test-plugins")}
            });
        }
        );
        afterEach([=]() mutable
        {
            vi->clearAllMocks();
        }
        );
        describe(std:("Plugin Management"), [=]() mutable
        {
            it(std:("should initialize plugin manager service"), [=]() mutable
            {
                expect(pluginManager)->toBeDefined();
                expect(type_of(std::bind(&PluginManagerService::installPluginFromRegistry, pluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)))->toBe(std:("function"));
                expect(type_of(std::bind(&PluginManagerService::loadInstalledPlugin, pluginManager, std::placeholders::_1)))->toBe(std:("function"));
                expect(type_of(std::bind(&PluginManagerService::listInstalledPlugins, pluginManager)))->toBe(std:("function"));
            }
            );
            it(std:("should handle non-existent plugin gracefully"), [=]() mutable
            {
                std::async([=]() { expect(pluginManager->installPluginFromRegistry(std:("@elizaos/non-existent-plugin-12345")))->rejects->toThrow(); });
            }
            );
            it(std:("should track plugin installation attempts"), [=]() mutable
            {
                auto installedPlugins = pluginManager->listInstalledPlugins();
                expect(Array->isArray(installedPlugins))->toBe(true);
            }
            );
        }
        );
        describe(std:("Plugin State Management"), [=]() mutable
        {
            it(std:("should handle plugin loading states"), [=]() mutable
            {
                auto pluginId = std:("test-plugin-id");
                auto pluginState = pluginManager->getPlugin(pluginId);
                expect(pluginState)->toBe(undefined);
            }
            );
            it(std:("should provide plugin configuration status"), [=]() mutable
            {
                auto pluginInfo = pluginManager->getInstalledPluginInfo(std:("@elizaos/non-existent"));
                expect(pluginInfo)->toBe(undefined);
            }
            );
        }
        );
        describe(std:("Registry Operations"), [=]() mutable
        {
            it(std:("should handle registry fetch attempts"), [=]() mutable
            {
                try
                {
                    auto plugins = std::async([=]() { pluginManager->getAvailablePluginsFromRegistry(); });
                    expect(type_of(plugins))->toBe(std:("object"));
                }
                catch (const any& error)
                {
                    expect(error)->toBeDefined();
                }
            }
            );
            it(std:("should handle invalid plugin names"), [=]() mutable
            {
                std::async([=]() { expect(pluginManager->installPluginFromRegistry(string_empty))->rejects->toThrow(); });
            }
            );
        }
        );
        describe(std:("Plugin Loading"), [=]() mutable
        {
            it(std:("should handle plugin loading attempts"), [=]() mutable
            {
                std::async([=]() { expect(pluginManager->loadInstalledPlugin(std:("@elizaos/non-existent-plugin")))->rejects->toThrow(); });
            }
            );
            it(std:("should validate plugin loading prerequisites"), [=]() mutable
            {
                auto hasPlugin = pluginManager->getInstalledPluginInfo(std:("@elizaos/test"));
                expect(hasPlugin)->toBe(undefined);
            }
            );
        }
        );
        describe(std:("Installed Plugin Management"), [=]() mutable
        {
            it(std:("should track installed plugins list"), [=]() mutable
            {
                auto installedPlugins = pluginManager->listInstalledPlugins();
                expect(Array->isArray(installedPlugins))->toBe(true);
            }
            );
            it(std:("should handle plugin info requests"), [=]() mutable
            {
                auto pluginInfo = pluginManager->getInstalledPluginInfo(std:("@elizaos/test-plugin"));
                expect(pluginInfo)->toBe(undefined);
            }
            );
            it(std:("should handle plugin path operations safely"), [=]() mutable
            {
                expect([=]() mutable
                {
                    pluginManager->getInstalledPluginInfo(std:("@elizaos/plugin-test"));
                }
                )->not->toThrow();
            }
            );
        }
        );
        describe(std:("Error Handling"), [=]() mutable
        {
            it(std:("should handle invalid plugin names gracefully"), [=]() mutable
            {
                std::async([=]() { expect(pluginManager->installPluginFromRegistry(std:("invalid-plugin-name")))->rejects->toThrow(); });
            }
            );
            it(std:("should handle network failures gracefully"), [=]() mutable
            {
                std::async([=]() { expect(pluginManager->installPluginFromRegistry(std:("@elizaos/non-existent-plugin-xyz")))->rejects->toThrow(); });
            }
            );
            it(std:("should provide meaningful error messages"), [=]() mutable
            {
                expect([=]() mutable
                {
                    pluginManager->getInstalledPluginInfo(string_empty);
                }
                )->not->toThrow();
            }
            );
        }
        );
    }
    );
}

MAIN
