#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v1/src/relationship.h"

Relationship fromV2Relationship(std::shared_ptr<RelationshipV2> relationshipV2)
{
    return object{
        object::pair{std::string("id"), relationshipV2->id}, 
        object::pair{std::string("userA"), relationshipV2->sourceEntityId}, 
        object::pair{std::string("userB"), relationshipV2->targetEntityId}, 
        object::pair{std::string("userId"), relationshipV2->sourceEntityId}, 
        object::pair{std::string("roomId"), relationshipV2->id}, 
        object::pair{std::string("status"), relationshipV2->tags->join(std::string(","))}, 
        object::pair{std::string("createdAt"), relationshipV2->createdAt}
    };
};


std::shared_ptr<RelationshipV2> toV2Relationship(Relationship relationship, std::shared_ptr<UUID> agentId)
{
    return object{
        object::pair{std::string("id"), relationship->id}, 
        object::pair{std::string("sourceEntityId"), relationship->userA}, 
        object::pair{std::string("targetEntityId"), relationship->userB}, 
        object::pair{std::string("agentId"), agentId}, 
        object::pair{std::string("tags"), (relationship->status) ? relationship->status->split(std::string(","))->map([=](auto s) mutable
        {
            return s->trim();
        }
        ) : array<any>()}, 
        object::pair{std::string("metadata"), object{
            object::pair{std::string("userId"), relationship->userId}, 
            object::pair{std::string("roomId"), relationship->roomId}
        }}, 
        object::pair{std::string("createdAt"), relationship->createdAt}
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
        object::pair{std::string("friend"), RELATIONSHIP_STATUSES["FRIEND"]}, 
        object::pair{std::string("blocked"), RELATIONSHIP_STATUSES["BLOCKED"]}, 
        object::pair{std::string("muted"), RELATIONSHIP_STATUSES["MUTED"]}, 
        object::pair{std::string("following"), RELATIONSHIP_STATUSES["FOLLOWING"]}, 
        object::pair{std::string("follower"), RELATIONSHIP_STATUSES["FOLLOWED_BY"]}, 
        object::pair{std::string("acquaintance"), RELATIONSHIP_STATUSES["ACQUAINTANCE"]}
    };
    for (auto& tag : tags)
    {
        auto normalizedTag = tag->toLowerCase();
        if (const_(statusMap)[normalizedTag]) {
            return const_(statusMap)[normalizedTag];
        }
    }
    return tags->join(std::string(","));
};


array<string> statusToTags(string status)
{
    if (!status) return array<any>();
    auto tagMap = object{
        object::pair{RELATIONSHIP_STATUSES["FRIEND"], array<string>{ std::string("friend") }}, 
        object::pair{RELATIONSHIP_STATUSES["BLOCKED"], array<string>{ std::string("blocked") }}, 
        object::pair{RELATIONSHIP_STATUSES["MUTED"], array<string>{ std::string("muted") }}, 
        object::pair{RELATIONSHIP_STATUSES["FOLLOWING"], array<string>{ std::string("following") }}, 
        object::pair{RELATIONSHIP_STATUSES["FOLLOWED_BY"], array<string>{ std::string("follower") }}, 
        object::pair{RELATIONSHIP_STATUSES["ACQUAINTANCE"], array<string>{ std::string("acquaintance") }}, 
        object::pair{RELATIONSHIP_STATUSES["UNKNOWN"], array<any>()}
    };
    auto normalizedStatus = status->toLowerCase();
    if (const_(tagMap)[normalizedStatus]) {
        return const_(tagMap)[normalizedStatus];
    }
    return status->split(std::string(","))->map([=](auto s) mutable
    {
        return s->trim();
    }
    )->filter(Boolean);
};


Relationship fromV2RelationshipEnhanced(std::shared_ptr<RelationshipV2> relationshipV2)
{
    return object{
        object::pair{std::string("id"), relationshipV2->id}, 
        object::pair{std::string("userA"), relationshipV2->sourceEntityId}, 
        object::pair{std::string("userB"), relationshipV2->targetEntityId}, 
        object::pair{std::string("userId"), relationshipV2->sourceEntityId}, 
        object::pair{std::string("roomId"), OR((as<std::shared_ptr<UUID>>(relationshipV2->metadata->roomId)), (relationshipV2->id))}, 
        object::pair{std::string("status"), tagsToStatus(relationshipV2->tags)}, 
        object::pair{std::string("createdAt"), relationshipV2->createdAt}
    };
};


std::shared_ptr<RelationshipV2> toV2RelationshipEnhanced(Relationship relationship, std::shared_ptr<UUID> agentId)
{
    return object{
        object::pair{std::string("id"), relationship->id}, 
        object::pair{std::string("sourceEntityId"), relationship->userA}, 
        object::pair{std::string("targetEntityId"), relationship->userB}, 
        object::pair{std::string("agentId"), agentId}, 
        object::pair{std::string("tags"), statusToTags(relationship->status)}, 
        object::pair{std::string("metadata"), object{
            object::pair{std::string("userId"), relationship->userId}, 
            object::pair{std::string("roomId"), relationship->roomId}, 
            object::pair{std::string("originalStatus"), relationship->status}
        }}, 
        object::pair{std::string("createdAt"), relationship->createdAt}
    };
};


Relationship createV1Relationship(std::shared_ptr<UUID> userA, std::shared_ptr<UUID> userB, string status, std::shared_ptr<UUID> roomId)
{
    return object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(string_empty + userA + std::string("-") + userB + std::string("-") + Date->now() + string_empty)}, 
        object::pair{std::string("userA"), std::string("userA")}, 
        object::pair{std::string("userB"), std::string("userB")}, 
        object::pair{std::string("userId"), userA}, 
        object::pair{std::string("roomId"), OR((roomId), (userA))}, 
        object::pair{std::string("status"), std::string("status")}, 
        object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->toISOString()}
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
    object::pair{std::string("FRIEND"), std::string("friend")}, 
    object::pair{std::string("BLOCKED"), std::string("blocked")}, 
    object::pair{std::string("MUTED"), std::string("muted")}, 
    object::pair{std::string("FOLLOWING"), std::string("following")}, 
    object::pair{std::string("FOLLOWED_BY"), std::string("followed_by")}, 
    object::pair{std::string("ACQUAINTANCE"), std::string("acquaintance")}, 
    object::pair{std::string("UNKNOWN"), std::string("unknown")}
});

void Main(void)
{
}

MAIN
