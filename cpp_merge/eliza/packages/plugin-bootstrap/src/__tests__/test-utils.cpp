#include "test-utils.h"

MockRuntime createMockRuntime(Partial<MockRuntime> overrides)
{
    auto mockRuntime = object{
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent-id"))}, 
        object::pair{std:("character"), as<std::shared_ptr<Character>>(object{
            object::pair{std:("name"), std:("Test Agent")}, 
            object::pair{std:("bio"), std:("This is a test agent for unit tests")}, 
            object::pair{std:("tone"), std:("helpful")}, 
            object::pair{std:("templates"), object{
                object::pair{std:("reflectionTemplate"), std:("Test reflection template {{recentMessages}}")}, 
                object::pair{std:("messageHandlerTemplate"), std:("Test message handler template {{recentMessages}}")}, 
                object::pair{std:("shouldRespondTemplate"), std:("Test should respond template {{recentMessages}}")}
            }}
        })}, 
        object::pair{std:("providers"), array<any>()}, 
        object::pair{std:("actions"), array<any>()}, 
        object::pair{std:("evaluators"), array<any>()}, 
        object::pair{std:("plugins"), array<any>()}, 
        object::pair{std:("services"), std::make_shared<Map>()}, 
        object::pair{std:("events"), std::make_shared<Map>()}, 
        object::pair{std:("routes"), array<any>()}, 
        object::pair{std:("registerPlugin"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("initialize"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("getKnowledge"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std:("addKnowledge"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("getService"), mock()->mockReturnValue(nullptr)}, 
        object::pair{std:("getAllServices"), mock()->mockReturnValue(std::make_shared<Map>())}, 
        object::pair{std:("registerService"), mock()}, 
        object::pair{std:("registerDatabaseAdapter"), mock()}, 
        object::pair{std:("setSetting"), mock()}, 
        object::pair{std:("getSetting"), mock()->mockReturnValue(nullptr)}, 
        object::pair{std:("getConversationLength"), mock()->mockReturnValue(10)}, 
        object::pair{std:("processActions"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("evaluate"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std:("registerProvider"), mock()}, 
        object::pair{std:("registerAction"), mock()}, 
        object::pair{std:("registerEvaluator"), mock()}, 
        object::pair{std:("ensureConnection"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("ensureParticipantInRoom"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("ensureWorldExists"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("ensureRoomExists"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("db"), object{}}, 
        object::pair{std:("init"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("close"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("getAgent"), mock()->mockResolvedValue(nullptr)}, 
        object::pair{std:("getAgents"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std:("createAgent"), mock()->mockResolvedValue(true)}, 
        object::pair{std:("updateAgent"), mock()->mockResolvedValue(true)}, 
        object::pair{std:("deleteAgent"), mock()->mockResolvedValue(true)}, 
        object::pair{std:("ensureAgentExists"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("ensureEmbeddingDimension"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("getEntityById"), mock()->mockResolvedValue(nullptr)}, 
        object::pair{std:("getEntitiesForRoom"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std:("createEntity"), mock()->mockResolvedValue(true)}, 
        object::pair{std:("updateEntity"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("getComponent"), mock()->mockResolvedValue(nullptr)}, 
        object::pair{std:("getComponents"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std:("createComponent"), mock()->mockResolvedValue(true)}, 
        object::pair{std:("updateComponent"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("deleteComponent"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("getMemories"), mock()->mockImplementation([=](auto params) mutable
        {
            if (AND((params["tableName"] == std:("facts")), (params["entityId"] == std:("test-entity-id")))) {
                return Promise->resolve(array<object>{ object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("memory-1"))}, 
                    object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("entity-1"))}, 
                    object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("agent-1"))}, 
                    object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-1"))}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("User likes chocolate")}
                    }}, 
                    object::pair{std:("embedding"), array<double>{ 0.1, 0.2, 0.3 }}, 
                    object::pair{std:("createdAt"), Date->now()}
                }, object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("memory-2"))}, 
                    object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("entity-1"))}, 
                    object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("agent-1"))}, 
                    object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-1"))}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("User dislikes spicy food")}
                    }}, 
                    object::pair{std:("embedding"), array<double>{ 0.2, 0.3, 0.4 }}, 
                    object::pair{std:("createdAt"), Date->now()}
                } });
            }
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getMemoryById"), mock()->mockResolvedValue(nullptr)}, 
        object::pair{std:("getMemoriesByIds"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std:("useModel"), mock()->mockImplementation([=](auto modelType, auto _params) mutable
        {
            if (modelType == ModelType->OBJECT_LARGE) {
                return Promise->resolve(object{
                    object::pair{std:("thought"), std:("I should respond in a friendly way")}, 
                    object::pair{std:("message"), std:("Hello there! How can I help you today?")}
                });
            } else if (modelType == ModelType->TEXT_SMALL) {
                return Promise->resolve(std:("yes"));
            } else if (modelType == ModelType->TEXT_EMBEDDING) {
                return Promise->resolve(array<double>{ 0.1, 0.2, 0.3, 0.4, 0.5 });
            }
            return Promise->resolve(object{});
        }
        )}, 
        object::pair{std:("composePrompt"), mock()->mockReturnValue(std:("Composed prompt"))}, 
        object::pair{std:("composeState"), mock()->mockResolvedValue(object{
            object::pair{std:("values"), object{}}, 
            object::pair{std:("data"), object{}}
        })}, 
        object::pair{std:("createMemory"), mock()->mockResolvedValue(object{
            object::pair{std:("id"), std:("memory-id")}
        })}, 
        object::pair{std:("getRoom"), mock()->mockResolvedValue(object{
            object::pair{std:("id"), std:("room-id")}, 
            object::pair{std:("name"), std:("Test Room")}, 
            object::pair{std:("worldId"), std:("test-world-id")}, 
            object::pair{std:("serverId"), std:("test-server-id")}
        })}, 
        object::pair{std:("getRooms"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std:("id"), std:("room-id")}, 
            object::pair{std:("name"), std:("Test Room")}, 
            object::pair{std:("worldId"), std:("test-world-id")}, 
            object::pair{std:("serverId"), std:("test-server-id")}
        } })}, 
        object::pair{std:("getWorld"), mock()->mockResolvedValue(object{
            object::pair{std:("id"), std:("test-world-id")}, 
            object::pair{std:("name"), std:("Test World")}, 
            object::pair{std:("serverId"), std:("test-server-id")}, 
            object::pair{std:("metadata"), object{
                object::pair{std:("roles"), object{
                    object::pair{std:("test-entity-id"), std:("ADMIN")}, 
                    object::pair{std:("test-agent-id"), std:("OWNER")}
                }}, 
                object::pair{std:("settings"), array<object>{ object{
                    object::pair{std:("name"), std:("setting1")}, 
                    object::pair{std:("value"), std:("value1")}, 
                    object::pair{std:("description"), std:("Description 1")}
                }, object{
                    object::pair{std:("name"), std:("setting2")}, 
                    object::pair{std:("value"), std:("value2")}, 
                    object::pair{std:("description"), std:("Description 2")}
                } }}
            }}
        })}, 
        object::pair{std:("addEmbeddingToMemory"), mock()->mockResolvedValue(object{
            object::pair{std:("id"), std:("memory-id")}, 
            object::pair{std:("entityId"), std:("test-entity-id")}, 
            object::pair{std:("roomId"), std:("test-room-id")}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("Test fact")}
            }}
        })}, 
        object::pair{std:("createRelationship"), mock()->mockResolvedValue(true)}, 
        object::pair{std:("updateRelationship"), mock()->mockResolvedValue(true)}, 
        object::pair{std:("getRelationships"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std:("addRelationship"), mock()->mockResolvedValue(true)}, 
        object::pair{std:("getTasks"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std:("getTasksByName"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std:("createTask"), mock()->mockResolvedValue(object{
            object::pair{std:("id"), std:("task-id")}
        })}, 
        object::pair{std:("updateTasks"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std:("deleteTasks"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std:("deleteTask"), mock()->mockResolvedValue(true)}, 
        object::pair{std:("emitEvent"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("registerEvent"), mock()}, 
        object::pair{std:("getCache"), mock()->mockResolvedValue(nullptr)}, 
        object::pair{std:("setCache"), mock()->mockResolvedValue(true)}, 
        object::pair{std:("registerTaskWorker"), mock()}, 
        object::pair{std:("getTaskWorker"), mock()->mockReturnValue(object{
            object::pair{std:("name"), std:("test-worker")}, 
            object::pair{std:("validate"), mock()->mockResolvedValue(true)}, 
            object::pair{std:("execute"), mock()->mockResolvedValue(object{})}
        })}, 
        object::pair{std:("getParticipantUserState"), mock()->mockResolvedValue(std:("ACTIVE"))}, 
        object::pair{std:("setParticipantUserState"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("updateParticipantUserState"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std:("getUserServerRole"), mock()->mockResolvedValue(std:("USER"))}, 
        object::pair{std:("findEntityByName"), mock()->mockResolvedValue(nullptr)}, 
        object::pair{std:("getMemberRole"), mock()->mockResolvedValue(std:("USER"))}, 
        object::pair{std:("searchMemories"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("memory-1"))}, 
            object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("entity-1"))}, 
            object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("agent-1"))}, 
            object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-1"))}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("User likes chocolate")}
            }}, 
            object::pair{std:("embedding"), array<double>{ 0.1, 0.2, 0.3 }}, 
            object::pair{std:("createdAt"), Date->now()}, 
            object::pair{std:("similarity"), 0.95}
        } })}, 
        object::pair{std:("getRoomsForParticipants"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std:("id"), std:("room-id")}, 
            object::pair{std:("name"), std:("Test Room")}, 
            object::pair{std:("worldId"), std:("test-world-id")}, 
            object::pair{std:("serverId"), std:("test-server-id")}
        } })}, 
        object::pair{std:("getRoomsForEntity"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std:("id"), std:("room-id")}, 
            object::pair{std:("name"), std:("Test Room")}, 
            object::pair{std:("worldId"), std:("test-world-id")}, 
            object::pair{std:("serverId"), std:("test-server-id")}
        } })}, 
        object::pair{std:("searchEntities"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std:("id"), std:("test-entity-id")}, 
            object::pair{std:("names"), array<string>{ std:("Test Entity") }}, 
            object::pair{std:("worldId"), std:("test-world-id")}
        } })}, 
        object::pair{std:("searchRooms"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std:("id"), std:("room-id")}, 
            object::pair{std:("name"), std:("Test Room")}, 
            object::pair{std:("worldId"), std:("test-world-id")}
        } })}, 
        object::pair{std:("getEntity"), mock()->mockResolvedValue(object{
            object::pair{std:("id"), std:("test-entity-id")}, 
            object::pair{std:("names"), array<string>{ std:("Test Entity") }}, 
            object::pair{std:("worldId"), std:("test-world-id")}, 
            object::pair{std:("serverId"), std:("test-server-id")}
        })}, 
        object::pair{std:("getWorldSettings"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std:("name"), std:("setting1")}, 
            object::pair{std:("value"), std:("value1")}, 
            object::pair{std:("description"), std:("Description 1")}
        }, object{
            object::pair{std:("name"), std:("setting2")}, 
            object::pair{std:("value"), std:("value2")}, 
            object::pair{std:("description"), std:("Description 2")}
        } })}, 
        object::pair{std:("findWorldsForOwner"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std:("id"), std:("test-world-id")}, 
            object::pair{std:("name"), std:("Test World")}, 
            object::pair{std:("serverId"), std:("test-server-id")}
        } })}, 
        object::pair{std:("uploadFile"), mock()->mockResolvedValue(object{
            object::pair{std:("id"), std:("file-id")}, 
            object::pair{std:("name"), std:("test.txt")}
        })}, 
        object::pair{std:("getFile"), mock()->mockResolvedValue(object{
            object::pair{std:("id"), std:("file-id")}, 
            object::pair{std:("content"), std:("Test file content")}
        })}, 
        object::pair{std:("listFiles"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std:("id"), std:("file-id")}, 
            object::pair{std:("name"), std:("test.txt")}
        } })}, 
        object::pair{std:("deleteFile"), mock()->mockResolvedValue(true)}, 
        object::pair{std:("extractTextFromPDF"), mock()->mockResolvedValue(std:("Extracted text from PDF"))}, 
        object::pair{std:("describeImage"), mock()->mockResolvedValue(std:("An image description"))}, 
        object::pair{std:("getMemoriesByRoomIds"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("memory-1"))}, 
            object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("test-entity-id"))}, 
            object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent-id"))}, 
            object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("test-room-id"))}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("Hello there!")}, 
                object::pair{std:("channelType"), ChannelType->GROUP}
            }}, 
            object::pair{std:("createdAt"), Date->now() - 1000}
        }, object{
            object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("memory-2"))}, 
            object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("test-agent-id"))}, 
            object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent-id"))}, 
            object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("test-room-id"))}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("How can I help you?")}, 
                object::pair{std:("channelType"), ChannelType->GROUP}
            }}, 
            object::pair{std:("createdAt"), Date->now()}
        } })}, 
        object::pair{std:("createRunId"), mock()->mockReturnValue(as<std::shared_ptr<UUID>>(std:("test-run-id")))}, 
        object::pair{std:("startRun"), mock()->mockReturnValue(as<std::shared_ptr<UUID>>(std:("test-run-id")))}, 
        object::pair{std:("endRun"), mock()->mockReturnValue(undefined)}, 
        object::pair{std:("getCurrentRunId"), mock()->mockReturnValue(as<std::shared_ptr<UUID>>(std:("test-run-id")))}
    };
    return utils::assign(object{
    }, mockRuntime, overrides);
};


