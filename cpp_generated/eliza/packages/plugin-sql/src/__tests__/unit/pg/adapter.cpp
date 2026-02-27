#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/unit/pg/adapter.test.h"

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
    describe(std::string("PgDatabaseAdapter"), [=]() mutable
    {
        shared<std::shared_ptr<PgDatabaseAdapter>> adapter;
        shared<any> mockManager;
        shared agentId = std::string("00000000-0000-0000-0000-000000000000");
        beforeEach([=]() mutable
        {
            (as<any>(logger->debug))["mockClear"]();
            (as<any>(logger->info))["mockClear"]();
            (as<any>(logger->warn))["mockClear"]();
            (as<any>(logger->error))["mockClear"]();
            mockManager = object{
                object::pair{std::string("getDatabase"), mock([=]() mutable
                {
                    return (object{
                        object::pair{std::string("query"), object{}}, 
                        object::pair{std::string("transaction"), mock([=]() mutable
                        {
                        }
                        )}
                    });
                }
                )}, 
                object::pair{std::string("getClient"), mock([=]() mutable
                {
                }
                )}, 
                object::pair{std::string("testConnection"), mock([=]() mutable
                {
                    return Promise->resolve(true);
                }
                )}, 
                object::pair{std::string("close"), mock([=]() mutable
                {
                    return Promise->resolve();
                }
                )}, 
                object::pair{std::string("getConnection"), mock([=]() mutable
                {
                    return (object{
                        object::pair{std::string("connect"), mock([=]() mutable
                        {
                        }
                        )}, 
                        object::pair{std::string("end"), mock([=]() mutable
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
                expect(logger->debug)->toHaveBeenCalledWith(std::string("PgDatabaseAdapter: Migrations should be handled externally"));
            }
            );
        }
        );
        describe(std::string("init"), [=]() mutable
        {
            it(std::string("should complete initialization"), [=]() mutable
            {
                std::async([=]() { adapter->init(); });
                expect(logger->debug)->toHaveBeenCalledWith(std::string("PgDatabaseAdapter initialized, skipping automatic migrations."));
            }
            );
        }
        );
        describe(std::string("isReady"), [=]() mutable
        {
            it(std::string("should return true when connection is healthy"), [=]() mutable
            {
                mockManager["testConnection"]["mockResolvedValue"](true);
                auto result = std::async([=]() { adapter->isReady(); });
                expect(result)->toBe(true);
                expect(mockManager["testConnection"])->toHaveBeenCalled();
            }
            );
            it(std::string("should return false when connection is unhealthy"), [=]() mutable
            {
                mockManager["testConnection"]["mockResolvedValue"](false);
                auto result = std::async([=]() { adapter->isReady(); });
                expect(result)->toBe(false);
                expect(mockManager["testConnection"])->toHaveBeenCalled();
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
            it(std::string("should handle close errors gracefully"), [=]() mutable
            {
                mockManager["close"]["mockRejectedValue"](std::make_shared<Error>(std::string("Close failed")));
                std::async([=]() { expect(adapter->close())->rejects->toThrow(std::string("Close failed")); });
            }
            );
        }
        );
        describe(std::string("getConnection"), [=]() mutable
        {
            it(std::string("should return connection from manager"), [=]() mutable
            {
                auto mockConnection = object{
                    object::pair{std::string("connect"), mock()}, 
                    object::pair{std::string("end"), mock()}
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
            it(std::string("should handle database operation errors"), [=]() mutable
            {
                expect(adapter)->toBeDefined();
                expect((as<any>(adapter))["manager"])->toBe(mockManager);
            }
            );
            it(std::string("should use the database from manager"), [=]() mutable
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
