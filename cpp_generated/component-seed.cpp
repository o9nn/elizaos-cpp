#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/component-seed.h"

any componentTestAgentSettings = as<std::shared_ptr<Agent>>(object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std::string("name"), std::string("Component Test Agent")}, 
    object::pair{std::string("username"), std::string("component_test_agent")}, 
    object::pair{std::string("system"), std::string("Test agent system prompt")}, 
    object::pair{std::string("bio"), std::string("An agent for testing component operations")}, 
    object::pair{std::string("enabled"), true}, 
    object::pair{std::string("status"), AgentStatus->ACTIVE}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("messageExamples"), array<any>()}, 
    object::pair{std::string("postExamples"), array<any>()}, 
    object::pair{std::string("topics"), array<any>()}, 
    object::pair{std::string("adjectives"), array<any>()}, 
    object::pair{std::string("knowledge"), array<any>()}, 
    object::pair{std::string("plugins"), array<any>()}, 
    object::pair{std::string("settings"), object{
        object::pair{std::string("componentTestSetting"), std::string("component test value")}
    }}, 
    object::pair{std::string("style"), object{
        object::pair{std::string("all"), array<any>()}, 
        object::pair{std::string("chat"), array<any>()}, 
        object::pair{std::string("post"), array<any>()}
    }}
});
object componentTestWorld = object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std::string("name"), std::string("Component Test World")}, 
    object::pair{std::string("agentId"), componentTestAgentSettings->id}, 
    object::pair{std::string("serverId"), std::string("component-test-server")}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("ownership"), object{
            object::pair{std::string("ownerId"), std::string("component-test-owner")}
        }}
    }}
};
object componentTestEntity = object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std::string("names"), array<string>{ std::string("Component Test Entity") }}, 
    object::pair{std::string("agentId"), componentTestAgentSettings->id}, 
    object::pair{std::string("metadata"), object{}}
};
object componentTestSourceEntity = object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std::string("names"), array<string>{ std::string("Component Test Source Entity") }}, 
    object::pair{std::string("agentId"), componentTestAgentSettings->id}, 
    object::pair{std::string("metadata"), object{}}
};
object componentTestRoom = object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std::string("name"), std::string("Component Test Room")}, 
    object::pair{std::string("type"), ChannelType->DM}, 
    object::pair{std::string("agentId"), componentTestAgentSettings->id}, 
    object::pair{std::string("source"), std::string("component-test")}, 
    object::pair{std::string("worldId"), componentTestWorld["id"]}, 
    object::pair{std::string("metadata"), object{}}
};
object componentTestComponents = object{
    object::pair{std::string("basic"), as<std::shared_ptr<Component>>(object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std::string("entityId"), componentTestEntity["id"]}, 
        object::pair{std::string("agentId"), componentTestAgentSettings->id}, 
        object::pair{std::string("roomId"), componentTestRoom["id"]}, 
        object::pair{std::string("type"), std::string("basic_component")}, 
        object::pair{std::string("data"), object{
            object::pair{std::string("value"), std::string("basic component data")}
        }}, 
        object::pair{std::string("worldId"), as<any>(undefined)}, 
        object::pair{std::string("sourceEntityId"), as<any>(undefined)}, 
        object::pair{std::string("createdAt"), Date->now()}
    })}, 
    object::pair{std::string("withWorldId"), as<std::shared_ptr<Component>>(object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std::string("entityId"), componentTestEntity["id"]}, 
        object::pair{std::string("agentId"), componentTestAgentSettings->id}, 
        object::pair{std::string("roomId"), componentTestRoom["id"]}, 
        object::pair{std::string("worldId"), componentTestWorld["id"]}, 
        object::pair{std::string("type"), std::string("world_component")}, 
        object::pair{std::string("data"), object{
            object::pair{std::string("worldValue"), 42}
        }}, 
        object::pair{std::string("sourceEntityId"), as<any>(undefined)}, 
        object::pair{std::string("createdAt"), Date->now()}
    })}, 
    object::pair{std::string("withSourceEntity"), as<std::shared_ptr<Component>>(object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std::string("entityId"), componentTestEntity["id"]}, 
        object::pair{std::string("agentId"), componentTestAgentSettings->id}, 
        object::pair{std::string("roomId"), componentTestRoom["id"]}, 
        object::pair{std::string("sourceEntityId"), componentTestSourceEntity["id"]}, 
        object::pair{std::string("type"), std::string("source_component")}, 
        object::pair{std::string("data"), object{
            object::pair{std::string("sourceValue"), std::string("from source entity")}
        }}, 
        object::pair{std::string("worldId"), as<any>(undefined)}, 
        object::pair{std::string("createdAt"), Date->now()}
    })}
};

void Main(void)
{
}

MAIN
