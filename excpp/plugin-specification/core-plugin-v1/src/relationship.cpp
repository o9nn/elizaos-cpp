#include "relationship.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

Relationship fromV2Relationship(RelationshipV2 relationshipV2) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        id: relationshipV2.id,
        userA: relationshipV2.sourceEntityId,
        userB: relationshipV2.targetEntityId,
        userId: relationshipV2.sourceEntityId, // Use source user
        roomId: relationshipV2.id, // V1 expects roomId, use relationship ID
        status: relationshipV2.tags.join(','), // Convert tags array to comma-separated string
        createdAt: relationshipV2.createdAt,
        };

}

RelationshipV2 toV2Relationship(Relationship relationship, UUID agentId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        id: relationship.id,
        sourceEntityId: relationship.userA,
        targetEntityId: relationship.userB,
        agentId: agentId,
        tags: relationship.status ? relationship.status.split(',').map(s => s.trim()) : [],
        metadata: {
            // Preserve any V1-specific fields in metadata
            userId: relationship.userId,
            roomId: relationship.roomId,
            },
            createdAt: relationship.createdAt,
            };

}

std::vector<Relationship> fromV2Relationships(const std::vector<RelationshipV2>& relationshipsV2) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return relationshipsV2.map(rel => fromV2Relationship(rel));

}

std::vector<RelationshipV2> toV2Relationships(const std::vector<Relationship>& relationships, UUID agentId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return relationships.map(rel => toV2Relationship(rel, agentId));

}

std::string tagsToStatus(const std::vector<std::string>& tags) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (tags.length == 0) return RELATIONSHIP_STATUSES.UNKNOWN;

    // Map common V2 tags to V1 statuses
    const std::unordered_map<std::string, std::string> statusMap = {;
        'friend': RELATIONSHIP_STATUSES.FRIEND,
        'blocked': RELATIONSHIP_STATUSES.BLOCKED,
        'muted': RELATIONSHIP_STATUSES.MUTED,
        'following': RELATIONSHIP_STATUSES.FOLLOWING,
        'follower': RELATIONSHIP_STATUSES.FOLLOWED_BY,
        'acquaintance': RELATIONSHIP_STATUSES.ACQUAINTANCE,
        };

        // Check for known status mappings first
        for (const auto& tag : tags)
            const auto normalizedTag = tag.toLowerCase();
            if (statusMap[normalizedTag]) {
                return statusMap[normalizedTag];
            }
        }

        // If no known mapping, join all tags
        return tags.join(',');

}

std::vector<std::string> statusToTags(const std::string& status) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!status) return [];

    // Map common V1 statuses to V2 tags
    const std::unordered_map<std::string, std::vector<std::string>> tagMap = {;
        [RELATIONSHIP_STATUSES.FRIEND]: ['friend'],
        [RELATIONSHIP_STATUSES.BLOCKED]: ['blocked'],
        [RELATIONSHIP_STATUSES.MUTED]: ['muted'],
        [RELATIONSHIP_STATUSES.FOLLOWING]: ['following'],
        [RELATIONSHIP_STATUSES.FOLLOWED_BY]: ['follower'],
        [RELATIONSHIP_STATUSES.ACQUAINTANCE]: ['acquaintance'],
        [RELATIONSHIP_STATUSES.UNKNOWN]: [],
        };

        const auto normalizedStatus = status.toLowerCase();
        if (tagMap[normalizedStatus]) {
            return tagMap[normalizedStatus];
        }

        // If no known mapping, split by comma
        return status.split(',').map(s => s.trim()).filter(Boolean);

}

Relationship fromV2RelationshipEnhanced(RelationshipV2 relationshipV2) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        id: relationshipV2.id,
        userA: relationshipV2.sourceEntityId,
        userB: relationshipV2.targetEntityId,
        userId: relationshipV2.sourceEntityId,
        roomId: relationshipV2.metadata.roomId || relationshipV2.id,
        status: tagsToStatus(relationshipV2.tags),
        createdAt: relationshipV2.createdAt,
        };

}

RelationshipV2 toV2RelationshipEnhanced(Relationship relationship, UUID agentId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        id: relationship.id,
        sourceEntityId: relationship.userA,
        targetEntityId: relationship.userB,
        agentId: agentId,
        tags: statusToTags(relationship.status),
        metadata: {
            userId: relationship.userId,
            roomId: relationship.roomId,
            // Preserve original status for reference
            originalStatus: relationship.status,
            },
            createdAt: relationship.createdAt,
            };

}

Relationship createV1Relationship(UUID userA, UUID userB, std::string status = RELATIONSHIP_STATUSES.UNKNOWN, std::optional<UUID> roomId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        std::to_string(userA) + "-" + std::to_string(userB) + "-" + std::to_string(Date.now())
        userA,
        userB,
        userId: userA,
        roomId: roomId || userA, // Use userA roomId
        status,
        createdAt: new Date().toISOString(),
        };

}

bool areRelationshipsEquivalent(Relationship rel1, Relationship rel2) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    (rel1.userA == rel2.userA && rel1.userB == rel2.userB) ||;
    (rel1.userA == rel2.userB && rel1.userB == rel2.userA);
    );

}

std::vector<Relationship> filterRelationshipsByStatus(const std::vector<Relationship>& relationships, const std::string& status) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return relationships.filter(rel => rel.status == status);

}

std::vector<Relationship> getRelationshipsForUser(const std::vector<Relationship>& relationships, UUID userId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return relationships.filter(rel => rel.userA == userId || rel.userB == userId);

}

} // namespace elizaos
