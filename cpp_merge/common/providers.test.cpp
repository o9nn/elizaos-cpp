#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-bootstrap/src/__tests__/providers.test.h"

void Main(void)
{
    describe(std:("Choice Provider"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<Partial<std::shared_ptr<Memory>>> mockMessage;
        shared<Partial<std::shared_ptr<State>>> mockState;
        beforeEach([=]() mutable
        {
            auto setup = setupActionTest(object{});
            mockRuntime = setup["mockRuntime"];
            mockMessage = setup["mockMessage"];
            mockState = setup["mockState"];
            mockRuntime->getTasks = mock()->mockResolvedValue(array<any>());
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std:("should list pending tasks with options"), [=]() mutable
        {
            auto tasks = array<object>{ object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("task-1"))}, 
                object::pair{std:("name"), std:("Approve Post")}, 
                object::pair{std:("description"), std:("A blog post is awaiting approval.")}, 
                object::pair{std:("roomId"), mockMessage->roomId}, 
                object::pair{std:("tags"), array<string>{ std:("AWAITING_CHOICE") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("options"), array<string>{ std:("approve"), std:("reject"), object{
                        object::pair{std:("name"), std:("edit")}, 
                        object::pair{std:("description"), std:("Edit the post")}
                    } }}
                }}
            }, object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("task-2"))}, 
                object::pair{std:("name"), std:("Select Image")}, 
                object::pair{std:("roomId"), mockMessage->roomId}, 
                object::pair{std:("tags"), array<string>{ std:("AWAITING_CHOICE") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("options"), array<object>{ object{
                        object::pair{std:("name"), std:("imageA.jpg")}, 
                        object::pair{std:("description"), std:("A cat")}
                    }, object{
                        object::pair{std:("name"), std:("imageB.jpg")}, 
                        object::pair{std:("description"), std:("A dog")}
                    } }}
                }}
            } };
            mockRuntime->getTasks = mock()->mockResolvedValue(tasks);
            auto result = std::async([=]() { choiceProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data)->toBeDefined();
            expect(result->data->tasks)->toHaveLength(2);
            expect(const_(result->data->tasks)[0]->name)->toBe(std:("Approve Post"));
            expect(result->text)->toContain(std:("Pending Tasks"));
            expect(result->text)->toContain(std:("1. **Approve Post**"));
            expect(result->text)->toContain(std:("A blog post is awaiting approval."));
            expect(result->text)->toContain(std:("- "approve""));
            expect(result->text)->toContain(std:("- "reject""));
            expect(result->text)->toContain(std:("- "edit" - Edit the post"));
            expect(result->text)->toContain(std:("2. **Select Image**"));
            expect(result->text)->toContain(std:("- "imageA.jpg" - A cat"));
            expect(result->text)->toContain(std:("- "imageB.jpg" - A dog"));
            expect(result->text)->toContain(std:("To select an option, reply with the option name (e.g., 'post' or 'cancel')."));
            expect(mockRuntime->getTasks)->toHaveBeenCalledWith(object{
                object::pair{std:("roomId"), mockMessage->roomId}, 
                object::pair{std:("tags"), array<string>{ std:("AWAITING_CHOICE") }}
            });
        }
        );
        it(std:("should handle no pending tasks gracefully"), [=]() mutable
        {
            mockRuntime->getTasks = mock()->mockResolvedValue(array<any>());
            auto result = std::async([=]() { choiceProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data)->toBeDefined();
            expect(result->data->tasks)->toHaveLength(0);
            expect(result->text)->toContain(std:("No pending choices for the moment."));
        }
        );
        it(std:("should handle tasks with no options gracefully"), [=]() mutable
        {
            auto tasks = array<object>{ object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("task-1"))}, 
                object::pair{std:("name"), std:("No Options Task")}, 
                object::pair{std:("roomId"), mockMessage->roomId}, 
                object::pair{std:("tags"), array<string>{ std:("AWAITING_CHOICE") }}, 
                object::pair{std:("metadata"), object{}}
            } };
            mockRuntime->getTasks = mock()->mockResolvedValue(tasks);
            auto result = std::async([=]() { choiceProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data)->toBeDefined();
            expect(result->data->tasks)->toHaveLength(0);
            expect(result->text)->toContain(std:("No pending choices for the moment."));
        }
        );
        it(std:("should handle errors from getTasks gracefully"), [=]() mutable
        {
            mockRuntime->getTasks = mock()->mockRejectedValue(std::make_shared<Error>(std:("Task service error")));
            auto result = std::async([=]() { choiceProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data)->toBeDefined();
            expect(result->data->tasks)->toHaveLength(0);
            expect(result->text)->toContain(std:("There was an error retrieving pending tasks with options."));
        }
        );
    }
    );
    describe(std:("Facts Provider"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<Partial<std::shared_ptr<Memory>>> mockMessage;
        shared<Partial<std::shared_ptr<State>>> mockState;
        beforeEach([=]() mutable
        {
            auto setup = setupActionTest();
            mockRuntime = setup["mockRuntime"];
            mockMessage = setup["mockMessage"];
            mockState = setup["mockState"];
            mockRuntime->getMemories = mock()->mockResolvedValue(array<object>{ object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("msg-prev-1"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Previous message 1")}
                }}, 
                object::pair{std:("createdAt"), Date->now() - 1000}
            } });
            mockRuntime->useModel = mock()->mockResolvedValue(array<double>{ 0.1, 0.2, 0.3 });
            mockRuntime->searchMemories = mock()->mockImplementation([=](auto params) mutable
            {
                if (AND((params["tableName"] == std:("facts")), (params["count"] == 6))) {
                    if (params["entityId"] == mockMessage->entityId) {
                        return array<object>{ object{
                            object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("memory-2"))}, 
                            object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("entity-1"))}, 
                            object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("agent-1"))}, 
                            object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-1"))}, 
                            object::pair{std:("content"), object{
                                object::pair{std:("text"), std:("User dislikes spicy food")}
                            }}, 
                            object::pair{std:("embedding"), array<double>{ 0.2, 0.3, 0.4 }}, 
                            object::pair{std:("createdAt"), Date->now()}
                        } };
                    } else {
                        return array<object>{ object{
                            object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("memory-1"))}, 
                            object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("entity-1"))}, 
                            object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("agent-1"))}, 
                            object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-1"))}, 
                            object::pair{std:("content"), object{
                                object::pair{std:("text"), std:("User likes chocolate")}
                            }}, 
                            object::pair{std:("embedding"), array<double>{ 0.1, 0.2, 0.3 }}, 
                            object::pair{std:("createdAt"), Date->now()}
                        } };
                    }
                }
                return array<any>();
            }
            );
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std:("should retrieve facts about a user"), [=]() mutable
        {
            auto result = std::async([=]() { factsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std:("User likes chocolate"));
            expect(result->text)->toContain(std:("User dislikes spicy food"));
            expect(mockRuntime->getMemories)->toHaveBeenCalledWith(object{
                object::pair{std:("tableName"), std:("messages")}, 
                object::pair{std:("roomId"), mockMessage->roomId}, 
                object::pair{std:("count"), 10}, 
                object::pair{std:("unique"), false}
            });
            expect(mockRuntime->useModel)->toHaveBeenCalled();
            expect(mockRuntime->searchMemories)->toHaveBeenCalledTimes(2);
        }
        );
        it(std:("should handle empty results gracefully"), [=]() mutable
        {
            mockRuntime->searchMemories = mock()->mockResolvedValue(array<any>());
            auto result = std::async([=]() { factsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std:("No facts available."));
        }
        );
        it(std:("should handle errors gracefully"), [=]() mutable
        {
            mockRuntime->getMemories = mock()->mockRejectedValue(std::make_shared<Error>(std:("Database error")));
            auto result = std::async([=]() { factsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std:("Error retrieving facts."));
        }
        );
    }
    );
    describe(std:("Providers Provider"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<Partial<std::shared_ptr<Memory>>> mockMessage;
        shared<Partial<std::shared_ptr<State>>> mockState;
        beforeEach([=]() mutable
        {
            mockRuntime = createMockRuntime(object{
                object::pair{std:("providers"), array<object>{ object{
                    object::pair{std:("name"), std:("TEST_PROVIDER_1")}, 
                    object::pair{std:("description"), std:("Test provider 1")}, 
                    object::pair{std:("dynamic"), true}, 
                    object::pair{std:("get"), mock()}
                }, object{
                    object::pair{std:("name"), std:("TEST_PROVIDER_2")}, 
                    object::pair{std:("description"), std:("Test provider 2")}, 
                    object::pair{std:("dynamic"), true}, 
                    object::pair{std:("get"), mock()}
                }, object{
                    object::pair{std:("name"), std:("INTERNAL_PROVIDER")}, 
                    object::pair{std:("description"), std:("Internal provider")}, 
                    object::pair{std:("dynamic"), false}, 
                    object::pair{std:("get"), mock()}
                } }}
            });
            mockMessage = createMockMemory();
            mockState = createMockState();
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std:("should list all dynamic providers"), [=]() mutable
        {
            auto result = std::async([=]() { providersProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std:("TEST_PROVIDER_1"));
            expect(result->text)->toContain(std:("Test provider 1"));
            expect(result->text)->toContain(std:("TEST_PROVIDER_2"));
            expect(result->text)->toContain(std:("Test provider 2"));
            expect(result->text)->not->toContain(std:("INTERNAL_PROVIDER"));
            expect(result->data)->toBeDefined();
            expect(result->data->dynamicProviders)->toHaveLength(2);
            expect(const_(result->data->dynamicProviders)[0]->name)->toBe(std:("TEST_PROVIDER_1"));
            expect(const_(result->data->dynamicProviders)[1]->name)->toBe(std:("TEST_PROVIDER_2"));
        }
        );
        it(std:("should handle empty provider list gracefully"), [=]() mutable
        {
            mockRuntime->providers = array<any>();
            auto result = std::async([=]() { providersProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std:("No dynamic providers are currently available"));
            expect(result->data)->toBeUndefined();
        }
        );
    }
    );
    describe(std:("Recent Messages Provider"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<Partial<std::shared_ptr<Memory>>> mockMessage;
        shared<Partial<std::shared_ptr<State>>> mockState;
        shared<array<Partial<std::shared_ptr<Memory>>>> mockMessages;
        beforeEach([=]() mutable
        {
            mockMessages = array<any>{ createMockMemory(object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("msg-1"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Hello there!")}, 
                    object::pair{std:("channelType"), ChannelType->GROUP}
                }}, 
                object::pair{std:("createdAt"), Date->now() - 3000}
            }), createMockMemory(object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("msg-2"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("How are you?")}, 
                    object::pair{std:("channelType"), ChannelType->GROUP}
                }}, 
                object::pair{std:("createdAt"), Date->now() - 2000}
            }), createMockMemory(object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("msg-3"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("I am doing well.")}, 
                    object::pair{std:("channelType"), ChannelType->GROUP}
                }}, 
                object::pair{std:("createdAt"), Date->now() - 1000}
            }) };
            mockRuntime = createMockRuntime();
            mockMessage = createMockMemory();
            mockState = createMockState();
            mockRuntime->getMemories = mock()->mockResolvedValue(mockMessages);
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std:("should retrieve recent messages"), [=]() mutable
        {
            auto result = std::async([=]() { recentMessagesProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std:("Hello there!"));
            expect(result->text)->toContain(std:("How are you?"));
            expect(result->text)->toContain(std:("I am doing well."));
            expect(mockRuntime->getMemories)->toHaveBeenCalledWith(object{
                object::pair{std:("tableName"), std:("messages")}, 
                object::pair{std:("roomId"), mockMessage->roomId}, 
                object::pair{std:("count"), 10}, 
                object::pair{std:("unique"), false}
            });
        }
        );
        it(std:("should handle empty message list gracefully"), [=]() mutable
        {
            mockRuntime->getMemories = mock()->mockResolvedValue(array<any>());
            mockMessage->content = utils::assign(object{
                , 
                object::pair{std:("text"), string_empty}
            }, mockMessage->content);
            auto result = std::async([=]() { recentMessagesProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std:("No recent messages available"));
        }
        );
        it(std:("should handle errors gracefully"), [=]() mutable
        {
            mockRuntime->getMemories = mock()->mockRejectedValue(std::make_shared<Error>(std:("Database error")));
            auto result = std::async([=]() { recentMessagesProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std:("Error retrieving recent messages."));
        }
        );
    }
    );
    describe(std:("Role Provider"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<Partial<std::shared_ptr<Memory>>> mockMessage;
        shared<Partial<std::shared_ptr<State>>> mockState;
        shared testEntityId = as<std::shared_ptr<UUID>>(std:("test-entity-id"));
        shared otherEntityId = as<std::shared_ptr<UUID>>(std:("other-entity-id"));
        beforeEach([=]() mutable
        {
            mockRuntime = createMockRuntime();
            mockState = createMockState();
            mock->restore();
            (as<any>(mockRuntime->getRoom))["mockResolvedValue"](object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("default-room"))}, 
                object::pair{std:("serverId"), as<std::shared_ptr<UUID>>(std:("default-server"))}, 
                object::pair{std:("type"), ChannelType->GROUP}, 
                object::pair{std:("source"), std:("discord")}
            });
            (as<any>(mockRuntime->getWorld))["mockResolvedValue"](object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("default-world"))}, 
                object::pair{std:("serverId"), as<std::shared_ptr<UUID>>(std:("default-server"))}, 
                object::pair{std:("name"), std:("Default Test World")}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("ownership"), object{
                        object::pair{std:("ownerId"), as<std::shared_ptr<UUID>>(std:("owner-user-id"))}
                    }}, 
                    object::pair{std:("roles"), object{}}
                }}
            });
            (as<any>(mockRuntime->getEntityById))["mockResolvedValue"](nullptr);
            mockState->data = object{
                object::pair{std:("room"), object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("state-room-id"))}, 
                    object::pair{std:("serverId"), as<std::shared_ptr<UUID>>(std:("state-server-id"))}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("source"), std:("discord")}
                }}
            };
            mockMessage = createMockMemory(object{
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("state-room-id"))}
            });
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std:("should retrieve and format role hierarchy"), [=]() mutable
        {
            shared serverId = as<std::shared_ptr<UUID>>(std:("server-with-roles-simple"));
            shared ownerId = as<std::shared_ptr<UUID>>(std:("owner-simple-test-id"));
            mockState->data = object{
                object::pair{std:("room"), object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("room-for-roles-simple-test"))}, 
                    object::pair{std:("serverId"), serverId}, 
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("source"), std:("discord")}
                }}
            };
            mockMessage->roomId = as<std::shared_ptr<UUID>>(std:("room-for-roles-simple-test"));
            (as<any>(mockRuntime->getWorld))["mockImplementation"]([=](auto id) mutable
            {
                return object{
                    object::pair{std:("id"), id}, 
                    object::pair{std:("serverId"), serverId}, 
                    object::pair{std:("name"), std:("Role Test World Simple")}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("ownership"), object{
                            object::pair{std:("ownerId"), as<std::shared_ptr<UUID>>(std:("any-owner-simple"))}
                        }}, 
                        object::pair{std:("roles"), object{
                            object::pair{ownerId, std:("OWNER")}
                        }}
                    }}
                };
            }
            );
            (as<any>(mockRuntime->getEntityById))["mockImplementation"]([=](auto id) mutable
            {
                if (id == ownerId) {
                    return object{
                        object::pair{std:("id"), ownerId}, 
                        object::pair{std:("names"), array<string>{ std:("Simple Owner") }}, 
                        object::pair{std:("metadata"), object{
                            object::pair{std:("name"), std:("SimpleOwnerName")}, 
                            object::pair{std:("username"), std:("simple_owner_discord")}
                        }}
                    };
                }
                return nullptr;
            }
            );
            auto result = std::async([=]() { roleProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std:("# Server Role Hierarchy"));
            expect(result->text)->toContain(std:("## Owners"));
            expect(result->text)->toContain(std:("SimpleOwnerName (Simple Owner)"));
            expect(result->text)->not->toContain(std:("## Administrators"));
            expect(result->text)->not->toContain(std:("## Members"));
            expect(mockRuntime->getWorld)->toHaveBeenCalled();
            expect(mockRuntime->getEntityById)->toHaveBeenCalledWith(ownerId);
        }
        );
        it(std:("should handle missing entity gracefully (now expecting rejection)"), [=]() mutable
        {
            mockMessage = createMockMemory(object{
                object::pair{std:("entityId"), otherEntityId}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-for-rejection-test-1"))}
            });
            mockState->data = utils::assign(object{
                , 
                object::pair{std:("room"), undefined}
            }, mockState->data);
            (as<any>(mockRuntime->getRoom))["mockRejectedValue"](std::make_shared<Error>(std:("Simulated DB error getting room")));
            std::async([=]() { expect(roleProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)))->rejects->toThrowError(std:("Simulated DB error getting room")); });
        }
        );
        it(std:("should handle missing roles gracefully"), [=]() mutable
        {
            auto entityMissingRolesId = as<std::shared_ptr<UUID>>(std:("entity-no-roles"));
            mockMessage = createMockMemory(object{
                object::pair{std:("entityId"), entityMissingRolesId}
            });
            auto mockRoomId = as<std::shared_ptr<UUID>>(std:("room-with-server"));
            shared mockServerId = as<std::shared_ptr<UUID>>(std:("server-for-roles"));
            mockMessage->roomId = mockRoomId;
            (as<any>(mockRuntime->getRoom))["mockResolvedValue"](object{
                object::pair{std:("id"), mockRoomId}, 
                object::pair{std:("serverId"), mockServerId}, 
                object::pair{std:("type"), ChannelType->GROUP}, 
                object::pair{std:("source"), std:("discord")}
            });
            (as<any>(mockRuntime->getWorld))["mockImplementation"]([=](auto id) mutable
            {
                return object{
                    object::pair{std:("id"), id}, 
                    object::pair{std:("serverId"), mockServerId}, 
                    object::pair{std:("name"), std:("Test World No Roles")}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("ownership"), object{
                            object::pair{std:("ownerId"), as<std::shared_ptr<UUID>>(std:("some-owner"))}
                        }}, 
                        object::pair{std:("roles"), object{}}
                    }}
                };
            }
            );
            auto result = std::async([=]() { roleProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toBeDefined();
            expect(type_of(result->text))->toBe(std:("string"));
            if (type_of(result->text) == std:("string")) {
                expect(result->text)->toContain(std:("No role information available for this server."));
            }
            expect(mockRuntime->getEntityById)->not->toHaveBeenCalled();
        }
        );
        it(std:("should handle errors gracefully (now expecting rejection)"), [=]() mutable
        {
            mockMessage = createMockMemory(object{
                object::pair{std:("entityId"), testEntityId}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-for-rejection-test-2"))}
            });
            mockState->data = utils::assign(object{
                , 
                object::pair{std:("room"), undefined}
            }, mockState->data);
            auto specificError = std::make_shared<Error>(std:("DB error for roles test"));
            (as<any>(mockRuntime->getRoom))["mockRejectedValue"](specificError);
            std::async([=]() { expect(roleProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)))->rejects->toThrowError(specificError); });
        }
        );
    }
    );
    describe(std:("Settings Provider"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<Partial<std::shared_ptr<Memory>>> mockMessage;
        shared<Partial<std::shared_ptr<State>>> mockState;
        beforeEach([=]() mutable
        {
            mockRuntime = createMockRuntime();
            mockMessage = createMockMemory(object{
                object::pair{std:("content"), object{}}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("test-owner-entity-id"))}
            });
            mockState = createMockState(object{
                object::pair{std:("data"), object{
                    object::pair{std:("room"), object{
                        object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("test-room-id"))}, 
                        object::pair{std:("worldId"), as<std::shared_ptr<UUID>>(std:("world-1"))}
                    }}
                }}
            });
            mockRuntime->getRoom = mock()->mockResolvedValue(object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("test-room-id"))}, 
                object::pair{std:("worldId"), as<std::shared_ptr<UUID>>(std:("world-1"))}, 
                object::pair{std:("type"), ChannelType->GROUP}
            });
            mockRuntime->getWorld = mock()->mockResolvedValue(object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("world-1"))}, 
                object::pair{std:("serverId"), std:("server-1")}, 
                object::pair{std:("name"), std:("Test World")}
            });
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std:("should retrieve settings in onboarding mode"), [=]() mutable
        {
            mockMessage->content = utils::assign(object{
                , 
                object::pair{std:("channelType"), ChannelType->DM}
            }, mockMessage->content);
            mockState->data = object{
                object::pair{std:("room"), object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("onboarding-room-id"))}, 
                    object::pair{std:("type"), ChannelType->DM}
                }}
            };
            mockMessage->roomId = as<std::shared_ptr<UUID>>(std:("onboarding-room-id"));
            (as<any>(mockRuntime->getRoom))["mockResolvedValue"](object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("onboarding-room-id"))}, 
                object::pair{std:("worldId"), as<std::shared_ptr<UUID>>(std:("world-1"))}, 
                object::pair{std:("type"), ChannelType->DM}
            });
            auto result = std::async([=]() { settingsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data)->toBeDefined();
            expect(result->text)->toBeDefined();
            expect(type_of(result->text))->toBe(std:("string"));
        }
        );
        it(std:("should retrieve settings in normal mode"), [=]() mutable
        {
            mockMessage->content = utils::assign(object{
                , 
                object::pair{std:("channelType"), ChannelType->GROUP}
            }, mockMessage->content);
            (as<any>(mockRuntime->getRoom))["mockResolvedValue"](object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("test-room-id"))}, 
                object::pair{std:("worldId"), as<std::shared_ptr<UUID>>(std:("world-1"))}, 
                object::pair{std:("type"), ChannelType->GROUP}
            });
            auto result = std::async([=]() { settingsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data)->toBeDefined();
            expect(result->text)->toBeDefined();
            expect(type_of(result->text))->toBe(std:("string"));
        }
        );
        it(std:("should handle errors gracefully when getWorldSettings fails"), [=]() mutable
        {
            mockMessage->content = object{
                object::pair{std:("channelType"), ChannelType->DM}
            };
            mockState->data = object{
                object::pair{std:("room"), object{
                    object::pair{std:("type"), ChannelType->DM}, 
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("dm-room-err"))}
                }}
            };
            mockMessage->roomId = as<std::shared_ptr<UUID>>(std:("dm-room-err"));
            (as<any>(mockRuntime->getWorld))["mockResolvedValue"](nullptr);
            auto result = std::async([=]() { settingsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toBeDefined();
        }
        );
        it(std:("should handle missing world gracefully"), [=]() mutable
        {
            mockRuntime->getWorld = mock()->mockResolvedValue(nullptr);
            mockMessage->content = object{
                object::pair{std:("channelType"), ChannelType->GROUP}
            };
            mockState->data = utils::assign(object{
                , 
                object::pair{std:("room"), object{
                    object::pair{std:("type"), ChannelType->GROUP}, 
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("group-room-err"))}
                }}
            }, mockState->data);
            mockMessage->roomId = as<std::shared_ptr<UUID>>(std:("group-room-err"));
            auto result = std::async([=]() { settingsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std:("Error retrieving configuration information. Please try again later."));
        }
        );
    }
    );
    describe(std:("Attachments Provider"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<Partial<std::shared_ptr<Memory>>> mockMessage;
        shared<Partial<std::shared_ptr<State>>> mockState;
        beforeEach([=]() mutable
        {
            auto setup = setupActionTest();
            mockRuntime = setup["mockRuntime"];
            mockMessage = setup["mockMessage"];
            mockState = setup["mockState"];
            mockRuntime->getConversationLength = mock()->mockReturnValue(10);
            mockRuntime->getMemories = mock()->mockResolvedValue(array<any>());
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std:("should handle messages with no attachments"), [=]() mutable
        {
            mockMessage->content = object{
                object::pair{std:("text"), std:("Hello, how are you?")}, 
                object::pair{std:("channelType"), ChannelType->GROUP}
            };
            auto result = std::async([=]() { attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data->attachments)->toHaveLength(0);
            expect(result->text)->toBe(string_empty);
            expect(result->values->attachments)->toBe(string_empty);
        }
        );
        it(std:("should return current message attachments"), [=]() mutable
        {
            auto testAttachments = array<std::shared_ptr<Media>>{ object{
                object::pair{std:("id"), std:("attach-1")}, 
                object::pair{std:("url"), std:("https://example.com/image1.jpg")}, 
                object::pair{std:("title"), std:("Test Image 1")}, 
                object::pair{std:("source"), std:("image/jpeg")}, 
                object::pair{std:("description"), std:("A test image")}, 
                object::pair{std:("text"), std:("Image content text")}
            }, object{
                object::pair{std:("id"), std:("attach-2")}, 
                object::pair{std:("url"), std:("https://example.com/document.pdf")}, 
                object::pair{std:("title"), std:("Test Document")}, 
                object::pair{std:("source"), std:("application/pdf")}, 
                object::pair{std:("description"), std:("A test PDF document")}, 
                object::pair{std:("text"), std:("Document content text")}
            } };
            mockMessage->content = object{
                object::pair{std:("text"), std:("Check out these attachments")}, 
                object::pair{std:("channelType"), ChannelType->GROUP}, 
                object::pair{std:("attachments"), testAttachments}
            };
            auto result = std::async([=]() { attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data->attachments)->toHaveLength(2);
            expect(const_(result->data->attachments)[0]->id)->toBe(std:("attach-1"));
            expect(const_(result->data->attachments)[1]->id)->toBe(std:("attach-2"));
            expect(result->text)->toContain(std:("# Attachments"));
            expect(result->text)->toContain(std:("Test Image 1"));
            expect(result->text)->toContain(std:("Test Document"));
            expect(result->text)->toContain(std:("https://example.com/image1.jpg"));
            expect(result->text)->toContain(std:("Image content text"));
        }
        );
        it(std:("should merge attachments from recent messages"), [=]() mutable
        {
            auto currentAttachment = object{
                object::pair{std:("id"), std:("current-attach")}, 
                object::pair{std:("url"), std:("https://example.com/current.jpg")}, 
                object::pair{std:("title"), std:("Current Image")}, 
                object::pair{std:("source"), std:("image/jpeg")}, 
                object::pair{std:("description"), std:("Current attachment")}, 
                object::pair{std:("text"), std:("Current content")}
            };
            mockMessage->content = object{
                object::pair{std:("text"), std:("Current message with attachment")}, 
                object::pair{std:("channelType"), ChannelType->GROUP}, 
                object::pair{std:("attachments"), array<any>{ currentAttachment }}
            };
            auto recentMessages = array<object>{ object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("msg-1"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Previous message 1")}, 
                    object::pair{std:("attachments"), array<object>{ object{
                        object::pair{std:("id"), std:("prev-attach-1")}, 
                        object::pair{std:("url"), std:("https://example.com/prev1.jpg")}, 
                        object::pair{std:("title"), std:("Previous Image 1")}, 
                        object::pair{std:("source"), std:("image/jpeg")}, 
                        object::pair{std:("description"), std:("Previous attachment 1")}, 
                        object::pair{std:("text"), std:("Previous content 1")}
                    } }}
                }}, 
                object::pair{std:("createdAt"), Date->now() - 30 * 60 * 1000}
            }, object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("msg-2"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Previous message 2")}, 
                    object::pair{std:("attachments"), array<object>{ object{
                        object::pair{std:("id"), std:("prev-attach-2")}, 
                        object::pair{std:("url"), std:("https://example.com/prev2.jpg")}, 
                        object::pair{std:("title"), std:("Previous Image 2")}, 
                        object::pair{std:("source"), std:("image/jpeg")}, 
                        object::pair{std:("description"), std:("Previous attachment 2")}, 
                        object::pair{std:("text"), std:("Previous content 2")}
                    } }}
                }}, 
                object::pair{std:("createdAt"), Date->now() - 15 * 60 * 1000}
            } };
            mockRuntime->getMemories = mock()->mockResolvedValue(recentMessages);
            auto result = std::async([=]() { attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data->attachments)->toHaveLength(3);
            expect(const_(result->data->attachments)[0]->id)->toBe(std:("current-attach"));
            expect(const_(result->data->attachments)[1]->id)->toBe(std:("prev-attach-2"));
            expect(const_(result->data->attachments)[2]->id)->toBe(std:("prev-attach-1"));
            expect(result->text)->toContain(std:("Current Image"));
            expect(result->text)->toContain(std:("Previous Image 1"));
            expect(result->text)->toContain(std:("Previous Image 2"));
        }
        );
        it(std:("should hide text for attachments older than 1 hour"), [=]() mutable
        {
            auto currentAttachment = object{
                object::pair{std:("id"), std:("current-attach")}, 
                object::pair{std:("url"), std:("https://example.com/current.jpg")}, 
                object::pair{std:("title"), std:("Current Image")}, 
                object::pair{std:("source"), std:("image/jpeg")}, 
                object::pair{std:("description"), std:("Current attachment")}, 
                object::pair{std:("text"), std:("Current content")}
            };
            mockMessage->content = object{
                object::pair{std:("text"), std:("Current message")}, 
                object::pair{std:("channelType"), ChannelType->GROUP}, 
                object::pair{std:("attachments"), array<any>{ currentAttachment }}
            };
            auto messages = array<object>{ object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("msg-recent"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Recent message")}, 
                    object::pair{std:("attachments"), array<object>{ object{
                        object::pair{std:("id"), std:("recent-attach")}, 
                        object::pair{std:("url"), std:("https://example.com/recent.jpg")}, 
                        object::pair{std:("title"), std:("Recent Image")}, 
                        object::pair{std:("source"), std:("image/jpeg")}, 
                        object::pair{std:("description"), std:("Recent attachment")}, 
                        object::pair{std:("text"), std:("This should be visible")}
                    } }}
                }}, 
                object::pair{std:("createdAt"), Date->now() - 30 * 60 * 1000}
            }, object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("msg-old"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Old message")}, 
                    object::pair{std:("attachments"), array<object>{ object{
                        object::pair{std:("id"), std:("old-attach")}, 
                        object::pair{std:("url"), std:("https://example.com/old.jpg")}, 
                        object::pair{std:("title"), std:("Old Image")}, 
                        object::pair{std:("source"), std:("image/jpeg")}, 
                        object::pair{std:("description"), std:("Old attachment")}, 
                        object::pair{std:("text"), std:("This should be hidden")}
                    } }}
                }}, 
                object::pair{std:("createdAt"), Date->now() - 2 * 60 * 60 * 1000}
            } };
            mockRuntime->getMemories = mock()->mockResolvedValue(messages);
            auto result = std::async([=]() { attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data->attachments)->toHaveLength(3);
            auto oldAttachment = result->data->attachments->find([=](auto a) mutable
            {
                return a["id"] == std:("old-attach");
            }
            );
            expect(oldAttachment->text)->toBe(std:("[Hidden]"));
            auto recentAttachment = result->data->attachments->find([=](auto a) mutable
            {
                return a["id"] == std:("recent-attach");
            }
            );
            expect(recentAttachment->text)->toBe(std:("This should be visible"));
            auto currentAtt = result->data->attachments->find([=](auto a) mutable
            {
                return a["id"] == std:("current-attach");
            }
            );
            expect(currentAtt->text)->toBe(std:("Current content"));
        }
        );
        it(std:("should not duplicate attachments with same ID"), [=]() mutable
        {
            auto sharedAttachment = object{
                object::pair{std:("id"), std:("shared-attach")}, 
                object::pair{std:("url"), std:("https://example.com/shared.jpg")}, 
                object::pair{std:("title"), std:("Shared Image")}, 
                object::pair{std:("source"), std:("image/jpeg")}, 
                object::pair{std:("description"), std:("Shared attachment")}, 
                object::pair{std:("text"), std:("Shared content with more details")}
            };
            mockMessage->content = object{
                object::pair{std:("text"), std:("Current message")}, 
                object::pair{std:("channelType"), ChannelType->GROUP}, 
                object::pair{std:("attachments"), array<any>{ sharedAttachment }}
            };
            auto recentMessages = array<object>{ object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("msg-1"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Previous message")}, 
                    object::pair{std:("attachments"), array<object>{ object{
                        object::pair{std:("id"), std:("shared-attach")}, 
                        object::pair{std:("url"), std:("https://example.com/shared.jpg")}, 
                        object::pair{std:("title"), std:("Shared Image")}, 
                        object::pair{std:("source"), std:("image/jpeg")}, 
                        object::pair{std:("description"), std:("Basic description")}, 
                        object::pair{std:("text"), std:("Basic text")}
                    } }}
                }}, 
                object::pair{std:("createdAt"), Date->now() - 10 * 60 * 1000}
            } };
            mockRuntime->getMemories = mock()->mockResolvedValue(recentMessages);
            auto result = std::async([=]() { attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data->attachments)->toHaveLength(1);
            expect(const_(result->data->attachments)[0]->id)->toBe(std:("shared-attach"));
            expect(const_(result->data->attachments)[0]->text)->toBe(std:("Shared content with more details"));
            expect(const_(result->data->attachments)[0]->description)->toBe(std:("Shared attachment"));
        }
        );
        it(std:("should format attachment data correctly"), [=]() mutable
        {
            auto testAttachment = object{
                object::pair{std:("id"), std:("format-test")}, 
                object::pair{std:("url"), std:("https://example.com/test.png")}, 
                object::pair{std:("title"), std:("Format Test Image")}, 
                object::pair{std:("source"), std:("image/png")}, 
                object::pair{std:("description"), std:("Testing formatted output")}, 
                object::pair{std:("text"), std:("This is the extracted text from the image")}
            };
            mockMessage->content = object{
                object::pair{std:("text"), std:("Testing format")}, 
                object::pair{std:("channelType"), ChannelType->GROUP}, 
                object::pair{std:("attachments"), array<any>{ testAttachment }}
            };
            auto result = std::async([=]() { attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std:("# Attachments"));
            expect(result->text)->toContain(std:("ID: format-test"));
            expect(result->text)->toContain(std:("Name: Format Test Image"));
            expect(result->text)->toContain(std:("URL: https://example.com/test.png"));
            expect(result->text)->toContain(std:("Type: image/png"));
            expect(result->text)->toContain(std:("Description: Testing formatted output"));
            expect(result->text)->toContain(std:("Text: This is the extracted text from the image"));
        }
        );
        it(std:("should handle messages with no recent attachments history"), [=]() mutable
        {
            auto currentAttachment = object{
                object::pair{std:("id"), std:("only-attach")}, 
                object::pair{std:("url"), std:("https://example.com/only.jpg")}, 
                object::pair{std:("title"), std:("Only Attachment")}, 
                object::pair{std:("source"), std:("image/jpeg")}, 
                object::pair{std:("description"), std:("The only attachment")}, 
                object::pair{std:("text"), std:("Only attachment content")}
            };
            mockMessage->content = object{
                object::pair{std:("text"), std:("Message with attachment")}, 
                object::pair{std:("channelType"), ChannelType->GROUP}, 
                object::pair{std:("attachments"), array<any>{ currentAttachment }}
            };
            auto recentMessages = array<object>{ object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("msg-1"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Text only message 1")}
                }}, 
                object::pair{std:("createdAt"), Date->now() - 5 * 60 * 1000}
            }, object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("msg-2"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Text only message 2")}
                }}, 
                object::pair{std:("createdAt"), Date->now() - 2 * 60 * 1000}
            } };
            mockRuntime->getMemories = mock()->mockResolvedValue(recentMessages);
            auto result = std::async([=]() { attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data->attachments)->toHaveLength(1);
            expect(const_(result->data->attachments)[0]->id)->toBe(std:("only-attach"));
            expect(result->text)->toContain(std:("Only Attachment"));
        }
        );
        it(std:("should handle errors by throwing them"), [=]() mutable
        {
            mockMessage->content = object{
                object::pair{std:("text"), std:("Test message")}, 
                object::pair{std:("channelType"), ChannelType->GROUP}, 
                object::pair{std:("attachments"), array<object>{ object{
                    object::pair{std:("id"), std:("test-attach")}, 
                    object::pair{std:("url"), std:("https://example.com/test.jpg")}, 
                    object::pair{std:("title"), std:("Test")}, 
                    object::pair{std:("source"), std:("image/jpeg")}
                } }}
            };
            mockRuntime->getMemories = mock()->mockRejectedValue(std::make_shared<Error>(std:("Database error")));
            expect(attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)))->rejects->toThrow(std:("Database error"));
        }
        );
    }
    );
}

MAIN
