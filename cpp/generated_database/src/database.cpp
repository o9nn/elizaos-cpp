#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v1/src/database.h"

std::shared_ptr<Account> fromV2Entity(std::shared_ptr<Entity> entity)
{
    return object{
        object::pair{std::string("id"), entity->id}, 
        object::pair{std::string("name"), OR((const_(entity->names)[0]), (std::string("Unknown")))}, 
        object::pair{std::string("username"), OR((OR((const_(entity->names)[1]), (const_(entity->names)[0]))), (std::string("unknown")))}, 
        object::pair{std::string("details"), OR((entity->metadata), (object{}))}, 
        object::pair{std::string("email"), as<string>(entity->metadata->email)}, 
        object::pair{std::string("avatarUrl"), as<string>(entity->metadata->avatarUrl)}
    };
};


std::shared_ptr<Entity> toV2Entity(std::shared_ptr<Account> account)
{
    return object{
        object::pair{std::string("id"), account->id}, 
        object::pair{std::string("names"), (array<string>{ account->name, account->username })->filter(Boolean)}, 
        object::pair{std::string("metadata"), utils::assign(object{
            , 
            object::pair{std::string("email"), account->email}, 
            object::pair{std::string("avatarUrl"), account->avatarUrl}
        }, account->details)}, 
        object::pair{std::string("agentId"), account->id}
    };
};


