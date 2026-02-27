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
            expect(TodoPlugin->actions)->toHaveLength(4);
            expect(TodoPlugin->services)->toHaveLength(2);
            expect(TodoPlugin->routes)->toBeDefined();
            expect(TodoPlugin->init)->toBeInstanceOf(Function);
        }
        );
        it(std:("should have all required actions"), [=]() mutable
        {
            auto actionNames = TodoPlugin->actions->map([=](auto action) mutable
            {
                return action["name"];
            }
            );
            expect(actionNames)->toContain(std:("CREATE_TODO"));
            expect(actionNames)->toContain(std:("COMPLETE_TODO"));
            expect(actionNames)->toContain(std:("UPDATE_TODO"));
            expect(actionNames)->toContain(std:("CANCEL_TODO"));
        }
        );
        it(std:("should have all required services"), [=]() mutable
        {
            expect(TodoPlugin->services)->toContain(TodoService);
            expect(TodoPlugin->services->some([=](auto s) mutable
            {
                return s["name"] == std:("TodoReminderService");
            }
            ))->toBe(true);
        }
        );
    }
    );
    describe(std:("TodoService"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            mockRuntime = as<any>(object{
                object::pair{std:("agentId"), as<any>(std:("test-agent"))}, 
                object::pair{std:("getSetting"), vi->fn()}, 
                object::pair{std:("getService"), vi->fn()}, 
                object::pair{std:("createTask"), vi->fn()}, 
                object::pair{std:("registerTaskWorker"), vi->fn()}, 
                object::pair{std:("getTasks"), vi->fn()}, 
                object::pair{std:("updateTask"), vi->fn()}
            });
        }
        );
        it(std:("should have correct service type"), [=]() mutable
        {
            expect(TodoService::serviceType)->toBe(std:("TODO"));
        }
        );
        it(std:("should start and initialize service"), [=]() mutable
        {
            auto service = std::async([=]() { TodoService::start(mockRuntime); });
            expect(service)->toBeInstanceOf(TodoService);
            expect(service->capabilityDescription)->toBe(std:("The agent can manage to-do lists with daily recurring and one-off tasks"));
        }
        );
        it(std:("should stop service gracefully"), [=]() mutable
        {
            auto service = std::async([=]() { TodoService::start(mockRuntime); });
            std::async([=]() { service->stop(); });
            expect(true)->toBe(true);
        }
        );
        it(std:("should stop service via static method"), [=]() mutable
        {
            auto service = std::async([=]() { TodoService::start(mockRuntime); });
            mockRuntime->getService = vi->fn()->mockReturnValue(service);
            std::async([=]() { TodoService::stop(mockRuntime); });
            expect(mockRuntime->getService)->toHaveBeenCalledWith(TodoService::serviceType);
        }
        );
    }
    );
}

MAIN
