#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/src/tests/e2e-simple.test.h"

any mockRuntime = createMockRuntime(object{
    object::pair{std:("db"), nullptr}, 
    object::pair{std:("getService"), [=]() mutable
    {
        return nullptr;
    }
    }, 
    object::pair{std:("useModel"), [=]() mutable
    {
        return Promise->resolve(std:("Mock response"));
    }
    }, 
    object::pair{std:("composeState"), [=]() mutable
    {
        return Promise->resolve(object{
            object::pair{std:("values"), object{}}, 
            object::pair{std:("text"), string_empty}, 
            object::pair{std:("data"), object{}}
        });
    }
    }, 
    object::pair{std:("getRoom"), [=]() mutable
    {
        return Promise->resolve(nullptr);
    }
    }, 
    object::pair{std:("emitEvent"), [=]() mutable
    {
        return Promise->resolve();
    }
    }
});

void Main(void)
{
    describe(std:("Todo Plugin E2E Simple Tests"), [=]() mutable
    {
        it(std:("should initialize plugin successfully"), [=]() mutable
        {
            expect([=]() mutable
            {
                return TodoPlugin->init(object{}, mockRuntime);
            }
            )->not->toThrow();
        }
        );
        it(std:("should create reminder service successfully"), [=]() mutable
        {
            auto service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            expect(service)->toBeDefined();
            expect(service->serviceName)->toBe(std:("TODO_REMINDER"));
            std::async([=]() { service->stop(); });
        }
        );
        it(std:("should have working action validation"), [=]() mutable
        {
            auto createAction = TodoPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("CREATE_TODO");
            }
            );
            expect(createAction)->toBeDefined();
            expect(createAction->validate)->toBeDefined();
            expect(type_of(createAction->handler))->toBe(std:("function"));
        }
        );
        it(std:("should all required types"), [=]() mutable
        {
            expect(type_of(TodoPlugin->name))->toBe(std:("string"));
            expect(Array->isArray(TodoPlugin->actions))->toBe(true);
            expect(Array->isArray(TodoPlugin->services))->toBe(true);
            expect(Array->isArray(TodoPlugin->providers))->toBe(true);
            expect(TodoPlugin->schema)->toBeDefined();
        }
        );
    }
    );
}

MAIN
