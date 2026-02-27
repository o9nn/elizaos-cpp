#include "mocks.hpp"

std::shared_ptr<IAgentRuntime> createMockAgentRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides)
{
    shared db = object{
        object::pair{std:("execute"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}
    };
    auto baseRuntime = utils::assign(object{
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174000"))}, 
        object::pair{std:("character"), as<std::shared_ptr<Character>>(object{
            object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("test-character-id"))}, 
            object::pair{std:("name"), std:("Test Character")}, 
            object::pair{std:("description"), std:("A test character")}, 
            object::pair{std:("bio"), array<string>{ std:("Test bio") }}, 
            object::pair{std:("system"), std:("Test system")}, 
            object::pair{std:("modelProvider"), std:("openai")}, 
            object::pair{std:("settings"), object{
                object::pair{std:("model"), std:("gpt-4")}, 
                object::pair{std:("secrets"), object{}}
            }}
        })}, 
        object::pair{std:("providers"), array<any>()}, 
        object::pair{std:("actions"), array<any>()}, 
        object::pair{std:("evaluators"), array<any>()}, 
        object::pair{std:("plugins"), array<any>()}, 
        object::pair{std:("services"), std::make_shared<Map>()}, 
        object::pair{std:("events"), std::make_shared<Map>()}, 
        object::pair{std:("fetch"), nullptr}, 
        object::pair{std:("routes"), array<any>()}, 
        object::pair{std:("registerPlugin"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("initialize"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getConnection"), jest->fn([=]() mutable
        {
            return Promise->resolve(db);
        }
        )}, 
        object::pair{std:("getService"), jest->fn([=]() mutable
        {
            return nullptr;
        }
        )}, 
        object::pair{std:("getAllServices"), jest->fn([=]() mutable
        {
            return std::make_shared<Map>();
        }
        )}, 
        object::pair{std:("registerService"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("registerDatabaseAdapter"), jest->fn()}, 
        object::pair{std:("setSetting"), jest->fn()}, 
        object::pair{std:("getSetting"), jest->fn([=](auto key) mutable
        {
            return const_(overrides->character->settings)[key];
        }
        )}, 
        object::pair{std:("getConversationLength"), jest->fn([=]() mutable
        {
            return 10;
        }
        )}, 
        object::pair{std:("processActions"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("evaluate"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<array<std::shared_ptr<Evaluator>>>(array<std::shared_ptr<Evaluator>>()));
        }
        )}, 
        object::pair{std:("registerProvider"), jest->fn()}, 
        object::pair{std:("registerAction"), jest->fn()}, 
        object::pair{std:("registerEvaluator"), jest->fn()}, 
        object::pair{std:("ensureConnections"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("ensureConnection"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("ensureParticipantInRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("ensureWorldExists"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("ensureRoomExists"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("composeState"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<State>>(object{}));
        }
        )}, 
        object::pair{std:("useModel"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<any>(std:("mock response")));
        }
        )}, 
        object::pair{std:("registerModel"), jest->fn()}, 
        object::pair{std:("getModel"), jest->fn([=]() mutable
        {
            return undefined;
        }
        )}, 
        object::pair{std:("registerEvent"), jest->fn()}, 
        object::pair{std:("getEvent"), jest->fn([=]() mutable
        {
            return undefined;
        }
        )}, 
        object::pair{std:("emitEvent"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("registerTaskWorker"), jest->fn()}, 
        object::pair{std:("getTaskWorker"), jest->fn([=]() mutable
        {
            return undefined;
        }
        )}, 
        object::pair{std:("stop"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("addEmbeddingToMemory"), jest->fn([=](auto memory) mutable
        {
            return Promise->resolve(memory);
        }
        )}, 
        object::pair{std:("createRunId"), jest->fn([=]() mutable
        {
            return as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174000"));
        }
        )}, 
        object::pair{std:("startRun"), jest->fn([=]() mutable
        {
            return as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174000"));
        }
        )}, 
        object::pair{std:("endRun"), jest->fn()}, 
        object::pair{std:("getCurrentRunId"), jest->fn([=]() mutable
        {
            return as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174000"));
        }
        )}, 
        object::pair{std:("getEntityById"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("createEntity"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("createRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174000")));
        }
        )}, 
        object::pair{std:("addParticipant"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("getRooms"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("registerSendHandler"), jest->fn()}, 
        object::pair{std:("sendMessageToTarget"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("db"), std:("db")}, 
        object::pair{std:("isReady"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("init"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("runMigrations"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("close"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getAgents"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("createAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("updateAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("deleteAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("ensureEmbeddingDimension"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getEntityByIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getEntitiesForRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("createEntities"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("updateEntity"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getComponents"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("createComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("updateComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("deleteComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getAllMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("clearAllAgentMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getMemoryById"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getMemoriesByIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getMemoriesByRoomIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getCachedEmbeddings"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("log"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getLogs"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("deleteLog"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("searchMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("createMemory"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174000")));
        }
        )}, 
        object::pair{std:("updateMemory"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("deleteMemory"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("deleteManyMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("deleteAllMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("countMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve(0);
        }
        )}, 
        object::pair{std:("createWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174000")));
        }
        )}, 
        object::pair{std:("getWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("removeWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getAllWorlds"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("updateWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getRoomsByIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("createRooms"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("deleteRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("deleteRoomsByWorldId"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("updateRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getRoomsForParticipant"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getRoomsForParticipants"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getRoomsByWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("removeParticipant"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("getParticipantsForEntity"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getParticipantsForRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("addParticipantsRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("getParticipantUserState"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("setParticipantUserState"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("createRelationship"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("updateRelationship"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getRelationship"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getRelationships"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getCache"), jest->fn([=]() mutable
        {
            return Promise->resolve(undefined);
        }
        )}, 
        object::pair{std:("setCache"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("deleteCache"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("createTask"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174000")));
        }
        )}, 
        object::pair{std:("getTasks"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getTask"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getTasksByName"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("updateTask"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("deleteTask"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getMemoriesByWorldId"), jest->fn([=]() mutable
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
        object::pair{std:("db"), object{
            object::pair{std:("execute"), jest->fn([=]() mutable
            {
                return Promise->resolve(array<any>());
            }
            )}
        }}, 
        object::pair{std:("init"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("initialize"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("isReady"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("runMigrations"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("close"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getConnection"), jest->fn([=]() mutable
        {
            return Promise->resolve(object{
                object::pair{std:("execute"), jest->fn([=]() mutable
                {
                    return Promise->resolve(array<any>());
                }
                )}
            });
        }
        )}, 
        object::pair{std:("getAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getAgents"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("createAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("updateAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("deleteAgent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("getEntityByIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getEntitiesForRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("createEntities"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("updateEntity"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getComponents"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("createComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("updateComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("deleteComponent"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getMemoryById"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getMemoriesByIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getMemoriesByRoomIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getCachedEmbeddings"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("searchMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("createMemory"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174000")));
        }
        )}, 
        object::pair{std:("updateMemory"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("deleteMemory"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("deleteManyMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("deleteAllMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("countMemories"), jest->fn([=]() mutable
        {
            return Promise->resolve(0);
        }
        )}, 
        object::pair{std:("getMemoriesByWorldId"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("ensureEmbeddingDimension"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("log"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getLogs"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("deleteLog"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("createWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174000")));
        }
        )}, 
        object::pair{std:("getWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("removeWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getAllWorlds"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("updateWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getRoomsByIds"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("createRooms"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("deleteRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("deleteRoomsByWorldId"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("updateRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getRoomsForParticipant"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getRoomsForParticipants"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getRoomsByWorld"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("removeParticipant"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("getParticipantsForEntity"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getParticipantsForRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("addParticipantsRoom"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("getParticipantUserState"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("setParticipantUserState"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("createRelationship"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("updateRelationship"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getRelationship"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getRelationships"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getCache"), jest->fn([=]() mutable
        {
            return Promise->resolve(undefined);
        }
        )}, 
        object::pair{std:("setCache"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("deleteCache"), jest->fn([=]() mutable
        {
            return Promise->resolve(true);
        }
        )}, 
        object::pair{std:("createTask"), jest->fn([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std:("123e4567-e89b-12d3-a456-426614174000")));
        }
        )}, 
        object::pair{std:("getTasks"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getTask"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getTasksByName"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("updateTask"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("deleteTask"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("createMessageServer"), jest->fn([=]() mutable
        {
            return Promise->resolve(object{
                object::pair{std:("id"), std:("00000000-0000-0000-0000-000000000000")}
            });
        }
        )}, 
        object::pair{std:("getMessageServers"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<object>{ object{
                object::pair{std:("id"), std:("00000000-0000-0000-0000-000000000000")}, 
                object::pair{std:("name"), std:("Default Server")}
            } });
        }
        )}, 
        object::pair{std:("getMessageServerById"), jest->fn([=]() mutable
        {
            return Promise->resolve(object{
                object::pair{std:("id"), std:("00000000-0000-0000-0000-000000000000")}, 
                object::pair{std:("name"), std:("Default Server")}
            });
        }
        )}, 
        object::pair{std:("addAgentToServer"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("removeAgentFromServer"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("getAgentsForServer"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("createChannel"), jest->fn([=]() mutable
        {
            return Promise->resolve(object{
                object::pair{std:("id"), std:("123e4567-e89b-12d3-a456-426614174000")}
            });
        }
        )}, 
        object::pair{std:("getChannelsForServer"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("getChannelDetails"), jest->fn([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("getChannelParticipants"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("addChannelParticipants"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("updateChannel"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("deleteChannel"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("createMessage"), jest->fn([=]() mutable
        {
            return Promise->resolve(object{
                object::pair{std:("id"), std:("message-id")}
            });
        }
        )}, 
        object::pair{std:("getMessagesForChannel"), jest->fn([=]() mutable
        {
            return Promise->resolve(array<any>());
        }
        )}, 
        object::pair{std:("deleteMessage"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("findOrCreateDmChannel"), jest->fn([=]() mutable
        {
            return Promise->resolve(object{
                object::pair{std:("id"), std:("dm-channel-id")}
            });
        }
        )}
    }, overrides);
    return as<any>(baseAdapter);
};


std::shared_ptr<Request> createMockRequest(Partial<std::shared_ptr<Request>> overrides)
{
    return as<any>(utils::assign(object{
        object::pair{std:("params"), object{}}, 
        object::pair{std:("query"), object{}}, 
        object::pair{std:("body"), object{}}, 
        object::pair{std:("headers"), object{}}, 
        object::pair{std:("method"), std:("GET")}, 
        object::pair{std:("originalUrl"), std:("/test")}, 
        object::pair{std:("url"), std:("/test")}, 
        object::pair{std:("path"), std:("/test")}, 
        object::pair{std:("ip"), std:("127.0.0.1")}, 
        object::pair{std:("get"), jest->fn([=](auto _header) mutable
        {
            return string_empty;
        }
        )}, 
        object::pair{std:("header"), jest->fn([=](auto _header) mutable
        {
            return string_empty;
        }
        )}, 
        object::pair{std:("accepts"), jest->fn()}, 
        object::pair{std:("acceptsCharsets"), jest->fn()}, 
        object::pair{std:("acceptsEncodings"), jest->fn()}, 
        object::pair{std:("acceptsLanguages"), jest->fn()}, 
        object::pair{std:("is"), jest->fn()}
    }, overrides));
};


std::shared_ptr<Response> createMockResponse()
{
    auto res = object{
        object::pair{std:("status"), jest->fn()->mockReturnThis()}, 
        object::pair{std:("json"), jest->fn()->mockReturnThis()}, 
        object::pair{std:("send"), jest->fn()->mockReturnThis()}, 
        object::pair{std:("end"), jest->fn()->mockReturnThis()}, 
        object::pair{std:("setHeader"), jest->fn()->mockReturnThis()}, 
        object::pair{std:("removeHeader"), jest->fn()->mockReturnThis()}, 
        object::pair{std:("set"), jest->fn()->mockReturnThis()}, 
        object::pair{std:("header"), jest->fn()->mockReturnThis()}, 
        object::pair{std:("type"), jest->fn()->mockReturnThis()}, 
        object::pair{std:("sendStatus"), jest->fn()->mockReturnThis()}, 
        object::pair{std:("redirect"), jest->fn()->mockReturnThis()}, 
        object::pair{std:("cookie"), jest->fn()->mockReturnThis()}, 
        object::pair{std:("clearCookie"), jest->fn()->mockReturnThis()}, 
        object::pair{std:("attachment"), jest->fn()->mockReturnThis()}, 
        object::pair{std:("sendFile"), jest->fn([=](auto _path, auto options = undefined, auto callback = undefined) mutable
        {
            if (type_of(options) == std:("function")) {
                callback = options;
            }
            if (callback) callback();
        }
        )}, 
        object::pair{std:("headersSent"), false}, 
        object::pair{std:("locals"), object{}}
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
        object::pair{std:("on"), jest->fn()}, 
        object::pair{std:("emit"), jest->fn()}, 
        object::pair{std:("to"), jest->fn([=]() mutable
        {
            return (object{
                object::pair{std:("emit"), jest->fn()}
            });
        }
        )}, 
        object::pair{std:("sockets"), object{
            object::pair{std:("sockets"), std::make_shared<Map>()}
        }}, 
        object::pair{std:("close"), jest->fn([=](auto callback = nullptr) mutable
        {
            if (callback) callback();
        }
        )}
    };
};


any createMockHttpServer()
{
    return object{
        object::pair{std:("listen"), jest->fn([=](auto _port, auto callback = nullptr) mutable
        {
            if (callback) callback();
        }
        )}, 
        object::pair{std:("close"), jest->fn([=](auto callback = nullptr) mutable
        {
            if (callback) callback();
        }
        )}, 
        object::pair{std:("listeners"), jest->fn([=]() mutable
        {
            return array<any>();
        }
        )}, 
        object::pair{std:("removeAllListeners"), jest->fn()}, 
        object::pair{std:("on"), jest->fn()}, 
        object::pair{std:("once"), jest->fn()}, 
        object::pair{std:("emit"), jest->fn()}, 
        object::pair{std:("address"), jest->fn([=]() mutable
        {
            return (object{
                object::pair{std:("port"), 3000}
            });
        }
        )}, 
        object::pair{std:("timeout"), 0}, 
        object::pair{std:("keepAliveTimeout"), 5000}
    };
};


std::shared_ptr<Service> createMockService(Partial<std::shared_ptr<Service>> overrides)
{
    return as<any>(utils::assign(object{
        object::pair{std:("name"), std:("MockService")}, 
        object::pair{std:("description"), std:("A mock service for testing")}, 
        object::pair{std:("serviceType"), ServiceType->WEB_SEARCH}, 
        object::pair{std:("getInstance"), jest->fn()}, 
        object::pair{std:("start"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("stop"), jest->fn([=]() mutable
        {
            return Promise->resolve();
        }
        )}
    }, overrides));
};


std::shared_ptr<Express::Multer::File> createMockUploadedFile(Partial<std::shared_ptr<Express::Multer::File>> overrides)
{
    return utils::assign(object{
        object::pair{std:("fieldname"), std:("file")}, 
        object::pair{std:("originalname"), std:("test.jpg")}, 
        object::pair{std:("encoding"), std:("7bit")}, 
        object::pair{std:("mimetype"), std:("image/jpeg")}, 
        object::pair{std:("buffer"), Buffer::from(std:("test"))}, 
        object::pair{std:("size"), 12345}, 
        object::pair{std:("stream"), as<any>(undefined)}, 
        object::pair{std:("destination"), string_empty}, 
        object::pair{std:("filename"), string_empty}, 
        object::pair{std:("path"), string_empty}
    }, overrides);
};


