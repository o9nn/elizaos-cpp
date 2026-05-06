#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/cache.test.h"

void Main(void)
{
    describe(std::string("Cache Integration Tests"), [=]() mutable
    {
        shared<any> adapter;
        shared<std::shared_ptr<AgentRuntime>> runtime;
        shared<std::function<std::shared_ptr<Promise<void>>()>> cleanup;
        shared<std::shared_ptr<UUID>> testAgentId;
        beforeAll([=]() mutable
        {
            auto setup = std::async([=]() { createIsolatedTestDatabase(std::string("cache-tests")); });
            adapter = setup["adapter"];
            runtime = setup["runtime"];
            cleanup = setup["cleanup"];
            testAgentId = setup["testAgentId"];
        }
        );
        afterAll([=]() mutable
        {
            if (cleanup) {
                std::async([=]() { cleanup(); });
            }
        }
        );
        describe(std::string("Cache Tests"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                std::async([=]() { adapter->getDatabase()["delete"](cacheTable); });
            }
            );
            it(std::string("should set and get a simple string value"), [=]() mutable
            {
                auto key = std::string("simple_key");
                auto value = std::string("hello world");
                std::async([=]() { adapter->setCache(key, value); });
                auto retrievedValue = std::async([=]() { adapter->getCache(key); });
                expect(retrievedValue)->toBe(value);
            }
            );
            it(std::string("should set and get a complex object value"), [=]() mutable
            {
                auto key = std::string("complex_key");
                auto value = object{
                    object::pair{std::string("a"), 1}, 
                    object::pair{std::string("b"), object{
                        object::pair{std::string("c"), std::string("nested")}
                    }}, 
                    object::pair{std::string("d"), array<double>{ 1, 2, 3 }}
                };
                std::async([=]() { adapter->setCache(key, value); });
                auto retrievedValue = std::async([=]() { adapter->getCache(key); });
                expect(retrievedValue)->toEqual(value);
            }
            );
            it(std::string("should update an existing cache value"), [=]() mutable
            {
                auto key = std::string("update_key");
                std::async([=]() { adapter->setCache(key, std::string("initial_value")); });
                std::async([=]() { adapter->setCache(key, std::string("updated_value")); });
                auto retrievedValue = std::async([=]() { adapter->getCache(key); });
                expect(retrievedValue)->toBe(std::string("updated_value"));
            }
            );
            it(std::string("should delete a cache value"), [=]() mutable
            {
                auto key = std::string("delete_key");
                std::async([=]() { adapter->setCache(key, std::string("some value")); });
                std::async([=]() { adapter->deleteCache(key); });
                auto retrievedValue = std::async([=]() { adapter->getCache(key); });
                expect(retrievedValue)->toBeUndefined();
            }
            );
            it(std::string("should return undefined for a non-existent key"), [=]() mutable
            {
                auto retrievedValue = std::async([=]() { adapter->getCache(std::string("non_existent_key")); });
                expect(retrievedValue)->toBeUndefined();
            }
            );
        }
        );
    }
    );
}

MAIN
