#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/test-helpers.h"

std::shared_ptr<Promise<object>> createTestDatabase(std::shared_ptr<UUID> testAgentId, array<std::shared_ptr<Plugin>> testPlugins)
{
    if (process->env->POSTGRES_URL) {
        console->log(std::string("[TEST] Using PostgreSQL for test database"));
        auto connectionManager = std::make_shared<PostgresConnectionManager>(process->env->POSTGRES_URL);
        shared adapter = std::make_shared<PgDatabaseAdapter>(testAgentId, connectionManager);
        std::async([=]() { adapter->init(); });
        auto runtime = std::make_shared<AgentRuntime>(object{
            object::pair{std::string("character"), utils::assign(object{
                , 
                object::pair{std::string("id"), undefined}
            }, mockCharacter)}, 
            object::pair{std::string("agentId"), testAgentId}, 
            object::pair{std::string("plugins"), array<any>{ sqlPlugin, testPlugins }}
        });
        runtime->registerDatabaseAdapter(adapter);
        shared schemaName = std::string("test_") + testAgentId->replace((new RegExp(std::string("-"))), std::string("_")) + string_empty;
        shared db = connectionManager->getDatabase();
        std::async([=]() { db->execute(sql->raw(std::string("DROP SCHEMA IF EXISTS ") + schemaName + std::string(" CASCADE"))); });
        std::async([=]() { db->execute(sql->raw(std::string("CREATE SCHEMA IF NOT EXISTS ") + schemaName + string_empty)); });
        std::async([=]() { db->execute(sql->raw(std::string("SET search_path TO ") + schemaName + std::string(", public"))); });
        auto migrationService = std::make_shared<DatabaseMigrationService>();
        std::async([=]() { migrationService->initializeWithDatabase(db); });
        migrationService->discoverAndRegisterPluginSchemas(array<any>{ sqlPlugin, testPlugins });
        std::async([=]() { migrationService->runAllPluginMigrations(); });
        std::async([=]() { adapter->createAgent(as<any>(utils::assign(object{
            object::pair{std::string("id"), testAgentId}
        }, mockCharacter))); });
        auto cleanup = [=]() mutable
        {
            std::async([=]() { db->execute(sql->raw(std::string("DROP SCHEMA IF EXISTS ") + schemaName + std::string(" CASCADE"))); });
            std::async([=]() { adapter->close(); });
        };
        return object{
            object::pair{std::string("adapter"), std::string("adapter")}, 
            object::pair{std::string("runtime"), std::string("runtime")}, 
            object::pair{std::string("cleanup"), std::string("cleanup")}
        };
    } else {
        shared tempDir = fs->mkdtempSync(path->join(os->tmpdir(), std::string("eliza-test-")));
        auto connectionManager = std::make_shared<PGliteClientManager>(object{
            object::pair{std::string("dataDir"), tempDir}
        });
        std::async([=]() { connectionManager->initialize(); });
        shared adapter = std::make_shared<PgliteDatabaseAdapter>(testAgentId, connectionManager);
        std::async([=]() { adapter->init(); });
        auto runtime = std::make_shared<AgentRuntime>(object{
            object::pair{std::string("character"), utils::assign(object{
                , 
                object::pair{std::string("id"), undefined}
            }, mockCharacter)}, 
            object::pair{std::string("agentId"), testAgentId}, 
            object::pair{std::string("plugins"), array<any>{ sqlPlugin, testPlugins }}
        });
        runtime->registerDatabaseAdapter(adapter);
        auto migrationService = std::make_shared<DatabaseMigrationService>();
        std::async([=]() { migrationService->initializeWithDatabase(adapter->getDatabase()); });
        migrationService->discoverAndRegisterPluginSchemas(array<any>{ sqlPlugin, testPlugins });
        std::async([=]() { migrationService->runAllPluginMigrations(); });
        std::async([=]() { adapter->createAgent(as<any>(utils::assign(object{
            object::pair{std::string("id"), testAgentId}
        }, mockCharacter))); });
        auto cleanup = [=]() mutable
        {
            std::async([=]() { adapter->close(); });
            fs->rmSync(tempDir, object{
                object::pair{std::string("recursive"), true}, 
                object::pair{std::string("force"), true}
            });
        };
        return object{
            object::pair{std::string("adapter"), std::string("adapter")}, 
            object::pair{std::string("runtime"), std::string("runtime")}, 
            object::pair{std::string("cleanup"), std::string("cleanup")}
        };
    }
};


