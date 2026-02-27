#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/src/tests/test-utils.h"

std::shared_ptr<IAgentRuntime> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides)
{
    auto defaultRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent"))}, 
        object::pair{std:("db"), nullptr}, 
        object::pair{std:("character"), object{
            object::pair{std:("name"), std:("TestAgent")}, 
            object::pair{std:("description"), std:("A test agent")}, 
            object::pair{std:("instructions"), std:("Test instructions")}, 
            object::pair{std:("personality"), std:("Test personality")}, 
            object::pair{std:("modelProvider"), std:("openai")}, 
            object::pair{std:("settings"), object{}}
        }}, 
        object::pair{std:("actions"), as<array<std::shared_ptr<Action>>>(array<std::shared_ptr<Action>>())}, 
        object::pair{std:("providers"), as<array<std::shared_ptr<Provider>>>(array<std::shared_ptr<Provider>>())}, 
        object::pair{std:("services"), std::make_shared<Map<string, std::shared_ptr<Service>>>()}, 
        object::pair{std:("plugins"), as<array<std::shared_ptr<Plugin>>>(array<std::shared_ptr<Plugin>>())}, 
        object::pair{std:("evaluators"), array<any>()}, 
        object::pair{std:("events"), std::make_shared<Map>()}, 
        object::pair{std:("routes"), array<any>()}, 
        object::pair{std:("logger"), console}, 
        object::pair{std:("fetch"), nullptr}, 
        object::pair{std:("initialize"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("stop"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getService"), mock([=]() mutable
        {
            return nullptr;
        }
        )}, 
        object::pair{std:("getServicesByType"), mock([=]() mutable
        {
            return array<any>();
        }
        )}, 
        object::pair{std:("getAllServices"), mock([=]() mutable
        {
            return std::make_shared<Map>();
        }
        )}, 
        object::pair{std:("registerService"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getRegisteredServiceTypes"), mock([=]() mutable
        {
            return array<any>();
        }
        )}, 
        object::pair{std:("hasService"), mock([=]() mutable
        {
            return false;
        }
        )}, 
        object::pair{std:("getSetting"), mock([=](auto key) mutable
        {
            return nullptr;
        }
        )}, 
        object::pair{std:("setSetting"), mock()}, 
        object::pair{std:("useModel"), mock([=]() mutable
        {
            return Promise->resolve(std:("Mock response"));
        }
        )}, 
        object::pair{std:("composeState"), mock([=]() mutable
        {
            return Promise->resolve(object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("text"), string_empty}, 
                object::pair{std:("data"), object{}}
            });
        }
        )}, 
        object::pair{std:("processActions"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("evaluate"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("ensureParticipantInRoom"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getRoom"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("ensureConnection"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("ensureConnections"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("ensureWorldExists"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("ensureRoomExists"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("registerDatabaseAdapter"), mock()}, 
        object::pair{std:("emitEvent"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("registerPlugin"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("registerProvider"), mock()}, 
        object::pair{std:("registerAction"), mock()}, 
        object::pair{std:("registerEvaluator"), mock()}, 
        object::pair{std:("registerModel"), mock()}, 
        object::pair{std:("getModel"), mock([=]() mutable
        {
            return undefined;
        }
        )}, 
        object::pair{std:("registerEvent"), mock()}, 
        object::pair{std:("getEvent"), mock([=]() mutable
        {
            return undefined;
        }
        )}, 
        object::pair{std:("registerTaskWorker"), mock()}, 
        object::pair{std:("getTaskWorker"), mock([=]() mutable
        {
            return undefined;
        }
        )}, 
        object::pair{std:("addEmbeddingToMemory"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<Memory>>(object{}));
        }
        )}, 
        object::pair{std:("getAllMemories"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("clearAllAgentMemories"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("createRunId"), mock([=]() mutable
        {
            return as<std::shared_ptr<UUID>>(std:("test-run-id"));
        }
        )}, 
        object::pair{std:("startRun"), mock([=]() mutable
        {
            return as<std::shared_ptr<UUID>>(std:("test-run-id"));
        }
        )}, 
        object::pair{std:("endRun"), mock()}, 
        object::pair{std:("getCurrentRunId"), mock([=]() mutable
        {
            return as<std::shared_ptr<UUID>>(std:("test-run-id"));
        }
        )}, 
        object::pair{std:("getEntityById"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("createEntity"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("createRoom"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std:("test-room-id")));
        }
        )}, 
        object::pair{std:("addParticipant"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("getRooms"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("registerSendHandler"), mock()}, 
        object::pair{std:("sendMessageToTarget"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getConversationLength"), mock([=]() mutable
        {
            return 0;
        }
        )}, 
        object::pair{std:("init"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("runMigrations"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("isReady"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("close"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getConnection"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getAgent"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getAgents"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("createAgent"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("updateAgent"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("deleteAgent"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("ensureEmbeddingDimension"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getEntitiesByIds"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getEntitiesForRoom"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("createEntities"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("updateEntity"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getComponent"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getComponents"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("createComponent"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("updateComponent"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("deleteComponent"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("createMemory"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std:("test-memory-id")));
        }
        )}, 
        object::pair{std:("searchMemories"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("searchMemoriesByEmbedding"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getMemoriesWithActions"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getMemoryActions"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getMemory"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getMemories"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getCachedEmbeddings"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getWorldsByParticipant"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getRelationship"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("createRelationship"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("updateRelationship"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("getRelationships"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getCache"), mock([=]() mutable
        {
            return Promise->resolve(undefined);
        }
        )}, 
        object::pair{std:("setCache"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("deleteCache"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("createTask"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std:("test-task-id")));
        }
        )}, 
        object::pair{std:("getTasks"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getTask"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getTasksByName"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("updateTask"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("deleteTask"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getMemoriesByWorldId"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}
    }));
    return utils::assign(object{
        , 
        object::pair{std:("character"), utils::assign(object{
        }, defaultRuntime->character, (OR((overrides->character), (object{}))))}
    }, defaultRuntime, overrides);
};


