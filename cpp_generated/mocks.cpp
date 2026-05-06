#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/__tests__/test-utils/mocks.h"

std::shared_ptr<IAgentRuntime> createMockAgentRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides)
{
    shared db = object{
        object::pair{std::string("execute"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}
    };
    auto baseRuntime = utils::assign(object{
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174000"))}, 
        object::pair{std::string("character"), as<std::shared_ptr<Character>>(object{
            object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("test-character-id"))}, 
            object::pair{std::string("name"), std::string("Test Character")}, 
            object::pair{std::string("description"), std::string("A test character")}, 
            object::pair{std::string("bio"), array<string>{ std::string("Test bio") }}, 
            object::pair{std::string("system"), std::string("Test system")}, 
            object::pair{std::string("modelProvider"), std::string("openai")}, 
            object::pair{std::string("settings"), object{
                object::pair{std::string("model"), std::string("gpt-4")}, 
                object::pair{std::string("secrets"), object{}}
            }}
        })}, 
        object::pair{std::string("providers"), array<any>()}, 
        object::pair{std::string("actions"), array<any>()}, 
        object::pair{std::string("evaluators"), array<any>()}, 
        object::pair{std::string("plugins"), array<any>()}, 
        object::pair{std::string("services"), std::make_shared<Map>()}, 
        object::pair{std::string("events"), std::make_shared<Map>()}, 
        object::pair{std::string("fetch"), nullptr}, 
        object::pair{std::string("routes"), array<any>()}, 
        object::pair{std::string("registerPlugin"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("initialize"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getConnection"), jest->fn([=]() mutable
        {
            return Promise->resolve(db);
        }
        )}, 
        object::pair{std::string("getService"), jest->fn([=]() mutable
        {
            return nullptr;
        }
        )}, 
        object::pair{std::string("getAllServices"), jest->fn([=]() mutable
        {
            return std::make_shared<Map>();
        }
        )}, 
        object::pair{std::string("registerService"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("registerDatabaseAdapter"), jest->fn()}, 
        object::pair{std::string("setSetting"), jest->fn()}, 
        object::pair{std::string("getSetting"), jest->fn([=](auto key) mutable
        {
            return const_(overrides->character->settings)[key];
        }
        )}, 
        object::pair{std::string("getConversationLength"), jest->fn([=]() mutable
        {
            return 10;
        }
        )}, 
        object::pair{std::string("processActions"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("evaluate"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<array<std::shared_ptr<Evaluator>>>(array<std::shared_ptr<Evaluator>>()));
        }
        )}, 
        object::pair{std::string("registerProvider"), jest->fn()}, 
        object::pair{std::string("registerAction"), jest->fn()}, 
        object::pair{std::string("registerEvaluator"), jest->fn()}, 
        object::pair{std::string("ensureConnections"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("ensureConnection"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("ensureParticipantInRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("ensureWorldExists"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("ensureRoomExists"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("composeState"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<State>>(object{}));
        }
        )}, 
        object::pair{std::string("useModel"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<any>(std::string("mock response")));
        }
        )}, 
        object::pair{std::string("registerModel"), jest->fn()}, 
        object::pair{std::string("getModel"), jest->fn([=]() mutable
        {
            return undefined;
        }
        )}, 
        object::pair{std::string("registerEvent"), jest->fn()}, 
        object::pair{std::string("getEvent"), jest->fn([=]() mutable
        {
            return undefined;
        }
        )}, 
        object::pair{std::string("emitEvent"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("registerTaskWorker"), jest->fn()}, 
        object::pair{std::string("getTaskWorker"), jest->fn([=]() mutable
        {
            return undefined;
        }
        )}, 
        object::pair{std::string("stop"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("addEmbeddingToMemory"), jest->fn([=](auto memory) mutable
        {
            return Promise->resolve(memory);
        }
        )}, 
        object::pair{std::string("createRunId"), jest->fn([=]() mutable
        {
            return as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174000"));
        }
        )}, 
        object::pair{std::string("startRun"), jest->fn([=]() mutable
        {
            return as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174000"));
        }
        )}, 
        object::pair{std::string("endRun"), jest->fn()}, 
        object::pair{std::string("getCurrentRunId"), jest->fn([=]() mutable
        {
            return as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174000"));
        }
        )}, 
        object::pair{std::string("getEntityById"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("createEntity"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("createRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174000")));
        }
        )}, 
        object::pair{std::string("addParticipant"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("getRooms"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("registerSendHandler"), jest->fn()}, 
        object::pair{std::string("sendMessageToTarget"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("db"), std::string("db")}, 
        object::pair{std::string("isReady"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("init"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("runMigrations"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("close"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getAgents"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("createAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("updateAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("deleteAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("ensureEmbeddingDimension"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getEntityByIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getEntitiesForRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("createEntities"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("updateEntity"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getComponents"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("createComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("updateComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("deleteComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getAllMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("clearAllAgentMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getMemoryById"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getMemoriesByIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getMemoriesByRoomIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getCachedEmbeddings"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("log"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getLogs"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("deleteLog"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("searchMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("createMemory"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174000")));
        }
        )}, 
        object::pair{std::string("updateMemory"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("deleteMemory"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("deleteManyMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("deleteAllMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("countMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve(0);
        }
        )}, 
        object::pair{std::string("createWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174000")));
        }
        )}, 
        object::pair{std::string("getWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("removeWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getAllWorlds"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("updateWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getRoomsByIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("createRooms"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("deleteRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("deleteRoomsByWorldId"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("updateRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getRoomsForParticipant"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getRoomsForParticipants"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getRoomsByWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("removeParticipant"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("getParticipantsForEntity"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getParticipantsForRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("addParticipantsRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("getParticipantUserState"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("setParticipantUserState"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("createRelationship"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("updateRelationship"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getRelationship"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getRelationships"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getCache"), jest->fn([=]() mutable
        {
            return Promise->resolve(undefined);
        }
        )}, 
        object::pair{std::string("setCache"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("deleteCache"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("createTask"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174000")));
        }
        )}, 
        object::pair{std::string("getTasks"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getTask"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getTasksByName"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("updateTask"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("deleteTask"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getMemoriesByWorldId"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}
    }, overrides);
    return baseRuntime;
};


any createMockDatabaseAdapter(any overrides)
{
    auto baseAdapter = utils::assign(object{
        object::pair{std::string("db"), object{
            object::pair{std::string("execute"), jest->fn([=]() mutable
            {
                return Promise->resolve(array<any>());
            }
            )}
        }}, 
        object::pair{std::string("init"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("initialize"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("isReady"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("runMigrations"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("close"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getConnection"), jest->fn([=]() mutable
        {
            return Promise->resolve(object{
                object::pair{std::string("execute"), jest->fn([=]() mutable
                {
                    return Promise->resolve(array<any>());
                }
                )}
            });
        }
        )}, 
        object::pair{std::string("getAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getAgents"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("createAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("updateAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("deleteAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("getEntityByIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getEntitiesForRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("createEntities"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("updateEntity"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getComponents"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("createComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("updateComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("deleteComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getMemoryById"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getMemoriesByIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getMemoriesByRoomIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getCachedEmbeddings"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("searchMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("createMemory"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174000")));
        }
        )}, 
        object::pair{std::string("updateMemory"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("deleteMemory"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("deleteManyMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("deleteAllMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("countMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve(0);
        }
        )}, 
        object::pair{std::string("getMemoriesByWorldId"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("ensureEmbeddingDimension"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("log"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getLogs"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("deleteLog"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("createWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174000")));
        }
        )}, 
        object::pair{std::string("getWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("removeWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getAllWorlds"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("updateWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getRoomsByIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("createRooms"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("deleteRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("deleteRoomsByWorldId"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("updateRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getRoomsForParticipant"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getRoomsForParticipants"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getRoomsByWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("removeParticipant"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("getParticipantsForEntity"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getParticipantsForRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("addParticipantsRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("getParticipantUserState"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("setParticipantUserState"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("createRelationship"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("updateRelationship"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getRelationship"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getRelationships"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getCache"), jest->fn([=]() mutable
        {
            return Promise->resolve(undefined);
        }
        )}, 
        object::pair{std::string("setCache"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("deleteCache"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std::string("createTask"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std::string("123e4567-e89b-12d3-a456-426614174000")));
        }
        )}, 
        object::pair{std::string("getTasks"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getTask"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getTasksByName"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("updateTask"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("deleteTask"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("createMessageServer"), jest->fn([=]() mutable
        {
            return Promise->resolve(object{
                object::pair{std::string("id"), std::string("00000000-0000-0000-0000-000000000000")}
            });
        }
        )}, 
        object::pair{std::string("getMessageServers"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<object>{ object{
                object::pair{std::string("id"), std::string("00000000-0000-0000-0000-000000000000")}, 
                object::pair{std::string("name"), std::string("Default Server")}
            } });
        }
        )}, 
        object::pair{std::string("getMessageServerById"), jest->fn([=]() mutable
        {
            return Promise->resolve(object{
                object::pair{std::string("id"), std::string("00000000-0000-0000-0000-000000000000")}, 
                object::pair{std::string("name"), std::string("Default Server")}
            });
        }
        )}, 
        object::pair{std::string("addAgentToServer"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("removeAgentFromServer"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("getAgentsForServer"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("createChannel"), jest->fn([=]() mutable
        {
            return Promise->resolve(object{
                object::pair{std::string("id"), std::string("123e4567-e89b-12d3-a456-426614174000")}
            });
        }
        )}, 
        object::pair{std::string("getChannelsForServer"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("getChannelDetails"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("getChannelParticipants"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("addChannelParticipants"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("updateChannel"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("deleteChannel"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("createMessage"), jest->fn([=]() mutable
        {
            return Promise->resolve(object{
                object::pair{std::string("id"), std::string("message-id")}
            });
        }
        )}, 
        object::pair{std::string("getMessagesForChannel"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std::string("deleteMessage"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("findOrCreateDmChannel"), jest->fn([=]() mutable
        {
            return Promise->resolve(object{
                object::pair{std::string("id"), std::string("dm-channel-id")}
            });
        }
        )}
    }, overrides);
    return as<any>(baseAdapter);
};


std::shared_ptr<Request> createMockRequest(Partial<std::shared_ptr<Request>> overrides)
{
    return as<any>(utils::assign(object{
        object::pair{std::string("params"), object{}}, 
        object::pair{std::string("query"), object{}}, 
        object::pair{std::string("body"), object{}}, 
        object::pair{std::string("headers"), object{}}, 
        object::pair{std::string("method"), std::string("GET")}, 
        object::pair{std::string("originalUrl"), std::string("/test")}, 
        object::pair{std::string("url"), std::string("/test")}, 
        object::pair{std::string("path"), std::string("/test")}, 
        object::pair{std::string("ip"), std::string("127.0.0.1")}, 
        object::pair{std::string("get"), jest->fn([=](auto _header) mutable
        {
            return string_empty;
        }
        )}, 
        object::pair{std::string("header"), jest->fn([=](auto _header) mutable
        {
            return string_empty;
        }
        )}, 
        object::pair{std::string("accepts"), jest->fn()}, 
        object::pair{std::string("acceptsCharsets"), jest->fn()}, 
        object::pair{std::string("acceptsEncodings"), jest->fn()}, 
        object::pair{std::string("acceptsLanguages"), jest->fn()}, 
        object::pair{std::string("is"), jest->fn()}
    }, overrides));
};


std::shared_ptr<Response> createMockResponse()
{
    auto res = object{
        object::pair{std::string("status"), jest->fn()->mockReturnThis()}, 
        object::pair{std::string("json"), jest->fn()->mockReturnThis()}, 
        object::pair{std::string("send"), jest->fn()->mockReturnThis()}, 
        object::pair{std::string("end"), jest->fn()->mockReturnThis()}, 
        object::pair{std::string("setHeader"), jest->fn()->mockReturnThis()}, 
        object::pair{std::string("removeHeader"), jest->fn()->mockReturnThis()}, 
        object::pair{std::string("set"), jest->fn()->mockReturnThis()}, 
        object::pair{std::string("header"), jest->fn()->mockReturnThis()}, 
        object::pair{std::string("type"), jest->fn()->mockReturnThis()}, 
        object::pair{std::string("sendStatus"), jest->fn()->mockReturnThis()}, 
        object::pair{std::string("redirect"), jest->fn()->mockReturnThis()}, 
        object::pair{std::string("cookie"), jest->fn()->mockReturnThis()}, 
        object::pair{std::string("clearCookie"), jest->fn()->mockReturnThis()}, 
        object::pair{std::string("attachment"), jest->fn()->mockReturnThis()}, 
        object::pair{std::string("sendFile"), jest->fn([=](auto _path, auto options = undefined, auto callback = undefined) mutable
        {
            if (type_of(options) == std::string("function")) {
                callback = options;
            }
            if (callback) callback();
        }
        )}, 
        object::pair{std::string("headersSent"), false}, 
        object::pair{std::string("locals"), object{}}
    };
    return as<any>(res);
};


std::shared_ptr<NextFunction> createMockNext()
{
    return as<any>(jest->fn());
};


any createMockSocketIO()
{
    return object{
        object::pair{std::string("on"), jest->fn()}, 
        object::pair{std::string("emit"), jest->fn()}, 
        object::pair{std::string("to"), jest->fn([=]() mutable
        {
            return (object{
                object::pair{std::string("emit"), jest->fn()}
            });
        }
        )}, 
        object::pair{std::string("sockets"), object{
            object::pair{std::string("sockets"), std::make_shared<Map>()}
        }}, 
        object::pair{std::string("close"), jest->fn([=](auto callback = nullptr) mutable
        {
            if (callback) callback();
        }
        )}
    };
};


any createMockHttpServer()
{
    return object{
        object::pair{std::string("listen"), jest->fn([=](auto _port, auto callback = nullptr) mutable
        {
            if (callback) callback();
        }
        )}, 
        object::pair{std::string("close"), jest->fn([=](auto callback = nullptr) mutable
        {
            if (callback) callback();
        }
        )}, 
        object::pair{std::string("listeners"), jest->fn([=]() mutable
        {
            return array<any>();
        }
        )}, 
        object::pair{std::string("removeAllListeners"), jest->fn()}, 
        object::pair{std::string("on"), jest->fn()}, 
        object::pair{std::string("once"), jest->fn()}, 
        object::pair{std::string("emit"), jest->fn()}, 
        object::pair{std::string("address"), jest->fn([=]() mutable
        {
            return (object{
                object::pair{std::string("port"), 3000}
            });
        }
        )}, 
        object::pair{std::string("timeout"), 0}, 
        object::pair{std::string("keepAliveTimeout"), 5000}
    };
};


std::shared_ptr<Service> createMockService(Partial<std::shared_ptr<Service>> overrides)
{
    return as<any>(utils::assign(object{
        object::pair{std::string("name"), std::string("MockService")}, 
        object::pair{std::string("description"), std::string("A mock service for testing")}, 
        object::pair{std::string("serviceType"), ServiceType->WEB_SEARCH}, 
        object::pair{std::string("getInstance"), jest->fn()}, 
        object::pair{std::string("start"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("stop"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}
    }, overrides));
};


std::shared_ptr<Express::Multer::File> createMockUploadedFile(Partial<std::shared_ptr<Express::Multer::File>> overrides)
{
    return utils::assign(object{
        object::pair{std::string("fieldname"), std::string("file")}, 
        object::pair{std::string("originalname"), std::string("test.jpg")}, 
        object::pair{std::string("encoding"), std::string("7bit")}, 
        object::pair{std::string("mimetype"), std::string("image/jpeg")}, 
        object::pair{std::string("buffer"), Buffer::from(std::string("test"))}, 
        object::pair{std::string("size"), 12345}, 
        object::pair{std::string("stream"), as<any>(undefined)}, 
        object::pair{std::string("destination"), string_empty}, 
        object::pair{std::string("filename"), string_empty}, 
        object::pair{std::string("path"), string_empty}
    }, overrides);
};


