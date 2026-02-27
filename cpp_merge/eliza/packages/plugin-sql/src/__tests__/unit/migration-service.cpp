#include "migration-service.test.h"

object mockLogger = object{
    object::pair{std:("info"), mock([=]() mutable
    {
    }
    )}, 
    object::pair{std:("warn"), mock([=]() mutable
    {
    }
    )}, 
    object::pair{std:("error"), mock([=]() mutable
    {
    }
    )}, 
    object::pair{std:("debug"), mock([=]() mutable
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
    describe(std:("DatabaseMigrationService"), [=]() mutable
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
                object::pair{std:("query"), object{
                    object::pair{std:("agentTable"), object{
                        object::pair{std:("findFirst"), mock([=]() mutable
                        {
                        }
                        )}
                    }}, 
                    object::pair{std:("entityTable"), object{
                        object::pair{std:("findFirst"), mock([=]() mutable
                        {
                        }
                        )}
                    }}, 
                    object::pair{std:("memoryTable"), object{
                        object::pair{std:("findFirst"), mock([=]() mutable
                        {
                        }
                        )}
                    }}
                }}, 
                object::pair{std:("transaction"), mock([=]() mutable
                {
                }
                )}, 
                object::pair{std:("execute"), mock([=]() mutable
                {
                    return Promise->resolve(object{
                        object::pair{std:("rows"), array<any>()}
                    });
                }
                )}
            };
            migrationService = std::make_shared<DatabaseMigrationService>();
        }
        );
        describe(std:("constructor"), [=]() mutable
        {
            it(std:("should create an instance"), [=]() mutable
            {
                expect(migrationService)->toBeDefined();
                expect(migrationService)->toBeInstanceOf(DatabaseMigrationService);
            }
            );
        }
        );
        describe(std:("initializeWithDatabase"), [=]() mutable
        {
            it(std:("should initialize with database"), [=]() mutable
            {
                std::async([=]() { migrationService->initializeWithDatabase(mockDb); });
                expect((as<any>(migrationService))["db"])->toBe(mockDb);
            }
            );
        }
        );
        describe(std:("discoverAndRegisterPluginSchemas"), [=]() mutable
        {
            it(std:("should register plugins with schemas"), [=]() mutable
            {
                auto plugins = array<std::shared_ptr<Plugin>>{ object{
                    object::pair{std:("name"), std:("plugin1")}, 
                    object::pair{std:("description"), std:("Test plugin 1")}, 
                    object::pair{std:("schema"), object{
                        object::pair{std:("table1"), object{}}
                    }}
                }, object{
                    object::pair{std:("name"), std:("plugin2")}, 
                    object::pair{std:("description"), std:("Test plugin 2")}, 
                    object::pair{std:("schema"), object{
                        object::pair{std:("table2"), object{}}
                    }}
                }, object{
                    object::pair{std:("name"), std:("plugin3")}, 
                    object::pair{std:("description"), std:("Plugin without schema")}
                } };
                migrationService->discoverAndRegisterPluginSchemas(plugins);
            }
            );
            it(std:("should handle empty plugin array"), [=]() mutable
            {
                migrationService->discoverAndRegisterPluginSchemas(array<any>());
            }
            );
            it(std:("should handle plugins without schemas"), [=]() mutable
            {
                auto plugins = array<std::shared_ptr<Plugin>>{ object{
                    object::pair{std:("name"), std:("plugin1")}, 
                    object::pair{std:("description"), std:("Plugin without schema")}
                }, object{
                    object::pair{std:("name"), std:("plugin2")}, 
                    object::pair{std:("description"), std:("Another plugin without schema")}
                } };
                migrationService->discoverAndRegisterPluginSchemas(plugins);
            }
            );
        }
        );
        describe(std:("runAllPluginMigrations"), [=]() mutable
        {
            it(std:("should throw if database not initialized"), [=]() mutable
            {
                std::async([=]() { expect(migrationService->runAllPluginMigrations())->rejects->toThrow(std:("Database not initialized in DatabaseMigrationService")); });
            }
            );
            it(std:("should run migrations for registered plugins"), [=]() mutable
            {
                std::async([=]() { migrationService->initializeWithDatabase(mockDb); });
                auto plugins = array<std::shared_ptr<Plugin>>{ object{
                    object::pair{std:("name"), std:("plugin1")}, 
                    object::pair{std:("description"), std:("Test plugin 1")}, 
                    object::pair{std:("schema"), object{
                        object::pair{std:("table1"), object{}}
                    }}
                }, object{
                    object::pair{std:("name"), std:("plugin2")}, 
                    object::pair{std:("description"), std:("Test plugin 2")}, 
                    object::pair{std:("schema"), object{
                        object::pair{std:("table2"), object{}}
                    }}
                } };
                migrationService->discoverAndRegisterPluginSchemas(plugins);
                std::async([=]() { migrationService->runAllPluginMigrations(); });
            }
            );
            it(std:("should handle migration errors"), [=]() mutable
            {
                std::async([=]() { migrationService->initializeWithDatabase(mockDb); });
                migrationService->discoverAndRegisterPluginSchemas(array<object>{ object{
                    object::pair{std:("name"), std:("error-plugin")}, 
                    object::pair{std:("description"), std:("Test plugin")}, 
                    object::pair{std:("schema"), object{
                        object::pair{std:("tables"), object{}}
                    }}
                } });
                std::async([=]() { migrationService->runAllPluginMigrations(); });
            }
            );
            it(std:("should run migrations even with no plugins"), [=]() mutable
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
