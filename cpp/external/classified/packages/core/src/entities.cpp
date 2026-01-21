#include "entities.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<> getRecentInteractions(IAgentRuntime runtime, UUID sourceEntityId, const std::vector<Entity>& candidateEntities, UUID roomId, const std::vector<Relationship>& relationships) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    entity: Entity; interactions: Memory[]; count: number
}

std::future<std::optional<Entity>> findEntityByName(IAgentRuntime runtime, Memory message, State state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto room = state.data.room || (runtime.getRoom(message.roomId));
    if (!room) {
        std::cout << "Room not found for entity search" << std::endl;
        return nullptr;
    }

    const auto world = room.worldId ? runtime.getWorld(room.worldId) : nullptr;

    // Get all entities in the room with their components
    const auto entitiesInRoom = runtime.getEntitiesForRoom(room.id, true);

    // Filter components for each entity based on permissions
    const auto filteredEntities = Promise.all(;
    entitiesInRoom.map(async (entity) => {
        if (!entity.components) return entity;

        // Get world roles if we have a world
        const auto worldRoles = world.metadata.roles || {};

        // Filter components based on permissions
        entity.components = entity.components.filter((component) => {
            // 1. Pass if sourceEntityId matches the requesting entity
            if (component.sourceEntityId == message.entityId) return true;

            // 2. Pass if sourceEntityId is an owner/admin of the current world
            if (world && component.sourceEntityId) {
                const auto sourceRole = worldRoles[component.sourceEntityId];
                if (sourceRole == 'OWNER' || sourceRole == 'ADMIN') return true;
            }

            // 3. Pass if sourceEntityId is the agentId
            if (component.sourceEntityId == runtime.agentId) return true;

            // Filter out components that don't meet any criteria
            return false;
            });

            return entity;
            });
            );

            // Get relationships for the message sender
            const auto relationships = runtime.getRelationships({;
                entityId: message.entityId,
                });

                // Get entities from relationships
                const auto relationshipEntities = Promise.all(;
                relationships.map(async (rel) => {
                    const auto entityId =;
                    rel.sourceEntityId == message.entityId ? rel.targetEntityId : rel.sourceEntityId;
                    return runtime.getEntityById(entityId);
                    });
                    );

                    // Filter out nulls and combine with room entities
                    const auto allEntities = [;
                    ...filteredEntities,
                    ...relationshipEntities.filter((e): e is Entity => e != nullptr),
                    ];

                    // Get interaction strength data for relationship entities
                    const auto interactionData = getRecentInteractions(;
                    runtime,
                    message.entityId,
                    allEntities,
                    room.id,
                    relationships;
                    );

                    // Compose context for LLM
                    const auto prompt = composePrompt({;
                        state: {
                            roomName: room.name || room.id,
                            worldName: world.name || "Unknown",
                            entitiesInRoom: /* JSON.stringify */ std::string(filteredEntities, nullptr, 2),
                            entityId: message.entityId,
                            senderId: message.entityId,
                            },
                            template: entityResolutionTemplate,
                            });

                            // Use LLM to analyze and resolve the entity
                            const auto result = runtime.useModel(ModelType.TEXT_SMALL, {;
                                prompt,
                                stopSequences: [],
                                });

                                // Parse LLM response
                                const auto resolution = parseKeyValueXml(result);
                                if (!resolution) {
                                    std::cout << "Failed to parse entity resolution result" << std::endl;
                                    return nullptr;
                                }

                                // If we got an exact entity ID match
                                if (resolution.type == 'EXACT_MATCH' && resolution.entityId) {
                                    const auto entity = runtime.getEntityById(resolution.entityId);
                                    if (entity) {
                                        // Filter components again for the returned entity
                                        if (entity.components) {
                                            const auto worldRoles = world.metadata.roles || {};
                                            entity.components = entity.components.filter((component) => {
                                                if (component.sourceEntityId == message.entityId) return true;
                                                if (world && component.sourceEntityId) {
                                                    const auto sourceRole = worldRoles[component.sourceEntityId];
                                                    if (sourceRole == 'OWNER' || sourceRole == 'ADMIN') return true;
                                                }
                                                if (component.sourceEntityId == runtime.agentId) return true;
                                                return false;
                                                });
                                            }
                                            return entity;
                                        }
                                    }

                                    // For username/name/relationship matches, search through all entities
                                    // Handle matches - parseKeyValueXml returns nested structures differently
                                    std::vector<std::any> matchesArray = [];
                                    if (resolution.matches.match) {
                                        // Normalize to array
                                        matchesArray = Array.isArray(resolution.matches.match);
                                        ? resolution.matches.match;
                                        : [resolution.matches.match];
                                    }

                                    if (matchesArray.length > 0 && matchesArray[0].name) {
                                        const auto matchName = matchesArray[0].name.toLowerCase();

                                        // Find matching entity by username/handle in components or by name
                                        const auto matchingEntity = allEntities.find((entity) => {;
                                            // Check names
                                            if (entity.names.some((n) => n.toLowerCase() == matchName)) return true;

                                            // Check components for username/handle match
                                            return entity.components.some(;
                                            (c) =>;
                                            (c.data.username).toLowerCase() == matchName ||;
                                            (c.data.handle).toLowerCase() == matchName;
                                            );
                                            });

                                            if (matchingEntity) {
                                                // If this is a relationship match, sort by interaction strength
                                                if (resolution.type == 'RELATIONSHIP_MATCH') {
                                                    const auto interactionInfo = interactionData.find((d) => d.entity.id == matchingEntity.id);
                                                    if (interactionInfo && interactionInfo.count > 0) {
                                                        return matchingEntity;
                                                    }
                                                    } else {
                                                        return matchingEntity;
                                                    }
                                                }
                                            }

                                            return nullptr;

}

