#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/postgres-init.test.h"

void Main(void)
{
    describe(std::string("PostgreSQL Initialization Tests"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<NodeJS::ProcessEnv>> originalEnv;
        beforeEach([=]() mutable
        {
            originalEnv = utils::assign(object{
            }, process->env);
            process->env.Delete("POSTGRES_URL");
            process->env.Delete("PGLITE_PATH");
            process->env.Delete("DATABASE_PATH");
            mockRuntime = as<any>(object{
                object::pair{std::string("agentId"), std::string("00000000-0000-0000-0000-000000000000")}, 
                object::pair{std::string("getSetting"), mock()}, 
                object::pair{std::string("registerDatabaseAdapter"), mock()}, 
                object::pair{std::string("registerService"), mock()}, 
                object::pair{std::string("getService"), mock()}
            });
        }
        );
        afterEach([=]() mutable
        {
            process->env = originalEnv;
        }
        );
        it(std::string("should initialize with PostgreSQL when POSTGRES_URL is provided"), [=]() mutable
        {
            shared postgresUrl = std::string("postgresql://test:test@localhost:5432/testdb");
            (as<any>(mockRuntime->getSetting))["mockImplementation"]([=](auto key) mutable
            {
                if (key == std::string("POSTGRES_URL")) return postgresUrl;
                return undefined;
            }
            );
            std::async([=]() { plugin->init(object{}, mockRuntime); });
            expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            auto adapter = const_(const_((as<any>(mockRuntime->registerDatabaseAdapter))["mock"]["calls"])[0])[0];
            expect(adapter)->toBeDefined();
            expect(adapter["constructor"]["name"])->toBe(std::string("PgDatabaseAdapter"));
        }
        );
        it(std::string("should skip initialization if database adapter already exists"), [=]() mutable
        {
            (as<any>(mockRuntime))["databaseAdapter"] = object{
                object::pair{std::string("test"), true}
            };
            std::async([=]() { plugin->init(object{}, mockRuntime); });
            expect(mockRuntime->registerDatabaseAdapter)->not->toHaveBeenCalled();
        }
        );
        it(std::string("should use PGLITE_PATH when provided"), [=]() mutable
        {
            shared pglitePath = join(tmpdir(), std::string("eliza-test-pglite-") + Date->now());
            (as<any>(mockRuntime->getSetting))["mockImplementation"]([=](auto key) mutable
            {
                if (key == std::string("PGLITE_PATH")) return pglitePath;
                return undefined;
            }
            );
            std::async([=]() { plugin->init(object{}, mockRuntime); });
            expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            auto adapter = const_(const_((as<any>(mockRuntime->registerDatabaseAdapter))["mock"]["calls"])[0])[0];
            expect(adapter)->toBeDefined();
            expect(adapter["constructor"]["name"])->toBe(std::string("PgliteDatabaseAdapter"));
        }
        );
        it(std::string("should use DATABASE_PATH when PGLITE_PATH is not provided"), [=]() mutable
        {
            shared databasePath = join(tmpdir(), std::string("eliza-test-db-") + Date->now());
            (as<any>(mockRuntime->getSetting))["mockImplementation"]([=](auto key) mutable
            {
                if (key == std::string("DATABASE_PATH")) return databasePath;
                return undefined;
            }
            );
            std::async([=]() { plugin->init(object{}, mockRuntime); });
            expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            auto adapter = const_(const_((as<any>(mockRuntime->registerDatabaseAdapter))["mock"]["calls"])[0])[0];
            expect(adapter)->toBeDefined();
            expect(adapter["constructor"]["name"])->toBe(std::string("PgliteDatabaseAdapter"));
        }
        );
        it(std::string("should use default path when no configuration is provided"), [=]() mutable
        {
            (as<any>(mockRuntime->getSetting))["mockReturnValue"](undefined);
            std::async([=]() { plugin->init(object{}, mockRuntime); });
            expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            auto adapter = const_(const_((as<any>(mockRuntime->registerDatabaseAdapter))["mock"]["calls"])[0])[0];
            expect(adapter)->toBeDefined();
            expect(adapter["constructor"]["name"])->toBe(std::string("PgliteDatabaseAdapter"));
        }
        );
        it(std::string("should handle errors gracefully during adapter check"), [=]() mutable
        {
            Object->defineProperty(mockRuntime, std::string("databaseAdapter"), object{
                , 
                object::pair{std::string("configurable"), true}
            });
            (as<any>(mockRuntime->getSetting))["mockReturnValue"](undefined);
            std::async([=]() { plugin->init(object{}, mockRuntime); });
            expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
        }
        );
    }
    );
}

MAIN
