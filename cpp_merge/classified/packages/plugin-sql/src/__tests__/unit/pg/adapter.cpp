#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sql/src/__tests__/unit/pg/adapter.test.h"

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
    describe(std:("PgDatabaseAdapter"), [=]() mutable
    {
        shared<std::shared_ptr<PgDatabaseAdapter>> adapter;
        shared<any> mockManager;
        shared agentId = std:("00000000-0000-0000-0000-000000000000");
        beforeEach([=]() mutable
        {
            (as<any>(logger->debug))["mockClear"]();
            (as<any>(logger->info))["mockClear"]();
            (as<any>(logger->warn))["mockClear"]();
            (as<any>(logger->error))["mockClear"]();
            mockManager = object{
                object::pair{std:("getDatabase"), mock([=]() mutable
                {
                    return (object{
                        object::pair{std:("query"), object{}}, 
                        object::pair{std:("transaction"), mock([=]() mutable
                        {
                        }
                        )}
                    });
                }
                )}, 
                object::pair{std:("getClient"), mock([=]() mutable
                {
                }
                )}, 
                object::pair{std:("testConnection"), mock([=]() mutable
                {
                    return Promise->resolve(true);
                }
                )}, 
                object::pair{std:("close"), mock([=]() mutable
                {
                    return Promise->resolve();
                }
                )}, 
                object::pair{std:("getConnection"), mock([=]() mutable
                {
                    return (object{
                        object::pair{std:("connect"), mock([=]() mutable
                        {
                        }
                        )}, 
                        object::pair{std:("end"), mock([=]() mutable
                        {
                        }
                        )}
                    });
                }
                )}
            };
            adapter = std::make_shared<PgDatabaseAdapter>(agentId, mockManager);
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
                expect(logger->debug)->toHaveBeenCalledWith(std:("PgDatabaseAdapter: Migrations should be handled externally"));
            }
            );
        }
        );
        describe(std:("init"), [=]() mutable
        {
            it(std:("should complete initialization"), [=]() mutable
            {
                std::async([=]() { adapter->init(); });
                expect(logger->debug)->toHaveBeenCalledWith(std:("PgDatabaseAdapter initialized, skipping automatic migrations."));
            }
            );
        }
        );
        describe(std:("isReady"), [=]() mutable
        {
            it(std:("should return true when connection is healthy"), [=]() mutable
            {
                mockManager["testConnection"]["mockResolvedValue"](true);
                auto result = std::async([=]() { adapter->isReady(); });
                expect(result)->toBe(true);
                expect(mockManager["testConnection"])->toHaveBeenCalled();
            }
            );
            it(std:("should return false when connection is unhealthy"), [=]() mutable
            {
                mockManager["testConnection"]["mockResolvedValue"](false);
                auto result = std::async([=]() { adapter->isReady(); });
                expect(result)->toBe(false);
                expect(mockManager["testConnection"])->toHaveBeenCalled();
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
            it(std:("should handle close errors gracefully"), [=]() mutable
            {
                mockManager["close"]["mockRejectedValue"](std::make_shared<Error>(std:("Close failed")));
                std::async([=]() { expect(adapter->close())->rejects->toThrow(std:("Close failed")); });
            }
            );
        }
        );
        describe(std:("getConnection"), [=]() mutable
        {
            it(std:("should return connection from manager"), [=]() mutable
            {
                auto mockConnection = object{
                    object::pair{std:("connect"), mock()}, 
                    object::pair{std:("end"), mock()}
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
            it(std:("should handle database operation errors"), [=]() mutable
            {
                expect(adapter)->toBeDefined();
                expect((as<any>(adapter))["manager"])->toBe(mockManager);
            }
            );
            it(std:("should use the database from manager"), [=]() mutable
            {
                auto db = mockManager["getDatabase"]();
                expect(db)->toBeDefined();
                expect(db["query"])->toBeDefined();
                expect(db["transaction"])->toBeDefined();
            }
            );
        }
        );
    }
    );
}

MAIN