std::shared_ptr<Promise<object>> createIsolatedTestDatabase(string testName, array<std::shared_ptr<Plugin>> testPlugins)
{
    auto testAgentId = as<std::shared_ptr<UUID>>(v4());
    auto testId = testName->replace((new RegExp(std::string("[^a-zA-Z0-9]"))), std::string("_"))->toLowerCase();
    if (process->env->POSTGRES_URL) {
        shared schemaName = std::string("test_") + testId + std::string("_") + Date->now() + string_empty;
        console->log(std::string("[TEST] Creating isolated PostgreSQL schema: ") + schemaName + string_empty);
        auto connectionManager = std::make_shared<PostgresConnectionManager>(process->env->POSTGRES_URL);
        shared adapter = std::make_shared<PgDatabaseAdapter>(testAgentId, connectionManager);
        std::async([=]() { adapter->init(); });
        auto runtime = std::make_shared<AgentRuntime>(object{
            object::pair{std::string("character"), utils::assign(object{
                , 
                object::pair{std::string("id"), undefined}
            }, mockCharacter)}, 
            object::pair{std::string("agentId"), testAgentId}, 
            object::pair{std::string("plugins"), array<any>{ sqlPlugin, testPlugins }}
        });
        runtime->registerDatabaseAdapter(adapter);
        shared db = connectionManager->getDatabase();
        std::async([=]() { db->execute(sql->raw(std::string("CREATE SCHEMA ") + schemaName + string_empty)); });
        std::async([=]() { db->execute(sql->raw(std::string("SET search_path TO ") + schemaName + std::string(", public"))); });
        auto migrationService = std::make_shared<DatabaseMigrationService>();
        std::async([=]() { migrationService->initializeWithDatabase(db); });
        migrationService->discoverAndRegisterPluginSchemas(array<any>{ sqlPlugin, testPlugins });
        std::async([=]() { migrationService->runAllPluginMigrations(); });
        std::async([=]() { adapter->createAgent(as<any>(utils::assign(object{
            object::pair{std::string("id"), testAgentId}
        }, mockCharacter))); });
        auto cleanup = [=]() mutable
        {
            try
            {
                std::async([=]() { db->execute(sql->raw(std::string("DROP SCHEMA IF EXISTS ") + schemaName + std::string(" CASCADE"))); });
            }
            catch (const any& error)
            {
                console->error(std::string("[TEST] Failed to drop schema ") + schemaName + std::string(":"), error);
            }
            std::async([=]() { adapter->close(); });
        };
        return object{
            object::pair{std::string("adapter"), std::string("adapter")}, 
            object::pair{std::string("runtime"), std::string("runtime")}, 
            object::pair{std::string("cleanup"), std::string("cleanup")}, 
            object::pair{std::string("testAgentId"), std::string("testAgentId")}
        };
    } else {
        shared tempDir = path->join(os->tmpdir(), std::string("eliza-test-") + testId + std::string("-") + Date->now() + string_empty);
        console->log(std::string("[TEST] Creating isolated PGLite database: ") + tempDir + string_empty);
        auto connectionManager = std::make_shared<PGliteClientManager>(object{
            object::pair{std::string("dataDir"), tempDir}
        });
        std::async([=]() { connectionManager->initialize(); });
        shared adapter = std::make_shared<PgliteDatabaseAdapter>(testAgentId, connectionManager);
        std::async([=]() { adapter->init(); });
        auto runtime = std::make_shared<AgentRuntime>(object{
            object::pair{std::string("character"), utils::assign(object{
                , 
                object::pair{std::string("id"), undefined}
            }, mockCharacter)}, 
            object::pair{std::string("agentId"), testAgentId}, 
            object::pair{std::string("plugins"), array<any>{ sqlPlugin, testPlugins }}
        });
        runtime->registerDatabaseAdapter(adapter);
        auto migrationService = std::make_shared<DatabaseMigrationService>();
        std::async([=]() { migrationService->initializeWithDatabase(adapter->getDatabase()); });
        migrationService->discoverAndRegisterPluginSchemas(array<any>{ sqlPlugin, testPlugins });
        std::async([=]() { migrationService->runAllPluginMigrations(); });
        std::async([=]() { adapter->createAgent(as<any>(utils::assign(object{
            object::pair{std::string("id"), testAgentId}
        }, mockCharacter))); });
        auto cleanup = [=]() mutable
        {
            std::async([=]() { adapter->close(); });
            try
            {
                fs->rmSync(tempDir, object{
                    object::pair{std::string("recursive"), true}, 
                    object::pair{std::string("force"), true}
                });
            }
            catch (const any& error)
            {
                console->error(std::string("[TEST] Failed to remove temp directory ") + tempDir + std::string(":"), error);
            }
        };
        return object{
            object::pair{std::string("adapter"), std::string("adapter")}, 
            object::pair{std::string("runtime"), std::string("runtime")}, 
            object::pair{std::string("cleanup"), std::string("cleanup")}, 
            object::pair{std::string("testAgentId"), std::string("testAgentId")}
        };
    }
};


