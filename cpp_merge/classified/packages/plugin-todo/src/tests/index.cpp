#include "index.test.h"
#include <string>

void Main(void)
{
    describe(std::string("TodoPlugin"), [=]() mutable
    {
        it(std::string("should TodoPlugin with correct structure"), [=]() mutable
        {
            expect(TodoPlugin)->toBeDefined();
            expect(TodoPlugin->name)->toBe(std::string("todo"));
            expect(TodoPlugin->description)->toBe(std::string("Provides task management functionality with daily recurring and one-off tasks."));
            expect(TodoPlugin->providers)->toHaveLength(1);
            expect(TodoPlugin->actions)->toHaveLength(5);
            expect(TodoPlugin->services)->toHaveLength(2);
            expect(TodoPlugin->routes)->toBeDefined();
            expect(TodoPlugin->init)->toBeInstanceOf(Function);
        }
        );
        it(std::string("should have all required actions"), [=]() mutable
        {
            auto actionNames = OR((TodoPlugin->actions->map([=](auto action) mutable
            {
                return action["name"];
            }
            )), (array<any>()));
            expect(actionNames)->toContain(std::string("CREATE_TODO"));
            expect(actionNames)->toContain(std::string("COMPLETE_TODO"));
            expect(actionNames)->toContain(std::string("CONFIRM_TODO"));
            expect(actionNames)->toContain(std::string("UPDATE_TODO"));
            expect(actionNames)->toContain(std::string("CANCEL_TODO"));
        }
        );
        it(std::string("should have all required services"), [=]() mutable
        {
            auto services = as<any>(TodoPlugin->services);
            expect(services->some([=](auto s) mutable
            {
                return s->serviceType == std::string("TODO");
            }
            ))->toBe(true);
            expect(services->some([=](auto s) mutable
            {
                return s->serviceType == std::string("TODO_REMINDER");
            }
            ))->toBe(true);
        }
        );
    }
    );
}

MAIN
