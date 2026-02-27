#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v1/src/relationship.h"

Relationship fromV2Relationship(std::shared_ptr<RelationshipV2> relationshipV2)
{
    return object{
        object::pair{std:("id"), relationshipV2->id}, 
        object::pair{std:("userA"), relationshipV2->sourceEntityId}, 
        object::pair{std:("userB"), relationshipV2->targetEntityId}, 
        object::pair{std:("userId"), relationshipV2->sourceEntityId}, 
        object::pair{std:("roomId"), relationshipV2->id}, 
        object::pair{std:("status"), relationshipV2->tags->join(std:(","))}, 
        object::pair{std:("createdAt"), relationshipV2->createdAt}
    };
};


std::shared_ptr<RelationshipV2> toV2Relationship(Relationship relationship, std::shared_ptr<UUID> agentId)
{
    return object{
        object::pair{std:("id"), relationship->id}, 
        object::pair{std:("sourceEntityId"), relationship->userA}, 
        object::pair{std:("targetEntityId"), relationship->userB}, 
        object::pair{std:("agentId"), agentId}, 
        object::pair{std:("tags"), (relationship->status) ? relationship->status->split(std:(","))->map([=](auto s) mutable
        {
            return s->trim();
        }
        ) : array<any>()}, 
        object::pair{std:("metadata"), object{
            object::pair{std:("userId"), relationship->userId}, 
            object::pair{std:("roomId"), relationship->roomId}
        }}, 
        object::pair{std:("createdAt"), relationship->createdAt}
    };
};


array<Relationship> fromV2Relationships(array<std::shared_ptr<RelationshipV2>> relationshipsV2)
{
    return relationshipsV2->map([=](auto rel) mutable
    {
        return fromV2Relationship(rel);
    }
    );
};


array<std::shared_ptr<RelationshipV2>> toV2Relationships(array<Relationship> relationships, std::shared_ptr<UUID> agentId)
{
    return relationships->map([=](auto rel) mutable
    {
        return toV2Relationship(rel, agentId);
    }
    );
};


string tagsToStatus(array<string> tags)
{
    if (tags->get_length() == 0) return RELATIONSHIP_STATUSES["UNKNOWN"];
    auto statusMap = object{
        object::pair{std:("friend"), RELATIONSHIP_STATUSES["FRIEND"]}, 
        object::pair{std:("blocked"), RELATIONSHIP_STATUSES["BLOCKED"]}, 
        object::pair{std:("muted"), RELATIONSHIP_STATUSES["MUTED"]}, 
        object::pair{std:("following"), RELATIONSHIP_STATUSES["FOLLOWING"]}, 
        object::pair{std:("follower"), RELATIONSHIP_STATUSES["FOLLOWED_BY"]}, 
        object::pair{std:("acquaintance"), RELATIONSHIP_STATUSES["ACQUAINTANCE"]}
    };
    for (auto& tag : tags)
    {
        auto normalizedTag = tag->toLowerCase();
        if (const_(statusMap)[normalizedTag]) {
            return const_(statusMap)[normalizedTag];
        }
    }
    return tags->join(std:(","));
};


array<string> statusToTags(string status)
{
    if (!status) return array<any>();
    auto tagMap = object{
        object::pair{RELATIONSHIP_STATUSES["FRIEND"], array<string>{ std:("friend") }}, 
        object::pair{RELATIONSHIP_STATUSES["BLOCKED"], array<string>{ std:("blocked") }}, 
        object::pair{RELATIONSHIP_STATUSES["MUTED"], array<string>{ std:("muted") }}, 
        object::pair{RELATIONSHIP_STATUSES["FOLLOWING"], array<string>{ std:("following") }}, 
        object::pair{RELATIONSHIP_STATUSES["FOLLOWED_BY"], array<string>{ std:("follower") }}, 
        object::pair{RELATIONSHIP_STATUSES["ACQUAINTANCE"], array<string>{ std:("acquaintance") }}, 
        object::pair{RELATIONSHIP_STATUSES["UNKNOWN"], array<any>()}
    };
    auto normalizedStatus = status->toLowerCase();
    if (const_(tagMap)[normalizedStatus]) {
        return const_(tagMap)[normalizedStatus];
    }
    return status->split(std:(","))->map([=](auto s) mutable
    {
        return s->trim();
    }
    )->filter(Boolean);
};


