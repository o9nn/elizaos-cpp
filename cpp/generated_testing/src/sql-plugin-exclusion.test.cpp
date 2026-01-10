#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/integration/sql-plugin-exclusion.test.h"

void Main(void)
{
    describe(std::string("SQL Plugin Test Exclusion"), [=]() mutable
    {
        it(std::string("should not run SQL plugin tests when testing another plugin"), [=]() mutable
        {
            process->env->ELIZA_TESTING_PLUGIN = std::string("true");
            {
                utils::finally __finally2423_2496([&]() mutable
                {
                    process->env.Delete("ELIZA_TESTING_PLUGIN");
                });
                try
                {
                    auto sqlPlugin = object{
                        object::pair{std::string("name"), std::string("@elizaos/plugin-sql")}, 
                        object::pair{std::string("description"), std::string("SQL Plugin")}, 
                        object::pair{std::string("tests"), array<object>{ object{
                            object::pair{std::string("name"), std::string("sql_test_suite")}, 
                            object::pair{std::string("tests"), array<object>{ object{
                                object::pair{std::string("name"), std::string("sql_test")}, 
                                object::pair{std::string("fn"), [=]() mutable
                                {
                                    throw any(std::make_shared<Error>(std::string("SQL test should not run!")));
                                }
                                }
                            } }}
                        } }}
                    };
                    auto myPlugin = object{
                        object::pair{std::string("name"), std::string("my-plugin")}, 
                        object::pair{std::string("description"), std::string("My Plugin")}, 
                        object::pair{std::string("tests"), array<object>{ object{
                            object::pair{std::string("name"), std::string("my_test_suite")}, 
                            object::pair{std::string("tests"), array<object>{ object{
                                object::pair{std::string("name"), std::string("my_test")}, 
                                object::pair{std::string("fn"), [=]() mutable
                                {
                                }
                                }
                            } }}
                        } }}
                    };
                    auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                        object::pair{std::string("agentId"), std::string("test-agent")}, 
                        object::pair{std::string("character"), as<std::shared_ptr<Character>>(object{
                            object::pair{std::string("name"), std::string("test-character")}, 
                            object::pair{std::string("bio"), std::string("test bio")}
                        })}, 
                        object::pair{std::string("plugins"), array<any>{ sqlPlugin, myPlugin }}
                    }));
                    auto projectAgent = as<std::shared_ptr<ProjectAgent>>(object{
                        object::pair{std::string("character"), as<std::shared_ptr<Character>>(object{
                            object::pair{std::string("name"), std::string("test-character")}, 
                            object::pair{std::string("bio"), std::string("test bio")}
                        })}, 
                        object::pair{std::string("plugins"), array<any>{ myPlugin }}
                    });
                    auto testRunner = std::make_shared<TestRunner>(mockRuntime, projectAgent);
                    auto results = std::async([=]() { testRunner->runTests(object{
                        object::pair{std::string("filter"), undefined}, 
                        object::pair{std::string("skipPlugins"), false}, 
                        object::pair{std::string("skipProjectTests"), false}, 
                        object::pair{std::string("skipE2eTests"), true}
                    }); });
                    expect(results->total)->toBe(1);
                    expect(results->passed)->toBe(1);
                    expect(results->failed)->toBe(0);
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        );
        it(std::string("should not run plugin tests when not in direct plugin test mode"), [=]() mutable
        {
            process->env.Delete("ELIZA_TESTING_PLUGIN");
            shared projectTestRan = false;
            auto sqlPlugin = object{
                object::pair{std::string("name"), std::string("@elizaos/plugin-sql")}, 
                object::pair{std::string("description"), std::string("SQL Plugin")}, 
                object::pair{std::string("tests"), array<object>{ object{
                    object::pair{std::string("name"), std::string("sql_test_suite")}, 
                    object::pair{std::string("tests"), array<object>{ object{
                        object::pair{std::string("name"), std::string("sql_test")}, 
                        object::pair{std::string("fn"), [=]() mutable
                        {
                            throw any(std::make_shared<Error>(std::string("Plugin tests should not run!")));
                        }
                        }
                    } }}
                } }}
            };
            auto myPlugin = object{
                object::pair{std::string("name"), std::string("my-plugin")}, 
                object::pair{std::string("description"), std::string("My Plugin")}, 
                object::pair{std::string("tests"), array<object>{ object{
                    object::pair{std::string("name"), std::string("my_test_suite")}, 
                    object::pair{std::string("tests"), array<object>{ object{
                        object::pair{std::string("name"), std::string("my_test")}, 
                        object::pair{std::string("fn"), [=]() mutable
                        {
                            throw any(std::make_shared<Error>(std::string("Plugin tests should not run!")));
                        }
                        }
                    } }}
                } }}
            };
            auto projectAgent = as<std::shared_ptr<ProjectAgent>>(object{
                object::pair{std::string("character"), as<std::shared_ptr<Character>>(object{
                    object::pair{std::string("name"), std::string("test-character")}, 
                    object::pair{std::string("bio"), std::string("test bio")}
                })}, 
                object::pair{std::string("plugins"), array<any>{ sqlPlugin, myPlugin }}, 
                object::pair{std::string("tests"), array<object>{ object{
                    object::pair{std::string("name"), std::string("project_test_suite")}, 
                    object::pair{std::string("tests"), array<object>{ object{
                        object::pair{std::string("name"), std::string("project_test")}, 
                        object::pair{std::string("fn"), [=]() mutable
                        {
                            projectTestRan = true;
                        }
                        }
                    } }}
                } }}
            });
            auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                object::pair{std::string("agentId"), std::string("test-agent")}, 
                object::pair{std::string("character"), as<std::shared_ptr<Character>>(object{
                    object::pair{std::string("name"), std::string("test-character")}, 
                    object::pair{std::string("bio"), std::string("test bio")}
                })}, 
                object::pair{std::string("plugins"), array<any>()}
            }));
            auto testRunner = std::make_shared<TestRunner>(mockRuntime, projectAgent);
            auto results = std::async([=]() { testRunner->runTests(object{
                object::pair{std::string("filter"), undefined}, 
                object::pair{std::string("skipPlugins"), false}, 
                object::pair{std::string("skipProjectTests"), false}, 
                object::pair{std::string("skipE2eTests"), true}
            }); });
            expect(results->total)->toBe(1);
            expect(results->passed)->toBe(1);
            expect(results->failed)->toBe(0);
            expect(projectTestRan)->toBe(true);
        }
        );
    }
    );
}

MAIN
