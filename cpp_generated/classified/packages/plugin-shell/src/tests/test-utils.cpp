#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-shell/src/tests/test-utils.h"

std::shared_ptr<IAgentRuntime> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides)
{
    return as<std::shared_ptr<IAgentRuntime>>(utils::assign(object{
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000001"))}, 
        object::pair{std::string("getConversationLength"), mock([=]() mutable
        {
            return 10;
        }
        )}, 
        object::pair{std::string("getSetting"), mock([=]() mutable
        {
            return nullptr;
        }
        )}, 
        object::pair{std::string("getService"), mock([=]() mutable
        {
            return nullptr;
        }
        )}, 
        object::pair{std::string("hasService"), mock([=]() mutable
        {
            return false;
        }
        )}, 
        object::pair{std::string("registerService"), mock()}, 
        object::pair{std::string("initialize"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("stop"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("evaluate"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("processActions"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("useModel"), as<any>(mock([=]() mutable
        {
            return Promise->resolve(std::string("test response"));
        }
        ))}, 
        object::pair{std::string("ensureConnection"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("composeState"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<State>>(object{
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("text"), string_empty}
            }));
        }
        )}, 
        object::pair{std::string("createMemory"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std::string("test-memory-id")));
        }
        )}, 
        object::pair{std::string("actions"), array<any>()}, 
        object::pair{std::string("providers"), array<any>()}, 
        object::pair{std::string("evaluators"), array<any>()}, 
        object::pair{std::string("services"), std::make_shared<Map>()}, 
        object::pair{std::string("db"), nullptr}, 
        object::pair{std::string("plugins"), array<any>()}, 
        object::pair{std::string("routes"), array<any>()}, 
        object::pair{std::string("logger"), console}, 
        object::pair{std::string("character"), object{
            object::pair{std::string("name"), std::string("Test Agent")}, 
            object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000001"))}, 
            object::pair{std::string("username"), std::string("test-agent")}, 
            object::pair{std::string("bio"), array<any>()}, 
            object::pair{std::string("settings"), object{}}, 
            object::pair{std::string("system"), std::string("test system")}, 
            object::pair{std::string("plugins"), array<any>()}
        }}
    }, overrides));
};


std::shared_ptr<Memory> createMockMemory(Partial<std::shared_ptr<Memory>> overrides)
{
    return utils::assign(object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000002"))}, 
        object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000003"))}, 
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000001"))}, 
        object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000004"))}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("test memory")}
        }}, 
        object::pair{std::string("createdAt"), Date->now()}
    }, overrides);
};


std::shared_ptr<State> createMockState(Partial<std::shared_ptr<State>> overrides)
{
    return utils::assign(object{
        object::pair{std::string("data"), object{}}, 
        object::pair{std::string("values"), object{}}, 
        object::pair{std::string("text"), string_empty}
    }, overrides);
};