Relationship fromV2RelationshipEnhanced(std::shared_ptr<RelationshipV2> relationshipV2)
{
    return object{
        object::pair{std:("id"), relationshipV2->id}, 
        object::pair{std:("userA"), relationshipV2->sourceEntityId}, 
        object::pair{std:("userB"), relationshipV2->targetEntityId}, 
        object::pair{std:("userId"), relationshipV2->sourceEntityId}, 
        object::pair{std:("roomId"), OR((as<std::shared_ptr<UUID>>(relationshipV2->metadata->roomId)), (relationshipV2->id))}, 
        object::pair{std:("status"), tagsToStatus(relationshipV2->tags)}, 
        object::pair{std:("createdAt"), relationshipV2->createdAt}
    };
};


std::shared_ptr<RelationshipV2> toV2RelationshipEnhanced(Relationship relationship, std::shared_ptr<UUID> agentId)
{
    return object{
        object::pair{std:("id"), relationship->id}, 
        object::pair{std:("sourceEntityId"), relationship->userA}, 
        object::pair{std:("targetEntityId"), relationship->userB}, 
        object::pair{std:("agentId"), agentId}, 
        object::pair{std:("tags"), statusToTags(relationship->status)}, 
        object::pair{std:("metadata"), object{
            object::pair{std:("userId"), relationship->userId}, 
            object::pair{std:("roomId"), relationship->roomId}, 
            object::pair{std:("originalStatus"), relationship->status}
        }}, 
        object::pair{std:("createdAt"), relationship->createdAt}
    };
};


Relationship createV1Relationship(std::shared_ptr<UUID> userA, std::shared_ptr<UUID> userB, string status, std::shared_ptr<UUID> roomId)
{
    return object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(string_empty + userA + std:("-") + userB + std:("-") + Date->now() + string_empty)}, 
        object::pair{std:("userA"), std:("userA")}, 
        object::pair{std:("userB"), std:("userB")}, 
        object::pair{std:("userId"), userA}, 
        object::pair{std:("roomId"), OR((roomId), (userA))}, 
        object::pair{std:("status"), std:("status")}, 
        object::pair{std:("createdAt"), ((std::make_shared<Date>()))->toISOString()}
    };
};


boolean areRelationshipsEquivalent(Relationship rel1, Relationship rel2)
{
    return (OR(((AND((rel1->userA == rel2->userA), (rel1->userB == rel2->userB)))), ((AND((rel1->userA == rel2->userB), (rel1->userB == rel2->userA))))));
};


array<Relationship> filterRelationshipsByStatus(array<Relationship> relationships, string status)
{
    return relationships->filter([=](auto rel) mutable
    {
        return rel->status == status;
    }
    );
};


array<Relationship> getRelationshipsForUser(array<Relationship> relationships, std::shared_ptr<UUID> userId)
{
    return relationships->filter([=](auto rel) mutable
    {
        return OR((rel->userA == userId), (rel->userB == userId));
    }
    );
};


object RELATIONSHIP_STATUSES = as<std::shared_ptr<const>>(object{
    object::pair{std:("FRIEND"), std:("friend")}, 
    object::pair{std:("BLOCKED"), std:("blocked")}, 
    object::pair{std:("MUTED"), std:("muted")}, 
    object::pair{std:("FOLLOWING"), std:("following")}, 
    object::pair{std:("FOLLOWED_BY"), std:("followed_by")}, 
    object::pair{std:("ACQUAINTANCE"), std:("acquaintance")}, 
    object::pair{std:("UNKNOWN"), std:("unknown")}
});

void Main(void)
{
}

MAIN
