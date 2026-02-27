#include "test-utils.h"
#include <string>

std::shared_ptr<IAgentRuntime> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides)
{
    auto defaultRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent"))}, 
        object::pair{std::string("db"), nullptr}, 
        object::pair{std::string("character"), object{
            object::pair{std::string("name"), std::string("TestAgent")}, 
            object::pair{std::string("description"), std::string("A test agent")}, 
            object::pair{std::string("instructions"), std::string("Test instructions")}, 
            object::pair{std::string("personality"), std::string("Test personality")}, 
            object::pair{std::string("modelProvider"), std::string("openai")}, 
            object::pair{std::string("settings"), object{}}
        }}, 
        object::pair{std::string("actions"), as<array<std::shared_ptr<Action>>>(array<std::shared_ptr<Action>>())}, 
        object::pair{std::string("providers"), as<array<std::shared_ptr<Provider>>>(array<std::shared_ptr<Provider>>())}, 
        object::pair{std::string("services"), std::make_shared<Map<string, std::shared_ptr<Service>>>()}, 
        object::pair{std::string("plugins"), as<array<std::shared_ptr<Plugin>>>(array<std::shared_ptr<Plugin>>())}, 
        object::pair{std::string("evaluators"), array<any>()}, 
        object::pair{std::string("events"), std::make_shared<Map>()}, 
        object::pair{std::string("routes"), array<any>()}, 
        object::pair{std::string("logger"), console}, 
        object::pair{std::string("fetch"), nullptr}, 
        object::pair{std::string("initialize"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("stop"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getService"), mock([=]() mutable
        {
            return nullptr;
        }
        )}, 
        object::pair{std::string("getServicesByType"), mock([=]() mutable
        {
            return array<any>();
        }
        )}, 
        object::pair{std::string("getAllServices"), mock([=]() mutable
        {
            return std::make_shared<Map>();
        }
        )}, 
        object::pair{std::string("registerService"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getRegisteredServiceTypes"), mock([=]() mutable
        {
            return array<any>();
        }
        )}, 
        object::pair{std::string("hasService"), mock([=]() mutable
        {
            return false;
        }
        )}, 
        object::pair{std::string("getSetting"), mock([=](auto key) mutable
        {
            return nullptr;
        }
        )}, 
        object::pair{std::string("setSetting"), mock()}, 
        object::pair{std::string("useModel"), mock([=]() mutable
        {
            return Promise->resolve(std::string("Mock response"));
        }
        )}, 
        object::pair{std::string("composeState"), mock([=]() mutable
        {
            return Promise->resolve(object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("text"), string_empty}, 
                object::pair{std::string("data"), object{}}
            });
        }
        )}, 
        object::pair{std::string("processActions"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("evaluate"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("ensureParticipantInRoom"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getRoom"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("ensureConnection"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("ensureConnections"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("ensureWorldExists"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("ensureRoomExists"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("registerDatabaseAdapter"), mock()}, 
        object::pair{std::string("emitEvent"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("registerPlugin"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("registerProvider"), mock()}, 
        object::pair{std::string("registerAction"), mock()}, 
        object::pair{std::string("registerEvaluator"), mock()}, 
        object::pair{std::string("registerModel"), mock()}, 
        object::pair{std::string("getModel"), mock([=]() mutable
        {
            return std::nullopt;
        }
        )}, 
        object::pair{std::string("registerEvent"), mock()}, 
        object::pair{std::string("getEvent"), mock([=]() mutable
        {
            return std::nullopt;
        }
        )}, 
        object::pair{std::string("registerTaskWorker"), mock()}, 
        object::pair{std::string("getTaskWorker"), mock([=]() mutable
        {
            return std::nullopt;
        }
        )}, 
        object::pair{std::string("addEmbeddingToMemory"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<Memory>>(object{}));
        }
        )}, 
        object::pair{std::string("getAllMemories"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("clearAllAgentMemories"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("createRunId"), mock([=]() mutable
        {
            return as<std::shared_ptr<UUID>>(std::string("test-run-id"));
        }
        )}, 
        object::pair{std::string("startRun"), mock([=]() mutable
        {
            return as<std::shared_ptr<UUID>>(std::string("test-run-id"));
        }
        )}, 
        object::pair{std::string("endRun"), mock()}, 
        object::pair{std::string("getCurrentRunId"), mock([=]() mutable
        {
            return as<std::shared_ptr<UUID>>(std::string("test-run-id"));
        }
        )}, 
        object::pair{std::string("getEntityById"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("createEntity"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("createRoom"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std::string("test-room-id")));
        }
        )}, 
        object::pair{std::string("addParticipant"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("getRooms"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("registerSendHandler"), mock()}, 
        object::pair{std::string("sendMessageToTarget"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getConversationLength"), mock([=]() mutable
        {
            return 0;
        }
        )}, 
        object::pair{std::string("init"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("runMigrations"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("isReady"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("close"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getConnection"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getAgent"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getAgents"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("createAgent"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("updateAgent"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("deleteAgent"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("ensureEmbeddingDimension"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getEntitiesByIds"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getEntitiesForRoom"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("createEntities"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("updateEntity"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getComponent"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getComponents"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("createComponent"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("updateComponent"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("deleteComponent"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("createMemory"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std::string("test-memory-id")));
        }
        )}, 
        object::pair{std::string("searchMemories"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("searchMemoriesByEmbedding"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getMemoriesWithActions"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getMemoryActions"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getMemory"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getMemories"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getCachedEmbeddings"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getWorldsByParticipant"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getRelationship"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("createRelationship"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("updateRelationship"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("getRelationships"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getCache"), mock([=]() mutable
        {
            return Promise->resolve(std::nullopt);
        }
        )}, 
        object::pair{std::string("setCache"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("deleteCache"), mock([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("createTask"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std::string("test-task-id")));
        }
        )}, 
        object::pair{std::string("getTasks"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getTask"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getTasksByName"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("updateTask"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("deleteTask"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getMemoriesByWorldId"), mock([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}
    }));
    return utils::assign(object{
        , 
        object::pair{std::string("character"), utils::assign(object{
        }, defaultRuntime->character, (OR((overrides->character), (object{}))))}
    }, defaultRuntime, overrides);
};


