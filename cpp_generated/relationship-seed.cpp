#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/relationship-seed.h"

any relationshipTestAgentId = as<std::shared_ptr<UUID>>(uuidv4());
any relationshipTestSourceEntityId = as<std::shared_ptr<UUID>>(uuidv4());
any relationshipTestTargetEntityId = as<std::shared_ptr<UUID>>(uuidv4());
std::shared_ptr<Agent> relationshipTestAgent = object{
    object::pair{std::string("id"), relationshipTestAgentId}, 
    object::pair{std::string("name"), std::string("Relationship Test Agent")}, 
    object::pair{std::string("bio"), std::string("Test agent for relationship integration tests")}, 
    object::pair{std::string("settings"), object{
        object::pair{std::string("profile"), object{
            object::pair{std::string("short_description"), std::string("Test agent for relationship integration tests")}
        }}
    }}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->getTime()}
};
std::shared_ptr<Entity> relationshipTestSourceEntity = object{
    object::pair{std::string("id"), relationshipTestSourceEntityId}, 
    object::pair{std::string("names"), array<string>{ std::string("Source Entity") }}, 
    object::pair{std::string("agentId"), relationshipTestAgentId}, 
    object::pair{std::string("components"), array<any>()}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("type"), std::string("user")}
    }}
};
std::shared_ptr<Entity> relationshipTestTargetEntity = object{
    object::pair{std::string("id"), relationshipTestTargetEntityId}, 
    object::pair{std::string("names"), array<string>{ std::string("Target Entity") }}, 
    object::pair{std::string("agentId"), relationshipTestAgentId}, 
    object::pair{std::string("components"), array<any>()}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("type"), std::string("user")}
    }}
};
array<std::shared_ptr<Relationship>> relationshipTestRelationships = array<std::shared_ptr<Relationship>>{ object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("sourceEntityId"), relationshipTestSourceEntityId}, 
    object::pair{std::string("targetEntityId"), relationshipTestTargetEntityId}, 
    object::pair{std::string("agentId"), relationshipTestAgentId}, 
    object::pair{std::string("tags"), array<string>{ std::string("friend") }}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("type"), std::string("social")}, 
        object::pair{std::string("strength"), std::string("high")}
    }}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()->toString()}
}, object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("sourceEntityId"), relationshipTestTargetEntityId}, 
    object::pair{std::string("targetEntityId"), relationshipTestSourceEntityId}, 
    object::pair{std::string("agentId"), relationshipTestAgentId}, 
    object::pair{std::string("tags"), array<string>{ std::string("colleague") }}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("type"), std::string("professional")}, 
        object::pair{std::string("strength"), std::string("medium")}
    }}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()->toString()}
} };
std::function<any(any, any, array<string>, std::shared_ptr<Record<string, any>>)> createTestRelationship = [=](auto sourceId, auto targetId, auto tags = array<string>(), auto metadata = object{}) mutable
{
    return object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
        object::pair{std::string("sourceEntityId"), sourceId}, 
        object::pair{std::string("targetEntityId"), targetId}, 
        object::pair{std::string("agentId"), relationshipTestAgentId}, 
        object::pair{std::string("tags"), std::string("tags")}, 
        object::pair{std::string("metadata"), std::string("metadata")}, 
        object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()->toString()}
    };
};

void Main(void)
{
}

MAIN