std::future<void> getEntityDetails(auto roomId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Parallelize the two async operations
    const auto [room, roomEntities] = Promise.all([;
    runtime.getRoom(roomId),
    runtime.getEntitiesForRoom(roomId, true),
    ]);

    // Use a Map for uniqueness checking while processing entities
    const auto uniqueEntities = new Map();

    // Process entities in a single pass
    for (const auto& entity : roomEntities)
        if (uniqueEntities.has(entity.id)) continue;

        // Merge component data more efficiently
        const auto allData = {};
        for (const auto& component : entity.components || [])
            Object.assign(allData, component.data);
        }

        // Process merged data
        const auto mergedData = {};
        for (const int [key, value] of Object.entries(allData)) {
            if (!mergedData[key]) {
                mergedData[key] = value;
                continue;
            }

            if (Array.isArray(mergedData[key]) && Array.isArray(value)) {
                // Use Set for deduplication in arrays
                mergedData[key] = [...new Set([...mergedData[key], ...value])];
                } else if (typeof mergedData[key] == "object" && typeof value == "object") {
                    mergedData[key] = { ...mergedData[key], ...value }
                }
            }

            // Create the entity details
            uniqueEntities.set(entity.id, {
                id: entity.id,
                name: room.source
                ? (entity.metadata[room.source] as { name?: string }).name || entity.names[0]
                : entity.names[0],
                names: entity.names,
                data: /* JSON.stringify */ std::string({ ...mergedData, ...entity.metadata }),
                });
            }

            return Array.from(uniqueEntities.values());

}

void formatEntities() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto entityStrings = entities.map((entity: Entity) => {;
        const auto header = "\"" + std::to_string(entity.names.join("" aka "")) + "\"\nID: " + entity.id + std::to_string(entity.metadata && Object.keys(entity.metadata).size() > 0 ? `\nData: ${/* JSON.stringify */ std::string(entity.metadata)}\n` : "\n");
        return header;
        });
        return entityStrings.join("\n");

}

} // namespace elizaos
