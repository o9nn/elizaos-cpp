#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/component-seed.h"

any componentTestAgentSettings = as<std::shared_ptr<Agent>>(object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std:("name"), std:("Component Test Agent")}, 
    object::pair{std:("username"), std:("component_test_agent")}, 
    object::pair{std:("system"), std:("Test agent system prompt")}, 
    object::pair{std:("bio"), std:("An agent for testing component operations")}, 
    object::pair{std:("enabled"), true}, 
    object::pair{std:("status"), AgentStatus->ACTIVE}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("updatedAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("messageExamples"), array<any>()}, 
    object::pair{std:("postExamples"), array<any>()}, 
    object::pair{std:("topics"), array<any>()}, 
    object::pair{std:("adjectives"), array<any>()}, 
    object::pair{std:("knowledge"), array<any>()}, 
    object::pair{std:("plugins"), array<any>()}, 
    object::pair{std:("settings"), object{
        object::pair{std:("componentTestSetting"), std:("component test value")}
    }}, 
    object::pair{std:("style"), object{
        object::pair{std:("all"), array<any>()}, 
        object::pair{std:("chat"), array<any>()}, 
        object::pair{std:("post"), array<any>()}
    }}
});
object componentTestWorld = object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std:("name"), std:("Component Test World")}, 
    object::pair{std:("agentId"), componentTestAgentSettings->id}, 
    object::pair{std:("serverId"), std:("component-test-server")}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("ownership"), object{
            object::pair{std:("ownerId"), std:("component-test-owner")}
        }}
    }}
};
object componentTestEntity = object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std:("names"), array<string>{ std:("Component Test Entity") }}, 
    object::pair{std:("agentId"), componentTestAgentSettings->id}, 
    object::pair{std:("metadata"), object{}}
};
object componentTestSourceEntity = object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std:("names"), array<string>{ std:("Component Test Source Entity") }}, 
    object::pair{std:("agentId"), componentTestAgentSettings->id}, 
    object::pair{std:("metadata"), object{}}
};
object componentTestRoom = object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std:("name"), std:("Component Test Room")}, 
    object::pair{std:("type"), ChannelType->DM}, 
    object::pair{std:("agentId"), componentTestAgentSettings->id}, 
    object::pair{std:("source"), std:("component-test")}, 
    object::pair{std:("worldId"), componentTestWorld["id"]}, 
    object::pair{std:("metadata"), object{}}
};
object componentTestComponents = object{
    object::pair{std:("basic"), as<std::shared_ptr<Component>>(object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std:("entityId"), componentTestEntity["id"]}, 
        object::pair{std:("agentId"), componentTestAgentSettings->id}, 
        object::pair{std:("roomId"), componentTestRoom["id"]}, 
        object::pair{std:("type"), std:("basic_component")}, 
        object::pair{std:("data"), object{
            object::pair{std:("value"), std:("basic component data")}
        }}, 
        object::pair{std:("worldId"), as<any>(undefined)}, 
        object::pair{std:("sourceEntityId"), as<any>(undefined)}, 
        object::pair{std:("createdAt"), Date->now()}
    })}, 
    object::pair{std:("withWorldId"), as<std::shared_ptr<Component>>(object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std:("entityId"), componentTestEntity["id"]}, 
        object::pair{std:("agentId"), componentTestAgentSettings->id}, 
        object::pair{std:("roomId"), componentTestRoom["id"]}, 
        object::pair{std:("worldId"), componentTestWorld["id"]}, 
        object::pair{std:("type"), std:("world_component")}, 
        object::pair{std:("data"), object{
            object::pair{std:("worldValue"), 42}
        }}, 
        object::pair{std:("sourceEntityId"), as<any>(undefined)}, 
        object::pair{std:("createdAt"), Date->now()}
    })}, 
    object::pair{std:("withSourceEntity"), as<std::shared_ptr<Component>>(object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std:("entityId"), componentTestEntity["id"]}, 
        object::pair{std:("agentId"), componentTestAgentSettings->id}, 
        object::pair{std:("roomId"), componentTestRoom["id"]}, 
        object::pair{std:("sourceEntityId"), componentTestSourceEntity["id"]}, 
        object::pair{std:("type"), std:("source_component")}, 
        object::pair{std:("data"), object{
            object::pair{std:("sourceValue"), std:("from source entity")}
        }}, 
        object::pair{std:("worldId"), as<any>(undefined)}, 
        object::pair{std:("createdAt"), Date->now()}
    })}
};

void Main(void)
{
}

MAIN
