#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/world-seed.h"

any worldTestAgentId = as<std::shared_ptr<UUID>>(uuidv4());
any worldTestEntityId = as<std::shared_ptr<UUID>>(uuidv4());
std::shared_ptr<Agent> worldTestAgent = object{
    object::pair{std::string("id"), worldTestAgentId}, 
    object::pair{std::string("name"), std::string("World Test Agent")}, 
    object::pair{std::string("bio"), std::string("Test agent for world integration tests")}, 
    object::pair{std::string("settings"), object{
        object::pair{std::string("profile"), object{
            object::pair{std::string("short_description"), std::string("Test agent for world integration tests")}
        }}
    }}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->getTime()}
};
std::shared_ptr<Entity> worldTestEntity = object{
    object::pair{std::string("id"), worldTestEntityId}, 
    object::pair{std::string("names"), array<string>{ std::string("World Test Entity") }}, 
    object::pair{std::string("agentId"), worldTestAgentId}, 
    object::pair{std::string("components"), array<any>()}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("type"), std::string("user")}
    }}
};
array<std::shared_ptr<World>> worldTestWorlds = array<std::shared_ptr<World>>{ object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("agentId"), worldTestAgentId}, 
    object::pair{std::string("name"), std::string("Test World 1")}, 
    object::pair{std::string("serverId"), std::string("test-server-1")}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("ownership"), object{
            object::pair{std::string("ownerId"), worldTestEntityId}
        }}, 
        object::pair{std::string("roles"), object{
            object::pair{worldTestEntityId, Role->OWNER}
        }}
    }}
}, object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("agentId"), worldTestAgentId}, 
    object::pair{std::string("name"), std::string("Test World 2")}, 
    object::pair{std::string("serverId"), std::string("test-server-2")}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("ownership"), object{
            object::pair{std::string("ownerId"), worldTestEntityId}
        }}
    }}
}, object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("agentId"), worldTestAgentId}, 
    object::pair{std::string("name"), std::string("Test World 3")}, 
    object::pair{std::string("serverId"), std::string("test-server-3")}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("custom"), std::string("value")}, 
        object::pair{std::string("tags"), array<string>{ std::string("test"), std::string("integration") }}
    }}
} };

void Main(void)
{
}

MAIN
