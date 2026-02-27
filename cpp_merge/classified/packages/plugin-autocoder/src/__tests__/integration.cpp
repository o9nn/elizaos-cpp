#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/__tests__/integration.test.h"

object mockSqlPlugin = object{
    object::pair{std:("name"), std:("@elizaos/plugin-sql")}, 
    object::pair{std:("description"), std:("Mock SQL plugin for testing")}, 
    object::pair{std:("actions"), array<any>()}, 
    object::pair{std:("providers"), array<any>()}, 
    object::pair{std:("services"), array<any>()}, 
    object::pair{std:("init"), [=]() mutable
    {
        elizaLogger->info(std:("Mock SQL plugin initialized"));
    }
    }
};
std::function<std::shared_ptr<Promise<any>>()> createRealRuntime = [=]() mutable
{
    process->env->FORCE_BUNSQLITE = std:("true");
    process->env->DATABASE_PATH = std:(":memory:");
    process->env->ELIZA_TEST_MODE = std:("true");
    process->env->SECRET_SALT = OR((process->env->SECRET_SALT), (std:("test-salt-for-testing-only-not-secure")));
    auto runtime = std::make_shared<AgentRuntime>(object{
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
        object::pair{std:("character"), object{
            object::pair{std:("name"), std:("Test Agent")}, 
            object::pair{std:("bio"), array<string>{ std:("Test agent for autocoder") }}, 
            object::pair{std:("secrets"), object{}}, 
            object::pair{std:("settings"), object{}}, 
            object::pair{std:("plugins"), array<any>()}
        }}
    });
    (as<any>(runtime))["adapter"] = object{
        object::pair{std:("query"), [=]() mutable
        {
            return array<any>();
        }
        }, 
        object::pair{std:("execute"), [=]() mutable
        {
            return (object{
                object::pair{std:("rows"), array<any>()}
            });
        }
        }, 
        object::pair{std:("initialized"), true}, 
        object::pair{std:("init"), [=]() mutable
        {
        }
        }, 
        object::pair{std:("close"), [=]() mutable
        {
        }
        }, 
        object::pair{std:("getAgents"), [=]() mutable
        {
            return array<any>();
        }
        }, 
        object::pair{std:("createAgent"), [=](auto agent) mutable
        {
            return agent;
        }
        }, 
        object::pair{std:("updateAgent"), [=](auto id, auto updates) mutable
        {
            return (utils::assign(object{
                object::pair{std:("id"), std:("id")}
            }, updates));
        }
        }, 
        object::pair{std:("deleteAgent"), [=](auto id) mutable
        {
        }
        }, 
        object::pair{std:("getAgent"), [=](auto id) mutable
        {
            return (object{
                object::pair{std:("id"), std:("id")}, 
                object::pair{std:("name"), std:("Test Agent")}
            });
        }
        }, 
        object::pair{std:("getRooms"), [=]() mutable
        {
            return array<any>();
        }
        }, 
        object::pair{std:("getRoomsByIds"), [=](auto ids) mutable
        {
            return ids->map([=](auto id) mutable
            {
                return (object{
                    object::pair{std:("id"), std:("id")}
                });
            }
            );
        }
        }, 
        object::pair{std:("createRoom"), [=](auto room) mutable
        {
            return room;
        }
        }, 
        object::pair{std:("updateRoom"), [=](auto id, auto updates) mutable
        {
            return (utils::assign(object{
                object::pair{std:("id"), std:("id")}
            }, updates));
        }
        }, 
        object::pair{std:("deleteRoom"), [=](auto id) mutable
        {
        }
        }, 
        object::pair{std:("getRoom"), [=](auto id) mutable
        {
            return (object{
                object::pair{std:("id"), std:("id")}
            });
        }
        }, 
        object::pair{std:("getParticipants"), [=]() mutable
        {
            return array<any>();
        }
        }, 
        object::pair{std:("getParticipantsByIds"), [=](auto ids) mutable
        {
            return ids->map([=](auto id) mutable
            {
                return (object{
                    object::pair{std:("id"), std:("id")}
                });
            }
            );
        }
        }, 
        object::pair{std:("getParticipantsForRoom"), [=](auto roomId) mutable
        {
            return array<any>();
        }
        }, 
        object::pair{std:("addParticipantsToRoom"), [=](auto participantIds, auto roomId) mutable
        {
            return true;
        }
        }, 
        object::pair{std:("createParticipant"), [=](auto participant) mutable
        {
            return participant;
        }
        }, 
        object::pair{std:("updateParticipant"), [=](auto id, auto updates) mutable
        {
            return (utils::assign(object{
                object::pair{std:("id"), std:("id")}
            }, updates));
        }
        }, 
        object::pair{std:("deleteParticipant"), [=](auto id) mutable
        {
        }
        }, 
        object::pair{std:("getParticipant"), [=](auto id) mutable
        {
            return (object{
                object::pair{std:("id"), std:("id")}
            });
        }
        }, 
        object::pair{std:("getMemories"), [=]() mutable
        {
            return array<any>();
        }
        }, 
        object::pair{std:("getMemoriesByIds"), [=](auto ids) mutable
        {
            return ids->map([=](auto id) mutable
            {
                return (object{
                    object::pair{std:("id"), std:("id")}
                });
            }
            );
        }
        }, 
        object::pair{std:("createMemory"), [=](auto memory) mutable
        {
            return memory;
        }
        }, 
        object::pair{std:("updateMemory"), [=](auto id, auto updates) mutable
        {
            return (utils::assign(object{
                object::pair{std:("id"), std:("id")}
            }, updates));
        }
        }, 
        object::pair{std:("deleteMemory"), [=](auto id) mutable
        {
        }
        }, 
        object::pair{std:("getMemory"), [=](auto id) mutable
        {
            return (object{
                object::pair{std:("id"), std:("id")}
            });
        }
        }, 
        object::pair{std:("getEntitiesByIds"), [=](auto ids) mutable
        {
            return ids->map([=](auto id) mutable
            {
                return (object{
                    object::pair{std:("id"), std:("id")}
                });
            }
            );
        }
        }, 
        object::pair{std:("createEntity"), [=](auto entity) mutable
        {
            return entity;
        }
        }, 
        object::pair{std:("updateEntity"), [=](auto id, auto updates) mutable
        {
            return (utils::assign(object{
                object::pair{std:("id"), std:("id")}
            }, updates));
        }
        }, 
        object::pair{std:("deleteEntity"), [=](auto id) mutable
        {
        }
        }, 
        object::pair{std:("getEntity"), [=](auto id) mutable
        {
            return (object{
                object::pair{std:("id"), std:("id")}
            });
        }
        }, 
        object::pair{std:("updateWorld"), [=](auto world) mutable
        {
            return world;
        }
        }, 
        object::pair{std:("addParticipantsRoom"), [=](auto entityIds, auto roomId) mutable
        {
            return true;
        }
        }, 
        object::pair{std:("removeParticipantsRoom"), [=](auto entityIds, auto roomId) mutable
        {
            return true;
        }
        }
    };
    std::async([=]() { runtime->registerPlugin(formsPlugin); });
    std::async([=]() { runtime->registerPlugin(autocoderPlugin); });
    std::async([=]() { runtime->initialize(); });
    elizaLogger->info(std:("Runtime initialized"), object{
        object::pair{std:("hasForms"), runtime->getService(std:("forms")) != undefined}, 
        object::pair{std:("hasCodeGen"), runtime->getService(std:("code-generation")) != undefined}
    });
    return runtime;
};

