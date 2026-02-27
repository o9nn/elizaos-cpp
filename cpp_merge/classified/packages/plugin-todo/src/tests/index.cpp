#include "index.test.h"

void Main(void)
{
    describe(std:("TodoPlugin"), [=]() mutable
    {
        it(std:("should TodoPlugin with correct structure"), [=]() mutable
        {
            expect(TodoPlugin)->toBeDefined();
            expect(TodoPlugin->name)->toBe(std:("todo"));
            expect(TodoPlugin->description)->toBe(std:("Provides task management functionality with daily recurring and one-off tasks."));
            expect(TodoPlugin->providers)->toHaveLength(1);
            expect(TodoPlugin->actions)->toHaveLength(5);
            expect(TodoPlugin->services)->toHaveLength(2);
            expect(TodoPlugin->routes)->toBeDefined();
            expect(TodoPlugin->init)->toBeInstanceOf(Function);
        }
        );
        it(std:("should have all required actions"), [=]() mutable
        {
            auto actionNames = OR((TodoPlugin->actions->map([=](auto action) mutable
            {
                return action["name"];
            }
            )), (array<any>()));
            expect(actionNames)->toContain(std:("CREATE_TODO"));
            expect(actionNames)->toContain(std:("COMPLETE_TODO"));
            expect(actionNames)->toContain(std:("CONFIRM_TODO"));
            expect(actionNames)->toContain(std:("UPDATE_TODO"));
            expect(actionNames)->toContain(std:("CANCEL_TODO"));
        }
        );
        it(std:("should have all required services"), [=]() mutable
        {
            auto services = as<any>(TodoPlugin->services);
            expect(services->some([=](auto s) mutable
            {
                return s->serviceType == std:("TODO");
            }
            ))->toBe(true);
            expect(services->some([=](auto s) mutable
            {
                return s->serviceType == std:("TODO_REMINDER");
            }
            ))->toBe(true);
        }
        );
    }
    );
}

MAIN
