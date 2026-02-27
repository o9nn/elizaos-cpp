#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/entity-seed.h"

any entityTestAgentSettings = as<std::shared_ptr<Agent>>(object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std:("name"), std:("Entity Test Agent")}, 
    object::pair{std:("username"), std:("entity_test_agent")}, 
    object::pair{std:("system"), std:("Test agent system prompt")}, 
    object::pair{std:("bio"), std:("An agent for testing entity operations")}, 
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
        object::pair{std:("entityTestSetting"), std:("entity test value")}
    }}, 
    object::pair{std:("style"), object{
        object::pair{std:("all"), array<any>()}, 
        object::pair{std:("chat"), array<any>()}, 
        object::pair{std:("post"), array<any>()}
    }}
});
object testEntities = object{
    object::pair{std:("basicEntity"), object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std:("names"), array<string>{ std:("Basic Entity") }}, 
        object::pair{std:("metadata"), object{
            object::pair{std:("type"), std:("basic")}, 
            object::pair{std:("description"), std:("A basic entity for testing")}
        }}, 
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(entityTestAgentSettings->id)}
    }}, 
    object::pair{std:("complexEntity"), object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std:("names"), array<string>{ std:("Complex Entity"), std:("Alternative Name") }}, 
        object::pair{std:("metadata"), object{
            object::pair{std:("type"), std:("complex")}, 
            object::pair{std:("description"), std:("A complex entity for testing")}, 
            object::pair{std:("properties"), object{
                object::pair{std:("strength"), 10}, 
                object::pair{std:("intelligence"), 15}, 
                object::pair{std:("isSpecial"), true}
            }}, 
            object::pair{std:("tags"), array<string>{ std:("test"), std:("entity"), std:("complex") }}
        }}, 
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(entityTestAgentSettings->id)}
    }}, 
    object::pair{std:("entityToUpdate"), object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std:("names"), array<string>{ std:("Entity to Update") }}, 
        object::pair{std:("metadata"), object{
            object::pair{std:("type"), std:("updatable")}, 
            object::pair{std:("version"), 1}
        }}, 
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(entityTestAgentSettings->id)}
    }}, 
    object::pair{std:("entityWithComponent"), object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std:("names"), array<string>{ std:("Entity with Component") }}, 
        object::pair{std:("metadata"), object{
            object::pair{std:("type"), std:("component-holder")}
        }}, 
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(entityTestAgentSettings->id)}
    }}
};

void Main(void)
{
}

MAIN
