#include "index.test.h"

void Main(void)
{
    describe(std:("SQL Plugin"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<string> tempDir;
        beforeEach([=]() mutable
        {
            tempDir = fs->mkdtempSync(path->join(os->tmpdir(), std:("eliza-plugin-sql-test-")));
            process->env.Delete("POSTGRES_URL");
            process->env.Delete("POSTGRES_USER");
            process->env.Delete("POSTGRES_PASSWORD");
            process->env.Delete("PGLITE_DATA_DIR");
            mockRuntime = as<any>(object{
                object::pair{std:("agentId"), std:("00000000-0000-0000-0000-000000000000")}, 
                object::pair{std:("getSetting"), mock([=]() mutable
                {
                    return nullptr;
                }
                )}, 
                object::pair{std:("registerDatabaseAdapter"), mock([=]() mutable
                {
                }
                )}, 
                object::pair{std:("registerService"), mock([=]() mutable
                {
                }
                )}, 
                object::pair{std:("getService"), mock([=]() mutable
                {
                }
                )}, 
                object::pair{std:("databaseAdapter"), undefined}
            });
        }
        );
        afterEach([=]() mutable
        {
            if (AND((tempDir), (fs->existsSync(tempDir)))) {
                fs->rmSync(tempDir, object{
                    object::pair{std:("recursive"), true}, 
                    object::pair{std:("force"), true}
                });
            }
            auto GLOBAL_SINGLETONS = Symbol->for(std:("@elizaos/plugin-sql/global-singletons"));
            auto globalSymbols = as<Record<any, any>>(as<any>(global));
            if ((*const_(globalSymbols))[GLOBAL_SINGLETONS]) {
                (*globalSymbols)[GLOBAL_SINGLETONS] = object{};
            }
            process->env.Delete("PGLITE_DATA_DIR");
        }
        );
        describe(std:("Plugin Structure"), [=]() mutable
        {
            it(std:("should have correct plugin metadata"), [=]() mutable
            {
                expect(plugin->name)->toBe(std:("@elizaos/plugin-sql"));
                expect(plugin->description)->toBe(std:("A plugin for SQL database access with dynamic schema migrations"));
                expect(plugin->priority)->toBe(0);
            }
            );
            it(std:("should have schema defined"), [=]() mutable
            {
                expect(plugin->schema)->toBeDefined();
                expect(plugin->schema)->toHaveProperty(std:("agentTable"));
                expect(plugin->schema)->toHaveProperty(std:("entityTable"));
                expect(plugin->schema)->toHaveProperty(std:("memoryTable"));
            }
            );
            it(std:("should have init function"), [=]() mutable
            {
                expect(plugin->init)->toBeDefined();
                expect(type_of(plugin->init))->toBe(std:("function"));
            }
            );
        }
        );
        describe(std:("Plugin Initialization"), [=]() mutable
        {
            it(std:("should skip initialization if adapter already exists"), [=]() mutable
            {
                (as<any>(mockRuntime))["databaseAdapter"] = object{
                    object::pair{std:("existing"), true}
                };
                std::async([=]() { plugin->init(object{}, mockRuntime); });
                expect(mockRuntime->registerDatabaseAdapter)->not->toHaveBeenCalled();
            }
            );
            it(std:("should register database adapter when none exists"), [=]() mutable
            {
                process->env->PGLITE_DATA_DIR = tempDir;
                mockRuntime->getSetting = mock([=](auto key) mutable
                {
                    if (OR((key == std:("PGLITE_PATH")), (key == std:("DATABASE_PATH")))) {
                        return tempDir;
                    }
                    return nullptr;
                }
                );
                std::async([=]() { plugin->init(object{}, mockRuntime); });
                expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            }
            );
            it(std:("should use POSTGRES_URL when available"), [=]() mutable
            {
                mockRuntime->getSetting = mock([=](auto key) mutable
                {
                    if (key == std:("POSTGRES_URL")) return std:("postgresql://localhost:5432/test");
                    return nullptr;
                }
                );
                std::async([=]() { plugin->init(object{}, mockRuntime); });
                expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            }
            );
            it(std:("should prioritize PGLITE_PATH over DATABASE_PATH"), [=]() mutable
            {
                mockRuntime->getSetting = mock([=](auto key) mutable
                {
                    if (key == std:("PGLITE_PATH")) return std:("/custom/pglite");
                    if (key == std:("DATABASE_PATH")) return std:("/custom/database");
                    return nullptr;
                }
                );
                std::async([=]() { plugin->init(object{}, mockRuntime); });
                expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            }
            );
            it(std:("should use DATABASE_PATH if PGLITE_PATH is not set"), [=]() mutable
            {
                mockRuntime->getSetting = mock([=](auto key) mutable
                {
                    if (key == std:("DATABASE_PATH")) return std:("/custom/database");
                    return nullptr;
                }
                );
                std::async([=]() { plugin->init(object{}, mockRuntime); });
                expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            }
            );
            it(std:("should use default path if neither PGLITE_PATH nor DATABASE_PATH is set"), [=]() mutable
            {
                process->env->PGLITE_DATA_DIR = tempDir;
                mockRuntime->getSetting = mock([=]() mutable
                {
                    return nullptr;
                }
                );
                std::async([=]() { plugin->init(object{}, mockRuntime); });
                expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std:("createDatabaseAdapter"), [=]() mutable
        {
            shared agentId = std:("00000000-0000-0000-0000-000000000000");
            it(std:("should create PgDatabaseAdapter when postgresUrl is provided"), [=]() mutable
            {
                auto config = object{
                    object::pair{std:("postgresUrl"), std:("postgresql://localhost:5432/test")}
                };
                auto adapter = createDatabaseAdapter(config, agentId);
                expect(adapter)->toBeDefined();
            }
            );
            it(std:("should create PgliteDatabaseAdapter when no postgresUrl is provided"), [=]() mutable
            {
                process->env->PGLITE_DATA_DIR = tempDir;
                auto config = object{
                    object::pair{std:("dataDir"), path->join(tempDir, std:("custom-data"))}
                };
                auto adapter = createDatabaseAdapter(config, agentId);
                expect(adapter)->toBeDefined();
            }
            );
            it(std:("should use default dataDir when none provided"), [=]() mutable
            {
                process->env->PGLITE_DATA_DIR = tempDir;
                auto config = object{};
                auto adapter = createDatabaseAdapter(config, agentId);
                expect(adapter)->toBeDefined();
            }
            );
            it(std:("should reuse singleton managers"), [=]() mutable
            {
                auto adapter1 = createDatabaseAdapter(object{
                    object::pair{std:("postgresUrl"), std:("postgresql://localhost:5432/test")}
                }, agentId);
                auto adapter2 = createDatabaseAdapter(object{
                    object::pair{std:("postgresUrl"), std:("postgresql://localhost:5432/test")}
                }, agentId);
                expect(adapter1)->toBeDefined();
                expect(adapter2)->toBeDefined();
            }
            );
        }
        );
    }
    );
}

MAIN
