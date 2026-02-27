#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sql/src/__tests__/integration/seed/world-seed.h"

any worldTestAgentId = as<std::shared_ptr<UUID>>(uuidv4());
any worldTestEntityId = as<std::shared_ptr<UUID>>(uuidv4());
std::shared_ptr<Agent> worldTestAgent = object{
    object::pair{std:("id"), worldTestAgentId}, 
    object::pair{std:("name"), std:("World Test Agent")}, 
    object::pair{std:("bio"), std:("Test agent for world integration tests")}, 
    object::pair{std:("settings"), object{
        object::pair{std:("profile"), object{
            object::pair{std:("short_description"), std:("Test agent for world integration tests")}
        }}
    }}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("updatedAt"), ((std::make_shared<Date>()))->getTime()}
};
std::shared_ptr<Entity> worldTestEntity = object{
    object::pair{std:("id"), worldTestEntityId}, 
    object::pair{std:("names"), array<string>{ std:("World Test Entity") }}, 
    object::pair{std:("agentId"), worldTestAgentId}, 
    object::pair{std:("components"), array<any>()}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("type"), std:("user")}
    }}
};
array<std::shared_ptr<World>> worldTestWorlds = array<std::shared_ptr<World>>{ object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("agentId"), worldTestAgentId}, 
    object::pair{std:("name"), std:("Test World 1")}, 
    object::pair{std:("serverId"), std:("test-server-1")}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("ownership"), object{
            object::pair{std:("ownerId"), worldTestEntityId}
        }}, 
        object::pair{std:("roles"), object{
            object::pair{worldTestEntityId, Role->OWNER}
        }}
    }}
}, object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("agentId"), worldTestAgentId}, 
    object::pair{std:("name"), std:("Test World 2")}, 
    object::pair{std:("serverId"), std:("test-server-2")}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("ownership"), object{
            object::pair{std:("ownerId"), worldTestEntityId}
        }}
    }}
}, object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("agentId"), worldTestAgentId}, 
    object::pair{std:("name"), std:("Test World 3")}, 
    object::pair{std:("serverId"), std:("test-server-3")}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("custom"), std:("value")}, 
        object::pair{std:("tags"), array<string>{ std:("test"), std:("integration") }}
    }}
} };

void Main(void)
{
}

MAIN
