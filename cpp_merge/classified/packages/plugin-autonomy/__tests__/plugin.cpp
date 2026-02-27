#include "plugin.test.h"

void Main(void)
{
    describe(std:("Autonomy Plugin Tests"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<Memory>> mockMessage;
        shared<std::shared_ptr<State>> mockState;
        shared updateAgentCalls = array<any>();
        beforeEach([=]() mutable
        {
            updateAgentCalls = array<any>();
            shared dynamicSettings = object{
                object::pair{std:("ADMIN_USER_ID"), std:("a1b2c3d4-5678-4abc-b123-123456789012")}, 
                object::pair{std:("AUTONOMY_ENABLED"), false}
            };
            mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                object::pair{std:("character"), object{
                    object::pair{std:("name"), std:("TestAgent")}, 
                    object::pair{std:("bio"), array<string>{ std:("Test agent for autonomy plugin") }}, 
                    object::pair{std:("settings"), object{}}
                }}, 
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-4567-8901-123456789012"))}, 
                object::pair{std:("getSetting"), [=](auto key) mutable
                {
                    return const_(dynamicSettings)[as<any>(key)];
                }
                }, 
                object::pair{std:("setSetting"), [=](auto key, auto value) mutable
                {
                    mockRuntime->character->settings = OR((mockRuntime->character->settings), (object{}));
                    mockRuntime->character->settings[key] = value;
                    (as<any>(dynamicSettings))[key] = value;
                }
                }, 
                object::pair{std:("updateAgent"), [=](auto agentId, auto updates) mutable
                {
                    updateAgentCalls->push(object{
                        object::pair{std:("agentId"), std:("agentId")}, 
                        object::pair{std:("updates"), std:("updates")}
                    });
                    return true;
                }
                }, 
                object::pair{std:("getMemories"), [=](auto params) mutable
                {
                    return array<any>();
                }
                }, 
                object::pair{std:("createMemory"), [=]() mutable
                {
                    return as<std::shared_ptr<UUID>>(std:("test-memory-id"));
                }
                }, 
                object::pair{std:("createRoom"), [=]() mutable
                {
                }
                }, 
                object::pair{std:("getRoom"), [=]() mutable
                {
                    return nullptr;
                }
                }, 
                object::pair{std:("sendMessageToTarget"), [=]() mutable
                {
                }
                }, 
                object::pair{std:("getService"), [=](auto serviceName) mutable
                {
                    if (serviceName == std:("autonomy")) {
                        return object{
                            object::pair{std:("getAutonomousRoomId"), [=]() mutable
                            {
                                return as<std::shared_ptr<UUID>>(std:("autonomous-room-id"));
                            }
                            }, 
                            object::pair{std:("isLoopRunning"), [=]() mutable
                            {
                                return false;
                            }
                            }, 
                            object::pair{std:("getLoopInterval"), [=]() mutable
                            {
                                return 30000;
                            }
                            }
                        };
                    }
                    return nullptr;
                }
                }, 
                object::pair{std:("processMessage"), [=]() mutable
                {
                }
                }
            }));
            mockMessage = as<std::shared_ptr<Memory>>(object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("test-msg-id"))}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("test-user-id"))}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("test-room-id"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Test message")}
                }}
            });
            mockState = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
        }
        );
        describe(std:("Plugin Structure Tests"), [=]() mutable
        {
            it(std:("should have correct plugin metadata"), [=]() mutable
            {
                expect(autonomyPlugin->name)->toBe(std:("autonomy"));
                expect(autonomyPlugin->description)->toContain(std:("autonomous"));
                expect(autonomyPlugin->services)->toBeDefined();
                expect(autonomyPlugin->actions)->toBeDefined();
                expect(autonomyPlugin->providers)->toBeDefined();
            }
            );
            it(std:("should required components"), [=]() mutable
            {
                expect(autonomyPlugin->services)->toHaveLength(1);
                expect(autonomyPlugin->actions)->toHaveLength(1);
                expect(autonomyPlugin->providers)->toHaveLength(2);
            }
            );
            it(std:("should have the correct service"), [=]() mutable
            {
                auto service = const_(autonomyPlugin->services)[0];
                expect(service)->toBe(AutonomyService);
            }
            );
            it(std:("should have the correct actions"), [=]() mutable
            {
                auto actionNames = OR((autonomyPlugin->actions->map([=](auto a) mutable
                {
                    return a["name"];
                }
                )), (array<any>()));
                expect(actionNames)->toContain(std:("SEND_TO_ADMIN"));
            }
            );
            it(std:("should have the correct providers"), [=]() mutable
            {
                auto providerNames = OR((autonomyPlugin->providers->map([=](auto p) mutable
                {
                    return p["name"];
                }
                )), (array<any>()));
                expect(providerNames)->toContain(std:("ADMIN_CHAT_HISTORY"));
                expect(providerNames)->toContain(std:("AUTONOMY_STATUS"));
            }
            );
        }
        );
        describe(std:("AutonomyService Tests"), [=]() mutable
        {
            it(std:("should be constructable"), [=]() mutable
            {
                expect([=]() mutable
                {
                    return std::make_shared<AutonomyService>(as<std::shared_ptr<IAgentRuntime>>(mockRuntime));
                }
                )->not->toThrow();
            }
            );
            it(std:("should have correct service metadata"), [=]() mutable
            {
                expect(AutonomyService::serviceType)->toBe(std:("AUTONOMOUS"));
            }
            );
        }
        );
        describe(std:("AdminChat Provider Tests"), [=]() mutable
        {
            it(std:("should have correct metadata"), [=]() mutable
            {
                expect(adminChatProvider->name)->toBe(std:("ADMIN_CHAT_HISTORY"));
                expect(adminChatProvider->description)->toContain(std:("conversation history"));
            }
            );
            it(std:("should handle no admin configured in autonomous context"), [=]() mutable
            {
                auto runtimeWithoutAdmin = utils::assign(object{
                    , 
                    object::pair{std:("getSetting"), [=]() mutable
                    {
                        return nullptr;
                    }
                    }
                }, mockRuntime);
                auto autonomousMessage = utils::assign(object{
                    , 
                    object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("autonomous-room-id"))}
                }, mockMessage);
                auto result = std::async([=]() { adminChatProvider->get(as<std::shared_ptr<IAgentRuntime>>(runtimeWithoutAdmin), autonomousMessage, mockState); });
                expect(result->text)->toContain(std:("No admin user configured"));
                expect(result->data->adminConfigured)->toBe(false);
            }
            );
            it(std:("should work when admin is configured in autonomous context"), [=]() mutable
            {
                auto autonomousMessage = utils::assign(object{
                    , 
                    object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("autonomous-room-id"))}
                }, mockMessage);
                auto result = std::async([=]() { adminChatProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), autonomousMessage, mockState); });
                expect(result->text)->toContain(std:("No recent messages found"));
                expect(result->data->adminConfigured)->toBe(true);
                expect(result->data->adminUserId)->toBe(std:("a1b2c3d4-5678-4abc-b123-123456789012"));
            }
            );
        }
        );
        describe(std:("Send To Admin Action Tests"), [=]() mutable
        {
            it(std:("should have correct metadata"), [=]() mutable
            {
                expect(sendToAdminAction->name)->toBe(std:("SEND_TO_ADMIN"));
                expect(sendToAdminAction->description)->toContain(std:("Send"));
                expect(sendToAdminAction->validate)->toBeDefined();
                expect(sendToAdminAction->handler)->toBeDefined();
            }
            );
            it(std:("should validate messages in autonomous context"), [=]() mutable
            {
                auto autonomousMessage = utils::assign(object{
                    , 
                    object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("autonomous-room-id"))}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("I need to tell the admin something")}
                    }}
                }, mockMessage);
                auto isValid = std::async([=]() { sendToAdminAction->validate(mockRuntime, autonomousMessage); });
                expect(isValid)->toBe(true);
            }
            );
            it(std:("should reject messages outside autonomous context"), [=]() mutable
            {
                auto regularMessage = utils::assign(object{
                    , 
                    object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("regular-room-id"))}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("I need to tell the admin something")}
                    }}
                }, mockMessage);
                auto isValid = std::async([=]() { sendToAdminAction->validate(mockRuntime, regularMessage); });
                expect(isValid)->toBe(false);
            }
            );
        }
        );
        describe(std:("Plugin Integration Tests"), [=]() mutable
        {
            it(std:("should have all components properly connected"), [=]() mutable
            {
                expect(autonomyPlugin->services)->toContain(AutonomyService);
                expect(autonomyPlugin->actions)->toContain(sendToAdminAction);
                expect(autonomyPlugin->providers)->toContain(adminChatProvider);
                expect(autonomyPlugin->providers)->toContain(autonomyStatusProvider);
            }
            );
            it(std:("should have consistent naming"), [=]() mutable
            {
                expect(autonomyPlugin->name)->toBe(std:("autonomy"));
                expect(AutonomyService::serviceType)->toBe(std:("AUTONOMOUS"));
                expect(adminChatProvider->name)->toBe(std:("ADMIN_CHAT_HISTORY"));
                expect(autonomyStatusProvider->name)->toBe(std:("AUTONOMY_STATUS"));
                expect(sendToAdminAction->name)->toBe(std:("SEND_TO_ADMIN"));
            }
            );
        }
        );
        describe(std:("Autonomy Service Integration Tests"), [=]() mutable
        {
            it(std:("should enable autonomy via settings"), [=]() mutable
            {
                auto service = std::make_shared<AutonomyService>(mockRuntime);
                std::async([=]() { service->initialize(); });
                std::async([=]() { service->enableAutonomy(); });
                expect(mockRuntime->character->settings->AUTONOMY_ENABLED)->toBe(true);
                expect(service->isLoopRunning())->toBe(true);
                std::async([=]() { service->stop(); });
            }
            );
            it(std:("should disable autonomy via settings"), [=]() mutable
            {
                auto service = std::make_shared<AutonomyService>(mockRuntime);
                std::async([=]() { service->initialize(); });
                std::async([=]() { service->enableAutonomy(); });
                std::async([=]() { service->disableAutonomy(); });
                expect(mockRuntime->character->settings->AUTONOMY_ENABLED)->toBe(false);
                expect(service->isLoopRunning())->toBe(false);
            }
            );
            it(std:("should provide status information"), [=]() mutable
            {
                auto service = std::make_shared<AutonomyService>(mockRuntime);
                std::async([=]() { service->initialize(); });
                auto status = service->getStatus();
                expect(status["enabled"])->toBe(false);
                expect(status["interval"])->toBe(1000);
                std::async([=]() { service->enableAutonomy(); });
                status = service->getStatus();
                expect(status["enabled"])->toBe(true);
                expect(status["running"])->toBe(true);
                expect(status["interval"])->toBe(1000);
                std::async([=]() { service->stop(); });
            }
            );
            it(std:("should have correct autonomous room ID"), [=]() mutable
            {
                auto service = std::make_shared<AutonomyService>(mockRuntime);
                std::async([=]() { service->initialize(); });
                auto roomId = service->getAutonomousRoomId();
                expect(roomId)->toMatch((new RegExp(std:("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$"))));
            }
            );
        }
        );
    }
    );
}

MAIN
