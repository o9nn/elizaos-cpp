#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/unit/migration-service.test.h"

object mockLogger = object{
    object::pair{std::string("info"), mock([=]() mutable
    {
    }
    )}, 
    object::pair{std::string("warn"), mock([=]() mutable
    {
    }
    )}, 
    object::pair{std::string("error"), mock([=]() mutable
    {
    }
    )}, 
    object::pair{std::string("debug"), mock([=]() mutable
    {
    }
    )}
};
any mockRunPluginMigrations = mock([=]() mutable
{
    return Promise->resolve();
}
);

void Main(void)
{
    describe(std::string("DatabaseMigrationService"), [=]() mutable
    {
        shared<std::shared_ptr<DatabaseMigrationService>> migrationService;
        shared<any> mockDb;
        beforeEach([=]() mutable
        {
            mockLogger["info"]->mockClear();
            mockLogger["warn"]->mockClear();
            mockLogger["error"]->mockClear();
            mockLogger["debug"]->mockClear();
            mockRunPluginMigrations->mockClear();
            mockDb = object{
                object::pair{std::string("query"), object{
                    object::pair{std::string("agentTable"), object{
                        object::pair{std::string("findFirst"), mock([=]() mutable
                        {
                        }
                        )}
                    }}, 
                    object::pair{std::string("entityTable"), object{
                        object::pair{std::string("findFirst"), mock([=]() mutable
                        {
                        }
                        )}
                    }}, 
                    object::pair{std::string("memoryTable"), object{
                        object::pair{std::string("findFirst"), mock([=]() mutable
                        {
                        }
                        )}
                    }}
                }}, 
                object::pair{std::string("transaction"), mock([=]() mutable
                {
                }
                )}, 
                object::pair{std::string("execute"), mock([=]() mutable
                {
                    return Promise->resolve(object{
                        object::pair{std::string("rows"), array<any>()}
                    });
                }
                )}
            };
            migrationService = std::make_shared<DatabaseMigrationService>();
        }
        );
        describe(std::string("constructor"), [=]() mutable
        {
            it(std::string("should create an instance"), [=]() mutable
            {
                expect(migrationService)->toBeDefined();
                expect(migrationService)->toBeInstanceOf(DatabaseMigrationService);
            }
            );
        }
        );
        describe(std::string("initializeWithDatabase"), [=]() mutable
        {
            it(std::string("should initialize with database"), [=]() mutable
            {
                std::async([=]() { migrationService->initializeWithDatabase(mockDb); });
                expect((as<any>(migrationService))["db"])->toBe(mockDb);
            }
            );
        }
        );
        describe(std::string("discoverAndRegisterPluginSchemas"), [=]() mutable
        {
            it(std::string("should register plugins with schemas"), [=]() mutable
            {
                auto plugins = array<std::shared_ptr<Plugin>>{ object{
                    object::pair{std::string("name"), std::string("plugin1")}, 
                    object::pair{std::string("description"), std::string("Test plugin 1")}, 
                    object::pair{std::string("schema"), object{
                        object::pair{std::string("table1"), object{}}
                    }}
                }, object{
                    object::pair{std::string("name"), std::string("plugin2")}, 
                    object::pair{std::string("description"), std::string("Test plugin 2")}, 
                    object::pair{std::string("schema"), object{
                        object::pair{std::string("table2"), object{}}
                    }}
                }, object{
                    object::pair{std::string("name"), std::string("plugin3")}, 
                    object::pair{std::string("description"), std::string("Plugin without schema")}
                } };
                migrationService->discoverAndRegisterPluginSchemas(plugins);
            }
            );
            it(std::string("should handle empty plugin array"), [=]() mutable
            {
                migrationService->discoverAndRegisterPluginSchemas(array<any>());
            }
            );
            it(std::string("should handle plugins without schemas"), [=]() mutable
            {
                auto plugins = array<std::shared_ptr<Plugin>>{ object{
                    object::pair{std::string("name"), std::string("plugin1")}, 
                    object::pair{std::string("description"), std::string("Plugin without schema")}
                }, object{
                    object::pair{std::string("name"), std::string("plugin2")}, 
                    object::pair{std::string("description"), std::string("Another plugin without schema")}
                } };
                migrationService->discoverAndRegisterPluginSchemas(plugins);
            }
            );
        }
        );
        describe(std::string("runAllPluginMigrations"), [=]() mutable
        {
            it(std::string("should throw if database not initialized"), [=]() mutable
            {
                std::async([=]() { expect(migrationService->runAllPluginMigrations())->rejects->toThrow(std::string("Database not initialized in DatabaseMigrationService")); });
            }
            );
            it(std::string("should run migrations for registered plugins"), [=]() mutable
            {
                std::async([=]() { migrationService->initializeWithDatabase(mockDb); });
                auto plugins = array<std::shared_ptr<Plugin>>{ object{
                    object::pair{std::string("name"), std::string("plugin1")}, 
                    object::pair{std::string("description"), std::string("Test plugin 1")}, 
                    object::pair{std::string("schema"), object{
                        object::pair{std::string("table1"), object{}}
                    }}
                }, object{
                    object::pair{std::string("name"), std::string("plugin2")}, 
                    object::pair{std::string("description"), std::string("Test plugin 2")}, 
                    object::pair{std::string("schema"), object{
                        object::pair{std::string("table2"), object{}}
                    }}
                } };
                migrationService->discoverAndRegisterPluginSchemas(plugins);
                std::async([=]() { migrationService->runAllPluginMigrations(); });
            }
            );
            it(std::string("should handle migration errors"), [=]() mutable
            {
                std::async([=]() { migrationService->initializeWithDatabase(mockDb); });
                migrationService->discoverAndRegisterPluginSchemas(array<object>{ object{
                    object::pair{std::string("name"), std::string("error-plugin")}, 
                    object::pair{std::string("description"), std::string("Test plugin")}, 
                    object::pair{std::string("schema"), object{
                        object::pair{std::string("tables"), object{}}
                    }}
                } });
                std::async([=]() { migrationService->runAllPluginMigrations(); });
            }
            );
            it(std::string("should run migrations even with no plugins"), [=]() mutable
            {
                std::async([=]() { migrationService->initializeWithDatabase(mockDb); });
                std::async([=]() { migrationService->runAllPluginMigrations(); });
            }
            );
        }
        );
    }
    );
}

MAIN
