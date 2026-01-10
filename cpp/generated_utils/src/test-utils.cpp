#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-bootstrap/src/__tests__/test-utils.h"

MockRuntime createMockRuntime(Partial<MockRuntime> overrides)
{
    auto mockRuntime = object{
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent-id"))}, 
        object::pair{std::string("character"), as<std::shared_ptr<Character>>(object{
            object::pair{std::string("name"), std::string("Test Agent")}, 
            object::pair{std::string("bio"), std::string("This is a test agent for unit tests")}, 
            object::pair{std::string("tone"), std::string("helpful")}, 
            object::pair{std::string("templates"), object{
                object::pair{std::string("reflectionTemplate"), std::string("Test reflection template {{recentMessages}}")}, 
                object::pair{std::string("messageHandlerTemplate"), std::string("Test message handler template {{recentMessages}}")}, 
                object::pair{std::string("shouldRespondTemplate"), std::string("Test should respond template {{recentMessages}}")}
            }}
        })}, 
        object::pair{std::string("providers"), array<any>()}, 
        object::pair{std::string("actions"), array<any>()}, 
        object::pair{std::string("evaluators"), array<any>()}, 
        object::pair{std::string("plugins"), array<any>()}, 
        object::pair{std::string("services"), std::make_shared<Map>()}, 
        object::pair{std::string("events"), std::make_shared<Map>()}, 
        object::pair{std::string("routes"), array<any>()}, 
        object::pair{std::string("registerPlugin"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("initialize"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("getKnowledge"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std::string("addKnowledge"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("getService"), mock()->mockReturnValue(nullptr)}, 
        object::pair{std::string("getAllServices"), mock()->mockReturnValue(std::make_shared<Map>())}, 
        object::pair{std::string("registerService"), mock()}, 
        object::pair{std::string("registerDatabaseAdapter"), mock()}, 
        object::pair{std::string("setSetting"), mock()}, 
        object::pair{std::string("getSetting"), mock()->mockReturnValue(nullptr)}, 
        object::pair{std::string("getConversationLength"), mock()->mockReturnValue(10)}, 
        object::pair{std::string("processActions"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("evaluate"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std::string("registerProvider"), mock()}, 
        object::pair{std::string("registerAction"), mock()}, 
        object::pair{std::string("registerEvaluator"), mock()}, 
        object::pair{std::string("ensureConnection"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("ensureParticipantInRoom"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("ensureWorldExists"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("ensureRoomExists"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("db"), object{}}, 
        object::pair{std::string("init"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("close"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("getAgent"), mock()->mockResolvedValue(nullptr)}, 
        object::pair{std::string("getAgents"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std::string("createAgent"), mock()->mockResolvedValue(true)}, 
        object::pair{std::string("updateAgent"), mock()->mockResolvedValue(true)}, 
        object::pair{std::string("deleteAgent"), mock()->mockResolvedValue(true)}, 
        object::pair{std::string("ensureAgentExists"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("ensureEmbeddingDimension"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("getEntityById"), mock()->mockResolvedValue(nullptr)}, 
        object::pair{std::string("getEntitiesForRoom"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std::string("createEntity"), mock()->mockResolvedValue(true)}, 
        object::pair{std::string("updateEntity"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("getComponent"), mock()->mockResolvedValue(nullptr)}, 
        object::pair{std::string("getComponents"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std::string("createComponent"), mock()->mockResolvedValue(true)}, 
        object::pair{std::string("updateComponent"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("deleteComponent"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("getMemories"), mock()->mockImplementation([=](auto params) mutable
        {
            if (AND((params["tableName"] == std::string("facts")), (params["entityId"] == std::string("test-entity-id")))) {
                return Promise->resolve(array<object>{ object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("memory-1"))}, 
                    object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("entity-1"))}, 
                    object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("agent-1"))}, 
                    object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-1"))}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("User likes chocolate")}
                    }}, 
                    object::pair{std::string("embedding"), array<double>{ 0.1, 0.2, 0.3 }}, 
                    object::pair{std::string("createdAt"), Date->now()}
                }, object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("memory-2"))}, 
                    object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("entity-1"))}, 
                    object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("agent-1"))}, 
                    object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-1"))}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("User dislikes spicy food")}
                    }}, 
                    object::pair{std::string("embedding"), array<double>{ 0.2, 0.3, 0.4 }}, 
                    object::pair{std::string("createdAt"), Date->now()}
                } });
            }
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getMemoryById"), mock()->mockResolvedValue(nullptr)}, 
        object::pair{std::string("getMemoriesByIds"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std::string("useModel"), mock()->mockImplementation([=](auto modelType, auto _params) mutable
        {
            if (modelType == ModelType->OBJECT_LARGE) {
                return Promise->resolve(object{
                    object::pair{std::string("thought"), std::string("I should respond in a friendly way")}, 
                    object::pair{std::string("message"), std::string("Hello there! How can I help you today?")}
                });
            } else if (modelType == ModelType->TEXT_SMALL) {
                return Promise->resolve(std::string("yes"));
            } else if (modelType == ModelType->TEXT_EMBEDDING) {
                return Promise->resolve(array<double>{ 0.1, 0.2, 0.3, 0.4, 0.5 });
            }
            return Promise->resolve(object{});
        }
        )}, 
        object::pair{std::string("composePrompt"), mock()->mockReturnValue(std::string("Composed prompt"))}, 
        object::pair{std::string("composeState"), mock()->mockResolvedValue(object{
            object::pair{std::string("values"), object{}}, 
            object::pair{std::string("data"), object{}}
        })}, 
        object::pair{std::string("createMemory"), mock()->mockResolvedValue(object{
            object::pair{std::string("id"), std::string("memory-id")}
        })}, 
        object::pair{std::string("getRoom"), mock()->mockResolvedValue(object{
            object::pair{std::string("id"), std::string("room-id")}, 
            object::pair{std::string("name"), std::string("Test Room")}, 
            object::pair{std::string("worldId"), std::string("test-world-id")}, 
            object::pair{std::string("serverId"), std::string("test-server-id")}
        })}, 
        object::pair{std::string("getRooms"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std::string("id"), std::string("room-id")}, 
            object::pair{std::string("name"), std::string("Test Room")}, 
            object::pair{std::string("worldId"), std::string("test-world-id")}, 
            object::pair{std::string("serverId"), std::string("test-server-id")}
        } })}, 
        object::pair{std::string("getWorld"), mock()->mockResolvedValue(object{
            object::pair{std::string("id"), std::string("test-world-id")}, 
            object::pair{std::string("name"), std::string("Test World")}, 
            object::pair{std::string("serverId"), std::string("test-server-id")}, 
            object::pair{std::string("metadata"), object{
                object::pair{std::string("roles"), object{
                    object::pair{std::string("test-entity-id"), std::string("ADMIN")}, 
                    object::pair{std::string("test-agent-id"), std::string("OWNER")}
                }}, 
                object::pair{std::string("settings"), array<object>{ object{
                    object::pair{std::string("name"), std::string("setting1")}, 
                    object::pair{std::string("value"), std::string("value1")}, 
                    object::pair{std::string("description"), std::string("Description 1")}
                }, object{
                    object::pair{std::string("name"), std::string("setting2")}, 
                    object::pair{std::string("value"), std::string("value2")}, 
                    object::pair{std::string("description"), std::string("Description 2")}
                } }}
            }}
        })}, 
        object::pair{std::string("addEmbeddingToMemory"), mock()->mockResolvedValue(object{
            object::pair{std::string("id"), std::string("memory-id")}, 
            object::pair{std::string("entityId"), std::string("test-entity-id")}, 
            object::pair{std::string("roomId"), std::string("test-room-id")}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("Test fact")}
            }}
        })}, 
        object::pair{std::string("createRelationship"), mock()->mockResolvedValue(true)}, 
        object::pair{std::string("updateRelationship"), mock()->mockResolvedValue(true)}, 
        object::pair{std::string("getRelationships"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std::string("addRelationship"), mock()->mockResolvedValue(true)}, 
        object::pair{std::string("getTasks"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std::string("getTasksByName"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std::string("createTask"), mock()->mockResolvedValue(object{
            object::pair{std::string("id"), std::string("task-id")}
        })}, 
        object::pair{std::string("updateTasks"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std::string("deleteTasks"), mock()->mockResolvedValue(array<any>())}, 
        object::pair{std::string("deleteTask"), mock()->mockResolvedValue(true)}, 
        object::pair{std::string("emitEvent"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("registerEvent"), mock()}, 
        object::pair{std::string("getCache"), mock()->mockResolvedValue(nullptr)}, 
        object::pair{std::string("setCache"), mock()->mockResolvedValue(true)}, 
        object::pair{std::string("registerTaskWorker"), mock()}, 
        object::pair{std::string("getTaskWorker"), mock()->mockReturnValue(object{
            object::pair{std::string("name"), std::string("test-worker")}, 
            object::pair{std::string("validate"), mock()->mockResolvedValue(true)}, 
            object::pair{std::string("execute"), mock()->mockResolvedValue(object{})}
        })}, 
        object::pair{std::string("getParticipantUserState"), mock()->mockResolvedValue(std::string("ACTIVE"))}, 
        object::pair{std::string("setParticipantUserState"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("updateParticipantUserState"), mock()->mockResolvedValue(undefined)}, 
        object::pair{std::string("getUserServerRole"), mock()->mockResolvedValue(std::string("USER"))}, 
        object::pair{std::string("findEntityByName"), mock()->mockResolvedValue(nullptr)}, 
        object::pair{std::string("getMemberRole"), mock()->mockResolvedValue(std::string("USER"))}, 
        object::pair{std::string("searchMemories"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("memory-1"))}, 
            object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("entity-1"))}, 
            object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("agent-1"))}, 
            object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-1"))}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("User likes chocolate")}
            }}, 
            object::pair{std::string("embedding"), array<double>{ 0.1, 0.2, 0.3 }}, 
            object::pair{std::string("createdAt"), Date->now()}, 
            object::pair{std::string("similarity"), 0.95}
        } })}, 
        object::pair{std::string("getRoomsForParticipants"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std::string("id"), std::string("room-id")}, 
            object::pair{std::string("name"), std::string("Test Room")}, 
            object::pair{std::string("worldId"), std::string("test-world-id")}, 
            object::pair{std::string("serverId"), std::string("test-server-id")}
        } })}, 
        object::pair{std::string("getRoomsForEntity"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std::string("id"), std::string("room-id")}, 
            object::pair{std::string("name"), std::string("Test Room")}, 
            object::pair{std::string("worldId"), std::string("test-world-id")}, 
            object::pair{std::string("serverId"), std::string("test-server-id")}
        } })}, 
        object::pair{std::string("searchEntities"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std::string("id"), std::string("test-entity-id")}, 
            object::pair{std::string("names"), array<string>{ std::string("Test Entity") }}, 
            object::pair{std::string("worldId"), std::string("test-world-id")}
        } })}, 
        object::pair{std::string("searchRooms"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std::string("id"), std::string("room-id")}, 
            object::pair{std::string("name"), std::string("Test Room")}, 
            object::pair{std::string("worldId"), std::string("test-world-id")}
        } })}, 
        object::pair{std::string("getEntity"), mock()->mockResolvedValue(object{
            object::pair{std::string("id"), std::string("test-entity-id")}, 
            object::pair{std::string("names"), array<string>{ std::string("Test Entity") }}, 
            object::pair{std::string("worldId"), std::string("test-world-id")}, 
            object::pair{std::string("serverId"), std::string("test-server-id")}
        })}, 
        object::pair{std::string("getWorldSettings"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std::string("name"), std::string("setting1")}, 
            object::pair{std::string("value"), std::string("value1")}, 
            object::pair{std::string("description"), std::string("Description 1")}
        }, object{
            object::pair{std::string("name"), std::string("setting2")}, 
            object::pair{std::string("value"), std::string("value2")}, 
            object::pair{std::string("description"), std::string("Description 2")}
        } })}, 
        object::pair{std::string("findWorldsForOwner"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std::string("id"), std::string("test-world-id")}, 
            object::pair{std::string("name"), std::string("Test World")}, 
            object::pair{std::string("serverId"), std::string("test-server-id")}
        } })}, 
        object::pair{std::string("uploadFile"), mock()->mockResolvedValue(object{
            object::pair{std::string("id"), std::string("file-id")}, 
            object::pair{std::string("name"), std::string("test.txt")}
        })}, 
        object::pair{std::string("getFile"), mock()->mockResolvedValue(object{
            object::pair{std::string("id"), std::string("file-id")}, 
            object::pair{std::string("content"), std::string("Test file content")}
        })}, 
        object::pair{std::string("listFiles"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std::string("id"), std::string("file-id")}, 
            object::pair{std::string("name"), std::string("test.txt")}
        } })}, 
        object::pair{std::string("deleteFile"), mock()->mockResolvedValue(true)}, 
        object::pair{std::string("extractTextFromPDF"), mock()->mockResolvedValue(std::string("Extracted text from PDF"))}, 
        object::pair{std::string("describeImage"), mock()->mockResolvedValue(std::string("An image description"))}, 
        object::pair{std::string("getMemoriesByRoomIds"), mock()->mockResolvedValue(array<object>{ object{
            object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("memory-1"))}, 
            object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("test-entity-id"))}, 
            object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent-id"))}, 
            object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("test-room-id"))}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("Hello there!")}, 
                object::pair{std::string("channelType"), ChannelType->GROUP}
            }}, 
            object::pair{std::string("createdAt"), Date->now() - 1000}
        }, object{
            object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("memory-2"))}, 
            object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("test-agent-id"))}, 
            object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent-id"))}, 
            object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("test-room-id"))}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("How can I help you?")}, 
                object::pair{std::string("channelType"), ChannelType->GROUP}
            }}, 
            object::pair{std::string("createdAt"), Date->now()}
        } })}, 
        object::pair{std::string("createRunId"), mock()->mockReturnValue(as<std::shared_ptr<UUID>>(std::string("test-run-id")))}, 
        object::pair{std::string("startRun"), mock()->mockReturnValue(as<std::shared_ptr<UUID>>(std::string("test-run-id")))}, 
        object::pair{std::string("endRun"), mock()->mockReturnValue(undefined)}, 
        object::pair{std::string("getCurrentRunId"), mock()->mockReturnValue(as<std::shared_ptr<UUID>>(std::string("test-run-id")))}
    };
    return utils::assign(object{
    }, mockRuntime, overrides);
};


