#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/unit/pglite/adapter.test.h"

void Main(void)
{
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std:("logger"), object{
                object::pair{std:("debug"), mock()}, 
                object::pair{std:("info"), mock()}, 
                object::pair{std:("warn"), mock()}, 
                object::pair{std:("error"), mock()}
            }}
        });
    }
    );
    describe(std:("PgliteDatabaseAdapter"), [=]() mutable
    {
        shared<std::shared_ptr<PgliteDatabaseAdapter>> adapter;
        shared<any> mockManager;
        shared agentId = std:("00000000-0000-0000-0000-000000000000");
        beforeEach([=]() mutable
        {
            (as<any>(logger->debug))["mockClear"]();
            (as<any>(logger->info))["mockClear"]();
            (as<any>(logger->warn))["mockClear"]();
            (as<any>(logger->error))["mockClear"]();
            mockManager = object{
                object::pair{std:("getConnection"), mock()->mockReturnValue(object{
                    object::pair{std:("query"), mock()->mockResolvedValue(object{
                        object::pair{std:("rows"), array<any>()}
                    })}, 
                    object::pair{std:("close"), mock()->mockResolvedValue(undefined)}, 
                    object::pair{std:("transaction"), mock()}
                })}, 
                object::pair{std:("close"), mock()->mockResolvedValue(undefined)}, 
                object::pair{std:("isShuttingDown"), mock()->mockReturnValue(false)}
            };
            adapter = std::make_shared<PgliteDatabaseAdapter>(agentId, mockManager);
        }
        );
        describe(std:("constructor"), [=]() mutable
        {
            it(std:("should initialize with correct agentId and manager"), [=]() mutable
            {
                expect(adapter)->toBeDefined();
                expect((as<any>(adapter))["agentId"])->toBe(agentId);
                expect((as<any>(adapter))["manager"])->toBe(mockManager);
            }
            );
            it(std:("should set embeddingDimension to default 384"), [=]() mutable
            {
                expect((as<any>(adapter))["embeddingDimension"])->toBe(std:("dim384"));
            }
            );
        }
        );
        describe(std:("runMigrations"), [=]() mutable
        {
            it(std:("should be a no-op"), [=]() mutable
            {
                std::async([=]() { adapter->runMigrations(); });
                expect(logger->debug)->toHaveBeenCalledWith(std:("PgliteDatabaseAdapter: Migrations are handled by the migration service"));
            }
            );
        }
        );
        describe(std:("init"), [=]() mutable
        {
            it(std:("should complete initialization"), [=]() mutable
            {
                std::async([=]() { adapter->init(); });
                expect(logger->debug)->toHaveBeenCalledWith(std:("PGliteDatabaseAdapter initialized, skipping automatic migrations."));
            }
            );
        }
        );
        describe(std:("close"), [=]() mutable
        {
            it(std:("should close the manager"), [=]() mutable
            {
                std::async([=]() { adapter->close(); });
                expect(mockManager["close"])->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std:("isReady"), [=]() mutable
        {
            it(std:("should return true when manager is not shutting down"), [=]() mutable
            {
                mockManager["isShuttingDown"]["mockReturnValue"](false);
                auto result = std::async([=]() { adapter->isReady(); });
                expect(result)->toBe(true);
            }
            );
            it(std:("should return false when manager is shutting down"), [=]() mutable
            {
                mockManager["isShuttingDown"]["mockReturnValue"](true);
                auto result = std::async([=]() { adapter->isReady(); });
                expect(result)->toBe(false);
            }
            );
        }
        );
        describe(std:("getConnection"), [=]() mutable
        {
            it(std:("should return the connection from manager"), [=]() mutable
            {
                auto mockConnection = object{
                    object::pair{std:("query"), mock()}, 
                    object::pair{std:("close"), mock()}
                };
                mockManager["getConnection"]["mockReturnValue"](mockConnection);
                auto result = std::async([=]() { adapter->getConnection(); });
                expect(result)->toBe(as<any>(mockConnection));
                expect(mockManager["getConnection"])->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std:("database operations"), [=]() mutable
        {
            it(std:("should use the connection from manager for operations"), [=]() mutable
            {
                auto mockConnection = mockManager["getConnection"]();
                expect(mockConnection)->toBeDefined();
                expect(mockConnection["query"])->toBeDefined();
                expect(mockConnection["transaction"])->toBeDefined();
            }
            );
            it(std:("should handle query errors gracefully"), [=]() mutable
            {
                auto mockConnection = object{
                    object::pair{std:("query"), mock()->mockRejectedValue(std::make_shared<Error>(std:("Query failed")))}
                };
                mockManager["getConnection"]["mockReturnValue"](mockConnection);
                auto connection = std::async([=]() { adapter->getConnection(); });
                std::async([=]() { expect(connection->query(std:("SELECT 1")))->rejects->toThrow(std:("Query failed")); });
            }
            );
        }
        );
    }
    );
}

MAIN
