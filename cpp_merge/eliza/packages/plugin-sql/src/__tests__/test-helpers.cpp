#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/test-helpers.h"

std::shared_ptr<Promise<object>> createTestDatabase(std::shared_ptr<UUID> testAgentId, array<std::shared_ptr<Plugin>> testPlugins)
{
    if (process->env->POSTGRES_URL) {
        console->log(std:("[TEST] Using PostgreSQL for test database"));
        auto connectionManager = std::make_shared<PostgresConnectionManager>(process->env->POSTGRES_URL);
        shared adapter = std::make_shared<PgDatabaseAdapter>(testAgentId, connectionManager);
        std::async([=]() { adapter->init(); });
        auto runtime = std::make_shared<AgentRuntime>(object{
            object::pair{std:("character"), utils::assign(object{
                , 
                object::pair{std:("id"), undefined}
            }, mockCharacter)}, 
            object::pair{std:("agentId"), testAgentId}, 
            object::pair{std:("plugins"), array<any>{ sqlPlugin, testPlugins }}
        });
        runtime->registerDatabaseAdapter(adapter);
        shared schemaName = std:("test_") + testAgentId->replace((new RegExp(std:("-"))), std:("_")) + string_empty;
        shared db = connectionManager->getDatabase();
        std::async([=]() { db->execute(sql->raw(std:("DROP SCHEMA IF EXISTS ") + schemaName + std:(" CASCADE"))); });
        std::async([=]() { db->execute(sql->raw(std:("CREATE SCHEMA IF NOT EXISTS ") + schemaName + string_empty)); });
        std::async([=]() { db->execute(sql->raw(std:("SET search_path TO ") + schemaName + std:(", public"))); });
        auto migrationService = std::make_shared<DatabaseMigrationService>();
        std::async([=]() { migrationService->initializeWithDatabase(db); });
        migrationService->discoverAndRegisterPluginSchemas(array<any>{ sqlPlugin, testPlugins });
        std::async([=]() { migrationService->runAllPluginMigrations(); });
        std::async([=]() { adapter->createAgent(as<any>(utils::assign(object{
            object::pair{std:("id"), testAgentId}
        }, mockCharacter))); });
        auto cleanup = [=]() mutable
        {
            std::async([=]() { db->execute(sql->raw(std:("DROP SCHEMA IF EXISTS ") + schemaName + std:(" CASCADE"))); });
            std::async([=]() { adapter->close(); });
        };
        return object{
            object::pair{std:("adapter"), std:("adapter")}, 
            object::pair{std:("runtime"), std:("runtime")}, 
            object::pair{std:("cleanup"), std:("cleanup")}
        };
    } else {
        shared tempDir = fs->mkdtempSync(path->join(os->tmpdir(), std:("eliza-test-")));
        auto connectionManager = std::make_shared<PGliteClientManager>(object{
            object::pair{std:("dataDir"), tempDir}
        });
        std::async([=]() { connectionManager->initialize(); });
        shared adapter = std::make_shared<PgliteDatabaseAdapter>(testAgentId, connectionManager);
        std::async([=]() { adapter->init(); });
        auto runtime = std::make_shared<AgentRuntime>(object{
            object::pair{std:("character"), utils::assign(object{
                , 
                object::pair{std:("id"), undefined}
            }, mockCharacter)}, 
            object::pair{std:("agentId"), testAgentId}, 
            object::pair{std:("plugins"), array<any>{ sqlPlugin, testPlugins }}
        });
        runtime->registerDatabaseAdapter(adapter);
        auto migrationService = std::make_shared<DatabaseMigrationService>();
        std::async([=]() { migrationService->initializeWithDatabase(adapter->getDatabase()); });
        migrationService->discoverAndRegisterPluginSchemas(array<any>{ sqlPlugin, testPlugins });
        std::async([=]() { migrationService->runAllPluginMigrations(); });
        std::async([=]() { adapter->createAgent(as<any>(utils::assign(object{
            object::pair{std:("id"), testAgentId}
        }, mockCharacter))); });
        auto cleanup = [=]() mutable
        {
            std::async([=]() { adapter->close(); });
            fs->rmSync(tempDir, object{
                object::pair{std:("recursive"), true}, 
                object::pair{std:("force"), true}
            });
        };
        return object{
            object::pair{std:("adapter"), std:("adapter")}, 
            object::pair{std:("runtime"), std:("runtime")}, 
            object::pair{std:("cleanup"), std:("cleanup")}
        };
    }
};