Partial<std::shared_ptr<Memory>> createMockMemory(Partial<std::shared_ptr<Memory>> overrides)
{
    return utils::assign(object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("test-message-id"))}, 
        object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("test-room-id"))}, 
        object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("test-entity-id"))}, 
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent-id"))}, 
        object::pair{std:("content"), as<std::shared_ptr<Content>>(object{
            object::pair{std:("text"), std:("Test message")}, 
            object::pair{std:("channelType"), ChannelType->GROUP}
        })}, 
        object::pair{std:("createdAt"), Date->now()}
    }, overrides);
};


Partial<std::shared_ptr<State>> createMockState(Partial<std::shared_ptr<State>> overrides)
{
    return utils::assign(object{
        object::pair{std:("values"), utils::assign(object{
            object::pair{std:("agentName"), std:("Test Agent")}, 
            object::pair{std:("recentMessages"), std:("User: Test message")}
        }, overrides->values)}, 
        object::pair{std:("data"), utils::assign(object{
            object::pair{std:("room"), object{
                object::pair{std:("id"), std:("test-room-id")}, 
                object::pair{std:("type"), ChannelType->GROUP}, 
                object::pair{std:("worldId"), std:("test-world-id")}, 
                object::pair{std:("serverId"), std:("test-server-id")}
            }}
        }, overrides->data)}
    }, overrides);
};


any createMockService(Partial<Record<string, any>> overrides)
{
    return utils::assign(object{
        object::pair{std:("name"), std:("mock-service")}, 
        object::pair{std:("type"), std:("mock")}, 
        object::pair{std:("execute"), mock()->mockResolvedValue(object{})}, 
        object::pair{std:("init"), mock()->mockResolvedValue(object{})}
    }, overrides);
};


any setupActionTest(object options)
{
    auto mockRuntime = createMockRuntime(options["runtimeOverrides"]);
    auto mockMessage = createMockMemory(options["messageOverrides"]);
    auto mockState = createMockState(options["stateOverrides"]);
    auto callbackFn = mock()->mockResolvedValue(as<array<std::shared_ptr<Memory>>>(array<std::shared_ptr<Memory>>()));
    return object{
        object::pair{std:("mockRuntime"), std:("mockRuntime")}, 
        object::pair{std:("mockMessage"), std:("mockMessage")}, 
        object::pair{std:("mockState"), std:("mockState")}, 
        object::pair{std:("callbackFn"), std:("callbackFn")}
    };
};


