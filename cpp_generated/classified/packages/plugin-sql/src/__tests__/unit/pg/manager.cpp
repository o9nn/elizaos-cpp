#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sql/src/__tests__/unit/pg/manager.test.h"

object mockPoolInstance = object{
    object::pair{std::string("connect"), mock()}, 
    object::pair{std::string("end"), mock()}, 
    object::pair{std::string("query"), mock()}
};

void Main(void)
{
    mock->module(std::string("pg"), [=]() mutable
    {
        return (object{
            object::pair{std::string("Pool"), mock([=]() mutable
            {
                return mockPoolInstance;
            }
            )}
        });
    }
    );
    describe(std::string("PostgresConnectionManager"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            mockPoolInstance["connect"]->mockClear();
            mockPoolInstance["end"]->mockClear();
            mockPoolInstance["query"]->mockClear();
        }
        );
        describe(std::string("constructor"), [=]() mutable
        {
            it(std::string("should create an instance with connection URL"), [=]() mutable
            {
                auto connectionUrl = std::string("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                expect(manager)->toBeDefined();
                expect(manager->getConnection())->toBeDefined();
                expect(manager->getDatabase())->toBeDefined();
            }
            );
        }
        );
        describe(std::string("getDatabase"), [=]() mutable
        {
            it(std::string("should return the drizzle database instance"), [=]() mutable
            {
                auto connectionUrl = std::string("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                auto db = manager->getDatabase();
                expect(db)->toBeDefined();
                expect(db->query)->toBeDefined();
            }
            );
        }
        );
        describe(std::string("getConnection"), [=]() mutable
        {
            it(std::string("should return the pool instance"), [=]() mutable
            {
                auto connectionUrl = std::string("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                auto connection = manager->getConnection();
                expect(connection)->toBeDefined();
                expect(connection)->toBe(as<any>(mockPoolInstance));
            }
            );
        }
        );
        describe(std::string("getClient"), [=]() mutable
        {
            it(std::string("should return a client from the pool"), [=]() mutable
            {
                auto connectionUrl = std::string("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                auto mockClient = object{
                    object::pair{std::string("query"), mock()->mockResolvedValue(object{
                        object::pair{std::string("rows"), array<any>()}
                    })}, 
                    object::pair{std::string("release"), mock()}
                };
                mockPoolInstance["connect"]->mockResolvedValue(mockClient);
                auto client = std::async([=]() { manager->getClient(); });
                expect(client)->toBe(as<any>(mockClient));
                expect(mockPoolInstance["connect"])->toHaveBeenCalled();
            }
            );
            it(std::string("should throw error when pool connection fails"), [=]() mutable
            {
                auto connectionUrl = std::string("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                mockPoolInstance["connect"]->mockRejectedValue(std::make_shared<Error>(std::string("Connection failed")));
                std::async([=]() { expect(manager->getClient())->rejects->toThrow(std::string("Connection failed")); });
            }
            );
        }
        );
        describe(std::string("testConnection"), [=]() mutable
        {
            it(std::string("should return true when connection is successful"), [=]() mutable
            {
                auto connectionUrl = std::string("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                auto mockClient = object{
                    object::pair{std::string("query"), mock()->mockResolvedValue(object{
                        object::pair{std::string("rows"), array<any>()}
                    })}, 
                    object::pair{std::string("release"), mock()}
                };
                mockPoolInstance["connect"]->mockResolvedValue(mockClient);
                auto result = std::async([=]() { manager->testConnection(); });
                expect(result)->toBe(true);
                expect(mockPoolInstance["connect"])->toHaveBeenCalled();
                expect(mockClient["query"])->toHaveBeenCalledWith(std::string("SELECT 1"));
                expect(mockClient["release"])->toHaveBeenCalled();
            }
            );
            it(std::string("should return false when connection fails"), [=]() mutable
            {
                auto connectionUrl = std::string("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                mockPoolInstance["connect"]->mockRejectedValue(std::make_shared<Error>(std::string("Connection failed")));
                auto result = std::async([=]() { manager->testConnection(); });
                expect(result)->toBe(false);
            }
            );
            it(std::string("should return false when query fails"), [=]() mutable
            {
                auto connectionUrl = std::string("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                auto mockClient = object{
                    object::pair{std::string("query"), mock()->mockRejectedValue(std::make_shared<Error>(std::string("Query failed")))}, 
                    object::pair{std::string("release"), mock()}
                };
                mockPoolInstance["connect"]->mockResolvedValue(mockClient);
                auto result = std::async([=]() { manager->testConnection(); });
                expect(result)->toBe(false);
                expect(mockClient["release"])->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std::string("close"), [=]() mutable
        {
            it(std::string("should end the pool connection"), [=]() mutable
            {
                auto connectionUrl = std::string("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                mockPoolInstance["end"]->mockResolvedValue(undefined);
                std::async([=]() { manager->close(); });
                expect(mockPoolInstance["end"])->toHaveBeenCalled();
            }
            );
            it(std::string("should propagate errors during close"), [=]() mutable
            {
                auto connectionUrl = std::string("postgresql://user:pass@localhost:5432/testdb");
                auto manager = std::make_shared<PostgresConnectionManager>(connectionUrl);
                mockPoolInstance["end"]->mockRejectedValue(std::make_shared<Error>(std::string("Close failed")));
                std::async([=]() { expect(manager->close())->rejects->toThrow(std::string("Close failed")); });
            }
            );
        }
        );
    }
    );
}

MAIN
