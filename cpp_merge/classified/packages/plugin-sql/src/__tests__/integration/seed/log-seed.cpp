#include "log-seed.h"

any logTestAgentSettings = as<std::shared_ptr<Agent>>(object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std:("name"), std:("Log Test Agent")}, 
    object::pair{std:("username"), std:("log_test_agent")}, 
    object::pair{std:("system"), std:("Test agent system prompt")}, 
    object::pair{std:("bio"), std:("An agent for testing log operations")}, 
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
        object::pair{std:("logTestSetting"), std:("log test value")}
    }}, 
    object::pair{std:("style"), object{
        object::pair{std:("all"), array<any>()}, 
        object::pair{std:("chat"), array<any>()}, 
        object::pair{std:("post"), array<any>()}
    }}
});
object logTestWorld = object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std:("agentId"), logTestAgentSettings->id}, 
    object::pair{std:("serverId"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std:("name"), std:("Log Test World")}, 
    object::pair{std:("description"), std:("A world for log integration tests")}, 
    object::pair{std:("metadata"), object{}}
};
object logTestEntity = object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std:("names"), array<string>{ std:("Log Test Entity") }}, 
    object::pair{std:("agentId"), logTestAgentSettings->id}, 
    object::pair{std:("metadata"), object{}}
};
object logTestRoom = object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std:("name"), std:("Log Test Room")}, 
    object::pair{std:("type"), ChannelType->DM}, 
    object::pair{std:("agentId"), logTestAgentSettings->id}, 
    object::pair{std:("worldId"), logTestWorld["id"]}, 
    object::pair{std:("source"), std:("log-test")}, 
    object::pair{std:("metadata"), object{}}
};
object logTestLogs = object{
    object::pair{std:("basic"), as<std::shared_ptr<Log>>(object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std:("entityId"), logTestEntity["id"]}, 
        object::pair{std:("roomId"), logTestRoom["id"]}, 
        object::pair{std:("body"), object{
            object::pair{std:("message"), std:("Test log message")}, 
            object::pair{std:("data"), object{
                object::pair{std:("key"), std:("value")}
            }}
        }}, 
        object::pair{std:("type"), std:("test_log")}, 
        object::pair{std:("createdAt"), std::make_shared<Date>()}
    })}, 
    object::pair{std:("withMetadata"), as<std::shared_ptr<Log>>(object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
        object::pair{std:("entityId"), logTestEntity["id"]}, 
        object::pair{std:("roomId"), logTestRoom["id"]}, 
        object::pair{std:("body"), object{
            object::pair{std:("message"), std:("Log with extra metadata")}, 
            object::pair{std:("metadata"), object{
                object::pair{std:("priority"), std:("high")}, 
                object::pair{std:("source"), std:("test_suite")}
            }}
        }}, 
        object::pair{std:("type"), std:("metadata_log")}, 
        object::pair{std:("createdAt"), std::make_shared<Date>()}
    })}
};

void Main(void)
{
}

MAIN
