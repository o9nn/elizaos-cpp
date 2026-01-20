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
        std::cout << { message } << "Missing agentId or roomId in message" << std::endl;
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
                        const auto response = runtime.useModel(ModelType.TEXT_SMALL, {;
                            prompt,
                            });

                            if (!response) {
                                std::cout << { prompt } << "Getting reflection failed - empty response" << std::endl;
                                return;
                            }

                            // Parse XML response
                            const auto reflection = parseKeyValueXml(response);

                            if (!reflection) {
                                std::cout << { response } << "Getting reflection failed - failed to parse XML" << std::endl;
                                return;
                            }

                            // Perform basic structure validation
                            if (!reflection.facts) {
                                std::cout << { reflection } << "Getting reflection failed - invalid facts structure" << std::endl;
                                return;
                            }

                            if (!reflection.relationships) {
                                std::cout << { reflection } << "Getting reflection failed - invalid relationships structure" << std::endl;
                                return;
                            }

                            // Handle facts - parseKeyValueXml returns nested structures differently
                            // Facts might be a single object or an array depending on the count
                            std::vector<std::any> factsArray = [];
                            if (reflection.facts.fact) {
                                // Normalize to array
                                factsArray = Array.isArray(reflection.facts.fact);
                                ? reflection.facts.fact;
                                : [reflection.facts.fact];
                            }

                            // Store new facts
                            const auto newFacts =;
                            factsArray.filter(;
                            (fact: any) =>
                            fact &&;
                            typeof fact == "object" &&;
                            fact.already_known == "false" &&;
                            fact.in_bio == "false" &&;
                            fact.claim &&;
                            typeof fact.claim == "string" &&;
                            fact.claim.trim() != "";
                            ) || [];

                            Promise.all(;
                            newFacts.map(async (fact: any) => {
                                const auto factMemory = {;
                                    id: asUUID(v4()),
                                    entityId: agentId,
                                    agentId,
                                    content: { text: fact.claim },
                                    roomId,
                                    createdAt: Date.now(),
                                    };
                                    // Create memory first and capture the returned ID
                                    const auto createdMemoryId = runtime.createMemory(factMemory, "facts", true);
                                    // Update the memory object with the actual ID from the database
                                    const auto createdMemory = { ...factMemory, id = createdMemoryId };
                                    // Queue embedding generation asynchronously for the memory with correct ID
                                    runtime.queueEmbeddingGeneration(createdMemory, "low");
                                    return createdMemory;
                                    });
                                    );

                                    // Handle relationships - similar structure normalization
                                    std::vector<std::any> relationshipsArray = [];
                                    if (reflection.relationships.relationship) {
                                        relationshipsArray = Array.isArray(reflection.relationships.relationship);
                                        ? reflection.relationships.relationship;
                                        : [reflection.relationships.relationship];
                                    }

                                    // Update or create relationships
                                    for (const auto& relationship : relationshipsArray)
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

                                                // Parse tags from comma-separated string
                                                const auto tags = relationship.tags;
                                                ? relationship.tags;
                                                .split(",");
                                                .map((tag: string) => tag.trim())
                                                .filter(Boolean);
                                                : [];

                                                if (existingRelationship) {
                                                    const auto updatedMetadata = {;
                                                        ...existingRelationship.metadata,
                                                        interactions:
                                                        ((existingRelationship.metadata.interactions | std::nullopt) || 0) + 1,
                                                        };

                                                        const auto updatedTags = Array.from(new Set([...(existingRelationship.tags || []), ...tags]));

                                                        runtime.updateRelationship({
                                                            ...existingRelationship,
                                                            tags: updatedTags,
                                                            metadata: updatedMetadata,
                                                            });
                                                            } else {
                                                                runtime.createRelationship({
                                                                    sourceEntityId: sourceId,
                                                                    targetEntityId: targetId,
                                                                    tags: tags,
                                                                    metadata: {
                                                                        interactions: 1,
                                                                        ...(relationship.metadata || {}),
                                                                        },
                                                                        });
                                                                    }
                                                                }

                                                                runtime.setCache<string>(;
                                                                message.roomId + "-reflection-last-processed"
                                                                message.id || "";
                                                                );
                                                                } catch (error) {
                                                                    std::cerr << { error } << "Error in reflection handler:" << std::endl;
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
