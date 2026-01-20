#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/entity-seed.h"

any entityTestAgentSettings = as<std::shared_ptr<Agent>>(object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std::string("name"), std::string("Entity Test Agent")}, 
    object::pair{std::string("username"), std::string("entity_test_agent")}, 
    object::pair{std::string("system"), std::string("Test agent system prompt")}, 
    object::pair{std::string("bio"), std::string("An agent for testing entity operations")}, 
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
        object::pair{std::string("entityTestSetting"), std::string("entity test value")}
    }}, 
    object::pair{std::string("style"), object{
        object::pair{std::string("all"), array<any>()}, 
        object::pair{std::string("chat"), array<any>()}, 
        object::pair{std::string("post"), array<any>()}
    }}
});
object testEntities = object{
    object::pair{std::string("basicEntity"), object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std::string("names"), array<string>{ std::string("Basic Entity") }}, 
        object::pair{std::string("metadata"), object{
            object::pair{std::string("type"), std::string("basic")}, 
            object::pair{std::string("description"), std::string("A basic entity for testing")}
        }}, 
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(entityTestAgentSettings->id)}
    }}, 
    object::pair{std::string("complexEntity"), object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std::string("names"), array<string>{ std::string("Complex Entity"), std::string("Alternative Name") }}, 
        object::pair{std::string("metadata"), object{
            object::pair{std::string("type"), std::string("complex")}, 
            object::pair{std::string("description"), std::string("A complex entity for testing")}, 
            object::pair{std::string("properties"), object{
                object::pair{std::string("strength"), 10}, 
                object::pair{std::string("intelligence"), 15}, 
                object::pair{std::string("isSpecial"), true}
            }}, 
            object::pair{std::string("tags"), array<string>{ std::string("test"), std::string("entity"), std::string("complex") }}
        }}, 
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(entityTestAgentSettings->id)}
    }}, 
    object::pair{std::string("entityToUpdate"), object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std::string("names"), array<string>{ std::string("Entity to Update") }}, 
        object::pair{std::string("metadata"), object{
            object::pair{std::string("type"), std::string("updatable")}, 
            object::pair{std::string("version"), 1}
        }}, 
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(entityTestAgentSettings->id)}
    }}, 
    object::pair{std::string("entityWithComponent"), object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std::string("names"), array<string>{ std::string("Entity with Component") }}, 
        object::pair{std::string("metadata"), object{
            object::pair{std::string("type"), std::string("component-holder")}
        }}, 
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(entityTestAgentSettings->id)}
    }}
};

void Main(void)
{
}

MAIN
