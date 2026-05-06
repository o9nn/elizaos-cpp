#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autonomy/__tests__/plugin.test.h"

void Main(void)
{
    describe(std::string("Autonomy Plugin Tests"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<Memory>> mockMessage;
        shared<std::shared_ptr<State>> mockState;
        shared updateAgentCalls = array<any>();
        beforeEach([=]() mutable
        {
            updateAgentCalls = array<any>();
            shared dynamicSettings = object{
                object::pair{std::string("ADMIN_USER_ID"), std::string("a1b2c3d4-5678-4abc-b123-123456789012")}, 
                object::pair{std::string("AUTONOMY_ENABLED"), false}
            };
            mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                object::pair{std::string("character"), object{
                    object::pair{std::string("name"), std::string("TestAgent")}, 
                    object::pair{std::string("bio"), array<string>{ std::string("Test agent for autonomy plugin") }}, 
                    object::pair{std::string("settings"), object{}}
                }}, 
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("12345678-1234-4567-8901-123456789012"))}, 
                object::pair{std::string("getSetting"), [=](auto key) mutable
                {
                    return const_(dynamicSettings)[as<any>(key)];
                }
                }, 
                object::pair{std::string("setSetting"), [=](auto key, auto value) mutable
                {
                    mockRuntime->character->settings = OR((mockRuntime->character->settings), (object{}));
                    mockRuntime->character->settings[key] = value;
                    (as<any>(dynamicSettings))[key] = value;
                }
                }, 
                object::pair{std::string("updateAgent"), [=](auto agentId, auto updates) mutable
                {
                    updateAgentCalls->push(object{
                        object::pair{std::string("agentId"), std::string("agentId")}, 
                        object::pair{std::string("updates"), std::string("updates")}
                    });
                    return true;
                }
                }, 
                object::pair{std::string("getMemories"), [=](auto params) mutable
                {
                    return array<any>();
                }
                }, 
                object::pair{std::string("createMemory"), [=]() mutable
                {
                    return as<std::shared_ptr<UUID>>(std::string("test-memory-id"));
                }
                }, 
                object::pair{std::string("createRoom"), [=]() mutable
                {
                }
                }, 
                object::pair{std::string("getRoom"), [=]() mutable
                {
                    return nullptr;
                }
                }, 
                object::pair{std::string("sendMessageToTarget"), [=]() mutable
                {
                }
                }, 
                object::pair{std::string("getService"), [=](auto serviceName) mutable
                {
                    if (serviceName == std::string("autonomy")) {
                        return object{
                            object::pair{std::string("getAutonomousRoomId"), [=]() mutable
                            {
                                return as<std::shared_ptr<UUID>>(std::string("autonomous-room-id"));
                            }
                            }, 
                            object::pair{std::string("isLoopRunning"), [=]() mutable
                            {
                                return false;
                            }
                            }, 
                            object::pair{std::string("getLoopInterval"), [=]() mutable
                            {
                                return 30000;
                            }
                            }
                        };
                    }
                    return nullptr;
                }
                }, 
                object::pair{std::string("processMessage"), [=]() mutable
                {
                }
                }
            }));
            mockMessage = as<std::shared_ptr<Memory>>(object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("test-msg-id"))}, 
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("test-user-id"))}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("test-room-id"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Test message")}
                }}
            });
            mockState = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
        }
        );
        describe(std::string("Plugin Structure Tests"), [=]() mutable
        {
            it(std::string("should have correct plugin metadata"), [=]() mutable
            {
                expect(autonomyPlugin->name)->toBe(std::string("autonomy"));
                expect(autonomyPlugin->description)->toContain(std::string("autonomous"));
                expect(autonomyPlugin->services)->toBeDefined();
                expect(autonomyPlugin->actions)->toBeDefined();
                expect(autonomyPlugin->providers)->toBeDefined();
            }
            );
            it(std::string("should required components"), [=]() mutable
            {
                expect(autonomyPlugin->services)->toHaveLength(1);
                expect(autonomyPlugin->actions)->toHaveLength(1);
                expect(autonomyPlugin->providers)->toHaveLength(2);
            }
            );
            it(std::string("should have the correct service"), [=]() mutable
            {
                auto service = const_(autonomyPlugin->services)[0];
                expect(service)->toBe(AutonomyService);
            }
            );
            it(std::string("should have the correct actions"), [=]() mutable
            {
                auto actionNames = OR((autonomyPlugin->actions->map([=](auto a) mutable
                {
                    return a["name"];
                }
                )), (array<any>()));
                expect(actionNames)->toContain(std::string("SEND_TO_ADMIN"));
            }
            );
            it(std::string("should have the correct providers"), [=]() mutable
            {
                auto providerNames = OR((autonomyPlugin->providers->map([=](auto p) mutable
                {
                    return p["name"];
                }
                )), (array<any>()));
                expect(providerNames)->toContain(std::string("ADMIN_CHAT_HISTORY"));
                expect(providerNames)->toContain(std::string("AUTONOMY_STATUS"));
            }
            );
        }
        );
        describe(std::string("AutonomyService Tests"), [=]() mutable
        {
            it(std::string("should be constructable"), [=]() mutable
            {
                expect([=]() mutable
                {
                    return std::make_shared<AutonomyService>(as<std::shared_ptr<IAgentRuntime>>(mockRuntime));
                }
                )->not->toThrow();
            }
            );
            it(std::string("should have correct service metadata"), [=]() mutable
            {
                expect(AutonomyService::serviceType)->toBe(std::string("AUTONOMOUS"));
            }
            );
        }
        );
        describe(std::string("AdminChat Provider Tests"), [=]() mutable
        {
            it(std::string("should have correct metadata"), [=]() mutable
            {
                expect(adminChatProvider->name)->toBe(std::string("ADMIN_CHAT_HISTORY"));
                expect(adminChatProvider->description)->toContain(std::string("conversation history"));
            }
            );
            it(std::string("should handle no admin configured in autonomous context"), [=]() mutable
            {
                auto runtimeWithoutAdmin = utils::assign(object{
                    , 
                    object::pair{std::string("getSetting"), [=]() mutable
                    {
                        return nullptr;
                    }
                    }
                }, mockRuntime);
                auto autonomousMessage = utils::assign(object{
                    , 
                    object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("autonomous-room-id"))}
                }, mockMessage);
                auto result = std::async([=]() { adminChatProvider->get(as<std::shared_ptr<IAgentRuntime>>(runtimeWithoutAdmin), autonomousMessage, mockState); });
                expect(result->text)->toContain(std::string("No admin user configured"));
                expect(result->data->adminConfigured)->toBe(false);
            }
            );
            it(std::string("should work when admin is configured in autonomous context"), [=]() mutable
            {
                auto autonomousMessage = utils::assign(object{
                    , 
                    object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("autonomous-room-id"))}
                }, mockMessage);
                auto result = std::async([=]() { adminChatProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), autonomousMessage, mockState); });
                expect(result->text)->toContain(std::string("No recent messages found"));
                expect(result->data->adminConfigured)->toBe(true);
                expect(result->data->adminUserId)->toBe(std::string("a1b2c3d4-5678-4abc-b123-123456789012"));
            }
            );
        }
        );
        describe(std::string("Send To Admin Action Tests"), [=]() mutable
        {
            it(std::string("should have correct metadata"), [=]() mutable
            {
                expect(sendToAdminAction->name)->toBe(std::string("SEND_TO_ADMIN"));
                expect(sendToAdminAction->description)->toContain(std::string("Send"));
                expect(sendToAdminAction->validate)->toBeDefined();
                expect(sendToAdminAction->handler)->toBeDefined();
            }
            );
            it(std::string("should validate messages in autonomous context"), [=]() mutable
            {
                auto autonomousMessage = utils::assign(object{
                    , 
                    object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("autonomous-room-id"))}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("I need to tell the admin something")}
                    }}
                }, mockMessage);
                auto isValid = std::async([=]() { sendToAdminAction->validate(mockRuntime, autonomousMessage); });
                expect(isValid)->toBe(true);
            }
            );
            it(std::string("should reject messages outside autonomous context"), [=]() mutable
            {
                auto regularMessage = utils::assign(object{
                    , 
                    object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("regular-room-id"))}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("I need to tell the admin something")}
                    }}
                }, mockMessage);
                auto isValid = std::async([=]() { sendToAdminAction->validate(mockRuntime, regularMessage); });
                expect(isValid)->toBe(false);
            }
            );
        }
        );
        describe(std::string("Plugin Integration Tests"), [=]() mutable
        {
            it(std::string("should have all components properly connected"), [=]() mutable
            {
                expect(autonomyPlugin->services)->toContain(AutonomyService);
                expect(autonomyPlugin->actions)->toContain(sendToAdminAction);
                expect(autonomyPlugin->providers)->toContain(adminChatProvider);
                expect(autonomyPlugin->providers)->toContain(autonomyStatusProvider);
            }
            );
            it(std::string("should have consistent naming"), [=]() mutable
            {
                expect(autonomyPlugin->name)->toBe(std::string("autonomy"));
                expect(AutonomyService::serviceType)->toBe(std::string("AUTONOMOUS"));
                expect(adminChatProvider->name)->toBe(std::string("ADMIN_CHAT_HISTORY"));
                expect(autonomyStatusProvider->name)->toBe(std::string("AUTONOMY_STATUS"));
                expect(sendToAdminAction->name)->toBe(std::string("SEND_TO_ADMIN"));
            }
            );
        }
        );
        describe(std::string("Autonomy Service Integration Tests"), [=]() mutable
        {
            it(std::string("should enable autonomy via settings"), [=]() mutable
            {
                auto service = std::make_shared<AutonomyService>(mockRuntime);
                std::async([=]() { service->initialize(); });
                std::async([=]() { service->enableAutonomy(); });
                expect(mockRuntime->character->settings->AUTONOMY_ENABLED)->toBe(true);
                expect(service->isLoopRunning())->toBe(true);
                std::async([=]() { service->stop(); });
            }
            );
            it(std::string("should disable autonomy via settings"), [=]() mutable
            {
                auto service = std::make_shared<AutonomyService>(mockRuntime);
                std::async([=]() { service->initialize(); });
                std::async([=]() { service->enableAutonomy(); });
                std::async([=]() { service->disableAutonomy(); });
                expect(mockRuntime->character->settings->AUTONOMY_ENABLED)->toBe(false);
                expect(service->isLoopRunning())->toBe(false);
            }
            );
            it(std::string("should provide status information"), [=]() mutable
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
            it(std::string("should have correct autonomous room ID"), [=]() mutable
            {
                auto service = std::make_shared<AutonomyService>(mockRuntime);
                std::async([=]() { service->initialize(); });
                auto roomId = service->getAutonomousRoomId();
                expect(roomId)->toMatch((new RegExp(std::string("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$"))));
            }
            );
        }
        );
    }
    );
}

MAIN
