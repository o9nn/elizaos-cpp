#include "integration.test.h"
#include <string>

object mockSqlPlugin = object{
    object::pair{std::string("name"), std::string("@elizaos/plugin-sql")}, 
    object::pair{std::string("description"), std::string("Mock SQL plugin for testing")}, 
    object::pair{std::string("actions"), array<any>()}, 
    object::pair{std::string("providers"), array<any>()}, 
    object::pair{std::string("services"), array<any>()}, 
    object::pair{std::string("init"), [=]() mutable
    {
        elizaLogger->info(std::string("Mock SQL plugin initialized"));
    }
    }
};
std::function<std::shared_ptr<Promise<any>>()> createRealRuntime = [=]() mutable
{
    process->env->FORCE_BUNSQLITE = std::string("true");
    process->env->DATABASE_PATH = std::string(":memory:");
    process->env->ELIZA_TEST_MODE = std::string("true");
    process->env->SECRET_SALT = OR((process->env->SECRET_SALT), (std::string("test-salt-for-testing-only-not-secure")));
    auto runtime = std::make_shared<AgentRuntime>(object{
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
        object::pair{std::string("character"), object{
            object::pair{std::string("name"), std::string("Test Agent")}, 
            object::pair{std::string("bio"), array<string>{ std::string("Test agent for autocoder") }}, 
            object::pair{std::string("secrets"), object{}}, 
            object::pair{std::string("settings"), object{}}, 
            object::pair{std::string("plugins"), array<any>()}
        }}
    });
    (as<any>(runtime))["adapter"] = object{
        object::pair{std::string("query"), [=]() mutable
        {
            return array<any>();
        }
        }, 
        object::pair{std::string("execute"), [=]() mutable
        {
            return (object{
                object::pair{std::string("rows"), array<any>()}
            });
        }
        }, 
        object::pair{std::string("initialized"), true}, 
        object::pair{std::string("init"), [=]() mutable
        {
        }
        }, 
        object::pair{std::string("close"), [=]() mutable
        {
        }
        }, 
        object::pair{std::string("getAgents"), [=]() mutable
        {
            return array<any>();
        }
        }, 
        object::pair{std::string("createAgent"), [=](auto agent) mutable
        {
            return agent;
        }
        }, 
        object::pair{std::string("updateAgent"), [=](auto id, auto updates) mutable
        {
            return (utils::assign(object{
                object::pair{std::string("id"), std::string("id")}
            }, updates));
        }
        }, 
        object::pair{std::string("deleteAgent"), [=](auto id) mutable
        {
        }
        }, 
        object::pair{std::string("getAgent"), [=](auto id) mutable
        {
            return (object{
                object::pair{std::string("id"), std::string("id")}, 
                object::pair{std::string("name"), std::string("Test Agent")}
            });
        }
        }, 
        object::pair{std::string("getRooms"), [=]() mutable
        {
            return array<any>();
        }
        }, 
        object::pair{std::string("getRoomsByIds"), [=](auto ids) mutable
        {
            return ids->map([=](auto id) mutable
            {
                return (object{
                    object::pair{std::string("id"), std::string("id")}
                });
            }
            );
        }
        }, 
        object::pair{std::string("createRoom"), [=](auto room) mutable
        {
            return room;
        }
        }, 
        object::pair{std::string("updateRoom"), [=](auto id, auto updates) mutable
        {
            return (utils::assign(object{
                object::pair{std::string("id"), std::string("id")}
            }, updates));
        }
        }, 
        object::pair{std::string("deleteRoom"), [=](auto id) mutable
        {
        }
        }, 
        object::pair{std::string("getRoom"), [=](auto id) mutable
        {
            return (object{
                object::pair{std::string("id"), std::string("id")}
            });
        }
        }, 
        object::pair{std::string("getParticipants"), [=]() mutable
        {
            return array<any>();
        }
        }, 
        object::pair{std::string("getParticipantsByIds"), [=](auto ids) mutable
        {
            return ids->map([=](auto id) mutable
            {
                return (object{
                    object::pair{std::string("id"), std::string("id")}
                });
            }
            );
        }
        }, 
        object::pair{std::string("getParticipantsForRoom"), [=](auto roomId) mutable
        {
            return array<any>();
        }
        }, 
        object::pair{std::string("addParticipantsToRoom"), [=](auto participantIds, auto roomId) mutable
        {
            return true;
        }
        }, 
        object::pair{std::string("createParticipant"), [=](auto participant) mutable
        {
            return participant;
        }
        }, 
        object::pair{std::string("updateParticipant"), [=](auto id, auto updates) mutable
        {
            return (utils::assign(object{
                object::pair{std::string("id"), std::string("id")}
            }, updates));
        }
        }, 
        object::pair{std::string("deleteParticipant"), [=](auto id) mutable
        {
        }
        }, 
        object::pair{std::string("getParticipant"), [=](auto id) mutable
        {
            return (object{
                object::pair{std::string("id"), std::string("id")}
            });
        }
        }, 
        object::pair{std::string("getMemories"), [=]() mutable
        {
            return array<any>();
        }
        }, 
        object::pair{std::string("getMemoriesByIds"), [=](auto ids) mutable
        {
            return ids->map([=](auto id) mutable
            {
                return (object{
                    object::pair{std::string("id"), std::string("id")}
                });
            }
            );
        }
        }, 
        object::pair{std::string("createMemory"), [=](auto memory) mutable
        {
            return memory;
        }
        }, 
        object::pair{std::string("updateMemory"), [=](auto id, auto updates) mutable
        {
            return (utils::assign(object{
                object::pair{std::string("id"), std::string("id")}
            }, updates));
        }
        }, 
        object::pair{std::string("deleteMemory"), [=](auto id) mutable
        {
        }
        }, 
        object::pair{std::string("getMemory"), [=](auto id) mutable
        {
            return (object{
                object::pair{std::string("id"), std::string("id")}
            });
        }
        }, 
        object::pair{std::string("getEntitiesByIds"), [=](auto ids) mutable
        {
            return ids->map([=](auto id) mutable
            {
                return (object{
                    object::pair{std::string("id"), std::string("id")}
                });
            }
            );
        }
        }, 
        object::pair{std::string("createEntity"), [=](auto entity) mutable
        {
            return entity;
        }
        }, 
        object::pair{std::string("updateEntity"), [=](auto id, auto updates) mutable
        {
            return (utils::assign(object{
                object::pair{std::string("id"), std::string("id")}
            }, updates));
        }
        }, 
        object::pair{std::string("deleteEntity"), [=](auto id) mutable
        {
        }
        }, 
        object::pair{std::string("getEntity"), [=](auto id) mutable
        {
            return (object{
                object::pair{std::string("id"), std::string("id")}
            });
        }
        }, 
        object::pair{std::string("updateWorld"), [=](auto world) mutable
        {
            return world;
        }
        }, 
        object::pair{std::string("addParticipantsRoom"), [=](auto entityIds, auto roomId) mutable
        {
            return true;
        }
        }, 
        object::pair{std::string("removeParticipantsRoom"), [=](auto entityIds, auto roomId) mutable
        {
            return true;
        }
        }
    };
    std::async([=]() { runtime->registerPlugin(formsPlugin); });
    std::async([=]() { runtime->registerPlugin(autocoderPlugin); });
    std::async([=]() { runtime->initialize(); });
    elizaLogger->info(std::string("Runtime initialized"), object{
        object::pair{std::string("hasForms"), runtime->getService(std::string("forms")) != std::nullopt}, 
        object::pair{std::string("hasCodeGen"), runtime->getService(std::string("code-generation")) != std::nullopt}
    });
    return runtime;
};

