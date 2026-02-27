#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V1_SRC_RELATIONSHIP_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V1_SRC_RELATIONSHIP_H
#include "core.hpp"
#include "./types.h"
using RelationshipFromTypes = Relationship;
// External dependency removed
using RelationshipV2 = Relationship;

typedef RelationshipFromTypes Relationship;


Relationship fromV2Relationship(std::shared_ptr<RelationshipV2> relationshipV2);

std::shared_ptr<RelationshipV2> toV2Relationship(Relationship relationship, std::shared_ptr<UUID> agentId);

array<Relationship> fromV2Relationships(array<std::shared_ptr<RelationshipV2>> relationshipsV2);

array<std::shared_ptr<RelationshipV2>> toV2Relationships(array<Relationship> relationships, std::shared_ptr<UUID> agentId);

extern object RELATIONSHIP_STATUSES;
string tagsToStatus(array<string> tags);

array<string> statusToTags(string status);

Relationship fromV2RelationshipEnhanced(std::shared_ptr<RelationshipV2> relationshipV2);

std::shared_ptr<RelationshipV2> toV2RelationshipEnhanced(Relationship relationship, std::shared_ptr<UUID> agentId);

Relationship createV1Relationship(std::shared_ptr<UUID> userA, std::shared_ptr<UUID> userB, string status = RELATIONSHIP_STATUSES["UNKNOWN"], std::shared_ptr<UUID> roomId = std::nullopt);

boolean areRelationshipsEquivalent(Relationship rel1, Relationship rel2);

array<Relationship> filterRelationshipsByStatus(array<Relationship> relationships, string status);

array<Relationship> getRelationshipsForUser(array<Relationship> relationships, std::shared_ptr<UUID> userId);

#endif
