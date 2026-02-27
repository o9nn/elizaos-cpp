#include "mock-runtime.h"

Partial<std::shared_ptr<IAgentRuntime>> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides)
{
    return utils::assign(object{
        object::pair{std:("agentId"), asUUID(std:("test-agent-id"))}, 
        object::pair{std:("services"), std::make_shared<Map>()}, 
        object::pair{std:("getService"), [=](auto name) mutable
        {
            return nullptr;
        }
        }, 
        object::pair{std:("registerService"), [=](auto service) mutable
        {
            return Promise->resolve();
        }
        }
    }, overrides);
};


std::shared_ptr<Memory> createMockMemory(string text, Partial<std::shared_ptr<Memory>> overrides)
{
    return utils::assign(object{
        object::pair{std:("id"), asUUID(std:("test-memory-id"))}, 
        object::pair{std:("entityId"), asUUID(std:("test-user-id"))}, 
        object::pair{std:("agentId"), asUUID(std:("test-agent-id"))}, 
        object::pair{std:("roomId"), asUUID(std:("test-room-id"))}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("text")}
        }}, 
        object::pair{std:("createdAt"), Date->now()}
    }, overrides);
};


std::shared_ptr<State> createMockState(Partial<std::shared_ptr<State>> overrides)
{
    return utils::assign(object{
        object::pair{std:("values"), object{}}, 
        object::pair{std:("data"), object{}}, 
        object::pair{std:("text"), string_empty}, 
        object::pair{std:("agentId"), asUUID(std:("test-agent-id"))}, 
        object::pair{std:("roomId"), asUUID(std:("test-room-id"))}, 
        object::pair{std:("userId"), asUUID(std:("test-user-id"))}, 
        object::pair{std:("agentName"), std:("Test Agent")}
    }, overrides);
};


