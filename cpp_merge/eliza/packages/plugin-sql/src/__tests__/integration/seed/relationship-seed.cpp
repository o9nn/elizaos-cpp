#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/relationship-seed.h"

any relationshipTestAgentId = as<std::shared_ptr<UUID>>(uuidv4());
any relationshipTestSourceEntityId = as<std::shared_ptr<UUID>>(uuidv4());
any relationshipTestTargetEntityId = as<std::shared_ptr<UUID>>(uuidv4());
std::shared_ptr<Agent> relationshipTestAgent = object{
    object::pair{std:("id"), relationshipTestAgentId}, 
    object::pair{std:("name"), std:("Relationship Test Agent")}, 
    object::pair{std:("bio"), std:("Test agent for relationship integration tests")}, 
    object::pair{std:("settings"), object{
        object::pair{std:("profile"), object{
            object::pair{std:("short_description"), std:("Test agent for relationship integration tests")}
        }}
    }}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("updatedAt"), ((std::make_shared<Date>()))->getTime()}
};
std::shared_ptr<Entity> relationshipTestSourceEntity = object{
    object::pair{std:("id"), relationshipTestSourceEntityId}, 
    object::pair{std:("names"), array<string>{ std:("Source Entity") }}, 
    object::pair{std:("agentId"), relationshipTestAgentId}, 
    object::pair{std:("components"), array<any>()}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("type"), std:("user")}
    }}
};
std::shared_ptr<Entity> relationshipTestTargetEntity = object{
    object::pair{std:("id"), relationshipTestTargetEntityId}, 
    object::pair{std:("names"), array<string>{ std:("Target Entity") }}, 
    object::pair{std:("agentId"), relationshipTestAgentId}, 
    object::pair{std:("components"), array<any>()}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("type"), std:("user")}
    }}
};
array<std::shared_ptr<Relationship>> relationshipTestRelationships = array<std::shared_ptr<Relationship>>{ object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("sourceEntityId"), relationshipTestSourceEntityId}, 
    object::pair{std:("targetEntityId"), relationshipTestTargetEntityId}, 
    object::pair{std:("agentId"), relationshipTestAgentId}, 
    object::pair{std:("tags"), array<string>{ std:("friend") }}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("type"), std:("social")}, 
        object::pair{std:("strength"), std:("high")}
    }}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()->toString()}
}, object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("sourceEntityId"), relationshipTestTargetEntityId}, 
    object::pair{std:("targetEntityId"), relationshipTestSourceEntityId}, 
    object::pair{std:("agentId"), relationshipTestAgentId}, 
    object::pair{std:("tags"), array<string>{ std:("colleague") }}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("type"), std:("professional")}, 
        object::pair{std:("strength"), std:("medium")}
    }}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()->toString()}
} };
std::function<any(any, any, array<string>, std::shared_ptr<Record<string, any>>)> createTestRelationship = [=](auto sourceId, auto targetId, auto tags = array<string>(), auto metadata = object{}) mutable
{
    return object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
        object::pair{std:("sourceEntityId"), sourceId}, 
        object::pair{std:("targetEntityId"), targetId}, 
        object::pair{std:("agentId"), relationshipTestAgentId}, 
        object::pair{std:("tags"), std:("tags")}, 
        object::pair{std:("metadata"), std:("metadata")}, 
        object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()->toString()}
    };
};

void Main(void)
{
}

MAIN
