#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sql/src/__tests__/integration/seed/room-seed.h"

any roomTestAgentId = as<std::shared_ptr<UUID>>(uuidv4());
any roomTestEntityId = as<std::shared_ptr<UUID>>(uuidv4());
any roomTestWorldId = as<std::shared_ptr<UUID>>(uuidv4());
any roomTestRoomId = as<std::shared_ptr<UUID>>(uuidv4());
any roomTestRoom2Id = as<std::shared_ptr<UUID>>(uuidv4());
std::shared_ptr<Agent> roomTestAgent = object{
    object::pair{std:("id"), roomTestAgentId}, 
    object::pair{std:("name"), std:("Room Test Agent")}, 
    object::pair{std:("bio"), std:("Test agent for room integration tests")}, 
    object::pair{std:("settings"), object{
        object::pair{std:("profile"), object{
            object::pair{std:("short_description"), std:("Test agent for room integration tests")}
        }}
    }}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("updatedAt"), ((std::make_shared<Date>()))->getTime()}
};
std::shared_ptr<Entity> roomTestEntity = object{
    object::pair{std:("id"), roomTestEntityId}, 
    object::pair{std:("names"), array<string>{ std:("Room Test Entity") }}, 
    object::pair{std:("agentId"), roomTestAgentId}, 
    object::pair{std:("components"), array<any>()}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("type"), std:("user")}, 
        object::pair{std:("worldId"), roomTestWorldId}
    }}
};
std::shared_ptr<World> roomTestWorld = object{
    object::pair{std:("id"), roomTestWorldId}, 
    object::pair{std:("agentId"), roomTestAgentId}, 
    object::pair{std:("name"), std:("Room Test World")}, 
    object::pair{std:("serverId"), std:("test-server")}, 
    object::pair{std:("metadata"), object{}}
};
array<std::shared_ptr<Room>> roomTestRooms = array<std::shared_ptr<Room>>{ object{
    object::pair{std:("id"), roomTestRoomId}, 
    object::pair{std:("name"), std:("Room Test Room 1")}, 
    object::pair{std:("agentId"), roomTestAgentId}, 
    object::pair{std:("source"), std:("test")}, 
    object::pair{std:("type"), ChannelType->GROUP}, 
    object::pair{std:("worldId"), roomTestWorldId}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("description"), std:("Test description for room 1")}, 
        object::pair{std:("tags"), array<string>{ std:("test"), std:("integration") }}
    }}
}, object{
    object::pair{std:("id"), roomTestRoom2Id}, 
    object::pair{std:("name"), std:("Room Test Room 2")}, 
    object::pair{std:("agentId"), roomTestAgentId}, 
    object::pair{std:("source"), std:("test")}, 
    object::pair{std:("type"), ChannelType->DM}, 
    object::pair{std:("worldId"), roomTestWorldId}, 
    object::pair{std:("channelId"), std:("test-channel-id")}, 
    object::pair{std:("serverId"), std:("test-server-id")}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("description"), std:("Test description for room 2")}, 
        object::pair{std:("tags"), array<string>{ std:("integration") }}
    }}
}, object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("name"), std:("Room Test Room 3")}, 
    object::pair{std:("agentId"), roomTestAgentId}, 
    object::pair{std:("source"), std:("discord")}, 
    object::pair{std:("type"), ChannelType->GROUP}, 
    object::pair{std:("worldId"), roomTestWorldId}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("description"), std:("Test description for room 3")}, 
        object::pair{std:("isPrivate"), true}
    }}
} };
std::function<any(any)> createModifiedRoom = [=](auto room) mutable
{
    return utils::assign(object{
        , 
        object::pair{std:("name"), string_empty + room->name + std:(" - Updated")}, 
        object::pair{std:("metadata"), utils::assign(object{
            , 
            object::pair{std:("updated"), true}, 
            object::pair{std:("timestamp"), Date->now()}
        }, (as<Record<string, any>>(room->metadata)))}
    }, room);
};

void Main(void)
{
}

MAIN
