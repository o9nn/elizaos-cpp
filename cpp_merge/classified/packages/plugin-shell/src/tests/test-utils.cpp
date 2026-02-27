#include "test-utils.h"

std::shared_ptr<IAgentRuntime> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides)
{
    return as<std::shared_ptr<IAgentRuntime>>(utils::assign(object{
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000001"))}, 
        object::pair{std:("getConversationLength"), mock([=]() mutable
        {
            return 10;
        }
        )}, 
        object::pair{std:("getSetting"), mock([=]() mutable
        {
            return nullptr;
        }
        )}, 
        object::pair{std:("getService"), mock([=]() mutable
        {
            return nullptr;
        }
        )}, 
        object::pair{std:("hasService"), mock([=]() mutable
        {
            return false;
        }
        )}, 
        object::pair{std:("registerService"), mock()}, 
        object::pair{std:("initialize"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("stop"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("evaluate"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("processActions"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("useModel"), as<any>(mock([=]() mutable
        {
            return Promise->resolve(std:("test response"));
        }
        ))}, 
        object::pair{std:("ensureConnection"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("composeState"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<State>>(object{
                object::pair{std:("data"), object{}}, 
                object::pair{std:("values"), object{}}, 
                object::pair{std:("text"), string_empty}
            }));
        }
        )}, 
        object::pair{std:("createMemory"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std:("test-memory-id")));
        }
        )}, 
        object::pair{std:("actions"), array<any>()}, 
        object::pair{std:("providers"), array<any>()}, 
        object::pair{std:("evaluators"), array<any>()}, 
        object::pair{std:("services"), std::make_shared<Map>()}, 
        object::pair{std:("db"), nullptr}, 
        object::pair{std:("plugins"), array<any>()}, 
        object::pair{std:("routes"), array<any>()}, 
        object::pair{std:("logger"), console}, 
        object::pair{std:("character"), object{
            object::pair{std:("name"), std:("Test Agent")}, 
            object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000001"))}, 
            object::pair{std:("username"), std:("test-agent")}, 
            object::pair{std:("bio"), array<any>()}, 
            object::pair{std:("settings"), object{}}, 
            object::pair{std:("system"), std:("test system")}, 
            object::pair{std:("plugins"), array<any>()}
        }}
    }, overrides));
};


std::shared_ptr<Memory> createMockMemory(Partial<std::shared_ptr<Memory>> overrides)
{
    return utils::assign(object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000002"))}, 
        object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000003"))}, 
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000001"))}, 
        object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000004"))}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("test memory")}
        }}, 
        object::pair{std:("createdAt"), Date->now()}
    }, overrides);
};


std::shared_ptr<State> createMockState(Partial<std::shared_ptr<State>> overrides)
{
    return utils::assign(object{
        object::pair{std:("data"), object{}}, 
        object::pair{std:("values"), object{}}, 
        object::pair{std:("text"), string_empty}
    }, overrides);
};


