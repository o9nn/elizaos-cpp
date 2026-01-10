#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/participant-seed.h"

any participantTestAgentId = as<std::shared_ptr<UUID>>(uuidv4());
any participantTestEntityId = as<std::shared_ptr<UUID>>(uuidv4());
any participantTestRoomId = as<std::shared_ptr<UUID>>(uuidv4());
any participantTestWorldId = as<std::shared_ptr<UUID>>(uuidv4());
std::shared_ptr<Agent> participantTestAgent = object{
    object::pair{std::string("id"), participantTestAgentId}, 
    object::pair{std::string("name"), std::string("Participant Test Agent")}, 
    object::pair{std::string("bio"), std::string("Test agent for participant integration tests")}, 
    object::pair{std::string("settings"), object{
        object::pair{std::string("profile"), object{
            object::pair{std::string("short_description"), std::string("Test agent for participant integration tests")}
        }}
    }}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->getTime()}
};
std::shared_ptr<Entity> participantTestEntity = object{
    object::pair{std::string("id"), participantTestEntityId}, 
    object::pair{std::string("names"), array<string>{ std::string("Participant Test Entity") }}, 
    object::pair{std::string("agentId"), participantTestAgentId}, 
    object::pair{std::string("components"), array<any>()}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("type"), std::string("user")}, 
        object::pair{std::string("worldId"), participantTestWorldId}
    }}
};
std::shared_ptr<World> participantTestWorld = object{
    object::pair{std::string("id"), participantTestWorldId}, 
    object::pair{std::string("agentId"), participantTestAgentId}, 
    object::pair{std::string("name"), std::string("Participant Test World")}, 
    object::pair{std::string("serverId"), std::string("test-server")}, 
    object::pair{std::string("metadata"), object{}}
};
std::shared_ptr<Room> participantTestRoom = object{
    object::pair{std::string("id"), participantTestRoomId}, 
    object::pair{std::string("name"), std::string("Participant Test Room")}, 
    object::pair{std::string("agentId"), participantTestAgentId}, 
    object::pair{std::string("source"), std::string("test")}, 
    object::pair{std::string("type"), ChannelType->GROUP}, 
    object::pair{std::string("worldId"), participantTestWorldId}, 
    object::pair{std::string("metadata"), object{}}
};

void Main(void)
{
}

MAIN
