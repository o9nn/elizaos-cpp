#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-plugin-manager/src/__tests__/unit/pluginProtection.test.h"

void Main(void)
{
    describe(std:("Plugin Protection Mechanism"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        shared<std::shared_ptr<PluginManagerService>> pluginManager;
        shared mockPlugin = [=](auto name) mutable
        {
            return (object{
                object::pair{std:("name"), std:("name")}, 
                object::pair{std:("description"), std:("Mock ") + name + std:(" plugin")}, 
                object::pair{std:("actions"), array<object>{ object{
                    object::pair{std:("name"), string_empty + name + std:("_ACTION")}, 
                    object::pair{std:("similes"), array<any>()}, 
                    object::pair{std:("description"), std:("Test action")}, 
                    object::pair{std:("validate"), [=]() mutable
                    {
                        return true;
                    }
                    }, 
                    object::pair{std:("handler"), [=]() mutable
                    {
                        return (object{
                            object::pair{std:("text"), std:("Success")}, 
                            object::pair{std:("success"), true}
                        });
                    }
                    }
                } }}
            });
        };
        beforeEach([=]() mutable
        {
            auto originalPlugins = array<any>{ mockPlugin(std:("@elizaos/plugin-sql")), mockPlugin(std:("bootstrap")), mockPlugin(std:("game-api")), mockPlugin(std:("inference")), mockPlugin(std:("autonomy")), mockPlugin(std:("knowledge")), mockPlugin(std:("@elizaos/plugin-personality")), mockPlugin(std:("experience")) };
            runtime = as<any>(object{
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent-id"))}, 
                object::pair{std:("plugins"), array<any>{ originalPlugins }}, 
                object::pair{std:("actions"), array<any>()}, 
                object::pair{std:("providers"), array<any>()}, 
                object::pair{std:("evaluators"), array<any>()}, 
                object::pair{std:("services"), std::make_shared<Map>()}, 
                object::pair{std:("registerPlugin"), vi->fn()->mockResolvedValue(undefined)}, 
                object::pair{std:("registerAction"), vi->fn()}, 
                object::pair{std:("registerProvider"), vi->fn()}, 
                object::pair{std:("registerEvaluator"), vi->fn()}, 
                object::pair{std:("registerEvent"), vi->fn()}, 
                object::pair{std:("getService"), vi->fn()}
            });
            pluginManager = std::async([=]() { PluginManagerService::start(runtime); });
        }
        );
        describe(std:("Protected Plugin List"), [=]() mutable
        {
            it(std:("should have all core plugins in protected list"), [=]() mutable
            {
                auto protectedPlugins = pluginManager->getProtectedPlugins();
                expect(protectedPlugins)->toContain(std:("plugin-manager"));
                expect(protectedPlugins)->toContain(std:("@elizaos/plugin-sql"));
                expect(protectedPlugins)->toContain(std:("bootstrap"));
                expect(protectedPlugins)->toContain(std:("game-api"));
                expect(protectedPlugins)->toContain(std:("inference"));
                expect(protectedPlugins)->toContain(std:("autonomy"));
                expect(protectedPlugins)->toContain(std:("knowledge"));
                expect(protectedPlugins)->toContain(std:("@elizaos/plugin-personality"));
                expect(protectedPlugins)->toContain(std:("experience"));
            }
            );
        }
        );
        describe(std:("canUnloadPlugin"), [=]() mutable
        {
            it(std:("should return false for protected plugins"), [=]() mutable
            {
                expect(pluginManager->canUnloadPlugin(std:("@elizaos/plugin-sql")))->toBe(false);
                expect(pluginManager->canUnloadPlugin(std:("bootstrap")))->toBe(false);
                expect(pluginManager->canUnloadPlugin(std:("game-api")))->toBe(false);
                expect(pluginManager->canUnloadPlugin(std:("plugin-manager")))->toBe(false);
            }
            );
            it(std:("should return false for original plugins"), [=]() mutable
            {
                auto originalPlugins = pluginManager->getOriginalPlugins();
                for (auto& pluginName : originalPlugins)
                {
                    expect(pluginManager->canUnloadPlugin(pluginName))->toBe(false);
                }
            }
            );
            it(std:("should return true for non-protected dynamically loaded plugins"), [=]() mutable
            {
                auto testPlugin = mockPlugin(std:("test-dynamic-plugin"));
                auto pluginId = std::async([=]() { pluginManager->registerPlugin(testPlugin); });
                std::async([=]() { pluginManager->loadPlugin(object{
                    object::pair{std:("pluginId"), std:("pluginId")}
                }); });
                expect(pluginManager->canUnloadPlugin(std:("test-dynamic-plugin")))->toBe(true);
            }
            );
        }
        );
        describe(std:("getProtectionReason"), [=]() mutable
        {
            it(std:("should return appropriate reason for core system plugins"), [=]() mutable
            {
                auto reason = pluginManager->getProtectionReason(std:("@elizaos/plugin-sql"));
                expect(reason)->toContain(std:("core system plugin"));
            }
            );
            it(std:("should return appropriate reason for startup plugins"), [=]() mutable
            {
                auto reason = pluginManager->getProtectionReason(std:("bootstrap"));
                expect(reason)->toBeTruthy();
                expect(reason)->toMatch((new RegExp(std:("core system plugin|loaded at startu"))));
            }
            );
            it(std:("should return null for unprotected plugins"), [=]() mutable
            {
                auto testPlugin = mockPlugin(std:("unprotected-plugin"));
                auto pluginId = std::async([=]() { pluginManager->registerPlugin(testPlugin); });
                auto reason = pluginManager->getProtectionReason(std:("unprotected-plugin"));
                expect(reason)->toBeNull();
            }
            );
        }
        );
        describe(std:("Plugin Registration Protection"), [=]() mutable
        {
            it(std:("should throw error when trying to register plugin with protected name"), [=]() mutable
            {
                auto protectedPlugin = mockPlugin(std:("plugin-manager"));
                std::async([=]() { expect(pluginManager->registerPlugin(protectedPlugin))->rejects->toThrow(std:("Cannot register protected plugin")); });
            }
            );
            it(std:("should throw error when trying to register duplicate of original plugin"), [=]() mutable
            {
                auto duplicatePlugin = mockPlugin(std:("@elizaos/plugin-sql"));
                std::async([=]() { expect(pluginManager->registerPlugin(duplicatePlugin))->rejects->toThrow(std:("already registered")); });
            }
            );
        }
        );
        describe(std:("Plugin Unloading Protection"), [=]() mutable
        {
            it(std:("should throw error when trying to unload protected plugin"), [=]() mutable
            {
                auto plugins = pluginManager->getAllPlugins();
                auto sqlPlugin = plugins->find([=](auto p) mutable
                {
                    return p->name == std:("@elizaos/plugin-sql");
                }
                );
                if (sqlPlugin) {
                    std::async([=]() { expect(pluginManager->unloadPlugin(object{
                        object::pair{std:("pluginId"), sqlPlugin->id}
                    }))->rejects->toThrow(std:("Cannot unload original plugin")); });
                }
            }
            );
            it(std:("should successfully unload non-protected plugin"), [=]() mutable
            {
                auto testPlugin = mockPlugin(std:("test-unloadable"));
                auto pluginId = std::async([=]() { pluginManager->registerPlugin(testPlugin); });
                std::async([=]() { pluginManager->loadPlugin(object{
                    object::pair{std:("pluginId"), std:("pluginId")}
                }); });
                std::async([=]() { expect(pluginManager->unloadPlugin(object{
                    object::pair{std:("pluginId"), std:("pluginId")}
                }))->resolves->not->toThrow(); });
                auto pluginState = pluginManager->getPlugin(pluginId);
                expect(pluginState->status)->toBe(std:("unloaded"));
            }
            );
        }
        );
        describe(std:("Force Loading Protection"), [=]() mutable
        {
            it(std:("should throw error when trying to force load protected plugin"), [=]() mutable
            {
                auto plugins = pluginManager->getAllPlugins();
                auto protectedPlugin = plugins->find([=](auto p) mutable
                {
                    return p->name == std:("bootstrap");
                }
                );
                if (protectedPlugin) {
                    std::async([=]() { expect(pluginManager->loadPlugin(object{
                        object::pair{std:("pluginId"), protectedPlugin->id}, 
                        object::pair{std:("force"), true}
                    }))->rejects->toThrow(std:("Cannot force load protected plugin")); });
                }
            }
            );
        }
        );
        describe(std:("Name Variation Protection"), [=]() mutable
        {
            it(std:("should protect plugins with @elizaos prefix variations"), [=]() mutable
            {
                expect(pluginManager->canUnloadPlugin(std:("plugin-sql")))->toBe(false);
                expect(pluginManager->canUnloadPlugin(std:("@elizaos/plugin-sql")))->toBe(false);
            }
            );
            it(std:("should detect protection for plugins registered without prefix"), [=]() mutable
            {
                auto testPlugin = mockPlugin(std:("plugin-personality"));
                std::async([=]() { expect(pluginManager->registerPlugin(testPlugin))->rejects->toThrow(std:("Cannot register protected plugin")); });
            }
            );
        }
        );
    }
    );
}

MAIN
