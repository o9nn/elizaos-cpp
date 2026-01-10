#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/log-seed.h"

any logTestAgentSettings = as<std::shared_ptr<Agent>>(object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std::string("name"), std::string("Log Test Agent")}, 
    object::pair{std::string("username"), std::string("log_test_agent")}, 
    object::pair{std::string("system"), std::string("Test agent system prompt")}, 
    object::pair{std::string("bio"), std::string("An agent for testing log operations")}, 
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
        object::pair{std::string("logTestSetting"), std::string("log test value")}
    }}, 
    object::pair{std::string("style"), object{
        object::pair{std::string("all"), array<any>()}, 
        object::pair{std::string("chat"), array<any>()}, 
        object::pair{std::string("post"), array<any>()}
    }}
});
object logTestWorld = object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std::string("agentId"), logTestAgentSettings->id}, 
    object::pair{std::string("serverId"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std::string("name"), std::string("Log Test World")}, 
    object::pair{std::string("description"), std::string("A world for log integration tests")}, 
    object::pair{std::string("metadata"), object{}}
};
object logTestEntity = object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std::string("names"), array<string>{ std::string("Log Test Entity") }}, 
    object::pair{std::string("agentId"), logTestAgentSettings->id}, 
    object::pair{std::string("metadata"), object{}}
};
object logTestRoom = object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std::string("name"), std::string("Log Test Room")}, 
    object::pair{std::string("type"), ChannelType->DM}, 
    object::pair{std::string("agentId"), logTestAgentSettings->id}, 
    object::pair{std::string("worldId"), logTestWorld["id"]}, 
    object::pair{std::string("source"), std::string("log-test")}, 
    object::pair{std::string("metadata"), object{}}
};
object logTestLogs = object{
    object::pair{std::string("basic"), as<std::shared_ptr<Log>>(object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std::string("entityId"), logTestEntity["id"]}, 
        object::pair{std::string("roomId"), logTestRoom["id"]}, 
        object::pair{std::string("body"), object{
            object::pair{std::string("message"), std::string("Test log message")}, 
            object::pair{std::string("data"), object{
                object::pair{std::string("key"), std::string("value")}
            }}
        }}, 
        object::pair{std::string("type"), std::string("test_log")}, 
        object::pair{std::string("createdAt"), std::make_shared<Date>()}
    })}, 
    object::pair{std::string("withMetadata"), as<std::shared_ptr<Log>>(object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std::string("entityId"), logTestEntity["id"]}, 
        object::pair{std::string("roomId"), logTestRoom["id"]}, 
        object::pair{std::string("body"), object{
            object::pair{std::string("message"), std::string("Log with extra metadata")}, 
            object::pair{std::string("metadata"), object{
                object::pair{std::string("priority"), std::string("high")}, 
                object::pair{std::string("source"), std::string("test_suite")}
            }}
        }}, 
        object::pair{std::string("type"), std::string("metadata_log")}, 
        object::pair{std::string("createdAt"), std::make_shared<Date>()}
    })}
};

void Main(void)
{
}

MAIN
