#include "installPluginFromRegistry.test.h"

void Main(void)
{
    describe(std:("installPluginFromRegistry"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<PluginManagerService>> mockPluginManager;
        beforeEach([=]() mutable
        {
            mockPluginManager = as<any>(object{
                object::pair{std:("installPluginFromRegistry"), vi->fn()}
            });
            mockRuntime = as<any>(object{
                object::pair{std:("getService"), vi->fn()->mockReturnValue(mockPluginManager)}, 
                object::pair{std:("getSetting"), vi->fn()}, 
                object::pair{std:("services"), object{
                    object::pair{std:("get"), vi->fn()->mockReturnValue(mockPluginManager)}
                }}
            });
        }
        );
        describe(std:("metadata"), [=]() mutable
        {
            it(std:("should have correct action metadata"), [=]() mutable
            {
                expect(installPluginFromRegistryAction->name)->toBe(std:("installPluginFromRegistry"));
                expect(installPluginFromRegistryAction->similes)->toContain(std:("install plugin from registry"));
                expect(installPluginFromRegistryAction->description)->toBe(std:("Install a plugin from the ElizaOS plugin registry"));
                expect(installPluginFromRegistryAction->validate)->toBeDefined();
                expect(installPluginFromRegistryAction->handler)->toBeDefined();
            }
            );
            it(std:("should have similes defined"), [=]() mutable
            {
                expect(installPluginFromRegistryAction->similes)->toBeDefined();
                expect(Array->isArray(installPluginFromRegistryAction->similes))->toBe(true);
                expect(installPluginFromRegistryAction->similes)->toContain(std:("install plugin from registry"));
                expect(installPluginFromRegistryAction->similes)->toContain(std:("add plugin from registry"));
            }
            );
        }
        );
        describe(std:("validate"), [=]() mutable
        {
            it(std:("should validate when plugin manager service is available"), [=]() mutable
            {
                auto mockMessage = object{
                    object::pair{std:("id"), std:("12345678-1234-1234-1234-123456789012")}, 
                    object::pair{std:("entityId"), std:("12345678-1234-1234-1234-123456789015")}, 
                    object::pair{std:("agentId"), std:("12345678-1234-1234-1234-123456789013")}, 
                    object::pair{std:("roomId"), std:("12345678-1234-1234-1234-123456789014")}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("install plugin from registry")}
                    }}
                };
                auto result = std::async([=]() { installPluginFromRegistryAction->validate(mockRuntime, mockMessage); });
                expect(result)->toBe(true);
            }
            );
            it(std:("should not validate when plugin manager service is not available"), [=]() mutable
            {
                (as<any>(mockRuntime->getService))["mockReturnValue"](nullptr);
                auto mockMessage = object{
                    object::pair{std:("id"), std:("12345678-1234-1234-1234-123456789012")}, 
                    object::pair{std:("entityId"), std:("12345678-1234-1234-1234-123456789015")}, 
                    object::pair{std:("agentId"), std:("12345678-1234-1234-1234-123456789013")}, 
                    object::pair{std:("roomId"), std:("12345678-1234-1234-1234-123456789014")}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("install plugin from registry")}
                    }}
                };
                auto result = std::async([=]() { installPluginFromRegistryAction->validate(mockRuntime, mockMessage); });
                expect(result)->toBe(true);
            }
            );
        }
        );
        describe(std:("handler"), [=]() mutable
        {
            shared message = object{
                object::pair{std:("id"), std:("12345678-1234-1234-1234-123456789012")}, 
                object::pair{std:("entityId"), std:("12345678-1234-1234-1234-123456789015")}, 
                object::pair{std:("agentId"), std:("12345678-1234-1234-1234-123456789013")}, 
                object::pair{std:("roomId"), std:("12345678-1234-1234-1234-123456789014")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("install plugin from registry @elizaos/plugin-example")}
                }}
            };
            it(std:("should extract plugin name from message and install"), [=]() mutable
            {
                (as<any>(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)))["mockResolvedValue"](object{
                    object::pair{std:("name"), std:("@elizaos/plugin-example")}, 
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("status"), std:("installed")}
                });
                auto mockCallback = vi->fn();
                std::async([=]() { installPluginFromRegistryAction->handler(mockRuntime, message, undefined, undefined, mockCallback); });
                expect(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))->toHaveBeenCalledWith(std:("@elizaos/plugin-example"));
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("text"), expect->stringContaining(std:("Successfully installed plugin @elizaos/plugin-example v1.0.0"))}
                });
            }
            );
            it(std:("should handle plugin names without @ prefix"), [=]() mutable
            {
                auto testMessage = utils::assign(object{
                    , 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("install plugin-example from registry")}
                    }}
                }, message);
                (as<any>(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)))["mockResolvedValue"](object{
                    object::pair{std:("name"), std:("plugin-example")}, 
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("status"), std:("installed")}
                });
                auto mockCallback = vi->fn();
                std::async([=]() { installPluginFromRegistryAction->handler(mockRuntime, testMessage, undefined, undefined, mockCallback); });
                expect(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))->toHaveBeenCalledWith(std:("plugin-example"));
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("text"), expect->stringContaining(std:("Successfully installed plugin plugin-example v1.0.0"))}
                });
            }
            );
            it(std:("should handle plugin names with version"), [=]() mutable
            {
                auto testMessage = utils::assign(object{
                    , 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("install @elizaos/plugin-example@1.2.3 from registry")}
                    }}
                }, message);
                (as<any>(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)))["mockResolvedValue"](object{
                    object::pair{std:("name"), std:("@elizaos/plugin-example")}, 
                    object::pair{std:("version"), std:("1.2.3")}, 
                    object::pair{std:("status"), std:("installed")}
                });
                auto mockCallback = vi->fn();
                std::async([=]() { installPluginFromRegistryAction->handler(mockRuntime, testMessage, undefined, undefined, mockCallback); });
                expect(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))->toHaveBeenCalledWith(std:("@elizaos/plugin-example@1.2.3"));
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("text"), expect->stringContaining(std:("Successfully installed plugin @elizaos/plugin-example v1.2.3"))}
                });
            }
            );
            it(std:("should handle plugins that need configuration"), [=]() mutable
            {
                (as<any>(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)))["mockResolvedValue"](object{
                    object::pair{std:("name"), std:("@elizaos/plugin-example")}, 
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("status"), std:("needs_configuration")}, 
                    object::pair{std:("requiredEnvVars"), array<object>{ object{
                        object::pair{std:("name"), std:("API_KEY")}, 
                        object::pair{std:("description"), std:("API Key for service")}, 
                        object::pair{std:("sensitive"), true}
                    }, object{
                        object::pair{std:("name"), std:("API_URL")}, 
                        object::pair{std:("description"), std:("API endpoint URL")}, 
                        object::pair{std:("sensitive"), false}
                    } }}
                });
                auto mockCallback = vi->fn();
                std::async([=]() { installPluginFromRegistryAction->handler(mockRuntime, message, undefined, undefined, mockCallback); });
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("text"), expect->stringContaining(std:("Plugin @elizaos/plugin-example has been installed but requires configuration"))}
                });
                auto callArg = const_(const_((as<any>(mockCallback))["mock"]["calls"])[0])[0]["text"];
                expect(callArg)->toContain(std:("API_KEY: API Key for service (sensitive)"));
                expect(callArg)->toContain(std:("API_URL: API endpoint URL"));
                expect(callArg)->toContain(std:("Use "configure plugin" to set up the required environment variables"));
            }
            );
            it(std:("should handle installation errors"), [=]() mutable
            {
                (as<any>(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)))["mockRejectedValue"](std::make_shared<Error>(std:("Failed to install plugin")));
                auto mockCallback = vi->fn();
                std::async([=]() { expect(installPluginFromRegistryAction->handler(mockRuntime, message, undefined, undefined, mockCallback))->rejects->toThrow(std:("Failed to install plugin")); });
            }
            );
            it(std:("should handle missing plugin manager service"), [=]() mutable
            {
                (as<any>(mockRuntime->getService))["mockReturnValue"](nullptr);
                auto mockCallback = vi->fn();
                std::async([=]() { installPluginFromRegistryAction->handler(mockRuntime, message, undefined, undefined, mockCallback); });
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("text"), std:("Plugin manager service not available")}
                });
            }
            );
            it(std:("should handle missing plugin name"), [=]() mutable
            {
                auto testMessage = utils::assign(object{
                    , 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("install from registry")}
                    }}
                }, message);
                auto mockCallback = vi->fn();
                std::async([=]() { installPluginFromRegistryAction->handler(mockRuntime, testMessage, undefined, undefined, mockCallback); });
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("text"), expect->stringContaining(std:("Please specify a plugin name to install"))}
                });
            }
            );
            it(std:("should handle various command formats"), [=]() mutable
            {
                auto commandFormats = array<object>{ object{
                    object::pair{std:("text"), std:("add plugin @elizaos/test from registry")}, 
                    object::pair{std:("expected"), std:("@elizaos/test")}
                }, object{
                    object::pair{std:("text"), std:("download plugin some-plugin")}, 
                    object::pair{std:("expected"), std:("some-plugin")}
                }, object{
                    object::pair{std:("text"), std:("get plugin test-pkg")}, 
                    object::pair{std:("expected"), std:("test-pkg")}
                } };
                (as<any>(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)))["mockResolvedValue"](object{
                    object::pair{std:("name"), std:("test")}, 
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("status"), std:("installed")}
                });
                for (auto& format : commandFormats)
                {
                    auto testMessage = utils::assign(object{
                        , 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), format["text"]}
                        }}
                    }, message);
                    auto mockCallback = vi->fn();
                    std::async([=]() { installPluginFromRegistryAction->handler(mockRuntime, testMessage, undefined, undefined, mockCallback); });
                    expect(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))->toHaveBeenCalledWith(format["expected"]);
                }
            }
            );
        }
        );
    }
    );
}

MAIN
