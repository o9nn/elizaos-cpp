#include "plugin-init.test.h"
#include <string>

void Main(void)
{
    describe(std::string("TodoPlugin Initialization"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        auto setupMocks = [=]() mutable
        {
            mockRuntime = createMockRuntime(object{
                object::pair{std::string("db"), nullptr}, 
                object::pair{std::string("getService"), [=]() mutable
                {
                    return nullptr;
                }
                }
            });
        };
        it(std::string("should have correct plugin metadata"), [=]() mutable
        {
            expect(TodoPlugin->name)->toBe(std::string("todo"));
            expect(TodoPlugin->description)->toContain(std::string("task management"));
            expect(TodoPlugin->services)->toBeDefined();
            expect(TodoPlugin->actions)->toBeDefined();
            expect(TodoPlugin->providers)->toBeDefined();
            expect(TodoPlugin->schema)->toBeDefined();
        }
        );
        it(std::string("should have the correct number of services"), [=]() mutable
        {
            expect(TodoPlugin->services)->toHaveLength(2);
            auto services = as<array<object>>(TodoPlugin->services);
            expect(const_(services)[0]["serviceType"])->toBe(std::string("TODO"));
            expect(const_(services)[1]["serviceType"])->toBe(std::string("TODO_REMINDER"));
        }
        );
        it(std::string("should have all required actions"), [=]() mutable
        {
            expect(TodoPlugin->actions)->toHaveLength(5);
            auto actionNames = TodoPlugin->actions->map([=](auto action) mutable
            {
                return action["name"];
            }
            );
            expect(actionNames)->toContain(std::string("CREATE_TODO"));
            expect(actionNames)->toContain(std::string("COMPLETE_TODO"));
            expect(actionNames)->toContain(std::string("CONFIRM_TODO"));
            expect(actionNames)->toContain(std::string("UPDATE_TODO"));
            expect(actionNames)->toContain(std::string("CANCEL_TODO"));
        }
        );
        it(std::string("should have the todos provider"), [=]() mutable
        {
            expect(TodoPlugin->providers)->toHaveLength(1);
            expect(const_(TodoPlugin->providers)[0]->name)->toBe(std::string("TODOS"));
        }
        );
        it(std::string("should have test dependencies"), [=]() mutable
        {
            expect(TodoPlugin->testDependencies)->toContain(std::string("@elizaos/plugin-sql"));
        }
        );
        it(std::string("should have schema with correct tables"), [=]() mutable
        {
            expect(TodoPlugin->schema)->toBeDefined();
            expect(TodoPlugin->schema->todosTable)->toBeDefined();
            expect(TodoPlugin->schema->todoTagsTable)->toBeDefined();
        }
        );
        it(std::string("should correct types"), [=]() mutable
        {
            expect(TodoPlugin)->toBeDefined();
            expect(TodoPlugin->name)->toBe(std::string("todo"));
            expect(type_of(TodoPlugin->init))->toBe(std::string("function"));
        }
        );
    }
    );
}

MAIN