std::shared_ptr<Promise<object>> createIsolatedTestDatabase(string testName, array<std::shared_ptr<Plugin>> testPlugins)
{
    auto testAgentId = as<std::shared_ptr<UUID>>(v4());
    auto testId = testName->replace((new RegExp(std:("[^a-zA-Z0-9]"))), std:("_"))->toLowerCase();
    if (process->env->POSTGRES_URL) {
        shared schemaName = std:("test_") + testId + std:("_") + Date->now() + string_empty;
        console->log(std:("[TEST] Creating isolated PostgreSQL schema: ") + schemaName + string_empty);
        auto connectionManager = std::make_shared<PostgresConnectionManager>(process->env->POSTGRES_URL);
        shared adapter = std::make_shared<PgDatabaseAdapter>(testAgentId, connectionManager);
        std::async([=]() { adapter->init(); });
        auto runtime = std::make_shared<AgentRuntime>(object{
            object::pair{std:("character"), utils::assign(object{
                , 
                object::pair{std:("id"), undefined}
            }, mockCharacter)}, 
            object::pair{std:("agentId"), testAgentId}, 
            object::pair{std:("plugins"), array<any>{ sqlPlugin, testPlugins }}
        });
        runtime->registerDatabaseAdapter(adapter);
        shared db = connectionManager->getDatabase();
        std::async([=]() { db->execute(sql->raw(std:("CREATE SCHEMA ") + schemaName + string_empty)); });
        std::async([=]() { db->execute(sql->raw(std:("SET search_path TO ") + schemaName + std:(", public"))); });
        auto migrationService = std::make_shared<DatabaseMigrationService>();
        std::async([=]() { migrationService->initializeWithDatabase(db); });
        migrationService->discoverAndRegisterPluginSchemas(array<any>{ sqlPlugin, testPlugins });
        std::async([=]() { migrationService->runAllPluginMigrations(); });
        std::async([=]() { adapter->createAgent(as<any>(utils::assign(object{
            object::pair{std:("id"), testAgentId}
        }, mockCharacter))); });
        auto cleanup = [=]() mutable
        {
            try
            {
                std::async([=]() { db->execute(sql->raw(std:("DROP SCHEMA IF EXISTS ") + schemaName + std:(" CASCADE"))); });
            }
            catch (const any& error)
            {
                console->error(std:("[TEST] Failed to drop schema ") + schemaName + std:(":"), error);
            }
            std::async([=]() { adapter->close(); });
        };
        return object{
            object::pair{std:("adapter"), std:("adapter")}, 
            object::pair{std:("runtime"), std:("runtime")}, 
            object::pair{std:("cleanup"), std:("cleanup")}, 
            object::pair{std:("testAgentId"), std:("testAgentId")}
        };
    } else {
        shared tempDir = path->join(os->tmpdir(), std:("eliza-test-") + testId + std:("-") + Date->now() + string_empty);
        console->log(std:("[TEST] Creating isolated PGLite database: ") + tempDir + string_empty);
        auto connectionManager = std::make_shared<PGliteClientManager>(object{
            object::pair{std:("dataDir"), tempDir}
        });
        std::async([=]() { connectionManager->initialize(); });
        shared adapter = std::make_shared<PgliteDatabaseAdapter>(testAgentId, connectionManager);
        std::async([=]() { adapter->init(); });
        auto runtime = std::make_shared<AgentRuntime>(object{
            object::pair{std:("character"), utils::assign(object{
                , 
                object::pair{std:("id"), undefined}
            }, mockCharacter)}, 
            object::pair{std:("agentId"), testAgentId}, 
            object::pair{std:("plugins"), array<any>{ sqlPlugin, testPlugins }}
        });
        runtime->registerDatabaseAdapter(adapter);
        auto migrationService = std::make_shared<DatabaseMigrationService>();
        std::async([=]() { migrationService->initializeWithDatabase(adapter->getDatabase()); });
        migrationService->discoverAndRegisterPluginSchemas(array<any>{ sqlPlugin, testPlugins });
        std::async([=]() { migrationService->runAllPluginMigrations(); });
        std::async([=]() { adapter->createAgent(as<any>(utils::assign(object{
            object::pair{std:("id"), testAgentId}
        }, mockCharacter))); });
        auto cleanup = [=]() mutable
        {
            std::async([=]() { adapter->close(); });
            try
            {
                fs->rmSync(tempDir, object{
                    object::pair{std:("recursive"), true}, 
                    object::pair{std:("force"), true}
                });
            }
            catch (const any& error)
            {
                console->error(std:("[TEST] Failed to remove temp directory ") + tempDir + std:(":"), error);
            }
        };
        return object{
            object::pair{std:("adapter"), std:("adapter")}, 
            object::pair{std:("runtime"), std:("runtime")}, 
            object::pair{std:("cleanup"), std:("cleanup")}, 
            object::pair{std:("testAgentId"), std:("testAgentId")}
        };
    }
};


