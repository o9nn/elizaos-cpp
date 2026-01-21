#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/cache-seed.h"

any cacheTestAgentSettings = as<std::shared_ptr<Agent>>(object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(v4())}, 
    object::pair{std::string("name"), std::string("Cache Test Agent")}, 
    object::pair{std::string("username"), std::string("cache_test_agent")}, 
    object::pair{std::string("system"), std::string("Test agent system prompt")}, 
    object::pair{std::string("bio"), std::string("An agent for testing cache operations")}, 
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
        object::pair{std::string("cacheTestSetting"), std::string("cache test value")}
    }}, 
    object::pair{std::string("style"), object{
        object::pair{std::string("all"), array<any>()}, 
        object::pair{std::string("chat"), array<any>()}, 
        object::pair{std::string("post"), array<any>()}
    }}
});
object testCacheEntries = object{
    object::pair{std::string("stringValue"), object{
        object::pair{std::string("key"), std::string("test_string")}, 
        object::pair{std::string("value"), std::string("test value")}
    }}, 
    object::pair{std::string("numberValue"), object{
        object::pair{std::string("key"), std::string("test_number")}, 
        object::pair{std::string("value"), 42}
    }}, 
    object::pair{std::string("objectValue"), object{
        object::pair{std::string("key"), std::string("test_object")}, 
        object::pair{std::string("value"), object{
            object::pair{std::string("name"), std::string("Test Object")}, 
            object::pair{std::string("properties"), object{
                object::pair{std::string("id"), 1}, 
                object::pair{std::string("active"), true}
            }}, 
            object::pair{std::string("tags"), array<string>{ std::string("test"), std::string("cache"), std::string("integration") }}
        }}
    }}, 
    object::pair{std::string("arrayValue"), object{
        object::pair{std::string("key"), std::string("test_array")}, 
        object::pair{std::string("value"), array<double>{ 1, 2, 3, std::string("four"), object{
            object::pair{std::string("five"), 5}
        } }}
    }}
};
object testCacheWithExpiration = object{
    object::pair{std::string("expired"), object{
        object::pair{std::string("key"), std::string("expired_cache")}, 
        object::pair{std::string("value"), std::string("This value has expired")}, 
        object::pair{std::string("expiresAt"), Date->now() - 60 * 60 * 1000}
    }}, 
    object::pair{std::string("notExpired"), object{
        object::pair{std::string("key"), std::string("not_expired_cache")}, 
        object::pair{std::string("value"), std::string("This value has not expired")}, 
        object::pair{std::string("expiresAt"), Date->now() + 60 * 60 * 1000}
    }}
};

void Main(void)
{
}

MAIN
