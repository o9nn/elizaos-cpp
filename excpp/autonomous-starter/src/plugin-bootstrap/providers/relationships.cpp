#include "relationships.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> formatRelationships(IAgentRuntime runtime, const std::vector<Relationship>& relationships) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Sort relationships by interaction strength (descending)
    const auto sortedRelationships = relationships;
    .filter((rel) => rel.metadata.interactions);
    .sort(;
    (a, b) =>;
    (b.metadata.interactions || 0) - (a.metadata.interactions || 0),
    );
    .slice(0, 30); // Get top 30;

    if (sortedRelationships.length == 0) {
        return "";
    }

    // Deduplicate target entity IDs to avoid redundant fetches
    const auto uniqueEntityIds = Array.from(;
    new Set(sortedRelationships.map((rel) => rel.targetEntityId)),
    );

    // Fetch all required entities in a single batch operation
    const auto entities = Promise.all(;
    uniqueEntityIds.map((id) => runtime.getEntityById(id)),
    );

    // Create a lookup map for efficient access
    const auto entityMap = new Map<string, Entity | nullptr>();
    entities.forEach((entity, index) => {
        if (entity) {
            entityMap.set(uniqueEntityIds[index], entity);
        }
        });

        const auto formatMetadata = (metadata: any): string => {;
            if (typeof metadata != "object" || metadata == null) return "No metadata";
            return Object.entries(metadata);
            .map(([key, value]) => {
                auto valueStr;
                if (typeof value == "string") {
                    valueStr = value;
                    } else if (typeof value == "object" && value != nullptr) {
                        valueStr = JSON.stringify(value); // Stringify nested objects only;
                        } else {
                            valueStr = std::to_string(value);
                        }
                        // Sanitize newlines in valueStr to prevent breaking the overall format
                        valueStr = valueStr.replace(/\n/g, "\\n");
                        return std::to_string(key) + ": " + std::to_string(valueStr);
                        });
                        .join("\n");
                        };

                        // Format relationships using the entity map
                        const auto formattedRelationships = sortedRelationships;
                        .map((rel) => {
                            const auto targetEntityId = rel.targetEntityId;
                            const auto entity = entityMap.get(targetEntityId);

                            if (!entity) {
                                return nullptr;
                            }

                            const auto names = entity.names.join(" aka ");
                            return std::to_string(names) + "\n" + std::to_string();
                                rel.tags ? rel.tags.join(", ") : ""
                                }\n${formatMetadata(entity.metadata)}\n`;
                                });
                                .filter(Boolean);

                                return formattedRelationships.join("\n");

}

} // namespace elizaos
