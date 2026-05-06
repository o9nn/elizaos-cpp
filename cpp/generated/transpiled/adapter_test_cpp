#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/unit/pglite/adapter.test.h"

void Main(void)
{
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std::string("logger"), object{
                object::pair{std::string("debug"), mock()}, 
                object::pair{std::string("info"), mock()}, 
                object::pair{std::string("warn"), mock()}, 
                object::pair{std::string("error"), mock()}
            }}
        });
    }
    );
    describe(std::string("PgliteDatabaseAdapter"), [=]() mutable
    {
        shared<std::shared_ptr<PgliteDatabaseAdapter>> adapter;
        shared<any> mockManager;
        shared agentId = std::string("00000000-0000-0000-0000-000000000000");
        beforeEach([=]() mutable
        {
            (as<any>(logger->debug))["mockClear"]();
            (as<any>(logger->info))["mockClear"]();
            (as<any>(logger->warn))["mockClear"]();
            (as<any>(logger->error))["mockClear"]();
            mockManager = object{
                object::pair{std::string("getConnection"), mock()->mockReturnValue(object{
                    object::pair{std::string("query"), mock()->mockResolvedValue(object{
                        object::pair{std::string("rows"), array<any>()}
                    })}, 
                    object::pair{std::string("close"), mock()->mockResolvedValue(undefined)}, 
                    object::pair{std::string("transaction"), mock()}
                })}, 
                object::pair{std::string("close"), mock()->mockResolvedValue(undefined)}, 
                object::pair{std::string("isShuttingDown"), mock()->mockReturnValue(false)}
            };
            adapter = std::make_shared<PgliteDatabaseAdapter>(agentId, mockManager);
        }
        );
        describe(std::string("constructor"), [=]() mutable
        {
            it(std::string("should initialize with correct agentId and manager"), [=]() mutable
            {
                expect(adapter)->toBeDefined();
                expect((as<any>(adapter))["agentId"])->toBe(agentId);
                expect((as<any>(adapter))["manager"])->toBe(mockManager);
            }
            );
            it(std::string("should set embeddingDimension to default 384"), [=]() mutable
            {
                expect((as<any>(adapter))["embeddingDimension"])->toBe(std::string("dim384"));
            }
            );
        }
        );
        describe(std::string("runMigrations"), [=]() mutable
        {
            it(std::string("should be a no-op"), [=]() mutable
            {
                std::async([=]() { adapter->runMigrations(); });
                expect(logger->debug)->toHaveBeenCalledWith(std::string("PgliteDatabaseAdapter: Migrations are handled by the migration service"));
            }
            );
        }
        );
        describe(std::string("init"), [=]() mutable
        {
            it(std::string("should complete initialization"), [=]() mutable
            {
                std::async([=]() { adapter->init(); });
                expect(logger->debug)->toHaveBeenCalledWith(std::string("PGliteDatabaseAdapter initialized, skipping automatic migrations."));
            }
            );
        }
        );
        describe(std::string("close"), [=]() mutable
        {
            it(std::string("should close the manager"), [=]() mutable
            {
                std::async([=]() { adapter->close(); });
                expect(mockManager["close"])->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std::string("isReady"), [=]() mutable
        {
            it(std::string("should return true when manager is not shutting down"), [=]() mutable
            {
                mockManager["isShuttingDown"]["mockReturnValue"](false);
                auto result = std::async([=]() { adapter->isReady(); });
                expect(result)->toBe(true);
            }
            );
            it(std::string("should return false when manager is shutting down"), [=]() mutable
            {
                mockManager["isShuttingDown"]["mockReturnValue"](true);
                auto result = std::async([=]() { adapter->isReady(); });
                expect(result)->toBe(false);
            }
            );
        }
        );
        describe(std::string("getConnection"), [=]() mutable
        {
            it(std::string("should return the connection from manager"), [=]() mutable
            {
                auto mockConnection = object{
                    object::pair{std::string("query"), mock()}, 
                    object::pair{std::string("close"), mock()}
                };
                mockManager["getConnection"]["mockReturnValue"](mockConnection);
                auto result = std::async([=]() { adapter->getConnection(); });
                expect(result)->toBe(as<any>(mockConnection));
                expect(mockManager["getConnection"])->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std::string("database operations"), [=]() mutable
        {
            it(std::string("should use the connection from manager for operations"), [=]() mutable
            {
                auto mockConnection = mockManager["getConnection"]();
                expect(mockConnection)->toBeDefined();
                expect(mockConnection["query"])->toBeDefined();
                expect(mockConnection["transaction"])->toBeDefined();
            }
            );
            it(std::string("should handle query errors gracefully"), [=]() mutable
            {
                auto mockConnection = object{
                    object::pair{std::string("query"), mock()->mockRejectedValue(std::make_shared<Error>(std::string("Query failed")))}
                };
                mockManager["getConnection"]["mockReturnValue"](mockConnection);
                auto connection = std::async([=]() { adapter->getConnection(); });
                std::async([=]() { expect(connection->query(std::string("SELECT 1")))->rejects->toThrow(std::string("Query failed")); });
            }
            );
        }
        );
    }
    );
}

MAIN