void Main(void)
{
    describe(std:("Generate Code Action"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        beforeEach([=]() mutable
        {
            runtime = std::async([=]() { createRealRuntime(); });
        }
        );
        it(std:("should validate messages correctly"), [=]() mutable
        {
            auto message = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("generate test-plugin")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { generateCodeAction->validate(runtime, message); });
            expect(isValid)->toBe(true);
        }
        );
        it(std:("should handle generation"), [=]() mutable
        {
            auto message = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("generate test-plugin for handling user authentication")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto callback = [=](auto response) mutable
            {
                console->log(std:("Generation response:"), response);
                return array<any>();
            };
            auto result = std::async([=]() { generateCodeAction->handler(runtime, message, state, object{}, callback); });
            expect(result)->toBeDefined();
        }
        );
    }
    );
    describe(std:("Projects Provider"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        beforeEach([=]() mutable
        {
            runtime = std::async([=]() { createRealRuntime(); });
        }
        );
        it(std:("should get projects from planning service"), [=]() mutable
        {
            auto memory = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto result = std::async([=]() { projectsProvider->get(runtime, memory, state); });
            expect(result)->toBeDefined();
            expect(result->text)->toBeDefined();
        }
        );
        it(std:("should handle missing service gracefully"), [=]() mutable
        {
            auto bareRuntime = std::make_shared<AgentRuntime>(object{
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("character"), object{
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("bio"), array<string>{ std:("Test agent") }}
                }}
            });
            auto memory = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("agentId"), bareRuntime->agentId}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto result = std::async([=]() { projectsProvider->get(bareRuntime, memory, state); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std:("not available"));
        }
        );
    }
    );
}

MAIN
