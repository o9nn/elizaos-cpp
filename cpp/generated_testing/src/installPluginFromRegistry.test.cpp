#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-plugin-manager/src/__tests__/unit/installPluginFromRegistry.test.h"

void Main(void)
{
    describe(std::string("installPluginFromRegistry"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<PluginManagerService>> mockPluginManager;
        beforeEach([=]() mutable
        {
            mockPluginManager = as<any>(object{
                object::pair{std::string("installPluginFromRegistry"), vi->fn()}
            });
            mockRuntime = as<any>(object{
                object::pair{std::string("getService"), vi->fn()->mockReturnValue(mockPluginManager)}, 
                object::pair{std::string("getSetting"), vi->fn()}, 
                object::pair{std::string("services"), object{
                    object::pair{std::string("get"), vi->fn()->mockReturnValue(mockPluginManager)}
                }}
            });
        }
        );
        describe(std::string("metadata"), [=]() mutable
        {
            it(std::string("should have correct action metadata"), [=]() mutable
            {
                expect(installPluginFromRegistryAction->name)->toBe(std::string("installPluginFromRegistry"));
                expect(installPluginFromRegistryAction->similes)->toContain(std::string("install plugin from registry"));
                expect(installPluginFromRegistryAction->description)->toBe(std::string("Install a plugin from the ElizaOS plugin registry"));
                expect(installPluginFromRegistryAction->validate)->toBeDefined();
                expect(installPluginFromRegistryAction->handler)->toBeDefined();
            }
            );
            it(std::string("should have similes defined"), [=]() mutable
            {
                expect(installPluginFromRegistryAction->similes)->toBeDefined();
                expect(Array->isArray(installPluginFromRegistryAction->similes))->toBe(true);
                expect(installPluginFromRegistryAction->similes)->toContain(std::string("install plugin from registry"));
                expect(installPluginFromRegistryAction->similes)->toContain(std::string("add plugin from registry"));
            }
            );
        }
        );
        describe(std::string("validate"), [=]() mutable
        {
            it(std::string("should validate when plugin manager service is available"), [=]() mutable
            {
                auto mockMessage = object{
                    object::pair{std::string("id"), std::string("12345678-1234-1234-1234-123456789012")}, 
                    object::pair{std::string("entityId"), std::string("12345678-1234-1234-1234-123456789015")}, 
                    object::pair{std::string("agentId"), std::string("12345678-1234-1234-1234-123456789013")}, 
                    object::pair{std::string("roomId"), std::string("12345678-1234-1234-1234-123456789014")}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("install plugin from registry")}
                    }}
                };
                auto result = std::async([=]() { installPluginFromRegistryAction->validate(mockRuntime, mockMessage); });
                expect(result)->toBe(true);
            }
            );
            it(std::string("should not validate when plugin manager service is not available"), [=]() mutable
            {
                (as<any>(mockRuntime->getService))["mockReturnValue"](nullptr);
                auto mockMessage = object{
                    object::pair{std::string("id"), std::string("12345678-1234-1234-1234-123456789012")}, 
                    object::pair{std::string("entityId"), std::string("12345678-1234-1234-1234-123456789015")}, 
                    object::pair{std::string("agentId"), std::string("12345678-1234-1234-1234-123456789013")}, 
                    object::pair{std::string("roomId"), std::string("12345678-1234-1234-1234-123456789014")}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("install plugin from registry")}
                    }}
                };
                auto result = std::async([=]() { installPluginFromRegistryAction->validate(mockRuntime, mockMessage); });
                expect(result)->toBe(true);
            }
            );
        }
        );
        describe(std::string("handler"), [=]() mutable
        {
            shared message = object{
                object::pair{std::string("id"), std::string("12345678-1234-1234-1234-123456789012")}, 
                object::pair{std::string("entityId"), std::string("12345678-1234-1234-1234-123456789015")}, 
                object::pair{std::string("agentId"), std::string("12345678-1234-1234-1234-123456789013")}, 
                object::pair{std::string("roomId"), std::string("12345678-1234-1234-1234-123456789014")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("install plugin from registry @elizaos/plugin-example")}
                }}
            };
            it(std::string("should extract plugin name from message and install"), [=]() mutable
            {
                (as<any>(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)))["mockResolvedValue"](object{
                    object::pair{std::string("name"), std::string("@elizaos/plugin-example")}, 
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("status"), std::string("installed")}
                });
                auto mockCallback = vi->fn();
                std::async([=]() { installPluginFromRegistryAction->handler(mockRuntime, message, undefined, undefined, mockCallback); });
                expect(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))->toHaveBeenCalledWith(std::string("@elizaos/plugin-example"));
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("text"), expect->stringContaining(std::string("Successfully installed plugin @elizaos/plugin-example v1.0.0"))}
                });
            }
            );
            it(std::string("should handle plugin names without @ prefix"), [=]() mutable
            {
                auto testMessage = utils::assign(object{
                    , 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("install plugin-example from registry")}
                    }}
                }, message);
                (as<any>(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)))["mockResolvedValue"](object{
                    object::pair{std::string("name"), std::string("plugin-example")}, 
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("status"), std::string("installed")}
                });
                auto mockCallback = vi->fn();
                std::async([=]() { installPluginFromRegistryAction->handler(mockRuntime, testMessage, undefined, undefined, mockCallback); });
                expect(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))->toHaveBeenCalledWith(std::string("plugin-example"));
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("text"), expect->stringContaining(std::string("Successfully installed plugin plugin-example v1.0.0"))}
                });
            }
            );
            it(std::string("should handle plugin names with version"), [=]() mutable
            {
                auto testMessage = utils::assign(object{
                    , 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("install @elizaos/plugin-example@1.2.3 from registry")}
                    }}
                }, message);
                (as<any>(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)))["mockResolvedValue"](object{
                    object::pair{std::string("name"), std::string("@elizaos/plugin-example")}, 
                    object::pair{std::string("version"), std::string("1.2.3")}, 
                    object::pair{std::string("status"), std::string("installed")}
                });
                auto mockCallback = vi->fn();
                std::async([=]() { installPluginFromRegistryAction->handler(mockRuntime, testMessage, undefined, undefined, mockCallback); });
                expect(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))->toHaveBeenCalledWith(std::string("@elizaos/plugin-example@1.2.3"));
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("text"), expect->stringContaining(std::string("Successfully installed plugin @elizaos/plugin-example v1.2.3"))}
                });
            }
            );
            it(std::string("should handle plugins that need configuration"), [=]() mutable
            {
                (as<any>(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)))["mockResolvedValue"](object{
                    object::pair{std::string("name"), std::string("@elizaos/plugin-example")}, 
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("status"), std::string("needs_configuration")}, 
                    object::pair{std::string("requiredEnvVars"), array<object>{ object{
                        object::pair{std::string("name"), std::string("API_KEY")}, 
                        object::pair{std::string("description"), std::string("API Key for service")}, 
                        object::pair{std::string("sensitive"), true}
                    }, object{
                        object::pair{std::string("name"), std::string("API_URL")}, 
                        object::pair{std::string("description"), std::string("API endpoint URL")}, 
                        object::pair{std::string("sensitive"), false}
                    } }}
                });
                auto mockCallback = vi->fn();
                std::async([=]() { installPluginFromRegistryAction->handler(mockRuntime, message, undefined, undefined, mockCallback); });
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("text"), expect->stringContaining(std::string("Plugin @elizaos/plugin-example has been installed but requires configuration"))}
                });
                auto callArg = const_(const_((as<any>(mockCallback))["mock"]["calls"])[0])[0]["text"];
                expect(callArg)->toContain(std::string("API_KEY: API Key for service (sensitive)"));
                expect(callArg)->toContain(std::string("API_URL: API endpoint URL"));
                expect(callArg)->toContain(std::string("Use "configure plugin" to set up the required environment variables"));
            }
            );
            it(std::string("should handle installation errors"), [=]() mutable
            {
                (as<any>(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)))["mockRejectedValue"](std::make_shared<Error>(std::string("Failed to install plugin")));
                auto mockCallback = vi->fn();
                std::async([=]() { expect(installPluginFromRegistryAction->handler(mockRuntime, message, undefined, undefined, mockCallback))->rejects->toThrow(std::string("Failed to install plugin")); });
            }
            );
            it(std::string("should handle missing plugin manager service"), [=]() mutable
            {
                (as<any>(mockRuntime->getService))["mockReturnValue"](nullptr);
                auto mockCallback = vi->fn();
                std::async([=]() { installPluginFromRegistryAction->handler(mockRuntime, message, undefined, undefined, mockCallback); });
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("text"), std::string("Plugin manager service not available")}
                });
            }
            );
            it(std::string("should handle missing plugin name"), [=]() mutable
            {
                auto testMessage = utils::assign(object{
                    , 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("install from registry")}
                    }}
                }, message);
                auto mockCallback = vi->fn();
                std::async([=]() { installPluginFromRegistryAction->handler(mockRuntime, testMessage, undefined, undefined, mockCallback); });
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("text"), expect->stringContaining(std::string("Please specify a plugin name to install"))}
                });
            }
            );
            it(std::string("should handle various command formats"), [=]() mutable
            {
                auto commandFormats = array<object>{ object{
                    object::pair{std::string("text"), std::string("add plugin @elizaos/test from registry")}, 
                    object::pair{std::string("expected"), std::string("@elizaos/test")}
                }, object{
                    object::pair{std::string("text"), std::string("download plugin some-plugin")}, 
                    object::pair{std::string("expected"), std::string("some-plugin")}
                }, object{
                    object::pair{std::string("text"), std::string("get plugin test-pkg")}, 
                    object::pair{std::string("expected"), std::string("test-pkg")}
                } };
                (as<any>(std::bind(&PluginManagerService::installPluginFromRegistry, mockPluginManager, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)))["mockResolvedValue"](object{
                    object::pair{std::string("name"), std::string("test")}, 
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("status"), std::string("installed")}
                });
                for (auto& format : commandFormats)
                {
                    auto testMessage = utils::assign(object{
                        , 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), format["text"]}
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
