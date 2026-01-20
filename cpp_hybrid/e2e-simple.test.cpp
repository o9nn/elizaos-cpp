#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/src/tests/e2e-simple.test.h"

any mockRuntime = createMockRuntime(object{
    object::pair{std::string("db"), nullptr}, 
    object::pair{std::string("getService"), [=]() mutable
    {
        return nullptr;
    }
    }, 
    object::pair{std::string("useModel"), [=]() mutable
    {
        return Promise->resolve(std::string("Mock response"));
    }
    }, 
    object::pair{std::string("composeState"), [=]() mutable
    {
        return Promise->resolve(object{
            object::pair{std::string("values"), object{}}, 
            object::pair{std::string("text"), string_empty}, 
            object::pair{std::string("data"), object{}}
        });
    }
    }, 
    object::pair{std::string("getRoom"), [=]() mutable
    {
        return Promise->resolve(nullptr);
    }
    }, 
    object::pair{std::string("emitEvent"), [=]() mutable
    {
        return Promise->resolve();
    }
    }
});

void Main(void)
{
    describe(std::string("Todo Plugin E2E Simple Tests"), [=]() mutable
    {
        it(std::string("should initialize plugin successfully"), [=]() mutable
        {
            expect([=]() mutable
            {
                return TodoPlugin->init(object{}, mockRuntime);
            }
            )->not->toThrow();
        }
        );
        it(std::string("should create reminder service successfully"), [=]() mutable
        {
            auto service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            expect(service)->toBeDefined();
            expect(service->serviceName)->toBe(std::string("TODO_REMINDER"));
            std::async([=]() { service->stop(); });
        }
        );
        it(std::string("should have working action validation"), [=]() mutable
        {
            auto createAction = TodoPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("CREATE_TODO");
            }
            );
            expect(createAction)->toBeDefined();
            expect(createAction->validate)->toBeDefined();
            expect(type_of(createAction->handler))->toBe(std::string("function"));
        }
        );
        it(std::string("should export all required types"), [=]() mutable
        {
            expect(type_of(TodoPlugin->name))->toBe(std::string("string"));
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
