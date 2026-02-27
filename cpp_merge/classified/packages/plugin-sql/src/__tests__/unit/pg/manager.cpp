#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sql/src/__tests__/unit/pg/manager.test.h"

object mockPoolInstance = object{
    object::pair{std:("connect"), mock()}, 
    object::pair{std:("end"), mock()}, 
    object::pair{std:("query"), mock()}
};

void Main(void)
{
    mock->module(std:("pg"), [=]() mutable
    {
        return (object{
            object::pair{std:("Pool"), mock([=]() mutable
            {
                return mockPoolInstance;
            }
            )}
        });
    }
    );
    describe(std:("PostgresConnectionManager"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            mockPoolInstance["connect"]->mockClear();
            mockPoolInstance["end"]->mockClear();
            mockPoolInstance["query"]->mockClear();
        }
        );
        describe(std:("constructor"), [=]() mutable
        {
            it(std:("should create an instance with connection URL"), [=]() mutable
            {
                auto connectionUrl = std:("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                expect(manager)->toBeDefined();
                expect(manager->getConnection())->toBeDefined();
                expect(manager->getDatabase())->toBeDefined();
            }
            );
        }
        );
        describe(std:("getDatabase"), [=]() mutable
        {
            it(std:("should return the drizzle database instance"), [=]() mutable
            {
                auto connectionUrl = std:("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                auto db = manager->getDatabase();
                expect(db)->toBeDefined();
                expect(db->query)->toBeDefined();
            }
            );
        }
        );
        describe(std:("getConnection"), [=]() mutable
        {
            it(std:("should return the pool instance"), [=]() mutable
            {
                auto connectionUrl = std:("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                auto connection = manager->getConnection();
                expect(connection)->toBeDefined();
                expect(connection)->toBe(as<any>(mockPoolInstance));
            }
            );
        }
        );
        describe(std:("getClient"), [=]() mutable
        {
            it(std:("should return a client from the pool"), [=]() mutable
            {
                auto connectionUrl = std:("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                auto mockClient = object{
                    object::pair{std:("query"), mock()->mockResolvedValue(object{
                        object::pair{std:("rows"), array<any>()}
                    })}, 
                    object::pair{std:("release"), mock()}
                };
                mockPoolInstance["connect"]->mockResolvedValue(mockClient);
                auto client = std::async([=]() { manager->getClient(); });
                expect(client)->toBe(as<any>(mockClient));
                expect(mockPoolInstance["connect"])->toHaveBeenCalled();
            }
            );
            it(std:("should throw error when pool connection fails"), [=]() mutable
            {
                auto connectionUrl = std:("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                mockPoolInstance["connect"]->mockRejectedValue(std::make_shared<Error>(std:("Connection failed")));
                std::async([=]() { expect(manager->getClient())->rejects->toThrow(std:("Connection failed")); });
            }
            );
        }
        );
        describe(std:("testConnection"), [=]() mutable
        {
            it(std:("should return true when connection is successful"), [=]() mutable
            {
                auto connectionUrl = std:("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                auto mockClient = object{
                    object::pair{std:("query"), mock()->mockResolvedValue(object{
                        object::pair{std:("rows"), array<any>()}
                    })}, 
                    object::pair{std:("release"), mock()}
                };
                mockPoolInstance["connect"]->mockResolvedValue(mockClient);
                auto result = std::async([=]() { manager->testConnection(); });
                expect(result)->toBe(true);
                expect(mockPoolInstance["connect"])->toHaveBeenCalled();
                expect(mockClient["query"])->toHaveBeenCalledWith(std:("SELECT 1"));
                expect(mockClient["release"])->toHaveBeenCalled();
            }
            );
            it(std:("should return false when connection fails"), [=]() mutable
            {
                auto connectionUrl = std:("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                mockPoolInstance["connect"]->mockRejectedValue(std::make_shared<Error>(std:("Connection failed")));
                auto result = std::async([=]() { manager->testConnection(); });
                expect(result)->toBe(false);
            }
            );
            it(std:("should return false when query fails"), [=]() mutable
            {
                auto connectionUrl = std:("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                auto mockClient = object{
                    object::pair{std:("query"), mock()->mockRejectedValue(std::make_shared<Error>(std:("Query failed")))}, 
                    object::pair{std:("release"), mock()}
                };
                mockPoolInstance["connect"]->mockResolvedValue(mockClient);
                auto result = std::async([=]() { manager->testConnection(); });
                expect(result)->toBe(false);
                expect(mockClient["release"])->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std:("close"), [=]() mutable
        {
            it(std:("should end the pool connection"), [=]() mutable
            {
                auto connectionUrl = std:("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                mockPoolInstance["end"]->mockResolvedValue(undefined);
                std::async([=]() { manager->close(); });
                expect(mockPoolInstance["end"])->toHaveBeenCalled();
            }
            );
            it(std:("should propagate errors during close"), [=]() mutable
            {
                auto connectionUrl = std:("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                mockPoolInstance["end"]->mockRejectedValue(std::make_shared<Error>(std:("Close failed")));
                std::async([=]() { expect(manager->close())->rejects->toThrow(std:("Close failed")); });
            }
            );
        }
        );
    }
    );
}

MAIN
