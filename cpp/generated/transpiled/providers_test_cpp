#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-bootstrap/src/__tests__/providers.test.h"

void Main(void)
{
    describe(std::string("Choice Provider"), [=]() mutable
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
        it(std::string("should list pending tasks with options"), [=]() mutable
        {
            auto tasks = array<object>{ object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("task-1"))}, 
                object::pair{std::string("name"), std::string("Approve Post")}, 
                object::pair{std::string("description"), std::string("A blog post is awaiting approval.")}, 
                object::pair{std::string("roomId"), mockMessage->roomId}, 
                object::pair{std::string("tags"), array<string>{ std::string("AWAITING_CHOICE") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("options"), array<string>{ std::string("approve"), std::string("reject"), object{
                        object::pair{std::string("name"), std::string("edit")}, 
                        object::pair{std::string("description"), std::string("Edit the post")}
                    } }}
                }}
            }, object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("task-2"))}, 
                object::pair{std::string("name"), std::string("Select Image")}, 
                object::pair{std::string("roomId"), mockMessage->roomId}, 
                object::pair{std::string("tags"), array<string>{ std::string("AWAITING_CHOICE") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("options"), array<object>{ object{
                        object::pair{std::string("name"), std::string("imageA.jpg")}, 
                        object::pair{std::string("description"), std::string("A cat")}
                    }, object{
                        object::pair{std::string("name"), std::string("imageB.jpg")}, 
                        object::pair{std::string("description"), std::string("A dog")}
                    } }}
                }}
            } };
            mockRuntime->getTasks = mock()->mockResolvedValue(tasks);
            auto result = std::async([=]() { choiceProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data)->toBeDefined();
            expect(result->data->tasks)->toHaveLength(2);
            expect(const_(result->data->tasks)[0]->name)->toBe(std::string("Approve Post"));
            expect(result->text)->toContain(std::string("Pending Tasks"));
            expect(result->text)->toContain(std::string("1. **Approve Post**"));
            expect(result->text)->toContain(std::string("A blog post is awaiting approval."));
            expect(result->text)->toContain(std::string("- `approve`"));
            expect(result->text)->toContain(std::string("- `reject`"));
            expect(result->text)->toContain(std::string("- `edit` - Edit the post"));
            expect(result->text)->toContain(std::string("2. **Select Image**"));
            expect(result->text)->toContain(std::string("- `imageA.jpg` - A cat"));
            expect(result->text)->toContain(std::string("- `imageB.jpg` - A dog"));
            expect(result->text)->toContain(std::string("To select an option, reply with the option name (e.g., 'post' or 'cancel')."));
            expect(mockRuntime->getTasks)->toHaveBeenCalledWith(object{
                object::pair{std::string("roomId"), mockMessage->roomId}, 
                object::pair{std::string("tags"), array<string>{ std::string("AWAITING_CHOICE") }}
            });
        }
        );
        it(std::string("should handle no pending tasks gracefully"), [=]() mutable
        {
            mockRuntime->getTasks = mock()->mockResolvedValue(array<any>());
            auto result = std::async([=]() { choiceProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data)->toBeDefined();
            expect(result->data->tasks)->toHaveLength(0);
            expect(result->text)->toContain(std::string("No pending choices for the moment."));
        }
        );
        it(std::string("should handle tasks with no options gracefully"), [=]() mutable
        {
            auto tasks = array<object>{ object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("task-1"))}, 
                object::pair{std::string("name"), std::string("No Options Task")}, 
                object::pair{std::string("roomId"), mockMessage->roomId}, 
                object::pair{std::string("tags"), array<string>{ std::string("AWAITING_CHOICE") }}, 
                object::pair{std::string("metadata"), object{}}
            } };
            mockRuntime->getTasks = mock()->mockResolvedValue(tasks);
            auto result = std::async([=]() { choiceProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data)->toBeDefined();
            expect(result->data->tasks)->toHaveLength(0);
            expect(result->text)->toContain(std::string("No pending choices for the moment."));
        }
        );
        it(std::string("should handle errors from getTasks gracefully"), [=]() mutable
        {
            mockRuntime->getTasks = mock()->mockRejectedValue(std::make_shared<Error>(std::string("Task service error")));
            auto result = std::async([=]() { choiceProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data)->toBeDefined();
            expect(result->data->tasks)->toHaveLength(0);
            expect(result->text)->toContain(std::string("There was an error retrieving pending tasks with options."));
        }
        );
    }
    );
    describe(std::string("Facts Provider"), [=]() mutable
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
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("msg-prev-1"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Previous message 1")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() - 1000}
            } });
            mockRuntime->useModel = mock()->mockResolvedValue(array<double>{ 0.1, 0.2, 0.3 });
            mockRuntime->searchMemories = mock()->mockImplementation([=](auto params) mutable
            {
                if (AND((params["tableName"] == std::string("facts")), (params["count"] == 6))) {
                    if (params["entityId"] == mockMessage->entityId) {
                        return array<object>{ object{
                            object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("memory-2"))}, 
                            object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("entity-1"))}, 
                            object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("agent-1"))}, 
                            object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-1"))}, 
                            object::pair{std::string("content"), object{
                                object::pair{std::string("text"), std::string("User dislikes spicy food")}
                            }}, 
                            object::pair{std::string("embedding"), array<double>{ 0.2, 0.3, 0.4 }}, 
                            object::pair{std::string("createdAt"), Date->now()}
                        } };
                    } else {
                        return array<object>{ object{
                            object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("memory-1"))}, 
                            object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("entity-1"))}, 
                            object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("agent-1"))}, 
                            object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-1"))}, 
                            object::pair{std::string("content"), object{
                                object::pair{std::string("text"), std::string("User likes chocolate")}
                            }}, 
                            object::pair{std::string("embedding"), array<double>{ 0.1, 0.2, 0.3 }}, 
                            object::pair{std::string("createdAt"), Date->now()}
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
        it(std::string("should retrieve facts about a user"), [=]() mutable
        {
            auto result = std::async([=]() { factsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std::string("User likes chocolate"));
            expect(result->text)->toContain(std::string("User dislikes spicy food"));
            expect(mockRuntime->getMemories)->toHaveBeenCalledWith(object{
                object::pair{std::string("tableName"), std::string("messages")}, 
                object::pair{std::string("roomId"), mockMessage->roomId}, 
                object::pair{std::string("count"), 10}, 
                object::pair{std::string("unique"), false}
            });
            expect(mockRuntime->useModel)->toHaveBeenCalled();
            expect(mockRuntime->searchMemories)->toHaveBeenCalledTimes(2);
        }
        );
        it(std::string("should handle empty results gracefully"), [=]() mutable
        {
            mockRuntime->searchMemories = mock()->mockResolvedValue(array<any>());
            auto result = std::async([=]() { factsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std::string("No facts available."));
        }
        );
        it(std::string("should handle errors gracefully"), [=]() mutable
        {
            mockRuntime->getMemories = mock()->mockRejectedValue(std::make_shared<Error>(std::string("Database error")));
            auto result = std::async([=]() { factsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std::string("Error retrieving facts."));
        }
        );
    }
    );
    describe(std::string("Providers Provider"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<Partial<std::shared_ptr<Memory>>> mockMessage;
        shared<Partial<std::shared_ptr<State>>> mockState;
        beforeEach([=]() mutable
        {
            mockRuntime = createMockRuntime(object{
                object::pair{std::string("providers"), array<object>{ object{
                    object::pair{std::string("name"), std::string("TEST_PROVIDER_1")}, 
                    object::pair{std::string("description"), std::string("Test provider 1")}, 
                    object::pair{std::string("dynamic"), true}, 
                    object::pair{std::string("get"), mock()}
                }, object{
                    object::pair{std::string("name"), std::string("TEST_PROVIDER_2")}, 
                    object::pair{std::string("description"), std::string("Test provider 2")}, 
                    object::pair{std::string("dynamic"), true}, 
                    object::pair{std::string("get"), mock()}
                }, object{
                    object::pair{std::string("name"), std::string("INTERNAL_PROVIDER")}, 
                    object::pair{std::string("description"), std::string("Internal provider")}, 
                    object::pair{std::string("dynamic"), false}, 
                    object::pair{std::string("get"), mock()}
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
        it(std::string("should list all dynamic providers"), [=]() mutable
        {
            auto result = std::async([=]() { providersProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std::string("TEST_PROVIDER_1"));
            expect(result->text)->toContain(std::string("Test provider 1"));
            expect(result->text)->toContain(std::string("TEST_PROVIDER_2"));
            expect(result->text)->toContain(std::string("Test provider 2"));
            expect(result->text)->not->toContain(std::string("INTERNAL_PROVIDER"));
            expect(result->data)->toBeDefined();
            expect(result->data->dynamicProviders)->toHaveLength(2);
            expect(const_(result->data->dynamicProviders)[0]->name)->toBe(std::string("TEST_PROVIDER_1"));
            expect(const_(result->data->dynamicProviders)[1]->name)->toBe(std::string("TEST_PROVIDER_2"));
        }
        );
        it(std::string("should handle empty provider list gracefully"), [=]() mutable
        {
            mockRuntime->providers = array<any>();
            auto result = std::async([=]() { providersProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std::string("No dynamic providers are currently available"));
            expect(result->data)->toBeUndefined();
        }
        );
    }
    );
    describe(std::string("Recent Messages Provider"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<Partial<std::shared_ptr<Memory>>> mockMessage;
        shared<Partial<std::shared_ptr<State>>> mockState;
        shared<array<Partial<std::shared_ptr<Memory>>>> mockMessages;
        beforeEach([=]() mutable
        {
            mockMessages = array<any>{ createMockMemory(object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("msg-1"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Hello there!")}, 
                    object::pair{std::string("channelType"), ChannelType->GROUP}
                }}, 
                object::pair{std::string("createdAt"), Date->now() - 3000}
            }), createMockMemory(object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("msg-2"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("How are you?")}, 
                    object::pair{std::string("channelType"), ChannelType->GROUP}
                }}, 
                object::pair{std::string("createdAt"), Date->now() - 2000}
            }), createMockMemory(object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("msg-3"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("I am doing well.")}, 
                    object::pair{std::string("channelType"), ChannelType->GROUP}
                }}, 
                object::pair{std::string("createdAt"), Date->now() - 1000}
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
        it(std::string("should retrieve recent messages"), [=]() mutable
        {
            auto result = std::async([=]() { recentMessagesProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std::string("Hello there!"));
            expect(result->text)->toContain(std::string("How are you?"));
            expect(result->text)->toContain(std::string("I am doing well."));
            expect(mockRuntime->getMemories)->toHaveBeenCalledWith(object{
                object::pair{std::string("tableName"), std::string("messages")}, 
                object::pair{std::string("roomId"), mockMessage->roomId}, 
                object::pair{std::string("count"), 10}, 
                object::pair{std::string("unique"), false}
            });
        }
        );
        it(std::string("should handle empty message list gracefully"), [=]() mutable
        {
            mockRuntime->getMemories = mock()->mockResolvedValue(array<any>());
            mockMessage->content = utils::assign(object{
                , 
                object::pair{std::string("text"), string_empty}
            }, mockMessage->content);
            auto result = std::async([=]() { recentMessagesProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std::string("No recent messages available"));
        }
        );
        it(std::string("should handle errors gracefully"), [=]() mutable
        {
            mockRuntime->getMemories = mock()->mockRejectedValue(std::make_shared<Error>(std::string("Database error")));
            auto result = std::async([=]() { recentMessagesProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std::string("Error retrieving recent messages."));
        }
        );
    }
    );
    describe(std::string("Role Provider"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<Partial<std::shared_ptr<Memory>>> mockMessage;
        shared<Partial<std::shared_ptr<State>>> mockState;
        shared testEntityId = as<std::shared_ptr<UUID>>(std::string("test-entity-id"));
        shared otherEntityId = as<std::shared_ptr<UUID>>(std::string("other-entity-id"));
        beforeEach([=]() mutable
        {
            mockRuntime = createMockRuntime();
            mockState = createMockState();
            mock->restore();
            (as<any>(mockRuntime->getRoom))["mockResolvedValue"](object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("default-room"))}, 
                object::pair{std::string("serverId"), as<std::shared_ptr<UUID>>(std::string("default-server"))}, 
                object::pair{std::string("type"), ChannelType->GROUP}, 
                object::pair{std::string("source"), std::string("discord")}
            });
            (as<any>(mockRuntime->getWorld))["mockResolvedValue"](object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("default-world"))}, 
                object::pair{std::string("serverId"), as<std::shared_ptr<UUID>>(std::string("default-server"))}, 
                object::pair{std::string("name"), std::string("Default Test World")}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("ownership"), object{
                        object::pair{std::string("ownerId"), as<std::shared_ptr<UUID>>(std::string("owner-user-id"))}
                    }}, 
                    object::pair{std::string("roles"), object{}}
                }}
            });
            (as<any>(mockRuntime->getEntityById))["mockResolvedValue"](nullptr);
            mockState->data = object{
                object::pair{std::string("room"), object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("state-room-id"))}, 
                    object::pair{std::string("serverId"), as<std::shared_ptr<UUID>>(std::string("state-server-id"))}, 
                    object::pair{std::string("type"), ChannelType->GROUP}, 
                    object::pair{std::string("source"), std::string("discord")}
                }}
            };
            mockMessage = createMockMemory(object{
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("state-room-id"))}
            });
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std::string("should retrieve and format role hierarchy"), [=]() mutable
        {
            shared serverId = as<std::shared_ptr<UUID>>(std::string("server-with-roles-simple"));
            shared ownerId = as<std::shared_ptr<UUID>>(std::string("owner-simple-test-id"));
            mockState->data = object{
                object::pair{std::string("room"), object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("room-for-roles-simple-test"))}, 
                    object::pair{std::string("serverId"), serverId}, 
                    object::pair{std::string("type"), ChannelType->GROUP}, 
                    object::pair{std::string("source"), std::string("discord")}
                }}
            };
            mockMessage->roomId = as<std::shared_ptr<UUID>>(std::string("room-for-roles-simple-test"));
            (as<any>(mockRuntime->getWorld))["mockImplementation"]([=](auto id) mutable
            {
                return object{
                    object::pair{std::string("id"), id}, 
                    object::pair{std::string("serverId"), serverId}, 
                    object::pair{std::string("name"), std::string("Role Test World Simple")}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("ownership"), object{
                            object::pair{std::string("ownerId"), as<std::shared_ptr<UUID>>(std::string("any-owner-simple"))}
                        }}, 
                        object::pair{std::string("roles"), object{
                            object::pair{ownerId, std::string("OWNER")}
                        }}
                    }}
                };
            }
            );
            (as<any>(mockRuntime->getEntityById))["mockImplementation"]([=](auto id) mutable
            {
                if (id == ownerId) {
                    return object{
                        object::pair{std::string("id"), ownerId}, 
                        object::pair{std::string("names"), array<string>{ std::string("Simple Owner") }}, 
                        object::pair{std::string("metadata"), object{
                            object::pair{std::string("name"), std::string("SimpleOwnerName")}, 
                            object::pair{std::string("username"), std::string("simple_owner_discord")}
                        }}
                    };
                }
                return nullptr;
            }
            );
            auto result = std::async([=]() { roleProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std::string("# Server Role Hierarchy"));
            expect(result->text)->toContain(std::string("## Owners"));
            expect(result->text)->toContain(std::string("SimpleOwnerName (Simple Owner)"));
            expect(result->text)->not->toContain(std::string("## Administrators"));
            expect(result->text)->not->toContain(std::string("## Members"));
            expect(mockRuntime->getWorld)->toHaveBeenCalled();
            expect(mockRuntime->getEntityById)->toHaveBeenCalledWith(ownerId);
        }
        );
        it(std::string("should handle missing entity gracefully (now expecting rejection)"), [=]() mutable
        {
            mockMessage = createMockMemory(object{
                object::pair{std::string("entityId"), otherEntityId}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-for-rejection-test-1"))}
            });
            mockState->data = utils::assign(object{
                , 
                object::pair{std::string("room"), undefined}
            }, mockState->data);
            (as<any>(mockRuntime->getRoom))["mockRejectedValue"](std::make_shared<Error>(std::string("Simulated DB error getting room")));
            std::async([=]() { expect(roleProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)))->rejects->toThrowError(std::string("Simulated DB error getting room")); });
        }
        );
        it(std::string("should handle missing roles gracefully"), [=]() mutable
        {
            auto entityMissingRolesId = as<std::shared_ptr<UUID>>(std::string("entity-no-roles"));
            mockMessage = createMockMemory(object{
                object::pair{std::string("entityId"), entityMissingRolesId}
            });
            auto mockRoomId = as<std::shared_ptr<UUID>>(std::string("room-with-server"));
            shared mockServerId = as<std::shared_ptr<UUID>>(std::string("server-for-roles"));
            mockMessage->roomId = mockRoomId;
            (as<any>(mockRuntime->getRoom))["mockResolvedValue"](object{
                object::pair{std::string("id"), mockRoomId}, 
                object::pair{std::string("serverId"), mockServerId}, 
                object::pair{std::string("type"), ChannelType->GROUP}, 
                object::pair{std::string("source"), std::string("discord")}
            });
            (as<any>(mockRuntime->getWorld))["mockImplementation"]([=](auto id) mutable
            {
                return object{
                    object::pair{std::string("id"), id}, 
                    object::pair{std::string("serverId"), mockServerId}, 
                    object::pair{std::string("name"), std::string("Test World No Roles")}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("ownership"), object{
                            object::pair{std::string("ownerId"), as<std::shared_ptr<UUID>>(std::string("some-owner"))}
                        }}, 
                        object::pair{std::string("roles"), object{}}
                    }}
                };
            }
            );
            auto result = std::async([=]() { roleProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toBeDefined();
            expect(type_of(result->text))->toBe(std::string("string"));
            if (type_of(result->text) == std::string("string")) {
                expect(result->text)->toContain(std::string("No role information available for this server."));
            }
            expect(mockRuntime->getEntityById)->not->toHaveBeenCalled();
        }
        );
        it(std::string("should handle errors gracefully (now expecting rejection)"), [=]() mutable
        {
            mockMessage = createMockMemory(object{
                object::pair{std::string("entityId"), testEntityId}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-for-rejection-test-2"))}
            });
            mockState->data = utils::assign(object{
                , 
                object::pair{std::string("room"), undefined}
            }, mockState->data);
            auto specificError = std::make_shared<Error>(std::string("DB error for roles test"));
            (as<any>(mockRuntime->getRoom))["mockRejectedValue"](specificError);
            std::async([=]() { expect(roleProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)))->rejects->toThrowError(specificError); });
        }
        );
    }
    );
    describe(std::string("Settings Provider"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<Partial<std::shared_ptr<Memory>>> mockMessage;
        shared<Partial<std::shared_ptr<State>>> mockState;
        beforeEach([=]() mutable
        {
            mockRuntime = createMockRuntime();
            mockMessage = createMockMemory(object{
                object::pair{std::string("content"), object{}}, 
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("test-owner-entity-id"))}
            });
            mockState = createMockState(object{
                object::pair{std::string("data"), object{
                    object::pair{std::string("room"), object{
                        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("test-room-id"))}, 
                        object::pair{std::string("worldId"), as<std::shared_ptr<UUID>>(std::string("world-1"))}
                    }}
                }}
            });
            mockRuntime->getRoom = mock()->mockResolvedValue(object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("test-room-id"))}, 
                object::pair{std::string("worldId"), as<std::shared_ptr<UUID>>(std::string("world-1"))}, 
                object::pair{std::string("type"), ChannelType->GROUP}
            });
            mockRuntime->getWorld = mock()->mockResolvedValue(object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("world-1"))}, 
                object::pair{std::string("serverId"), std::string("server-1")}, 
                object::pair{std::string("name"), std::string("Test World")}
            });
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std::string("should retrieve settings in onboarding mode"), [=]() mutable
        {
            mockMessage->content = utils::assign(object{
                , 
                object::pair{std::string("channelType"), ChannelType->DM}
            }, mockMessage->content);
            mockState->data = object{
                object::pair{std::string("room"), object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("onboarding-room-id"))}, 
                    object::pair{std::string("type"), ChannelType->DM}
                }}
            };
            mockMessage->roomId = as<std::shared_ptr<UUID>>(std::string("onboarding-room-id"));
            (as<any>(mockRuntime->getRoom))["mockResolvedValue"](object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("onboarding-room-id"))}, 
                object::pair{std::string("worldId"), as<std::shared_ptr<UUID>>(std::string("world-1"))}, 
                object::pair{std::string("type"), ChannelType->DM}
            });
            auto result = std::async([=]() { settingsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data)->toBeDefined();
            expect(result->text)->toBeDefined();
            expect(type_of(result->text))->toBe(std::string("string"));
        }
        );
        it(std::string("should retrieve settings in normal mode"), [=]() mutable
        {
            mockMessage->content = utils::assign(object{
                , 
                object::pair{std::string("channelType"), ChannelType->GROUP}
            }, mockMessage->content);
            (as<any>(mockRuntime->getRoom))["mockResolvedValue"](object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("test-room-id"))}, 
                object::pair{std::string("worldId"), as<std::shared_ptr<UUID>>(std::string("world-1"))}, 
                object::pair{std::string("type"), ChannelType->GROUP}
            });
            auto result = std::async([=]() { settingsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data)->toBeDefined();
            expect(result->text)->toBeDefined();
            expect(type_of(result->text))->toBe(std::string("string"));
        }
        );
        it(std::string("should handle errors gracefully when getWorldSettings fails"), [=]() mutable
        {
            mockMessage->content = object{
                object::pair{std::string("channelType"), ChannelType->DM}
            };
            mockState->data = object{
                object::pair{std::string("room"), object{
                    object::pair{std::string("type"), ChannelType->DM}, 
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("dm-room-err"))}
                }}
            };
            mockMessage->roomId = as<std::shared_ptr<UUID>>(std::string("dm-room-err"));
            (as<any>(mockRuntime->getWorld))["mockResolvedValue"](nullptr);
            auto result = std::async([=]() { settingsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toBeDefined();
        }
        );
        it(std::string("should handle missing world gracefully"), [=]() mutable
        {
            mockRuntime->getWorld = mock()->mockResolvedValue(nullptr);
            mockMessage->content = object{
                object::pair{std::string("channelType"), ChannelType->GROUP}
            };
            mockState->data = utils::assign(object{
                , 
                object::pair{std::string("room"), object{
                    object::pair{std::string("type"), ChannelType->GROUP}, 
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("group-room-err"))}
                }}
            }, mockState->data);
            mockMessage->roomId = as<std::shared_ptr<UUID>>(std::string("group-room-err"));
            auto result = std::async([=]() { settingsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std::string("Error retrieving configuration information. Please try again later."));
        }
        );
    }
    );
    describe(std::string("Attachments Provider"), [=]() mutable
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
        it(std::string("should handle messages with no attachments"), [=]() mutable
        {
            mockMessage->content = object{
                object::pair{std::string("text"), std::string("Hello, how are you?")}, 
                object::pair{std::string("channelType"), ChannelType->GROUP}
            };
            auto result = std::async([=]() { attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data->attachments)->toHaveLength(0);
            expect(result->text)->toBe(string_empty);
            expect(result->values->attachments)->toBe(string_empty);
        }
        );
        it(std::string("should return current message attachments"), [=]() mutable
        {
            auto testAttachments = array<std::shared_ptr<Media>>{ object{
                object::pair{std::string("id"), std::string("attach-1")}, 
                object::pair{std::string("url"), std::string("https://example.com/image1.jpg")}, 
                object::pair{std::string("title"), std::string("Test Image 1")}, 
                object::pair{std::string("source"), std::string("image/jpeg")}, 
                object::pair{std::string("description"), std::string("A test image")}, 
                object::pair{std::string("text"), std::string("Image content text")}
            }, object{
                object::pair{std::string("id"), std::string("attach-2")}, 
                object::pair{std::string("url"), std::string("https://example.com/document.pdf")}, 
                object::pair{std::string("title"), std::string("Test Document")}, 
                object::pair{std::string("source"), std::string("application/pdf")}, 
                object::pair{std::string("description"), std::string("A test PDF document")}, 
                object::pair{std::string("text"), std::string("Document content text")}
            } };
            mockMessage->content = object{
                object::pair{std::string("text"), std::string("Check out these attachments")}, 
                object::pair{std::string("channelType"), ChannelType->GROUP}, 
                object::pair{std::string("attachments"), testAttachments}
            };
            auto result = std::async([=]() { attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data->attachments)->toHaveLength(2);
            expect(const_(result->data->attachments)[0]->id)->toBe(std::string("attach-1"));
            expect(const_(result->data->attachments)[1]->id)->toBe(std::string("attach-2"));
            expect(result->text)->toContain(std::string("# Attachments"));
            expect(result->text)->toContain(std::string("Test Image 1"));
            expect(result->text)->toContain(std::string("Test Document"));
            expect(result->text)->toContain(std::string("https://example.com/image1.jpg"));
            expect(result->text)->toContain(std::string("Image content text"));
        }
        );
        it(std::string("should merge attachments from recent messages"), [=]() mutable
        {
            auto currentAttachment = object{
                object::pair{std::string("id"), std::string("current-attach")}, 
                object::pair{std::string("url"), std::string("https://example.com/current.jpg")}, 
                object::pair{std::string("title"), std::string("Current Image")}, 
                object::pair{std::string("source"), std::string("image/jpeg")}, 
                object::pair{std::string("description"), std::string("Current attachment")}, 
                object::pair{std::string("text"), std::string("Current content")}
            };
            mockMessage->content = object{
                object::pair{std::string("text"), std::string("Current message with attachment")}, 
                object::pair{std::string("channelType"), ChannelType->GROUP}, 
                object::pair{std::string("attachments"), array<any>{ currentAttachment }}
            };
            auto recentMessages = array<object>{ object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("msg-1"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Previous message 1")}, 
                    object::pair{std::string("attachments"), array<object>{ object{
                        object::pair{std::string("id"), std::string("prev-attach-1")}, 
                        object::pair{std::string("url"), std::string("https://example.com/prev1.jpg")}, 
                        object::pair{std::string("title"), std::string("Previous Image 1")}, 
                        object::pair{std::string("source"), std::string("image/jpeg")}, 
                        object::pair{std::string("description"), std::string("Previous attachment 1")}, 
                        object::pair{std::string("text"), std::string("Previous content 1")}
                    } }}
                }}, 
                object::pair{std::string("createdAt"), Date->now() - 30 * 60 * 1000}
            }, object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("msg-2"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Previous message 2")}, 
                    object::pair{std::string("attachments"), array<object>{ object{
                        object::pair{std::string("id"), std::string("prev-attach-2")}, 
                        object::pair{std::string("url"), std::string("https://example.com/prev2.jpg")}, 
                        object::pair{std::string("title"), std::string("Previous Image 2")}, 
                        object::pair{std::string("source"), std::string("image/jpeg")}, 
                        object::pair{std::string("description"), std::string("Previous attachment 2")}, 
                        object::pair{std::string("text"), std::string("Previous content 2")}
                    } }}
                }}, 
                object::pair{std::string("createdAt"), Date->now() - 15 * 60 * 1000}
            } };
            mockRuntime->getMemories = mock()->mockResolvedValue(recentMessages);
            auto result = std::async([=]() { attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data->attachments)->toHaveLength(3);
            expect(const_(result->data->attachments)[0]->id)->toBe(std::string("current-attach"));
            expect(const_(result->data->attachments)[1]->id)->toBe(std::string("prev-attach-2"));
            expect(const_(result->data->attachments)[2]->id)->toBe(std::string("prev-attach-1"));
            expect(result->text)->toContain(std::string("Current Image"));
            expect(result->text)->toContain(std::string("Previous Image 1"));
            expect(result->text)->toContain(std::string("Previous Image 2"));
        }
        );
        it(std::string("should hide text for attachments older than 1 hour"), [=]() mutable
        {
            auto currentAttachment = object{
                object::pair{std::string("id"), std::string("current-attach")}, 
                object::pair{std::string("url"), std::string("https://example.com/current.jpg")}, 
                object::pair{std::string("title"), std::string("Current Image")}, 
                object::pair{std::string("source"), std::string("image/jpeg")}, 
                object::pair{std::string("description"), std::string("Current attachment")}, 
                object::pair{std::string("text"), std::string("Current content")}
            };
            mockMessage->content = object{
                object::pair{std::string("text"), std::string("Current message")}, 
                object::pair{std::string("channelType"), ChannelType->GROUP}, 
                object::pair{std::string("attachments"), array<any>{ currentAttachment }}
            };
            auto messages = array<object>{ object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("msg-recent"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Recent message")}, 
                    object::pair{std::string("attachments"), array<object>{ object{
                        object::pair{std::string("id"), std::string("recent-attach")}, 
                        object::pair{std::string("url"), std::string("https://example.com/recent.jpg")}, 
                        object::pair{std::string("title"), std::string("Recent Image")}, 
                        object::pair{std::string("source"), std::string("image/jpeg")}, 
                        object::pair{std::string("description"), std::string("Recent attachment")}, 
                        object::pair{std::string("text"), std::string("This should be visible")}
                    } }}
                }}, 
                object::pair{std::string("createdAt"), Date->now() - 30 * 60 * 1000}
            }, object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("msg-old"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Old message")}, 
                    object::pair{std::string("attachments"), array<object>{ object{
                        object::pair{std::string("id"), std::string("old-attach")}, 
                        object::pair{std::string("url"), std::string("https://example.com/old.jpg")}, 
                        object::pair{std::string("title"), std::string("Old Image")}, 
                        object::pair{std::string("source"), std::string("image/jpeg")}, 
                        object::pair{std::string("description"), std::string("Old attachment")}, 
                        object::pair{std::string("text"), std::string("This should be hidden")}
                    } }}
                }}, 
                object::pair{std::string("createdAt"), Date->now() - 2 * 60 * 60 * 1000}
            } };
            mockRuntime->getMemories = mock()->mockResolvedValue(messages);
            auto result = std::async([=]() { attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data->attachments)->toHaveLength(3);
            auto oldAttachment = result->data->attachments->find([=](auto a) mutable
            {
                return a["id"] == std::string("old-attach");
            }
            );
            expect(oldAttachment->text)->toBe(std::string("[Hidden]"));
            auto recentAttachment = result->data->attachments->find([=](auto a) mutable
            {
                return a["id"] == std::string("recent-attach");
            }
            );
            expect(recentAttachment->text)->toBe(std::string("This should be visible"));
            auto currentAtt = result->data->attachments->find([=](auto a) mutable
            {
                return a["id"] == std::string("current-attach");
            }
            );
            expect(currentAtt->text)->toBe(std::string("Current content"));
        }
        );
        it(std::string("should not duplicate attachments with same ID"), [=]() mutable
        {
            auto sharedAttachment = object{
                object::pair{std::string("id"), std::string("shared-attach")}, 
                object::pair{std::string("url"), std::string("https://example.com/shared.jpg")}, 
                object::pair{std::string("title"), std::string("Shared Image")}, 
                object::pair{std::string("source"), std::string("image/jpeg")}, 
                object::pair{std::string("description"), std::string("Shared attachment")}, 
                object::pair{std::string("text"), std::string("Shared content with more details")}
            };
            mockMessage->content = object{
                object::pair{std::string("text"), std::string("Current message")}, 
                object::pair{std::string("channelType"), ChannelType->GROUP}, 
                object::pair{std::string("attachments"), array<any>{ sharedAttachment }}
            };
            auto recentMessages = array<object>{ object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("msg-1"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Previous message")}, 
                    object::pair{std::string("attachments"), array<object>{ object{
                        object::pair{std::string("id"), std::string("shared-attach")}, 
                        object::pair{std::string("url"), std::string("https://example.com/shared.jpg")}, 
                        object::pair{std::string("title"), std::string("Shared Image")}, 
                        object::pair{std::string("source"), std::string("image/jpeg")}, 
                        object::pair{std::string("description"), std::string("Basic description")}, 
                        object::pair{std::string("text"), std::string("Basic text")}
                    } }}
                }}, 
                object::pair{std::string("createdAt"), Date->now() - 10 * 60 * 1000}
            } };
            mockRuntime->getMemories = mock()->mockResolvedValue(recentMessages);
            auto result = std::async([=]() { attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data->attachments)->toHaveLength(1);
            expect(const_(result->data->attachments)[0]->id)->toBe(std::string("shared-attach"));
            expect(const_(result->data->attachments)[0]->text)->toBe(std::string("Shared content with more details"));
            expect(const_(result->data->attachments)[0]->description)->toBe(std::string("Shared attachment"));
        }
        );
        it(std::string("should format attachment data correctly"), [=]() mutable
        {
            auto testAttachment = object{
                object::pair{std::string("id"), std::string("format-test")}, 
                object::pair{std::string("url"), std::string("https://example.com/test.png")}, 
                object::pair{std::string("title"), std::string("Format Test Image")}, 
                object::pair{std::string("source"), std::string("image/png")}, 
                object::pair{std::string("description"), std::string("Testing formatted output")}, 
                object::pair{std::string("text"), std::string("This is the extracted text from the image")}
            };
            mockMessage->content = object{
                object::pair{std::string("text"), std::string("Testing format")}, 
                object::pair{std::string("channelType"), ChannelType->GROUP}, 
                object::pair{std::string("attachments"), array<any>{ testAttachment }}
            };
            auto result = std::async([=]() { attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std::string("# Attachments"));
            expect(result->text)->toContain(std::string("ID: format-test"));
            expect(result->text)->toContain(std::string("Name: Format Test Image"));
            expect(result->text)->toContain(std::string("URL: https://example.com/test.png"));
            expect(result->text)->toContain(std::string("Type: image/png"));
            expect(result->text)->toContain(std::string("Description: Testing formatted output"));
            expect(result->text)->toContain(std::string("Text: This is the extracted text from the image"));
        }
        );
        it(std::string("should handle messages with no recent attachments history"), [=]() mutable
        {
            auto currentAttachment = object{
                object::pair{std::string("id"), std::string("only-attach")}, 
                object::pair{std::string("url"), std::string("https://example.com/only.jpg")}, 
                object::pair{std::string("title"), std::string("Only Attachment")}, 
                object::pair{std::string("source"), std::string("image/jpeg")}, 
                object::pair{std::string("description"), std::string("The only attachment")}, 
                object::pair{std::string("text"), std::string("Only attachment content")}
            };
            mockMessage->content = object{
                object::pair{std::string("text"), std::string("Message with attachment")}, 
                object::pair{std::string("channelType"), ChannelType->GROUP}, 
                object::pair{std::string("attachments"), array<any>{ currentAttachment }}
            };
            auto recentMessages = array<object>{ object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("msg-1"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Text only message 1")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() - 5 * 60 * 1000}
            }, object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("msg-2"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Text only message 2")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() - 2 * 60 * 1000}
            } };
            mockRuntime->getMemories = mock()->mockResolvedValue(recentMessages);
            auto result = std::async([=]() { attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)); });
            expect(result)->toBeDefined();
            expect(result->data->attachments)->toHaveLength(1);
            expect(const_(result->data->attachments)[0]->id)->toBe(std::string("only-attach"));
            expect(result->text)->toContain(std::string("Only Attachment"));
        }
        );
        it(std::string("should handle errors by throwing them"), [=]() mutable
        {
            mockMessage->content = object{
                object::pair{std::string("text"), std::string("Test message")}, 
                object::pair{std::string("channelType"), ChannelType->GROUP}, 
                object::pair{std::string("attachments"), array<object>{ object{
                    object::pair{std::string("id"), std::string("test-attach")}, 
                    object::pair{std::string("url"), std::string("https://example.com/test.jpg")}, 
                    object::pair{std::string("title"), std::string("Test")}, 
                    object::pair{std::string("source"), std::string("image/jpeg")}
                } }}
            };
            mockRuntime->getMemories = mock()->mockRejectedValue(std::make_shared<Error>(std::string("Database error")));
            expect(attachmentsProvider->get(as<std::shared_ptr<IAgentRuntime>>(mockRuntime), as<std::shared_ptr<Memory>>(mockMessage), as<std::shared_ptr<State>>(mockState)))->rejects->toThrow(std::string("Database error"));
        }
        );
    }
    );
}

MAIN
