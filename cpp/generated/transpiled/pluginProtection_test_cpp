#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-plugin-manager/src/__tests__/unit/pluginProtection.test.h"

void Main(void)
{
    describe(std::string("Plugin Protection Mechanism"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        shared<std::shared_ptr<PluginManagerService>> pluginManager;
        shared mockPlugin = [=](auto name) mutable
        {
            return (object{
                object::pair{std::string("name"), std::string("name")}, 
                object::pair{std::string("description"), std::string("Mock ") + name + std::string(" plugin")}, 
                object::pair{std::string("actions"), array<object>{ object{
                    object::pair{std::string("name"), string_empty + name + std::string("_ACTION")}, 
                    object::pair{std::string("similes"), array<any>()}, 
                    object::pair{std::string("description"), std::string("Test action")}, 
                    object::pair{std::string("validate"), [=]() mutable
                    {
                        return true;
                    }
                    }, 
                    object::pair{std::string("handler"), [=]() mutable
                    {
                        return (object{
                            object::pair{std::string("text"), std::string("Success")}, 
                            object::pair{std::string("success"), true}
                        });
                    }
                    }
                } }}
            });
        };
        beforeEach([=]() mutable
        {
            auto originalPlugins = array<any>{ mockPlugin(std::string("@elizaos/plugin-sql")), mockPlugin(std::string("bootstrap")), mockPlugin(std::string("game-api")), mockPlugin(std::string("inference")), mockPlugin(std::string("autonomy")), mockPlugin(std::string("knowledge")), mockPlugin(std::string("@elizaos/plugin-personality")), mockPlugin(std::string("experience")) };
            runtime = as<any>(object{
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent-id"))}, 
                object::pair{std::string("plugins"), array<any>{ originalPlugins }}, 
                object::pair{std::string("actions"), array<any>()}, 
                object::pair{std::string("providers"), array<any>()}, 
                object::pair{std::string("evaluators"), array<any>()}, 
                object::pair{std::string("services"), std::make_shared<Map>()}, 
                object::pair{std::string("registerPlugin"), vi->fn()->mockResolvedValue(undefined)}, 
                object::pair{std::string("registerAction"), vi->fn()}, 
                object::pair{std::string("registerProvider"), vi->fn()}, 
                object::pair{std::string("registerEvaluator"), vi->fn()}, 
                object::pair{std::string("registerEvent"), vi->fn()}, 
                object::pair{std::string("getService"), vi->fn()}
            });
            pluginManager = std::async([=]() { PluginManagerService::start(runtime); });
        }
        );
        describe(std::string("Protected Plugin List"), [=]() mutable
        {
            it(std::string("should have all core plugins in protected list"), [=]() mutable
            {
                auto protectedPlugins = pluginManager->getProtectedPlugins();
                expect(protectedPlugins)->toContain(std::string("plugin-manager"));
                expect(protectedPlugins)->toContain(std::string("@elizaos/plugin-sql"));
                expect(protectedPlugins)->toContain(std::string("bootstrap"));
                expect(protectedPlugins)->toContain(std::string("game-api"));
                expect(protectedPlugins)->toContain(std::string("inference"));
                expect(protectedPlugins)->toContain(std::string("autonomy"));
                expect(protectedPlugins)->toContain(std::string("knowledge"));
                expect(protectedPlugins)->toContain(std::string("@elizaos/plugin-personality"));
                expect(protectedPlugins)->toContain(std::string("experience"));
            }
            );
        }
        );
        describe(std::string("canUnloadPlugin"), [=]() mutable
        {
            it(std::string("should return false for protected plugins"), [=]() mutable
            {
                expect(pluginManager->canUnloadPlugin(std::string("@elizaos/plugin-sql")))->toBe(false);
                expect(pluginManager->canUnloadPlugin(std::string("bootstrap")))->toBe(false);
                expect(pluginManager->canUnloadPlugin(std::string("game-api")))->toBe(false);
                expect(pluginManager->canUnloadPlugin(std::string("plugin-manager")))->toBe(false);
            }
            );
            it(std::string("should return false for original plugins"), [=]() mutable
            {
                auto originalPlugins = pluginManager->getOriginalPlugins();
                for (auto& pluginName : originalPlugins)
                {
                    expect(pluginManager->canUnloadPlugin(pluginName))->toBe(false);
                }
            }
            );
            it(std::string("should return true for non-protected dynamically loaded plugins"), [=]() mutable
            {
                auto testPlugin = mockPlugin(std::string("test-dynamic-plugin"));
                auto pluginId = std::async([=]() { pluginManager->registerPlugin(testPlugin); });
                std::async([=]() { pluginManager->loadPlugin(object{
                    object::pair{std::string("pluginId"), std::string("pluginId")}
                }); });
                expect(pluginManager->canUnloadPlugin(std::string("test-dynamic-plugin")))->toBe(true);
            }
            );
        }
        );
        describe(std::string("getProtectionReason"), [=]() mutable
        {
            it(std::string("should return appropriate reason for core system plugins"), [=]() mutable
            {
                auto reason = pluginManager->getProtectionReason(std::string("@elizaos/plugin-sql"));
                expect(reason)->toContain(std::string("core system plugin"));
            }
            );
            it(std::string("should return appropriate reason for startup plugins"), [=]() mutable
            {
                auto reason = pluginManager->getProtectionReason(std::string("bootstrap"));
                expect(reason)->toBeTruthy();
                expect(reason)->toMatch((new RegExp(std::string("core system plugin|loaded at startu"))));
            }
            );
            it(std::string("should return null for unprotected plugins"), [=]() mutable
            {
                auto testPlugin = mockPlugin(std::string("unprotected-plugin"));
                auto pluginId = std::async([=]() { pluginManager->registerPlugin(testPlugin); });
                auto reason = pluginManager->getProtectionReason(std::string("unprotected-plugin"));
                expect(reason)->toBeNull();
            }
            );
        }
        );
        describe(std::string("Plugin Registration Protection"), [=]() mutable
        {
            it(std::string("should throw error when trying to register plugin with protected name"), [=]() mutable
            {
                auto protectedPlugin = mockPlugin(std::string("plugin-manager"));
                std::async([=]() { expect(pluginManager->registerPlugin(protectedPlugin))->rejects->toThrow(std::string("Cannot register protected plugin")); });
            }
            );
            it(std::string("should throw error when trying to register duplicate of original plugin"), [=]() mutable
            {
                auto duplicatePlugin = mockPlugin(std::string("@elizaos/plugin-sql"));
                std::async([=]() { expect(pluginManager->registerPlugin(duplicatePlugin))->rejects->toThrow(std::string("already registered")); });
            }
            );
        }
        );
        describe(std::string("Plugin Unloading Protection"), [=]() mutable
        {
            it(std::string("should throw error when trying to unload protected plugin"), [=]() mutable
            {
                auto plugins = pluginManager->getAllPlugins();
                auto sqlPlugin = plugins->find([=](auto p) mutable
                {
                    return p->name == std::string("@elizaos/plugin-sql");
                }
                );
                if (sqlPlugin) {
                    std::async([=]() { expect(pluginManager->unloadPlugin(object{
                        object::pair{std::string("pluginId"), sqlPlugin->id}
                    }))->rejects->toThrow(std::string("Cannot unload original plugin")); });
                }
            }
            );
            it(std::string("should successfully unload non-protected plugin"), [=]() mutable
            {
                auto testPlugin = mockPlugin(std::string("test-unloadable"));
                auto pluginId = std::async([=]() { pluginManager->registerPlugin(testPlugin); });
                std::async([=]() { pluginManager->loadPlugin(object{
                    object::pair{std::string("pluginId"), std::string("pluginId")}
                }); });
                std::async([=]() { expect(pluginManager->unloadPlugin(object{
                    object::pair{std::string("pluginId"), std::string("pluginId")}
                }))->resolves->not->toThrow(); });
                auto pluginState = pluginManager->getPlugin(pluginId);
                expect(pluginState->status)->toBe(std::string("unloaded"));
            }
            );
        }
        );
        describe(std::string("Force Loading Protection"), [=]() mutable
        {
            it(std::string("should throw error when trying to force load protected plugin"), [=]() mutable
            {
                auto plugins = pluginManager->getAllPlugins();
                auto protectedPlugin = plugins->find([=](auto p) mutable
                {
                    return p->name == std::string("bootstrap");
                }
                );
                if (protectedPlugin) {
                    std::async([=]() { expect(pluginManager->loadPlugin(object{
                        object::pair{std::string("pluginId"), protectedPlugin->id}, 
                        object::pair{std::string("force"), true}
                    }))->rejects->toThrow(std::string("Cannot force load protected plugin")); });
                }
            }
            );
        }
        );
        describe(std::string("Name Variation Protection"), [=]() mutable
        {
            it(std::string("should protect plugins with @elizaos prefix variations"), [=]() mutable
            {
                expect(pluginManager->canUnloadPlugin(std::string("plugin-sql")))->toBe(false);
                expect(pluginManager->canUnloadPlugin(std::string("@elizaos/plugin-sql")))->toBe(false);
            }
            );
            it(std::string("should detect protection for plugins registered without prefix"), [=]() mutable
            {
                auto testPlugin = mockPlugin(std::string("plugin-personality"));
                std::async([=]() { expect(pluginManager->registerPlugin(testPlugin))->rejects->toThrow(std::string("Cannot register protected plugin")); });
            }
            );
        }
        );
    }
    );
}

MAIN
