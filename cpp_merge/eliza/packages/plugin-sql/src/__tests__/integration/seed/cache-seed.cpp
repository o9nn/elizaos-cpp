#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/cache-seed.h"

any cacheTestAgentSettings = as<std::shared_ptr<Agent>>(object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std:("name"), std:("Cache Test Agent")}, 
    object::pair{std:("username"), std:("cache_test_agent")}, 
    object::pair{std:("system"), std:("Test agent system prompt")}, 
    object::pair{std:("bio"), std:("An agent for testing cache operations")}, 
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
        object::pair{std:("cacheTestSetting"), std:("cache test value")}
    }}, 
    object::pair{std:("style"), object{
        object::pair{std:("all"), array<any>()}, 
        object::pair{std:("chat"), array<any>()}, 
        object::pair{std:("post"), array<any>()}
    }}
});
object testCacheEntries = object{
    object::pair{std:("stringValue"), object{
        object::pair{std:("key"), std:("test_string")}, 
        object::pair{std:("value"), std:("test value")}
    }}, 
    object::pair{std:("numberValue"), object{
        object::pair{std:("key"), std:("test_number")}, 
        object::pair{std:("value"), 42}
    }}, 
    object::pair{std:("objectValue"), object{
        object::pair{std:("key"), std:("test_object")}, 
        object::pair{std:("value"), object{
            object::pair{std:("name"), std:("Test Object")}, 
            object::pair{std:("properties"), object{
                object::pair{std:("id"), 1}, 
                object::pair{std:("active"), true}
            }}, 
            object::pair{std:("tags"), array<string>{ std:("test"), std:("cache"), std:("integration") }}
        }}
    }}, 
    object::pair{std:("arrayValue"), object{
        object::pair{std:("key"), std:("test_array")}, 
        object::pair{std:("value"), array<double>{ 1, 2, 3, std:("four"), object{
            object::pair{std:("five"), 5}
        } }}
    }}
};
object testCacheWithExpiration = object{
    object::pair{std:("expired"), object{
        object::pair{std:("key"), std:("expired_cache")}, 
        object::pair{std:("value"), std:("This value has expired")}, 
        object::pair{std:("expiresAt"), Date->now() - 60 * 60 * 1000}
    }}, 
    object::pair{std:("notExpired"), object{
        object::pair{std:("key"), std:("not_expired_cache")}, 
        object::pair{std:("value"), std:("This value has not expired")}, 
        object::pair{std:("expiresAt"), Date->now() + 60 * 60 * 1000}
    }}
};

void Main(void)
{
}

MAIN
