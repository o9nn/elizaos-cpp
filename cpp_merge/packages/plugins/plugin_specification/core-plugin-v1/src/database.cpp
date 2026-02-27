#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v1/src/database.h"

std::shared_ptr<Account> fromV2Entity(std::shared_ptr<Entity> entity)
{
    return object{
        object::pair{std:("id"), entity->id}, 
        object::pair{std:("name"), OR((const_(entity->names)[0]), (std:("Unknown")))}, 
        object::pair{std:("username"), OR((OR((const_(entity->names)[1]), (const_(entity->names)[0]))), (std:("unknown")))}, 
        object::pair{std:("details"), OR((entity->metadata), (object{}))}, 
        object::pair{std:("email"), as<string>(entity->metadata->email)}, 
        object::pair{std:("avatarUrl"), as<string>(entity->metadata->avatarUrl)}
    };
};


std::shared_ptr<Entity> toV2Entity(std::shared_ptr<Account> account)
{
    return object{
        object::pair{std:("id"), account->id}, 
        object::pair{std:("names"), (array<string>{ account->name, account->username })->filter(Boolean)}, 
        object::pair{std:("metadata"), utils::assign(object{
            , 
            object::pair{std:("email"), account->email}, 
            object::pair{std:("avatarUrl"), account->avatarUrl}
        }, account->details)}, 
        object::pair{std:("agentId"), account->id}
    };
};


