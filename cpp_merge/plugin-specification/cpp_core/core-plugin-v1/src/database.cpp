#include "database.hpp"
#include <string>
#include <map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

Account fromV2Entity(Entity entity) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        id: entity.id!,
        name: entity.names[0] || "Unknown",
        username: entity.names[1] || entity.names[0] || "unknown",
        details: entity.metadata || {},
        email: entity.metadata.email,
        avatarUrl: entity.metadata.avatarUrl,
        };

}

Entity toV2Entity(Account account) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        id: account.id,
        names: [account.name, account.username].filter(Boolean),
        metadata: {
            ...account.details,
            email: account.email,
            avatarUrl: account.avatarUrl,
            },
            agentId: account.id, // Use account ID agent ID
            };

}

IDatabaseAdapter fromV2DatabaseAdapter(IDatabaseAdapterV2 adapterV2) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        db: adapterV2.db,

        init: () => adapterV2.init(),
        close: () => adapterV2.close(),

        // V1 Account methods mapped to V2 Entity methods
        getAccountById: std::async (userId: UUID): Promise<Account | nullptr> => {
            try {
                const auto entities = adapterV2.getEntityByIds([userId]);
                if (entities && entities.size() > 0) {
                    return fromV2Entity(entities[0]);
                }
                return nullptr;
                } catch (error) {
                    std::cerr << "Error getting account by ID:" << error << std::endl;
                    return nullptr;
                }
                },

                createAccount: std::async (account: Account): Promise<boolean> => {
                    try {
                        const auto entity = toV2Entity(account);
                        return adapterV2.createEntities([entity]);
                        } catch (error) {
                            std::cerr << "Error creating account:" << error << std::endl;
                            return false;
                        }
                        },

                        // Memory methods - std::map V1 params to V2 structure
                        getMemories: std::async [&](params) {
                            return adapterV2.getMemories({;
                                entityId: params.agentId, // V2 uses entityId instead of agentId
                                agentId: params.agentId,
                                roomId: params.roomId,
                                count: params.count,
                                unique: params.unique,
                                tableName: params.tableName,
                                start: params.start,
                                end: params.end,
                                });
                                },

                                getMemoryById: (id: UUID) => adapterV2.getMemoryById(id),

                                getMemoriesByIds: (ids: UUID[], tableName?: std::string) =>
                                adapterV2.getMemoriesByIds(ids, tableName),

                                getMemoriesByRoomIds: (params) =>
                                adapterV2.getMemoriesByRoomIds(params),

                                getCachedEmbeddings: (params) =>
                                adapterV2.getCachedEmbeddings(params),

                                log: std::async [&](params) {
                                    // V2 uses entityId, V1 uses userId
                                    return adapterV2.log({;
                                        body: params.body,
                                        entityId: params.userId,
                                        roomId: params.roomId,
                                        type: params.type,
                                        });
                                        },

                                        // Actor details - V2 doesn't have this exact method, implement basic version
                                        getActorDetails: std::async (params): Promise<Actor[]> => {
                                            try {
                                                const auto entities = adapterV2.getEntitiesForRoom(params.roomId);
                                                return entities.std::map(entity => ({;
                                                    id: entity.id!,
                                                    name: entity.names[0] || "Unknown",
                                                    username: entity.names[1] || entity.names[0] || "unknown",
                                                    details: {
                                                        tagline: entity.metadata.tagline || "",
                                                        summary: entity.metadata.summary || "",
                                                        quote: entity.metadata.quote || "",
                                                    }
                                                    }));
                                                    } catch (error) {
                                                        std::cerr << "Error getting actor details:" << error << std::endl;
                                                        return [];
                                                    }
                                                    },

                                                    searchMemories: [&](params) {
                                                        return adapterV2.searchMemories({;
                                                            embedding: params.embedding,
                                                            match_threshold: params.match_threshold,
                                                            count: params.match_count,
                                                            unique: params.unique,
                                                            tableName: params.tableName,
                                                            roomId: params.roomId,
                                                            entityId: params.agentId, // Map agentId to entityId
                                                            });
                                                            },

                                                            // Goal methods - V2 doesn't have goals, implement stub methods
                                                            updateGoalStatus: std::async (params): Promise<void> => {
                                                                std::cout << "updateGoalStatus not implemented in V2 adapter" << std::endl;
                                                                },

                                                                searchMemoriesByEmbedding: [&](embedding[], params) {
                                                                    return adapterV2.searchMemories({;
                                                                        embedding,
                                                                        match_threshold: params.match_threshold,
                                                                        count: params.count,
                                                                        roomId: params.roomId,
                                                                        entityId: params.agentId,
                                                                        unique: params.unique,
                                                                        tableName: params.tableName,
                                                                        });
                                                                        },

                                                                        createMemory: std::async [&](memory: Memory, tableName: std::string, unique?) {
                                                                            adapterV2.createMemory(memory, tableName, unique);
                                                                            },

                                                                            removeMemory: [&](memoryId: UUID, tableName: std::string) {
                                                                                // V2 uses deleteMemory instead of removeMemory
                                                                                return adapterV2.deleteMemory(memoryId);
                                                                                },

                                                                                removeAllMemories: [&](roomId: UUID, tableName: std::string) {
                                                                                    return adapterV2.deleteAllMemories(roomId, tableName);
                                                                                    },

                                                                                    countMemories: (roomId: UUID, unique?, tableName?: std::string) =>
                                                                                    adapterV2.countMemories(roomId, unique, tableName),

                                                                                    // Goal methods - not implemented in V2, return empty/stub implementations
                                                                                    getGoals: std::async (): Promise<Goal[]> => {
                                                                                        std::cout << "getGoals not implemented in V2 adapter" << std::endl;
                                                                                        return [];
                                                                                        },

                                                                                        updateGoal: std::async (goal: Goal): Promise<void> => {
                                                                                            std::cout << "updateGoal not implemented in V2 adapter" << std::endl;
                                                                                            },

                                                                                            createGoal: std::async (goal: Goal): Promise<void> => {
                                                                                                std::cout << "createGoal not implemented in V2 adapter" << std::endl;
                                                                                                },

                                                                                                removeGoal: std::async (goalId: UUID): Promise<void> => {
                                                                                                    std::cout << "removeGoal not implemented in V2 adapter" << std::endl;
                                                                                                    },

                                                                                                    removeAllGoals: std::async (roomId: UUID): Promise<void> => {
                                                                                                        std::cout << "removeAllGoals not implemented in V2 adapter" << std::endl;
                                                                                                        },

                                                                                                        // Room methods
                                                                                                        getRoom: std::async [&](roomId: UUID) {
                                                                                                            const auto rooms = adapterV2.getRoomsByIds([roomId]);
                                                                                                            return rooms && rooms.size() > 0 ? roomId : nullptr;
                                                                                                            },

                                                                                                            createRoom: std::async (roomId?: UUID): Promise<UUID> => {
                                                                                                                // V2 createRooms expects array and returns array
                                                                                                                const auto room = {;
                                                                                                                    id: roomId,
                                                                                                                    source: "v1-adapter",
                                                                                                                    type: "GROUP",
                                                                                                                    };
                                                                                                                    const auto roomIds = adapterV2.createRooms([room]);
                                                                                                                    return roomIds[0];
                                                                                                                    },

                                                                                                                    removeRoom: (roomId: UUID) => adapterV2.deleteRoom(roomId),

                                                                                                                    getRoomsForParticipant: (userId: UUID) =>
                                                                                                                    adapterV2.getRoomsForParticipant(userId),

                                                                                                                    getRoomsForParticipants: (userIds: UUID[]) =>
                                                                                                                    adapterV2.getRoomsForParticipants(userIds),

                                                                                                                    addParticipant: (userId: UUID, roomId: UUID) =>
                                                                                                                    adapterV2.addParticipantsRoom([userId], roomId),

                                                                                                                    removeParticipant: (userId: UUID, roomId: UUID) =>
                                                                                                                    adapterV2.removeParticipant(userId, roomId),

                                                                                                                    // Participant methods
                                                                                                                    getParticipantsForAccount: std::async (userId: UUID): Promise<Participant[]> => {
                                                                                                                        const auto participants = adapterV2.getParticipantsForEntity(userId);
                                                                                                                        return participants.std::map(p => ({;
                                                                                                                            id: p.id,
                                                                                                                            account: fromV2Entity(p.entity),
                                                                                                                            }));
                                                                                                                            },

                                                                                                                            getParticipantsForRoom: (roomId: UUID) =>
                                                                                                                            adapterV2.getParticipantsForRoom(roomId),

                                                                                                                            getParticipantUserState: (roomId: UUID, userId: UUID) =>
                                                                                                                            adapterV2.getParticipantUserState(roomId, userId),

                                                                                                                            setParticipantUserState: (roomId: UUID, userId: UUID, state) =>
                                                                                                                            adapterV2.setParticipantUserState(roomId, userId, state),

                                                                                                                            // Relationship methods - V2 has different signature
                                                                                                                            createRelationship: std::async (params): Promise<boolean> => {
                                                                                                                                return adapterV2.createRelationship({;
                                                                                                                                    sourceEntityId: params.userA,
                                                                                                                                    targetEntityId: params.userB,
                                                                                                                                    });
                                                                                                                                    },

                                                                                                                                    getRelationship: std::async (params): Promise<Relationship | nullptr> => {
                                                                                                                                        const auto relationship = adapterV2.getRelationship({;
                                                                                                                                            sourceEntityId: params.userA,
                                                                                                                                            targetEntityId: params.userB,
                                                                                                                                            });

                                                                                                                                            if (!relationship) return null;

                                                                                                                                            // Convert V2 relationship to V1 format
                                                                                                                                            return {
                                                                                                                                                id: relationship.id,
                                                                                                                                                userA: relationship.sourceEntityId,
                                                                                                                                                userB: relationship.targetEntityId,
                                                                                                                                                userId: relationship.sourceEntityId, // Use source user
                                                                                                                                                roomId: relationship.id, // V1 expects roomId, use relationship ID
                                                                                                                                                status: relationship.tags.join(","), // Convert tags to status std::string createdAt: relationship.createdAt,
                                                                                                                                                };
                                                                                                                                                },

                                                                                                                                                getRelationships: std::async (params): Promise<Relationship[]> => {
                                                                                                                                                    const auto relationships = adapterV2.getRelationships({;
                                                                                                                                                        entityId: params.userId,
                                                                                                                                                        });

                                                                                                                                                        return relationships.std::map(rel => ({;
                                                                                                                                                            id: rel.id,
                                                                                                                                                            userA: rel.sourceEntityId,
                                                                                                                                                            userB: rel.targetEntityId,
                                                                                                                                                            userId: params.userId,
                                                                                                                                                            roomId: rel.id, // Use relationship ID
                                                                                                                                                            status: rel.tags.join(","),
                                                                                                                                                            createdAt: rel.createdAt,
                                                                                                                                                            }));
                                                                                                                                                            },

                                                                                                                                                            // Knowledge methods - V2 doesn't have exact equivalent, implement stubs
                                                                                                                                                            getKnowledge: std::async (): Promise<RAGKnowledgeItem[]> => {
                                                                                                                                                                std::cout << "getKnowledge not fully implemented in V2 adapter" << std::endl;
                                                                                                                                                                return [];
                                                                                                                                                                },

                                                                                                                                                                searchKnowledge: std::async (): Promise<RAGKnowledgeItem[]> => {
                                                                                                                                                                    std::cout << "searchKnowledge not fully implemented in V2 adapter" << std::endl;
                                                                                                                                                                    return [];
                                                                                                                                                                    },

                                                                                                                                                                    createKnowledge: std::async (knowledge: RAGKnowledgeItem): Promise<void> => {
                                                                                                                                                                        std::cout << "createKnowledge not fully implemented in V2 adapter" << std::endl;
                                                                                                                                                                        },

                                                                                                                                                                        removeKnowledge: std::async (id: UUID): Promise<void> => {
                                                                                                                                                                            std::cout << "removeKnowledge not fully implemented in V2 adapter" << std::endl;
                                                                                                                                                                            },

                                                                                                                                                                            clearKnowledge: std::async (agentId: UUID, shared?): Promise<void> => {
                                                                                                                                                                                std::cout << "clearKnowledge not fully implemented in V2 adapter" << std::endl;
                                                                                                                                                                                },
                                                                                                                                                                                };

}

IDatabaseAdapterV2 toV2DatabaseAdapter(IDatabaseAdapter adapterV1) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // This is a complex conversion that would require extensive implementation
        // For now, we'll provide a basic stub that logs warnings for unimplemented methods
        throw std::runtime_error('toV2DatabaseAdapter not yet fully implemented - V2 interface is significantly different');

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
