#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/core/src/test-utils/mock-runtime.h"

Partial<std::shared_ptr<IAgentRuntime>> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides)
{
    return utils::assign(object{
        object::pair{std::string("agentId"), asUUID(std::string("test-agent-id"))}, 
        object::pair{std::string("services"), std::make_shared<Map>()}, 
        object::pair{std::string("getService"), [=](auto name) mutable
        {
            return nullptr;
        }
        }, 
        object::pair{std::string("registerService"), [=](auto service) mutable
        {
            return Promise->resolve();
        }
        }
    }, overrides);
};


std::shared_ptr<Memory> createMockMemory(string text, Partial<std::shared_ptr<Memory>> overrides)
{
    return utils::assign(object{
        object::pair{std::string("id"), asUUID(std::string("test-memory-id"))}, 
        object::pair{std::string("entityId"), asUUID(std::string("test-user-id"))}, 
        object::pair{std::string("agentId"), asUUID(std::string("test-agent-id"))}, 
        object::pair{std::string("roomId"), asUUID(std::string("test-room-id"))}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("text")}
        }}, 
        object::pair{std::string("createdAt"), Date->now()}
    }, overrides);
};


std::shared_ptr<State> createMockState(Partial<std::shared_ptr<State>> overrides)
{
    return utils::assign(object{
        object::pair{std::string("values"), object{}}, 
        object::pair{std::string("data"), object{}}, 
        object::pair{std::string("text"), string_empty}, 
        object::pair{std::string("agentId"), asUUID(std::string("test-agent-id"))}, 
        object::pair{std::string("roomId"), asUUID(std::string("test-room-id"))}, 
        object::pair{std::string("userId"), asUUID(std::string("test-user-id"))}, 
        object::pair{std::string("agentName"), std::string("Test Agent")}
    }, overrides);
};