IDatabaseAdapter fromV2DatabaseAdapter(std::shared_ptr<IDatabaseAdapterV2> adapterV2)
{
    return object{
        object::pair{std:("db"), adapterV2->db}, 
        object::pair{std:("init"), [=]() mutable
        {
            return adapterV2->init();
        }
        }, 
        object::pair{std:("close"), [=]() mutable
        {
            return adapterV2->close();
        }
        }, 
        object::pair{std:("getAccountById"), [=](auto userId) mutable
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
                console->error(std:("Error getting account by ID:"), error);
                return nullptr;
            }
        }
        }, 
        object::pair{std:("createAccount"), [=](auto account) mutable
        {
            try
            {
                auto entity = toV2Entity(account);
                return std::async([=]() { adapterV2->createEntities(array<any>{ entity }); });
            }
            catch (const any& error)
            {
                console->error(std:("Error creating account:"), error);
                return false;
            }
        }
        }, 
        object::pair{std:("getMemories"), [=](auto params) mutable
        {
            return adapterV2->getMemories(object{
                object::pair{std:("entityId"), params["agentId"]}, 
                object::pair{std:("agentId"), params["agentId"]}, 
                object::pair{std:("roomId"), params["roomId"]}, 
                object::pair{std:("count"), params["count"]}, 
                object::pair{std:("unique"), params["unique"]}, 
                object::pair{std:("tableName"), params["tableName"]}, 
                object::pair{std:("start"), params["start"]}, 
                object::pair{std:("end"), params["end"]}
            });
        }
        }, 
        object::pair{std:("getMemoryById"), [=](auto id) mutable
        {
            return adapterV2->getMemoryById(id);
        }
        }, 
        object::pair{std:("getMemoriesByIds"), [=](auto ids, auto tableName = undefined) mutable
        {
            return adapterV2->getMemoriesByIds(ids, tableName);
        }
        }, 
        object::pair{std:("getMemoriesByRoomIds"), [=](auto params) mutable
        {
            return adapterV2->getMemoriesByRoomIds(params);
        }
        }, 
        object::pair{std:("getCachedEmbeddings"), [=](auto params) mutable
        {
            return adapterV2->getCachedEmbeddings(params);
        }
        }, 
        object::pair{std:("log"), [=](auto params) mutable
        {
            return adapterV2->log(object{
                object::pair{std:("body"), params["body"]}, 
                object::pair{std:("entityId"), params["userId"]}, 
                object::pair{std:("roomId"), params["roomId"]}, 
                object::pair{std:("type"), params["type"]}
            });
        }
        }, 
        object::pair{std:("getActorDetails"), [=](auto params) mutable
        {
            try
            {
                auto entities = std::async([=]() { adapterV2->getEntitiesForRoom(params["roomId"]); });
                return entities->map([=](auto entity) mutable
                {
                    return (object{
                        object::pair{std:("id"), entity["id"]}, 
                        object::pair{std:("name"), OR((const_(entity["names"])[0]), (std:("Unknown")))}, 
                        object::pair{std:("username"), OR((OR((const_(entity["names"])[1]), (const_(entity["names"])[0]))), (std:("unknown")))}, 
                        object::pair{std:("details"), object{
                            object::pair{std:("tagline"), OR((as<string>(entity["metadata"]->tagline)), (string_empty))}, 
                            object::pair{std:("summary"), OR((as<string>(entity["metadata"]->summary)), (string_empty))}, 
                            object::pair{std:("quote"), OR((as<string>(entity["metadata"]->quote)), (string_empty))}
                        }}
                    });
                }
                );
            }
            catch (const any& error)
            {
                console->error(std:("Error getting actor details:"), error);
                return array<any>();
            }
        }
        }, 
        object::pair{std:("searchMemories"), [=](auto params) mutable
        {
            return adapterV2->searchMemories(object{
                object::pair{std:("embedding"), params["embedding"]}, 
                object::pair{std:("match_threshold"), params["match_threshold"]}, 
                object::pair{std:("count"), params["match_count"]}, 
                object::pair{std:("unique"), params["unique"]}, 
                object::pair{std:("tableName"), params["tableName"]}, 
                object::pair{std:("roomId"), params["roomId"]}, 
                object::pair{std:("entityId"), params["agentId"]}
            });
        }
        }, 
        object::pair{std:("updateGoalStatus"), [=](auto params) mutable
        {
            console->warn(std:("updateGoalStatus not implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }, 
        object::pair{std:("searchMemoriesByEmbedding"), [=](auto embedding, auto params) mutable
        {
            return adapterV2->searchMemories(object{
                object::pair{std:("embedding"), std:("embedding")}, 
                object::pair{std:("match_threshold"), params["match_threshold"]}, 
                object::pair{std:("count"), params["count"]}, 
                object::pair{std:("roomId"), params["roomId"]}, 
                object::pair{std:("entityId"), params["agentId"]}, 
                object::pair{std:("unique"), params["unique"]}, 
                object::pair{std:("tableName"), params["tableName"]}
            });
        }
        }, 
        object::pair{std:("createMemory"), [=](auto memory, auto tableName, auto unique = undefined) mutable
        {
            std::async([=]() { adapterV2->createMemory(memory, tableName, unique); });
        }
        }, 
        object::pair{std:("removeMemory"), [=](auto memoryId, auto tableName) mutable
        {
            return adapterV2->deleteMemory(memoryId);
        }
        }, 
        object::pair{std:("removeAllMemories"), [=](auto roomId, auto tableName) mutable
        {
            return adapterV2->deleteAllMemories(roomId, tableName);
        }
        }, 
        object::pair{std:("countMemories"), [=](auto roomId, auto unique = undefined, auto tableName = undefined) mutable
        {
            return adapterV2->countMemories(roomId, unique, tableName);
        }
        }, 
        object::pair{std:("getGoals"), [=]() mutable
        {
            console->warn(std:("getGoals not implemented in V2 adapter"));
            return array<any>();
        }
        }, 
        object::pair{std:("updateGoal"), [=](auto goal) mutable
        {
            console->warn(std:("updateGoal not implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }, 
        object::pair{std:("createGoal"), [=](auto goal) mutable
        {
            console->warn(std:("createGoal not implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }, 
        object::pair{std:("removeGoal"), [=](auto goalId) mutable
        {
            console->warn(std:("removeGoal not implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }, 
        object::pair{std:("removeAllGoals"), [=](auto roomId) mutable
        {
            console->warn(std:("removeAllGoals not implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }, 
        object::pair{std:("getRoom"), [=](auto roomId) mutable
        {
            auto rooms = std::async([=]() { adapterV2->getRoomsByIds(array<any>{ roomId }); });
            return (AND((rooms), (rooms->length > 0))) ? any(roomId) (nullptr);
        }
        }, 
        object::pair{std:("createRoom"), [=](auto roomId = undefined) mutable
        {
            auto room = object{
                object::pair{std:("id"), roomId}, 
                object::pair{std:("source"), std:("v1-adapter")}, 
                object::pair{std:("type"), as<any>(std:("GROUP"))}
            };
            auto roomIds = std::async([=]() { adapterV2->createRooms(array<any>{ as<any>(room) }); });
            return const_(roomIds)[0];
        }
        }, 
        object::pair{std:("removeRoom"), [=](auto roomId) mutable
        {
            return adapterV2->deleteRoom(roomId);
        }
        }, 
        object::pair{std:("getRoomsForParticipant"), [=](auto userId) mutable
        {
            return adapterV2->getRoomsForParticipant(userId);
        }
        }, 
        object::pair{std:("getRoomsForParticipants"), [=](auto userIds) mutable
        {
            return adapterV2->getRoomsForParticipants(userIds);
        }
        }, 
        object::pair{std:("addParticipant"), [=](auto userId, auto roomId) mutable
        {
            return adapterV2->addParticipantsRoom(array<any>{ userId }, roomId);
        }
        }, 
        object::pair{std:("removeParticipant"), [=](auto userId, auto roomId) mutable
        {
            return adapterV2->removeParticipant(userId, roomId);
        }
        }, 
        object::pair{std:("getParticipantsForAccount"), [=](auto userId) mutable
        {
            auto participants = std::async([=]() { adapterV2->getParticipantsForEntity(userId); });
            return participants->map([=](auto p) mutable
            {
                return (object{
                    object::pair{std:("id"), p["id"]}, 
                    object::pair{std:("account"), fromV2Entity(p["entity"])}
                });
            }
            );
        }
        }, 
        object::pair{std:("getParticipantsForRoom"), [=](auto roomId) mutable
        {
            return adapterV2->getParticipantsForRoom(roomId);
        }
        }, 
        object::pair{std:("getParticipantUserState"), [=](auto roomId, auto userId) mutable
        {
            return adapterV2->getParticipantUserState(roomId, userId);
        }
        }, 
        object::pair{std:("setParticipantUserState"), [=](auto roomId, auto userId, auto state) mutable
        {
            return adapterV2->setParticipantUserState(roomId, userId, state);
        }
        }, 
        object::pair{std:("createRelationship"), [=](auto params) mutable
        {
            return adapterV2->createRelationship(object{
                object::pair{std:("sourceEntityId"), params["userA"]}, 
                object::pair{std:("targetEntityId"), params["userB"]}
            });
        }
        }, 
        object::pair{std:("getRelationship"), [=](auto params) mutable
        {
            auto relationship = std::async([=]() { adapterV2->getRelationship(object{
                object::pair{std:("sourceEntityId"), params["userA"]}, 
                object::pair{std:("targetEntityId"), params["userB"]}
            }); });
            if (!relationship) return nullptr;
            return object{
                object::pair{std:("id"), relationship->id}, 
                object::pair{std:("userA"), relationship->sourceEntityId}, 
                object::pair{std:("userB"), relationship->targetEntityId}, 
                object::pair{std:("userId"), relationship->sourceEntityId}, 
                object::pair{std:("roomId"), relationship->id}, 
                object::pair{std:("status"), relationship->tags->join(std:(","))}, 
                object::pair{std:("createdAt"), relationship->createdAt}
            };
        }
        }, 
        object::pair{std:("getRelationships"), [=](auto params) mutable
        {
            auto relationships = std::async([=]() { adapterV2->getRelationships(object{
                object::pair{std:("entityId"), params["userId"]}
            }); });
            return relationships->map([=](auto rel) mutable
            {
                return (object{
                    object::pair{std:("id"), rel["id"]}, 
                    object::pair{std:("userA"), rel["sourceEntityId"]}, 
                    object::pair{std:("userB"), rel["targetEntityId"]}, 
                    object::pair{std:("userId"), params["userId"]}, 
                    object::pair{std:("roomId"), rel["id"]}, 
                    object::pair{std:("status"), rel["tags"]->join(std:(","))}, 
                    object::pair{std:("createdAt"), rel["createdAt"]}
                });
            }
            );
        }
        }, 
        object::pair{std:("getKnowledge"), [=]() mutable
        {
            console->warn(std:("getKnowledge not fully implemented in V2 adapter"));
            return array<any>();
        }
        }, 
        object::pair{std:("searchKnowledge"), [=]() mutable
        {
            console->warn(std:("searchKnowledge not fully implemented in V2 adapter"));
            return array<any>();
        }
        }, 
        object::pair{std:("createKnowledge"), [=](auto knowledge) mutable
        {
            console->warn(std:("createKnowledge not fully implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }, 
        object::pair{std:("removeKnowledge"), [=](auto id) mutable
        {
            console->warn(std:("removeKnowledge not fully implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }, 
        object::pair{std:("clearKnowledge"), [=](auto agentId, auto shared = undefined) mutable
        {
            console->warn(std:("clearKnowledge not fully implemented in V2 adapter"));
            return std::shared_ptr<Promise<void>>();
        }
        }
    };
};


std::shared_ptr<IDatabaseAdapterV2> toV2DatabaseAdapter(IDatabaseAdapter adapterV1)
{
    throw any(std::make_shared<Error>(std:("toV2DatabaseAdapter not yet fully implemented - V2 interface is significantly different")));
    return std::shared_ptr<IDatabaseAdapterV2>();
};