void Main(void)
{
    describe(std::string("Generate Code Action"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        beforeEach([=]() mutable
        {
            runtime = std::async([=]() { createRealRuntime(); });
        }
        );
        it(std::string("should validate messages correctly"), [=]() mutable
        {
            auto message = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("generate test-plugin")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { generateCodeAction->validate(runtime, message); });
            expect(isValid)->toBe(true);
        }
        );
        it(std::string("should handle generation"), [=]() mutable
        {
            auto message = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("generate test-plugin for handling user authentication")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto callback = [=](auto response) mutable
            {
                console->log(std::string("Generation response:"), response);
                return array<any>();
            };
            auto result = std::async([=]() { generateCodeAction->handler(runtime, message, state, object{}, callback); });
            expect(result)->toBeDefined();
        }
        );
    }
    );
    describe(std::string("Projects Provider"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        beforeEach([=]() mutable
        {
            runtime = std::async([=]() { createRealRuntime(); });
        }
        );
        it(std::string("should get projects from planning service"), [=]() mutable
        {
            auto memory = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto result = std::async([=]() { projectsProvider->get(runtime, memory, state); });
            expect(result)->toBeDefined();
            expect(result->text)->toBeDefined();
        }
        );
        it(std::string("should handle missing service gracefully"), [=]() mutable
        {
            auto bareRuntime = std::make_shared<AgentRuntime>(object{
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("character"), object{
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("bio"), array<string>{ std::string("Test agent") }}
                }}
            });
            auto memory = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("agentId"), bareRuntime->agentId}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto result = std::async([=]() { projectsProvider->get(bareRuntime, memory, state); });
            expect(result)->toBeDefined();
            expect(result->text)->toContain(std::string("not available"));
        }
        );
    }
    );
}

MAIN
