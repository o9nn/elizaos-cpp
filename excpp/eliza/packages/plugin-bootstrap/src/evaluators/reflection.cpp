#include "reflection.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

UUID resolveEntity(UUID entityId, const std::vector<Entity>& entities) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // First try exact UUID match
        if (/^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$/i.test(entityId)) {
            return entityId;
        }

        auto entity: Entity | std::nullopt;

        // Try to match the entityId exactly
        entity = entities.find((a) => a.id == entityId);
        if (entity.id) {
            return entity.id;
        }

        // Try partial UUID match with entityId
        entity = entities.find((a) => a.(std::find(id.begin(), id.end(), entityId) != id.end()));
        if (entity.id) {
            return entity.id;
        }

        // Try name match as last resort
        entity = entities.find((a) =>;
        a.names.some((n) => n.toLowerCase().includes(entityId.toLowerCase()));
        );
        if (entity.id) {
            return entity.id;
        }

        throw std::runtime_error(`Could not resolve entityId "${entityId}" to a valid UUID`);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> handler(IAgentRuntime runtime, Memory message, std::optional<State> state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { agentId, roomId } = message;

    if (!agentId || !roomId) {
        std::cout << "Missing agentId or roomId in message" << message << std::endl;
        return;
    }

    // Run all queries in parallel
    const auto [existingRelationships, entities, knownFacts] = Promise.all([;
    runtime.getRelationships({
        entityId: message.entityId,
        }),
        getEntityDetails({ runtime, roomId }),
        runtime.getMemories({
            tableName: "facts",
            roomId,
            count: 30,
            unique: true,
            }),
            ]);

            const auto prompt = composePrompt({;
                state: {
                    ...(state.values || {}),
                    knownFacts: formatFacts(knownFacts),
                    roomType: message.content.channelType,
                    entitiesInRoom: /* JSON.stringify */ std::string(entities),
                    existingRelationships: /* JSON.stringify */ std::string(existingRelationships),
                    senderId: message.entityId,
                    },
                    template: runtime.character.templates.reflectionTemplate || reflectionTemplate,
                    });

                    // Use the model without schema validation
                    try {
                        const auto reflection = runtime.useModel(ModelType.OBJECT_SMALL, {;
                            prompt,
                            // Remove schema validation to avoid zod issues
                            });

                            if (!reflection) {
                                std::cout << "Getting reflection failed - empty response" << prompt << std::endl;
                                return;
                            }

                            // Perform basic structure validation instead of using zod
                            if (!reflection.facts || !Array.isArray(reflection.facts)) {
                                std::cout << "Getting reflection failed - invalid facts structure" << reflection << std::endl;
                                return;
                            }

                            if (!reflection.relationships || !Array.isArray(reflection.relationships)) {
                                std::cout << "Getting reflection failed - invalid relationships structure" << reflection << std::endl;
                                return;
                            }

                            // Store new facts
                            const auto newFacts =;
                            reflection.facts.filter(;
                            (fact) =>;
                            fact &&;
                            typeof fact == "object" &&;
                            !fact.already_known &&;
                            !fact.in_bio &&;
                            fact.claim &&;
                            typeof fact.claim == "string" &&;
                            fact.claim.trim() != "";
                            ) || [];

                            Promise.all(;
                            newFacts.map(async (fact) => {
                                const auto factMemory = runtime.addEmbeddingToMemory({;
                                    entityId: agentId,
                                    agentId,
                                    content: { text: fact.claim },
                                    roomId,
                                    createdAt: Date.now(),
                                    });
                                    return runtime.createMemory(factMemory, "facts", true);
                                    });
                                    );

                                    // Update or create relationships
                                    for (const auto& relationship : reflection.relationships)
                                        auto sourceId: UUID;
                                        auto targetId: UUID;

                                        try {
                                            sourceId = resolveEntity(relationship.sourceEntityId, entities);
                                            targetId = resolveEntity(relationship.targetEntityId, entities);
                                            } catch (error) {
                                                std::cout << "Failed to resolve relationship entities:" << error << std::endl;
                                                std::cout << "relationship:\n" << relationship << std::endl;
                                                continue; // Skip this relationship if we can't resolve the IDs;
                                            }

                                            const auto existingRelationship = existingRelationships.find((r) => {;
                                                return r.sourceEntityId == sourceId && r.targetEntityId == targetId;
                                                });

                                                if (existingRelationship) {
                                                    const auto updatedMetadata = {;
                                                        ...existingRelationship.metadata,
                                                        interactions:
                                                        ((existingRelationship.metadata.interactions | std::nullopt) || 0) + 1,
                                                        };

                                                        const auto updatedTags = Array.from(;
                                                        new Set([...(existingRelationship.tags || []), ...relationship.tags]);
                                                        );

                                                        runtime.updateRelationship({
                                                            ...existingRelationship,
                                                            tags: updatedTags,
                                                            metadata: updatedMetadata,
                                                            });
                                                            } else {
                                                                runtime.createRelationship({
                                                                    sourceEntityId: sourceId,
                                                                    targetEntityId: targetId,
                                                                    tags: relationship.tags,
                                                                    metadata: {
                                                                        interactions: 1,
                                                                        ...relationship.metadata,
                                                                        },
                                                                        });
                                                                    }
                                                                }

                                                                runtime.setCache<string>(;
                                                                message.roomId + "-reflection-last-processed"
                                                                message.id || "";
                                                                );

                                                                return reflection;
                                                                } catch (error) {
                                                                    std::cerr << "Error in reflection handler:" << error << std::endl;
                                                                    return;
                                                                }

}

void formatFacts(const std::vector<Memory>& facts) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return facts;
    .reverse();
    .map((fact: Memory) => fact.content.text)
    .join("\n");

}

} // namespace elizaos
