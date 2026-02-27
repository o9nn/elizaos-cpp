#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-dummy-services/src/e2e/test-utils.h"

std::shared_ptr<Promise<object>> setupScenario(std::shared_ptr<IAgentRuntime> runtime)
{
    assert(runtime->agentId, std:("Runtime must have an agentId to run a scenario"));
    auto user = object{
        object::pair{std:("id"), asUUID(uuid())}, 
        object::pair{std:("names"), array<string>{ std:("Test User") }}, 
        object::pair{std:("agentId"), runtime->agentId}, 
        object::pair{std:("metadata"), object{
            object::pair{std:("type"), std:("user")}
        }}
    };
    std::async([=]() { runtime->createEntity(user); });
    assert(user->id, std:("Created user must have an id"));
    auto world = object{
        object::pair{std:("id"), asUUID(uuid())}, 
        object::pair{std:("agentId"), runtime->agentId}, 
        object::pair{std:("name"), std:("E2E Test World")}, 
        object::pair{std:("serverId"), std:("e2e-test-server")}, 
        object::pair{std:("metadata"), object{
            object::pair{std:("ownership"), object{
                object::pair{std:("ownerId"), user->id}
            }}
        }}
    };
    std::async([=]() { runtime->ensureWorldExists(world); });
    auto room = object{
        object::pair{std:("id"), asUUID(uuid())}, 
        object::pair{std:("name"), std:("Test DM Room")}, 
        object::pair{std:("type"), ChannelType->DM}, 
        object::pair{std:("source"), std:("e2e-test")}, 
        object::pair{std:("worldId"), world->id}, 
        object::pair{std:("serverId"), world->serverId}
    };
    std::async([=]() { runtime->createRoom(room); });
    std::async([=]() { runtime->ensureParticipantInRoom(runtime->agentId, room->id); });
    std::async([=]() { runtime->ensureParticipantInRoom(user->id, room->id); });
    return object{
        object::pair{std:("user"), std:("user")}, 
        object::pair{std:("room"), std:("room")}, 
        object::pair{std:("world"), std:("world")}
    };
};


std::shared_ptr<Promise<std::shared_ptr<Content>>> sendMessageAndWaitForResponse(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Room> room, std::shared_ptr<Entity> user, string text)
{
    return std::make_shared<Promise>([=](auto resolve) mutable
    {
        assert(runtime->agentId, std:("Runtime must have an agentId to send a message"));
        assert(user->id, std:("User must have an id to send a message"));
        auto message = object{
            object::pair{std:("id"), createUniqueUuid(runtime, string_empty + user->id + std:("-") + Date->now() + string_empty)}, 
            object::pair{std:("agentId"), runtime->agentId}, 
            object::pair{std:("entityId"), user->id}, 
            object::pair{std:("roomId"), room->id}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("text")}
            }}, 
            object::pair{std:("createdAt"), Date->now()}
        };
        auto callback = [=](auto responseContent) mutable
        {
            resolve(responseContent);
        };
        runtime->emitEvent(EventType->MESSAGE_RECEIVED, object{
            object::pair{std:("runtime"), std:("runtime")}, 
            object::pair{std:("message"), std:("message")}, 
            object::pair{std:("callback"), std:("callback")}
        });
    }
    );
};


