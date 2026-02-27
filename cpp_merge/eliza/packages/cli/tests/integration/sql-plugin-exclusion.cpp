#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/integration/sql-plugin-exclusion.test.h"

void Main(void)
{
    describe(std:("SQL Plugin Test Exclusion"), [=]() mutable
    {
        it(std:("should not run SQL plugin tests when testing another plugin"), [=]() mutable
        {
            process->env->ELIZA_TESTING_PLUGIN = std:("true");
            {
                utils::finally __finally2423_2496([&]() mutable
                {
                    process->env.Delete("ELIZA_TESTING_PLUGIN");
                });
                try
                {
                    auto sqlPlugin = object{
                        object::pair{std:("name"), std:("@elizaos/plugin-sql")}, 
                        object::pair{std:("description"), std:("SQL Plugin")}, 
                        object::pair{std:("tests"), array<object>{ object{
                            object::pair{std:("name"), std:("sql_test_suite")}, 
                            object::pair{std:("tests"), array<object>{ object{
                                object::pair{std:("name"), std:("sql_test")}, 
                                object::pair{std:("fn"), [=]() mutable
                                {
                                    throw any(std::make_shared<Error>(std:("SQL test should not run!")));
                                }
                                }
                            } }}
                        } }}
                    };
                    auto myPlugin = object{
                        object::pair{std:("name"), std:("my-plugin")}, 
                        object::pair{std:("description"), std:("My Plugin")}, 
                        object::pair{std:("tests"), array<object>{ object{
                            object::pair{std:("name"), std:("my_test_suite")}, 
                            object::pair{std:("tests"), array<object>{ object{
                                object::pair{std:("name"), std:("my_test")}, 
                                object::pair{std:("fn"), [=]() mutable
                                {
                                }
                                }
                            } }}
                        } }}
                    };
                    auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                        object::pair{std:("agentId"), std:("test-agent")}, 
                        object::pair{std:("character"), as<std::shared_ptr<Character>>(object{
                            object::pair{std:("name"), std:("test-character")}, 
                            object::pair{std:("bio"), std:("test bio")}
                        })}, 
                        object::pair{std:("plugins"), array<any>{ sqlPlugin, myPlugin }}
                    }));
                    auto projectAgent = as<std::shared_ptr<ProjectAgent>>(object{
                        object::pair{std:("character"), as<std::shared_ptr<Character>>(object{
                            object::pair{std:("name"), std:("test-character")}, 
                            object::pair{std:("bio"), std:("test bio")}
                        })}, 
                        object::pair{std:("plugins"), array<any>{ myPlugin }}
                    });
                    auto testRunner = std::make_shared<TestRunner>(mockRuntime, projectAgent);
                    auto results = std::async([=]() { testRunner->runTests(object{
                        object::pair{std:("filter"), undefined}, 
                        object::pair{std:("skipPlugins"), false}, 
                        object::pair{std:("skipProjectTests"), false}, 
                        object::pair{std:("skipE2eTests"), true}
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
        it(std:("should not run plugin tests when not in direct plugin test mode"), [=]() mutable
        {
            process->env.Delete("ELIZA_TESTING_PLUGIN");
            shared projectTestRan = false;
            auto sqlPlugin = object{
                object::pair{std:("name"), std:("@elizaos/plugin-sql")}, 
                object::pair{std:("description"), std:("SQL Plugin")}, 
                object::pair{std:("tests"), array<object>{ object{
                    object::pair{std:("name"), std:("sql_test_suite")}, 
                    object::pair{std:("tests"), array<object>{ object{
                        object::pair{std:("name"), std:("sql_test")}, 
                        object::pair{std:("fn"), [=]() mutable
                        {
                            throw any(std::make_shared<Error>(std:("Plugin tests should not run!")));
                        }
                        }
                    } }}
                } }}
            };
            auto myPlugin = object{
                object::pair{std:("name"), std:("my-plugin")}, 
                object::pair{std:("description"), std:("My Plugin")}, 
                object::pair{std:("tests"), array<object>{ object{
                    object::pair{std:("name"), std:("my_test_suite")}, 
                    object::pair{std:("tests"), array<object>{ object{
                        object::pair{std:("name"), std:("my_test")}, 
                        object::pair{std:("fn"), [=]() mutable
                        {
                            throw any(std::make_shared<Error>(std:("Plugin tests should not run!")));
                        }
                        }
                    } }}
                } }}
            };
            auto projectAgent = as<std::shared_ptr<ProjectAgent>>(object{
                object::pair{std:("character"), as<std::shared_ptr<Character>>(object{
                    object::pair{std:("name"), std:("test-character")}, 
                    object::pair{std:("bio"), std:("test bio")}
                })}, 
                object::pair{std:("plugins"), array<any>{ sqlPlugin, myPlugin }}, 
                object::pair{std:("tests"), array<object>{ object{
                    object::pair{std:("name"), std:("project_test_suite")}, 
                    object::pair{std:("tests"), array<object>{ object{
                        object::pair{std:("name"), std:("project_test")}, 
                        object::pair{std:("fn"), [=]() mutable
                        {
                            projectTestRan = true;
                        }
                        }
                    } }}
                } }}
            });
            auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                object::pair{std:("agentId"), std:("test-agent")}, 
                object::pair{std:("character"), as<std::shared_ptr<Character>>(object{
                    object::pair{std:("name"), std:("test-character")}, 
                    object::pair{std:("bio"), std:("test bio")}
                })}, 
                object::pair{std:("plugins"), array<any>()}
            }));
            auto testRunner = std::make_shared<TestRunner>(mockRuntime, projectAgent);
            auto results = std::async([=]() { testRunner->runTests(object{
                object::pair{std:("filter"), undefined}, 
                object::pair{std:("skipPlugins"), false}, 
                object::pair{std:("skipProjectTests"), false}, 
                object::pair{std:("skipE2eTests"), true}
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
