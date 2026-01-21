#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core-plugin-v2.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Relationship interface for v1 compatibility
 */
using Relationship = RelationshipFromTypes;

/**
 * Converts V2 Relationship to V1 compatible Relationship
 * Main differences:
 * - V2 uses sourceEntityId/targetEntityId, V1 uses userA/userB
 * - V2 has tags array and metadata object, V1 has status string
 * - V1 has userId and roomId fields that don't exist in V2
 */
Relationship fromV2Relationship(RelationshipV2 relationshipV2);

/**
 * Converts V1 Relationship to V2 compatible Relationship
 * Maps V1 relationship structure to V2 format
 */
RelationshipV2 toV2Relationship(Relationship relationship, UUID agentId);

/**
 * Converts an array of V2 Relationships to V1 format
 */
std::vector<Relationship> fromV2Relationships(const std::vector<RelationshipV2>& relationshipsV2);

/**
 * Converts an array of V1 Relationships to V2 format
 */
std::vector<RelationshipV2> toV2Relationships(const std::vector<Relationship>& relationships, UUID agentId);

/**
 * Relationship status constants commonly used in V1
 * These can be used to standardize relationship statuses
 */

/**
 * Converts V2 tags to V1 status string with common mappings
 * Provides more intelligent conversion than simple join
 */
std::string tagsToStatus(const std::vector<std::string>& tags);

/**
 * Converts V1 status string to V2 tags array with common mappings
 * Provides more intelligent conversion than simple split
 */
std::vector<std::string> statusToTags(const std::string& status);

/**
 * Enhanced conversion with intelligent status mapping
 */
Relationship fromV2RelationshipEnhanced(RelationshipV2 relationshipV2);

/**
 * Enhanced conversion with intelligent tag mapping
 */
RelationshipV2 toV2RelationshipEnhanced(Relationship relationship, UUID agentId);

/**
 * Utility functions for relationship management
 */

/**
 * Creates a new V1 relationship with default values
 */
Relationship createV1Relationship(UUID userA, UUID userB, std::string status = RELATIONSHIP_STATUSES.UNKNOWN, std::optional<UUID> roomId);

/**
 * Checks if two relationships represent the same connection (bidirectional)
 */
bool areRelationshipsEquivalent(Relationship rel1, Relationship rel2);

/**
 * Filters relationships by status
 */
std::vector<Relationship> filterRelationshipsByStatus(const std::vector<Relationship>& relationships, const std::string& status);

/**
 * Gets all relationships for a specific user (where user is either userA or userB)
 */
std::vector<Relationship> getRelationshipsForUser(const std::vector<Relationship>& relationships, UUID userId);
} // namespace elizaos
