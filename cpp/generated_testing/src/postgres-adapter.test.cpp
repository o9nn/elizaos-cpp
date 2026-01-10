#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/postgres-adapter.test.h"

void Main(void)
{
    describe(std::string("PostgreSQL Adapter Integration Tests"), [=]() mutable
    {
        shared<std::shared_ptr<PgliteDatabaseAdapter>> adapter;
        shared<std::shared_ptr<PGliteClientManager>> manager;
        shared<std::function<std::shared_ptr<Promise<void>>()>> cleanup;
        shared<std::shared_ptr<UUID>> agentId;
        beforeEach([=]() mutable
        {
            agentId = as<std::shared_ptr<UUID>>(uuidv4());
            auto client = std::make_shared<PGlite>();
            manager = std::make_shared<PGliteClientManager>(client);
            adapter = std::make_shared<PgliteDatabaseAdapter>(agentId, manager);
            std::async([=]() { adapter->init(); });
            auto migrationService = std::make_shared<DatabaseMigrationService>();
            auto db = adapter->getDatabase();
            std::async([=]() { migrationService->initializeWithDatabase(db); });
            migrationService->discoverAndRegisterPluginSchemas(array<object>{ object{
                object::pair{std::string("name"), std::string("@elizaos/plugin-sql")}, 
                object::pair{std::string("description"), std::string("SQL plugin")}, 
                object::pair{std::string("schema"), std::string("schema")}
            } });
            std::async([=]() { migrationService->runAllPluginMigrations(); });
            cleanup = [=]() mutable
            {
                std::async([=]() { adapter->close(); });
            };
        }
        );
        afterEach([=]() mutable
        {
            if (cleanup) {
                std::async([=]() { cleanup(); });
            }
        }
        );
        describe(std::string("Connection Management"), [=]() mutable
        {
            it(std::string("should initialize successfully"), [=]() mutable
            {
                auto isReady = std::async([=]() { adapter->isReady(); });
                expect(isReady)->toBe(true);
            }
            );
            it(std::string("should get database connection"), [=]() mutable
            {
                auto connection = std::async([=]() { adapter->getConnection(); });
                expect(connection)->toBeDefined();
            }
            );
            it(std::string("should close connection gracefully"), [=]() mutable
            {
                std::async([=]() { adapter->close(); });
                expect(true)->toBe(true);
            }
            );
            it(std::string("should handle isReady when adapter is closed"), [=]() mutable
            {
                std::async([=]() { adapter->close(); });
                auto isReady = std::async([=]() { adapter->isReady(); });
                expect(isReady)->toBe(false);
            }
            );
        }
        );
        describe(std::string("Database Operations"), [=]() mutable
        {
            it(std::string("should perform withDatabase operation"), [=]() mutable
            {
                auto result = std::async([=]() { (as<any>(adapter))["withDatabase"]([=]() mutable
                {
                    return std::string("success");
                }
                ); });
                expect(result)->toBe(std::string("success"));
            }
            );
            it(std::string("should handle withDatabase errors"), [=]() mutable
            {
                auto errorCaught = false;
                try
                {
                    std::async([=]() { (as<any>(adapter))["withDatabase"]([=]() mutable
                    {
                        throw any(std::make_shared<Error>(std::string("Test error")));
                    }
                    ); });
                }
                catch (const any& error)
                {
                    errorCaught = true;
                    expect((as<std::shared_ptr<Error>>(error))->message)->toBe(std::string("Test error"));
                }
                expect(errorCaught)->toBe(true);
            }
            );
            it(std::string("should handle database operations"), [=]() mutable
            {
                auto result = std::async([=]() { (as<any>(adapter))["withDatabase"]([=]() mutable
                {
                    return object{
                        object::pair{std::string("status"), std::string("ok")}
                    };
                }
                ); });
                expect(result)->toEqual(object{
                    object::pair{std::string("status"), std::string("ok")}
                });
            }
            );
            it(std::string("should propagate errors from database operations"), [=]() mutable
            {
                auto errorCaught = false;
                try
                {
                    std::async([=]() { (as<any>(adapter))["withDatabase"]([=]() mutable
                    {
                        throw any(std::make_shared<Error>(std::string("Database operation failed")));
                    }
                    ); });
                }
                catch (const any& error)
                {
                    errorCaught = true;
                    expect((as<std::shared_ptr<Error>>(error))->message)->toBe(std::string("Database operation failed"));
                }
                expect(errorCaught)->toBe(true);
            }
            );
        }
        );
        describe(std::string("Manager Operations"), [=]() mutable
        {
            it(std::string("should get connection instance"), [=]() mutable
            {
                auto connection = manager->getConnection();
                expect(connection)->toBeDefined();
            }
            );
            it(std::string("should check if shutting down"), [=]() mutable
            {
                auto isShuttingDown = manager->isShuttingDown();
                expect(isShuttingDown)->toBe(false);
            }
            );
            it(std::string("should handle close operation"), [=]() mutable
            {
                std::async([=]() { manager->close(); });
                auto isShuttingDown = manager->isShuttingDown();
                expect(isShuttingDown)->toBe(true);
            }
            );
            it(std::string("should test connection through adapter"), [=]() mutable
            {
                auto isReady = std::async([=]() { adapter->isReady(); });
                expect(isReady)->toBe(true);
            }
            );
            it(std::string("should handle connection errors"), [=]() mutable
            {
                std::async([=]() { adapter->close(); });
                auto isReady = std::async([=]() { adapter->isReady(); });
                expect(isReady)->toBe(false);
            }
            );
            it(std::string("should handle query failures"), [=]() mutable
            {
                auto mockClient = std::make_shared<PGlite>();
                auto mockManager = std::make_shared<PGliteClientManager>(as<any>(mockClient));
                auto mockAdapter = std::make_shared<PgliteDatabaseAdapter>(as<std::shared_ptr<UUID>>(uuidv4()), mockManager);
                std::async([=]() { mockManager->close(); });
                auto isReady = std::async([=]() { mockAdapter->isReady(); });
                expect(isReady)->toBe(false);
            }
            );
        }
        );
        describe(std::string("Agent Operations"), [=]() mutable
        {
            it(std::string("should create an agent"), [=]() mutable
            {
                auto result = std::async([=]() { adapter->createAgent(as<any>(object{
                    object::pair{std::string("id"), agentId}, 
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
                    object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->getTime()}, 
                    object::pair{std::string("bio"), std::string("Test agent bio")}
                })); });
                expect(result)->toBe(true);
            }
            );
            it(std::string("should retrieve an agent"), [=]() mutable
            {
                std::async([=]() { adapter->createAgent(as<any>(object{
                    object::pair{std::string("id"), agentId}, 
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
                    object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->getTime()}, 
                    object::pair{std::string("bio"), std::string("Test agent bio")}
                })); });
                auto agent = std::async([=]() { adapter->getAgent(agentId); });
                expect(agent)->toBeDefined();
                expect(agent["name"])->toBe(std::string("Test Agent"));
            }
            );
            it(std::string("should update an agent"), [=]() mutable
            {
                std::async([=]() { adapter->createAgent(as<any>(object{
                    object::pair{std::string("id"), agentId}, 
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
                    object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->getTime()}, 
                    object::pair{std::string("bio"), std::string("Test agent bio")}
                })); });
                std::async([=]() { adapter->updateAgent(agentId, object{
                    object::pair{std::string("name"), std::string("Updated Agent")}
                }); });
                auto agent = std::async([=]() { adapter->getAgent(agentId); });
                expect(agent["name"])->toBe(std::string("Updated Agent"));
            }
            );
            it(std::string("should delete an agent"), [=]() mutable
            {
                std::async([=]() { adapter->createAgent(as<any>(object{
                    object::pair{std::string("id"), agentId}, 
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
                    object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->getTime()}, 
                    object::pair{std::string("bio"), std::string("Test agent bio")}
                })); });
                auto deleted = std::async([=]() { adapter->deleteAgent(agentId); });
                expect(deleted)->toBe(true);
                auto agent = std::async([=]() { adapter->getAgent(agentId); });
                expect(agent)->toBeNull();
            }
            );
        }
        );
    }
    );
}

MAIN
