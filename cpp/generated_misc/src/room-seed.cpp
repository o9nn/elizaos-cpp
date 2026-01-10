#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/room-seed.h"

any roomTestAgentId = as<std::shared_ptr<UUID>>(uuidv4());
any roomTestEntityId = as<std::shared_ptr<UUID>>(uuidv4());
any roomTestWorldId = as<std::shared_ptr<UUID>>(uuidv4());
any roomTestRoomId = as<std::shared_ptr<UUID>>(uuidv4());
any roomTestRoom2Id = as<std::shared_ptr<UUID>>(uuidv4());
std::shared_ptr<Agent> roomTestAgent = object{
    object::pair{std::string("id"), roomTestAgentId}, 
    object::pair{std::string("name"), std::string("Room Test Agent")}, 
    object::pair{std::string("bio"), std::string("Test agent for room integration tests")}, 
    object::pair{std::string("settings"), object{
        object::pair{std::string("profile"), object{
            object::pair{std::string("short_description"), std::string("Test agent for room integration tests")}
        }}
    }}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->getTime()}
};
std::shared_ptr<Entity> roomTestEntity = object{
    object::pair{std::string("id"), roomTestEntityId}, 
    object::pair{std::string("names"), array<string>{ std::string("Room Test Entity") }}, 
    object::pair{std::string("agentId"), roomTestAgentId}, 
    object::pair{std::string("components"), array<any>()}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("type"), std::string("user")}, 
        object::pair{std::string("worldId"), roomTestWorldId}
    }}
};
std::shared_ptr<World> roomTestWorld = object{
    object::pair{std::string("id"), roomTestWorldId}, 
    object::pair{std::string("agentId"), roomTestAgentId}, 
    object::pair{std::string("name"), std::string("Room Test World")}, 
    object::pair{std::string("serverId"), std::string("test-server")}, 
    object::pair{std::string("metadata"), object{}}
};
array<std::shared_ptr<Room>> roomTestRooms = array<std::shared_ptr<Room>>{ object{
    object::pair{std::string("id"), roomTestRoomId}, 
    object::pair{std::string("name"), std::string("Room Test Room 1")}, 
    object::pair{std::string("agentId"), roomTestAgentId}, 
    object::pair{std::string("source"), std::string("test")}, 
    object::pair{std::string("type"), ChannelType->GROUP}, 
    object::pair{std::string("worldId"), roomTestWorldId}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("description"), std::string("Test description for room 1")}, 
        object::pair{std::string("tags"), array<string>{ std::string("test"), std::string("integration") }}
    }}
}, object{
    object::pair{std::string("id"), roomTestRoom2Id}, 
    object::pair{std::string("name"), std::string("Room Test Room 2")}, 
    object::pair{std::string("agentId"), roomTestAgentId}, 
    object::pair{std::string("source"), std::string("test")}, 
    object::pair{std::string("type"), ChannelType->DM}, 
    object::pair{std::string("worldId"), roomTestWorldId}, 
    object::pair{std::string("channelId"), std::string("test-channel-id")}, 
    object::pair{std::string("serverId"), std::string("test-server-id")}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("description"), std::string("Test description for room 2")}, 
        object::pair{std::string("tags"), array<string>{ std::string("integration") }}
    }}
}, object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("name"), std::string("Room Test Room 3")}, 
    object::pair{std::string("agentId"), roomTestAgentId}, 
    object::pair{std::string("source"), std::string("discord")}, 
    object::pair{std::string("type"), ChannelType->GROUP}, 
    object::pair{std::string("worldId"), roomTestWorldId}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("description"), std::string("Test description for room 3")}, 
        object::pair{std::string("isPrivate"), true}
    }}
} };
std::function<any(any)> createModifiedRoom = [=](auto room) mutable
{
    return utils::assign(object{
        , 
        object::pair{std::string("name"), string_empty + room->name + std::string(" - Updated")}, 
        object::pair{std::string("metadata"), utils::assign(object{
            , 
            object::pair{std::string("updated"), true}, 
            object::pair{std::string("timestamp"), Date->now()}
        }, (as<Record<string, any>>(room->metadata)))}
    }, room);
};

void Main(void)
{
}

MAIN
