#include "index.test.h"
#include <string>

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
    describe(std::string("GoalsPlugin"), [=]() mutable
    {
        auto goalsPluginSuite = std::make_shared<TestSuite>(std::string("GoalsPlugin"), object{});
        goalsPluginSuite->addTest(createUnitTest(object{
            object::pair{std::string("name"), std::string("should GoalsPlugin with correct structure")}, 
            object::pair{std::string("fn"), [=]() mutable
            {
                expect(GoalsPlugin)->toBeDefined();
                expect(GoalsPlugin->name)->toBe(std::string("goals"));
                expect(GoalsPlugin->description)->toBe(std::string("Provides goal management functionality for tracking and achieving objectives."));
                expect(GoalsPlugin->providers)->toHaveLength(1);
                expect(GoalsPlugin->actions)->toHaveLength(5);
                expect(GoalsPlugin->services)->toHaveLength(1);
                expect(GoalsPlugin->routes)->toBeDefined();
                expect(GoalsPlugin->init)->toBeInstanceOf(Function);
            }
            }
        }));
        goalsPluginSuite->addTest(createUnitTest(object{
            object::pair{std::string("name"), std::string("should have all required actions")}, 
            object::pair{std::string("fn"), [=]() mutable
            {
                auto actionNames = OR((GoalsPlugin->actions->map([=](auto action) mutable
                {
                    return action["name"];
                }
                )), (array<any>()));
                expect(actionNames)->toContain(std::string("CREATE_GOAL"));
                expect(actionNames)->toContain(std::string("COMPLETE_GOAL"));
                expect(actionNames)->toContain(std::string("CONFIRM_GOAL"));
                expect(actionNames)->toContain(std::string("UPDATE_GOAL"));
                expect(actionNames)->toContain(std::string("CANCEL_GOAL"));
            }
            }
        }));
        goalsPluginSuite->addTest(createUnitTest(object{
            object::pair{std::string("name"), std::string("should initialize without errors when database is available")}, 
            object::pair{std::string("fn"), [=]() mutable
            {
                auto mockRuntime = as<any>(object{
                    object::pair{std::string("agentId"), as<any>(std::string("test-agent"))}, 
                    object::pair{std::string("db"), object{
                        object::pair{std::string("execute"), mock([=]() mutable
                        {
                            return Promise->resolve(object{
                                object::pair{std::string("rows"), array<any>()}
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
            object::pair{std::string("name"), std::string("should handle missing database gracefully")}, 
            object::pair{std::string("fn"), [=]() mutable
            {
                auto mockRuntime = as<any>(object{
                    object::pair{std::string("agentId"), as<any>(std::string("test-agent"))}, 
                    object::pair{std::string("db"), nullptr}
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
