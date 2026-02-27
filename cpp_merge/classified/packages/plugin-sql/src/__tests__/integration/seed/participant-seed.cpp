#include "participant-seed.h"

any participantTestAgentId = as<std::shared_ptr<UUID>>(uuidv4());
any participantTestEntityId = as<std::shared_ptr<UUID>>(uuidv4());
any participantTestRoomId = as<std::shared_ptr<UUID>>(uuidv4());
any participantTestWorldId = as<std::shared_ptr<UUID>>(uuidv4());
std::shared_ptr<Agent> participantTestAgent = object{
    object::pair{std:("id"), participantTestAgentId}, 
    object::pair{std:("name"), std:("Participant Test Agent")}, 
    object::pair{std:("bio"), std:("Test agent for participant integration tests")}, 
    object::pair{std:("settings"), object{
        object::pair{std:("profile"), object{
            object::pair{std:("short_description"), std:("Test agent for participant integration tests")}
        }}
    }}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("updatedAt"), ((std::make_shared<Date>()))->getTime()}
};
std::shared_ptr<Entity> participantTestEntity = object{
    object::pair{std:("id"), participantTestEntityId}, 
    object::pair{std:("names"), array<string>{ std:("Participant Test Entity") }}, 
    object::pair{std:("agentId"), participantTestAgentId}, 
    object::pair{std:("components"), array<any>()}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("type"), std:("user")}, 
        object::pair{std:("worldId"), participantTestWorldId}
    }}
};
std::shared_ptr<World> participantTestWorld = object{
    object::pair{std:("id"), participantTestWorldId}, 
    object::pair{std:("agentId"), participantTestAgentId}, 
    object::pair{std:("name"), std:("Participant Test World")}, 
    object::pair{std:("serverId"), std:("test-server")}, 
    object::pair{std:("metadata"), object{}}
};
std::shared_ptr<Room> participantTestRoom = object{
    object::pair{std:("id"), participantTestRoomId}, 
    object::pair{std:("name"), std:("Participant Test Room")}, 
    object::pair{std:("agentId"), participantTestAgentId}, 
    object::pair{std:("source"), std:("test")}, 
    object::pair{std:("type"), ChannelType->GROUP}, 
    object::pair{std:("worldId"), participantTestWorldId}, 
    object::pair{std:("metadata"), object{}}
};

void Main(void)
{
}

MAIN