Partial<std::shared_ptr<Memory>> createMockMemory(Partial<std::shared_ptr<Memory>> overrides)
{
    return utils::assign(object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("test-message-id"))}, 
        object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("test-room-id"))}, 
        object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("test-entity-id"))}, 
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent-id"))}, 
        object::pair{std::string("content"), as<std::shared_ptr<Content>>(object{
            object::pair{std::string("text"), std::string("Test message")}, 
            object::pair{std::string("channelType"), ChannelType->GROUP}
        })}, 
        object::pair{std::string("createdAt"), Date->now()}
    }, overrides);
};


Partial<std::shared_ptr<State>> createMockState(Partial<std::shared_ptr<State>> overrides)
{
    return utils::assign(object{
        object::pair{std::string("values"), utils::assign(object{
            object::pair{std::string("agentName"), std::string("Test Agent")}, 
            object::pair{std::string("recentMessages"), std::string("User: Test message")}
        }, overrides->values)}, 
        object::pair{std::string("data"), utils::assign(object{
            object::pair{std::string("room"), object{
                object::pair{std::string("id"), std::string("test-room-id")}, 
                object::pair{std::string("type"), ChannelType->GROUP}, 
                object::pair{std::string("worldId"), std::string("test-world-id")}, 
                object::pair{std::string("serverId"), std::string("test-server-id")}
            }}
        }, overrides->data)}
    }, overrides);
};


any createMockService(Partial<Record<string, any>> overrides)
{
    return utils::assign(object{
        object::pair{std::string("name"), std::string("mock-service")}, 
        object::pair{std::string("type"), std::string("mock")}, 
        object::pair{std::string("execute"), mock()->mockResolvedValue(object{})}, 
        object::pair{std::string("init"), mock()->mockResolvedValue(object{})}
    }, overrides);
};


any setupActionTest(object options)
{
    auto mockRuntime = createMockRuntime(options["runtimeOverrides"]);
    auto mockMessage = createMockMemory(options["messageOverrides"]);
    auto mockState = createMockState(options["stateOverrides"]);
    auto callbackFn = mock()->mockResolvedValue(as<array<std::shared_ptr<Memory>>>(array<std::shared_ptr<Memory>>()));
    return object{
        object::pair{std::string("mockRuntime"), std::string("mockRuntime")}, 
        object::pair{std::string("mockMessage"), std::string("mockMessage")}, 
        object::pair{std::string("mockState"), std::string("mockState")}, 
        object::pair{std::string("callbackFn"), std::string("callbackFn")}
    };
};


