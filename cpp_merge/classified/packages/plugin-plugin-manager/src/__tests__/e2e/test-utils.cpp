#include "test-utils.h"
#include <string>

std::shared_ptr<Promise<object>> setupScenario(std::shared_ptr<IAgentRuntime> runtime)
{
    assert(runtime->agentId, std::string("Runtime must have an agentId to run a scenario"));
    auto user = object{
        object::pair{std::string("id"), asUUID(uuid())}, 
        object::pair{std::string("names"), array<string>{ std::string("Test User") }}, 
        object::pair{std::string("agentId"), runtime->agentId}, 
        object::pair{std::string("metadata"), object{
            object::pair{std::string("type"), std::string("user")}
        }}
    };
    std::async([=]() { runtime->createEntity(user); });
    assert(user->id, std::string("Created user must have an id"));
    auto room = object{
        object::pair{std::string("id"), asUUID(uuid())}, 
        object::pair{std::string("name"), std::string("Test Plugin Manager Room")}, 
        object::pair{std::string("type"), ChannelType->DM}, 
        object::pair{std::string("source"), std::string("e2e-test")}, 
        object::pair{std::string("serverId"), std::string("e2e-test-server")}
    };
    std::async([=]() { runtime->createRoom(room); });
    std::async([=]() { runtime->ensureParticipantInRoom(runtime->agentId, room->id); });
    std::async([=]() { runtime->ensureParticipantInRoom(user->id, room->id); });
    return object{
        object::pair{std::string("user"), std::string("user")}, 
        object::pair{std::string("room"), std::string("room")}
    };
};


std::shared_ptr<Promise<std::shared_ptr<Content>>> sendMessageAndWaitForResponse(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Room> room, std::shared_ptr<Entity> user, string text)
{
    return std::make_shared<Promise>([=](auto resolve) mutable
    {
        assert(runtime->agentId, std::string("Runtime must have an agentId to send a message"));
        assert(user->id, std::string("User must have an id to send a message"));
        auto message = object{
            object::pair{std::string("id"), createUniqueUuid(runtime, string_empty + user->id + std::string("-") + Date->now() + string_empty)}, 
            object::pair{std::string("agentId"), runtime->agentId}, 
            object::pair{std::string("entityId"), user->id}, 
            object::pair{std::string("roomId"), room->id}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("text")}
            }}, 
            object::pair{std::string("createdAt"), Date->now()}
        };
        auto callback = [=](auto responseContent) mutable
        {
            resolve(responseContent);
        };
        runtime->emitEvent(EventType->MESSAGE_RECEIVED, object{
            object::pair{std::string("runtime"), std::string("runtime")}, 
            object::pair{std::string("message"), std::string("message")}, 
            object::pair{std::string("callback"), std::string("callback")}
        });
    }
    );
};


