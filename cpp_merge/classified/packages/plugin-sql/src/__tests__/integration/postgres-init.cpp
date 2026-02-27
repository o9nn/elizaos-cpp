#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sql/src/__tests__/integration/postgres-init.test.h"

void Main(void)
{
    describe(std:("PostgreSQL Initialization Tests"), [=]() mutable
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
                object::pair{std:("agentId"), std:("00000000-0000-0000-0000-000000000000")}, 
                object::pair{std:("getSetting"), mock()}, 
                object::pair{std:("registerDatabaseAdapter"), mock()}, 
                object::pair{std:("registerService"), mock()}, 
                object::pair{std:("getService"), mock()}
            });
        }
        );
        afterEach([=]() mutable
        {
            process->env = originalEnv;
        }
        );
        it(std:("should initialize with PostgreSQL when POSTGRES_URL is provided"), [=]() mutable
        {
            shared postgresUrl = std:("postgresql://test:test@localhost:5432/testdb");
            (as<any>(mockRuntime->getSetting))["mockImplementation"]([=](auto key) mutable
            {
                if (key == std:("POSTGRES_URL")) return postgresUrl;
                return undefined;
            }
            );
            std::async([=]() { plugin->init(object{}, mockRuntime); });
            expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            auto adapter = const_(const_((as<any>(mockRuntime->registerDatabaseAdapter))["mock"]["calls"])[0])[0];
            expect(adapter)->toBeDefined();
            expect(adapter["constructor"]["name"])->toBe(std:("PgDatabaseAdapter"));
        }
        );
        it(std:("should skip initialization if database adapter already exists"), [=]() mutable
        {
            (as<any>(mockRuntime))["databaseAdapter"] = object{
                object::pair{std:("test"), true}
            };
            std::async([=]() { plugin->init(object{}, mockRuntime); });
            expect(mockRuntime->registerDatabaseAdapter)->not->toHaveBeenCalled();
        }
        );
        it(std:("should use PGLITE_PATH when provided"), [=]() mutable
        {
            shared pglitePath = join(tmpdir(), std:("eliza-test-pglite-") + Date->now());
            (as<any>(mockRuntime->getSetting))["mockImplementation"]([=](auto key) mutable
            {
                if (key == std:("PGLITE_PATH")) return pglitePath;
                return undefined;
            }
            );
            std::async([=]() { plugin->init(object{}, mockRuntime); });
            expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            auto adapter = const_(const_((as<any>(mockRuntime->registerDatabaseAdapter))["mock"]["calls"])[0])[0];
            expect(adapter)->toBeDefined();
            expect(adapter["constructor"]["name"])->toBe(std:("PgliteDatabaseAdapter"));
        }
        );
        it(std:("should use DATABASE_PATH when PGLITE_PATH is not provided"), [=]() mutable
        {
            shared databasePath = join(tmpdir(), std:("eliza-test-db-") + Date->now());
            (as<any>(mockRuntime->getSetting))["mockImplementation"]([=](auto key) mutable
            {
                if (key == std:("DATABASE_PATH")) return databasePath;
                return undefined;
            }
            );
            std::async([=]() { plugin->init(object{}, mockRuntime); });
            expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            auto adapter = const_(const_((as<any>(mockRuntime->registerDatabaseAdapter))["mock"]["calls"])[0])[0];
            expect(adapter)->toBeDefined();
            expect(adapter["constructor"]["name"])->toBe(std:("PgliteDatabaseAdapter"));
        }
        );
        it(std:("should use default path when no configuration is provided"), [=]() mutable
        {
            (as<any>(mockRuntime->getSetting))["mockReturnValue"](undefined);
            std::async([=]() { plugin->init(object{}, mockRuntime); });
            expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            auto adapter = const_(const_((as<any>(mockRuntime->registerDatabaseAdapter))["mock"]["calls"])[0])[0];
            expect(adapter)->toBeDefined();
            expect(adapter["constructor"]["name"])->toBe(std:("PgliteDatabaseAdapter"));
        }
        );
        it(std:("should handle errors gracefully during adapter check"), [=]() mutable
        {
            Object->defineProperty(mockRuntime, std:("databaseAdapter"), object{
                , 
                object::pair{std:("configurable"), true}
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
