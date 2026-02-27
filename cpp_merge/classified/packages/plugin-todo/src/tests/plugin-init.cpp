#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/src/tests/plugin-init.test.h"

void Main(void)
{
    describe(std:("TodoPlugin Initialization"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        auto setupMocks = [=]() mutable
        {
            mockRuntime = createMockRuntime(object{
                object::pair{std:("db"), nullptr}, 
                object::pair{std:("getService"), [=]() mutable
                {
                    return nullptr;
                }
                }
            });
        };
        it(std:("should have correct plugin metadata"), [=]() mutable
        {
            expect(TodoPlugin->name)->toBe(std:("todo"));
            expect(TodoPlugin->description)->toContain(std:("task management"));
            expect(TodoPlugin->services)->toBeDefined();
            expect(TodoPlugin->actions)->toBeDefined();
            expect(TodoPlugin->providers)->toBeDefined();
            expect(TodoPlugin->schema)->toBeDefined();
        }
        );
        it(std:("should have the correct number of services"), [=]() mutable
        {
            expect(TodoPlugin->services)->toHaveLength(2);
            auto services = as<array<object>>(TodoPlugin->services);
            expect(const_(services)[0]["serviceType"])->toBe(std:("TODO"));
            expect(const_(services)[1]["serviceType"])->toBe(std:("TODO_REMINDER"));
        }
        );
        it(std:("should have all required actions"), [=]() mutable
        {
            expect(TodoPlugin->actions)->toHaveLength(5);
            auto actionNames = TodoPlugin->actions->map([=](auto action) mutable
            {
                return action["name"];
            }
            );
            expect(actionNames)->toContain(std:("CREATE_TODO"));
            expect(actionNames)->toContain(std:("COMPLETE_TODO"));
            expect(actionNames)->toContain(std:("CONFIRM_TODO"));
            expect(actionNames)->toContain(std:("UPDATE_TODO"));
            expect(actionNames)->toContain(std:("CANCEL_TODO"));
        }
        );
        it(std:("should have the todos provider"), [=]() mutable
        {
            expect(TodoPlugin->providers)->toHaveLength(1);
            expect(const_(TodoPlugin->providers)[0]->name)->toBe(std:("TODOS"));
        }
        );
        it(std:("should have test dependencies"), [=]() mutable
        {
            expect(TodoPlugin->testDependencies)->toContain(std:("@elizaos/plugin-sql"));
        }
        );
        it(std:("should have schema with correct tables"), [=]() mutable
        {
            expect(TodoPlugin->schema)->toBeDefined();
            expect(TodoPlugin->schema->todosTable)->toBeDefined();
            expect(TodoPlugin->schema->todoTagsTable)->toBeDefined();
        }
        );
        it(std:("should correct types"), [=]() mutable
        {
            expect(TodoPlugin)->toBeDefined();
            expect(TodoPlugin->name)->toBe(std:("todo"));
            expect(type_of(TodoPlugin->init))->toBe(std:("function"));
        }
        );
    }
    );
}

MAIN
