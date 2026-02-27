#include "index.test.h"

TestSuite::TestSuite(string name_, any config_) : name(name_), config(config_)  {
}

void TestSuite::addTest(any test)
{
    it(test["name"], [=]() mutable
    {
        auto context = (this->config["beforeEach"]) ? any(this->config["beforeEach"]()) (object{});
        std::async([=]() { test["fn"](context); });
    }
    );
}

void TestSuite::run()
{
}

std::function<object(object)> createUnitTest = [=](auto config) mutable
{
    return config;
};

void Main(void)
{
    describe(std:("GoalsPlugin"), [=]() mutable
    {
        auto goalsPluginSuite = std::make_shared<TestSuite>(std:("GoalsPlugin"), object{});
        goalsPluginSuite->addTest(createUnitTest(object{
            object::pair{std:("name"), std:("should GoalsPlugin with correct structure")}, 
            object::pair{std:("fn"), [=]() mutable
            {
                expect(GoalsPlugin)->toBeDefined();
                expect(GoalsPlugin->name)->toBe(std:("goals"));
                expect(GoalsPlugin->description)->toBe(std:("Provides goal management functionality for tracking and achieving objectives."));
                expect(GoalsPlugin->providers)->toHaveLength(1);
                expect(GoalsPlugin->actions)->toHaveLength(5);
                expect(GoalsPlugin->services)->toHaveLength(1);
                expect(GoalsPlugin->routes)->toBeDefined();
                expect(GoalsPlugin->init)->toBeInstanceOf(Function);
            }
            }
        }));
        goalsPluginSuite->addTest(createUnitTest(object{
            object::pair{std:("name"), std:("should have all required actions")}, 
            object::pair{std:("fn"), [=]() mutable
            {
                auto actionNames = OR((GoalsPlugin->actions->map([=](auto action) mutable
                {
                    return action["name"];
                }
                )), (array<any>()));
                expect(actionNames)->toContain(std:("CREATE_GOAL"));
                expect(actionNames)->toContain(std:("COMPLETE_GOAL"));
                expect(actionNames)->toContain(std:("CONFIRM_GOAL"));
                expect(actionNames)->toContain(std:("UPDATE_GOAL"));
                expect(actionNames)->toContain(std:("CANCEL_GOAL"));
            }
            }
        }));
        goalsPluginSuite->addTest(createUnitTest(object{
            object::pair{std:("name"), std:("should initialize without errors when database is available")}, 
            object::pair{std:("fn"), [=]() mutable
            {
                auto mockRuntime = as<any>(object{
                    object::pair{std:("agentId"), as<any>(std:("test-agent"))}, 
                    object::pair{std:("db"), object{
                        object::pair{std:("execute"), mock([=]() mutable
                        {
                            return Promise->resolve(object{
                                object::pair{std:("rows"), array<any>()}
                            });
                        }
                        )}
                    }}
                });
                auto config = object{};
                std::async([=]() { expect(GoalsPlugin->init(config, mockRuntime))->resolves->toBeUndefined(); });
            }
            }
        }));
        goalsPluginSuite->addTest(createUnitTest(object{
            object::pair{std:("name"), std:("should handle missing database gracefully")}, 
            object::pair{std:("fn"), [=]() mutable
            {
                auto mockRuntime = as<any>(object{
                    object::pair{std:("agentId"), as<any>(std:("test-agent"))}, 
                    object::pair{std:("db"), nullptr}
                });
                auto config = object{};
                std::async([=]() { expect(GoalsPlugin->init(config, mockRuntime))->resolves->toBeUndefined(); });
            }
            }
        }));
        goalsPluginSuite->run();
    }
    );
}

MAIN