IDatabaseAdapter fromV2DatabaseAdapter(std::shared_ptr<IDatabaseAdapterV2> adapterV2)
{
    return object{
        object::pair{std::string("db"), adapterV2->db}, 
        object::pair{std::string("init"), [=]() mutable
        {
            return adapterV2->init();
        }
        }, 
        object::pair{std::string("close"), [=]() mutable
        {
            return adapterV2->close();
        }
        }, 
        object::pair{std::string("getAccountById"), [=](auto userId) mutable
        {
            try
            {
                auto entities = std::async([=]() { adapterV2->getEntityByIds(array<any>{ userId }); });
                if (AND((entities), (entities->length > 0))) {
                    return fromV2Entity(const_(entities)[0]);
                }
                return nullptr;
            }
            catch (const any& error)
            {
                console->error(std::string("Error getting account by ID:"), error);
                return nullptr;
            }
        }
        }, 
        object::pair{std::string("createAccount"), [=](auto account) mutable
        {
            try
            {
                auto entity = toV2Entity(account);
                return std::async([=]() { adapterV2->createEntities(array<any>{ entity }); });
            }
            catch (const any& error)
            {
                console->error(std::string("Error creating account:"), error);
                return false;
            }
        }
        }, 
        object::pair{std::string("getMemories"), [=](auto params) mutable
        {
            return adapterV2->getMemories(object{
                object::pair{std::string("entityId"), params["agentId"]}, 
                object::pair{std::string("agentId"), params["agentId"]}, 
                object::pair{std::string("roomId"), params["roomId"]}, 
                object::pair{std::string("count"), params["count"]}, 
                object::pair{std::string("unique"), params["unique"]}, 
                object::pair{std::string("tableName"), params["tableName"]}, 
                object::pair{std::string("start"), params["start"]}, 
                object::pair{std::string("end"), params["end"]}
            });
        }
        }, 
        object::pair{std::string("getMemoryById"), [=](auto id) mutable
        {
            return adapterV2->getMemoryById(id);
        }
        }, 
        object::pair{std::string("getMemoriesByIds"), [=](auto ids, auto tableName = undefined) mutable
        {
            return adapterV2->getMemoriesByIds(ids, tableName);
        }
        }, 
        object::pair{std::string("getMemoriesByRoomIds"), [=](auto params) mutable
        {
            return adapterV2->getMemoriesByRoomIds(params);
        }
        }, 
        object::pair{std::string("getCachedEmbeddings"), [=](auto params) mutable
        {
            return adapterV2->getCachedEmbeddings(params);
        }
        }, 
        object::pair{std::string("log"), [=](auto params) mutable
        {
            return adapterV2->log(object{
                object::pair{std::string("body"), params["body"]}, 
                object::pair{std::string("entityId"), params["userId"]}, 
                object::pair{std::string("roomId"), params["roomId"]}, 
                object::pair{std::string("type"), params["type"]}
            });
        }
        }, 
        object::pair{std::string("getActorDetails"), [=](auto params) mutable
        {
            try
            {
                auto entities = std::async([=]() { adapterV2->getEntitiesForRoom(params["roomId"]); });
                return entities->map([=](auto entity) mutable
                {
                    return (object{
                        object::pair{std::string("id"), entity["id"]}, 
                        object::pair{std::string("name"), OR((const_(entity["names"])[0]), (std::string("Unknown")))}, 
                        object::pair{std::string("username"), OR((OR((const_(entity["names"])[1]), (const_(entity["names"])[0]))), (std::string("unknown")))}, 
                        object::pair{std::string("details"), object{
                            object::pair{std::string("tagline"), OR((as<string>(entity["metadata"]->tagline)), (string_empty))}, 
                            object::pair{std::string("summary"), OR((as<string>(entity["metadata"]->summary)), (string_empty))}, 
                            object::pair{std::string("quote"), OR((as<string>(entity["metadata"]->quote)), (string_empty))}
                        }}
                    });
                }
                );
            }
            catch (const any& error)
            {
                console->error(std::string("Error getting actor details:"), error);
                return array<any>();
            }
        }
        }, 
        object::pair{std::string("searchMemories"), [=](auto params) mutable
        {
            return adapterV2->searchMemories(object{
                object::pair{std::string("embedding"), params["embedding"]}, 
                object::pair{std::string("match_threshold"), params["match_threshold"]}, 
                object::pair{std::string("count"), params["match_count"]}, 
                object::pair{std::string("unique"), params["unique"]}, 
                object::pair{std::string("tableName"), params["tableName"]}, 
                object::pair{std::string("roomId"), params["roomId"]}, 
                object::pair{std::string("entityId"), params["agentId"]}
            });
        }
        }, 
        object::pair{std::string("updateGoalStatus"), [=](auto params) mutable
        {
            console->warn(std::string("updateGoalStatus not implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }, 
        object::pair{std::string("searchMemoriesByEmbedding"), [=](auto embedding, auto params) mutable
        {
            return adapterV2->searchMemories(object{
                object::pair{std::string("embedding"), std::string("embedding")}, 
                object::pair{std::string("match_threshold"), params["match_threshold"]}, 
                object::pair{std::string("count"), params["count"]}, 
                object::pair{std::string("roomId"), params["roomId"]}, 
                object::pair{std::string("entityId"), params["agentId"]}, 
                object::pair{std::string("unique"), params["unique"]}, 
                object::pair{std::string("tableName"), params["tableName"]}
            });
        }
        }, 
        object::pair{std::string("createMemory"), [=](auto memory, auto tableName, auto unique = undefined) mutable
        {
            std::async([=]() { adapterV2->createMemory(memory, tableName, unique); });
        }
        }, 
        object::pair{std::string("removeMemory"), [=](auto memoryId, auto tableName) mutable
        {
            return adapterV2->deleteMemory(memoryId);
        }
        }, 
        object::pair{std::string("removeAllMemories"), [=](auto roomId, auto tableName) mutable
        {
            return adapterV2->deleteAllMemories(roomId, tableName);
        }
        }, 
        object::pair{std::string("countMemories"), [=](auto roomId, auto unique = undefined, auto tableName = undefined) mutable
        {
            return adapterV2->countMemories(roomId, unique, tableName);
        }
        }, 
        object::pair{std::string("getGoals"), [=]() mutable
        {
            console->warn(std::string("getGoals not implemented in V2 adapter"));
            return array<any>();
        }
        }, 
        object::pair{std::string("updateGoal"), [=](auto goal) mutable
        {
            console->warn(std::string("updateGoal not implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }, 
        object::pair{std::string("createGoal"), [=](auto goal) mutable
        {
            console->warn(std::string("createGoal not implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }, 
        object::pair{std::string("removeGoal"), [=](auto goalId) mutable
        {
            console->warn(std::string("removeGoal not implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }, 
        object::pair{std::string("removeAllGoals"), [=](auto roomId) mutable
        {
            console->warn(std::string("removeAllGoals not implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }, 
        object::pair{std::string("getRoom"), [=](auto roomId) mutable
        {
            auto rooms = std::async([=]() { adapterV2->getRoomsByIds(array<any>{ roomId }); });
            return (AND((rooms), (rooms->length > 0))) ? any(roomId) : any(nullptr);
        }
        }, 
        object::pair{std::string("createRoom"), [=](auto roomId = undefined) mutable
        {
            auto room = object{
                object::pair{std::string("id"), roomId}, 
                object::pair{std::string("source"), std::string("v1-adapter")}, 
                object::pair{std::string("type"), as<any>(std::string("GROUP"))}
            };
            auto roomIds = std::async([=]() { adapterV2->createRooms(array<any>{ as<any>(room) }); });
            return const_(roomIds)[0];
        }
        }, 
        object::pair{std::string("removeRoom"), [=](auto roomId) mutable
        {
            return adapterV2->deleteRoom(roomId);
        }
        }, 
        object::pair{std::string("getRoomsForParticipant"), [=](auto userId) mutable
        {
            return adapterV2->getRoomsForParticipant(userId);
        }
        }, 
        object::pair{std::string("getRoomsForParticipants"), [=](auto userIds) mutable
        {
            return adapterV2->getRoomsForParticipants(userIds);
        }
        }, 
        object::pair{std::string("addParticipant"), [=](auto userId, auto roomId) mutable
        {
            return adapterV2->addParticipantsRoom(array<any>{ userId }, roomId);
        }
        }, 
        object::pair{std::string("removeParticipant"), [=](auto userId, auto roomId) mutable
        {
            return adapterV2->removeParticipant(userId, roomId);
        }
        }, 
        object::pair{std::string("getParticipantsForAccount"), [=](auto userId) mutable
        {
            auto participants = std::async([=]() { adapterV2->getParticipantsForEntity(userId); });
            return participants->map([=](auto p) mutable
            {
                return (object{
                    object::pair{std::string("id"), p["id"]}, 
                    object::pair{std::string("account"), fromV2Entity(p["entity"])}
                });
            }
            );
        }
        }, 
        object::pair{std::string("getParticipantsForRoom"), [=](auto roomId) mutable
        {
            return adapterV2->getParticipantsForRoom(roomId);
        }
        }, 
        object::pair{std::string("getParticipantUserState"), [=](auto roomId, auto userId) mutable
        {
            return adapterV2->getParticipantUserState(roomId, userId);
        }
        }, 
        object::pair{std::string("setParticipantUserState"), [=](auto roomId, auto userId, auto state) mutable
        {
            return adapterV2->setParticipantUserState(roomId, userId, state);
        }
        }, 
        object::pair{std::string("createRelationship"), [=](auto params) mutable
        {
            return adapterV2->createRelationship(object{
                object::pair{std::string("sourceEntityId"), params["userA"]}, 
                object::pair{std::string("targetEntityId"), params["userB"]}
            });
        }
        }, 
        object::pair{std::string("getRelationship"), [=](auto params) mutable
        {
            auto relationship = std::async([=]() { adapterV2->getRelationship(object{
                object::pair{std::string("sourceEntityId"), params["userA"]}, 
                object::pair{std::string("targetEntityId"), params["userB"]}
            }); });
            if (!relationship) return nullptr;
            return object{
                object::pair{std::string("id"), relationship->id}, 
                object::pair{std::string("userA"), relationship->sourceEntityId}, 
                object::pair{std::string("userB"), relationship->targetEntityId}, 
                object::pair{std::string("userId"), relationship->sourceEntityId}, 
                object::pair{std::string("roomId"), relationship->id}, 
                object::pair{std::string("status"), relationship->tags->join(std::string(","))}, 
                object::pair{std::string("createdAt"), relationship->createdAt}
            };
        }
        }, 
        object::pair{std::string("getRelationships"), [=](auto params) mutable
        {
            auto relationships = std::async([=]() { adapterV2->getRelationships(object{
                object::pair{std::string("entityId"), params["userId"]}
            }); });
            return relationships->map([=](auto rel) mutable
            {
                return (object{
                    object::pair{std::string("id"), rel["id"]}, 
                    object::pair{std::string("userA"), rel["sourceEntityId"]}, 
                    object::pair{std::string("userB"), rel["targetEntityId"]}, 
                    object::pair{std::string("userId"), params["userId"]}, 
                    object::pair{std::string("roomId"), rel["id"]}, 
                    object::pair{std::string("status"), rel["tags"]->join(std::string(","))}, 
                    object::pair{std::string("createdAt"), rel["createdAt"]}
                });
            }
            );
        }
        }, 
        object::pair{std::string("getKnowledge"), [=]() mutable
        {
            console->warn(std::string("getKnowledge not fully implemented in V2 adapter"));
            return array<any>();
        }
        }, 
        object::pair{std::string("searchKnowledge"), [=]() mutable
        {
            console->warn(std::string("searchKnowledge not fully implemented in V2 adapter"));
            return array<any>();
        }
        }, 
        object::pair{std::string("createKnowledge"), [=](auto knowledge) mutable
        {
            console->warn(std::string("createKnowledge not fully implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }, 
        object::pair{std::string("removeKnowledge"), [=](auto id) mutable
        {
            console->warn(std::string("removeKnowledge not fully implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }, 
        object::pair{std::string("clearKnowledge"), [=](auto agentId, auto shared = undefined) mutable
        {
            console->warn(std::string("clearKnowledge not fully implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }
    };
};


std::shared_ptr<IDatabaseAdapterV2> toV2DatabaseAdapter(IDatabaseAdapter adapterV1)
{
    throw any(std::make_shared<Error>(std::string("toV2DatabaseAdapter not yet fully implemented - V2 interface is significantly different")));
    return std::shared_ptr<IDatabaseAdapterV2>();
};


