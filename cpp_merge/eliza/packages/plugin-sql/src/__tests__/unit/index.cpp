#include "index.test.h"
#include <string>

void Main(void)
{
    describe(std::string("SQL Plugin"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<string> tempDir;
        beforeEach([=]() mutable
        {
            tempDir = fs->mkdtempSync(path->join(os->tmpdir(), std::string("eliza-plugin-sql-test-")));
            process->env.Delete("POSTGRES_URL");
            process->env.Delete("POSTGRES_USER");
            process->env.Delete("POSTGRES_PASSWORD");
            process->env.Delete("PGLITE_DATA_DIR");
            mockRuntime = as<any>(object{
                object::pair{std::string("agentId"), std::string("00000000-0000-0000-0000-000000000000")}, 
                object::pair{std::string("getSetting"), mock([=]() mutable
                {
                    return nullptr;
                }
                )}, 
                object::pair{std::string("registerDatabaseAdapter"), mock([=]() mutable
                {
                }
                )}, 
                object::pair{std::string("registerService"), mock([=]() mutable
                {
                }
                )}, 
                object::pair{std::string("getService"), mock([=]() mutable
                {
                }
                )}, 
                object::pair{std::string("databaseAdapter"), std::nullopt}
            });
        }
        );
        afterEach([=]() mutable
        {
            if (AND((tempDir), (fs->existsSync(tempDir)))) {
                fs->rmSync(tempDir, object{
                    object::pair{std::string("recursive"), true}, 
                    object::pair{std::string("force"), true}
                });
            }
            auto GLOBAL_SINGLETONS = Symbol->for(std::string("@elizaos/plugin-sql/global-singletons"));
            auto globalSymbols = as<Record<any, any>>(as<any>(global));
            if ((*const_(globalSymbols))[GLOBAL_SINGLETONS]) {
                (*globalSymbols)[GLOBAL_SINGLETONS] = object{};
            }
            process->env.Delete("PGLITE_DATA_DIR");
        }
        );
        describe(std::string("Plugin Structure"), [=]() mutable
        {
            it(std::string("should have correct plugin metadata"), [=]() mutable
            {
                expect(plugin->name)->toBe(std::string("@elizaos/plugin-sql"));
                expect(plugin->description)->toBe(std::string("A plugin for SQL database access with dynamic schema migrations"));
                expect(plugin->priority)->toBe(0);
            }
            );
            it(std::string("should have schema defined"), [=]() mutable
            {
                expect(plugin->schema)->toBeDefined();
                expect(plugin->schema)->toHaveProperty(std::string("agentTable"));
                expect(plugin->schema)->toHaveProperty(std::string("entityTable"));
                expect(plugin->schema)->toHaveProperty(std::string("memoryTable"));
            }
            );
            it(std::string("should have init function"), [=]() mutable
            {
                expect(plugin->init)->toBeDefined();
                expect(type_of(plugin->init))->toBe(std::string("function"));
            }
            );
        }
        );
        describe(std::string("Plugin Initialization"), [=]() mutable
        {
            it(std::string("should skip initialization if adapter already exists"), [=]() mutable
            {
                (as<any>(mockRuntime))["databaseAdapter"] = object{
                    object::pair{std::string("existing"), true}
                };
                std::async([=]() { plugin->init(object{}, mockRuntime); });
                expect(mockRuntime->registerDatabaseAdapter)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should register database adapter when none exists"), [=]() mutable
            {
                process->env->PGLITE_DATA_DIR = tempDir;
                mockRuntime->getSetting = mock([=](auto key) mutable
                {
                    if (OR((key == std::string("PGLITE_PATH")), (key == std::string("DATABASE_PATH")))) {
                        return tempDir;
                    }
                    return nullptr;
                }
                );
                std::async([=]() { plugin->init(object{}, mockRuntime); });
                expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            }
            );
            it(std::string("should use POSTGRES_URL when available"), [=]() mutable
            {
                mockRuntime->getSetting = mock([=](auto key) mutable
                {
                    if (key == std::string("POSTGRES_URL")) return std::string("postgresql://localhost:5432/test");
                    return nullptr;
                }
                );
                std::async([=]() { plugin->init(object{}, mockRuntime); });
                expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            }
            );
            it(std::string("should prioritize PGLITE_PATH over DATABASE_PATH"), [=]() mutable
            {
                mockRuntime->getSetting = mock([=](auto key) mutable
                {
                    if (key == std::string("PGLITE_PATH")) return std::string("/custom/pglite");
                    if (key == std::string("DATABASE_PATH")) return std::string("/custom/database");
                    return nullptr;
                }
                );
                std::async([=]() { plugin->init(object{}, mockRuntime); });
                expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            }
            );
            it(std::string("should use DATABASE_PATH if PGLITE_PATH is not set"), [=]() mutable
            {
                mockRuntime->getSetting = mock([=](auto key) mutable
                {
                    if (key == std::string("DATABASE_PATH")) return std::string("/custom/database");
                    return nullptr;
                }
                );
                std::async([=]() { plugin->init(object{}, mockRuntime); });
                expect(mockRuntime->registerDatabaseAdapter)->toHaveBeenCalled();
            }
            );
            it(std::string("should use default path if neither PGLITE_PATH nor DATABASE_PATH is set"), [=]() mutable
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
        describe(std::string("createDatabaseAdapter"), [=]() mutable
        {
            shared agentId = std::string("00000000-0000-0000-0000-000000000000");
            it(std::string("should create PgDatabaseAdapter when postgresUrl is provided"), [=]() mutable
            {
                auto config = object{
                    object::pair{std::string("postgresUrl"), std::string("postgresql://localhost:5432/test")}
                };
                auto adapter = createDatabaseAdapter(config, agentId);
                expect(adapter)->toBeDefined();
            }
            );
            it(std::string("should create PgliteDatabaseAdapter when no postgresUrl is provided"), [=]() mutable
            {
                process->env->PGLITE_DATA_DIR = tempDir;
                auto config = object{
                    object::pair{std::string("dataDir"), path->join(tempDir, std::string("custom-data"))}
                };
                auto adapter = createDatabaseAdapter(config, agentId);
                expect(adapter)->toBeDefined();
            }
            );
            it(std::string("should use default dataDir when none provided"), [=]() mutable
            {
                process->env->PGLITE_DATA_DIR = tempDir;
                auto config = object{};
                auto adapter = createDatabaseAdapter(config, agentId);
                expect(adapter)->toBeDefined();
            }
            );
            it(std::string("should reuse singleton managers"), [=]() mutable
            {
                auto adapter1 = createDatabaseAdapter(object{
                    object::pair{std::string("postgresUrl"), std::string("postgresql://localhost:5432/test")}
                }, agentId);
                auto adapter2 = createDatabaseAdapter(object{
                    object::pair{std::string("postgresUrl"), std::string("postgresql://localhost:5432/test")}
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
