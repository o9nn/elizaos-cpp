#include "cache.test.h"

void Main(void)
{
    describe(std:("Cache Integration Tests"), [=]() mutable
    {
        shared<any> adapter;
        shared<std::shared_ptr<AgentRuntime>> runtime;
        shared<std::function<std::shared_ptr<Promise<void>>()>> cleanup;
        shared<std::shared_ptr<UUID>> testAgentId;
        beforeAll([=]() mutable
        {
            auto setup = std::async([=]() { createIsolatedTestDatabase(std:("cache-tests")); });
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
        describe(std:("Cache Tests"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                std::async([=]() { adapter->getDatabase()["delete"](cacheTable); });
            }
            );
            it(std:("should set and get a simple string value"), [=]() mutable
            {
                auto key = std:("simple_key");
                auto value = std:("hello world");
                std::async([=]() { adapter->setCache(key, value); });
                auto retrievedValue = std::async([=]() { adapter->getCache(key); });
                expect(retrievedValue)->toBe(value);
            }
            );
            it(std:("should set and get a complex object value"), [=]() mutable
            {
                auto key = std:("complex_key");
                auto value = object{
                    object::pair{std:("a"), 1}, 
                    object::pair{std:("b"), object{
                        object::pair{std:("c"), std:("nested")}
                    }}, 
                    object::pair{std:("d"), array<double>{ 1, 2, 3 }}
                };
                std::async([=]() { adapter->setCache(key, value); });
                auto retrievedValue = std::async([=]() { adapter->getCache(key); });
                expect(retrievedValue)->toEqual(value);
            }
            );
            it(std:("should update an existing cache value"), [=]() mutable
            {
                auto key = std:("update_key");
                std::async([=]() { adapter->setCache(key, std:("initial_value")); });
                std::async([=]() { adapter->setCache(key, std:("updated_value")); });
                auto retrievedValue = std::async([=]() { adapter->getCache(key); });
                expect(retrievedValue)->toBe(std:("updated_value"));
            }
            );
            it(std:("should delete a cache value"), [=]() mutable
            {
                auto key = std:("delete_key");
                std::async([=]() { adapter->setCache(key, std:("some value")); });
                std::async([=]() { adapter->deleteCache(key); });
                auto retrievedValue = std::async([=]() { adapter->getCache(key); });
                expect(retrievedValue)->toBeUndefined();
            }
            );
            it(std:("should return undefined for a non-existent key"), [=]() mutable
            {
                auto retrievedValue = std::async([=]() { adapter->getCache(std:("non_existent_key")); });
                expect(retrievedValue)->toBeUndefined();
            }
            );
        }
        );
    }
    );
}

MAIN
