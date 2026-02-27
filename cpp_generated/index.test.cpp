#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-todo/tests/index.test.h"

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
            expect(TodoPlugin->actions)->toHaveLength(4);
            expect(TodoPlugin->services)->toHaveLength(2);
            expect(TodoPlugin->routes)->toBeDefined();
            expect(TodoPlugin->init)->toBeInstanceOf(Function);
        }
        );
        it(std::string("should have all required actions"), [=]() mutable
        {
            auto actionNames = TodoPlugin->actions->map([=](auto action) mutable
            {
                return action["name"];
            }
            );
            expect(actionNames)->toContain(std::string("CREATE_TODO"));
            expect(actionNames)->toContain(std::string("COMPLETE_TODO"));
            expect(actionNames)->toContain(std::string("UPDATE_TODO"));
            expect(actionNames)->toContain(std::string("CANCEL_TODO"));
        }
        );
        it(std::string("should have all required services"), [=]() mutable
        {
            expect(TodoPlugin->services)->toContain(TodoService);
            expect(TodoPlugin->services->some([=](auto s) mutable
            {
                return s["name"] == std::string("TodoReminderService");
            }
            ))->toBe(true);
        }
        );
    }
    );
    describe(std::string("TodoService"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            mockRuntime = as<any>(object{
                object::pair{std::string("agentId"), as<any>(std::string("test-agent"))}, 
                object::pair{std::string("getSetting"), vi->fn()}, 
                object::pair{std::string("getService"), vi->fn()}, 
                object::pair{std::string("createTask"), vi->fn()}, 
                object::pair{std::string("registerTaskWorker"), vi->fn()}, 
                object::pair{std::string("getTasks"), vi->fn()}, 
                object::pair{std::string("updateTask"), vi->fn()}
            });
        }
        );
        it(std::string("should have correct service type"), [=]() mutable
        {
            expect(TodoService::serviceType)->toBe(std::string("TODO"));
        }
        );
        it(std::string("should start and initialize service"), [=]() mutable
        {
            auto service = std::async([=]() { TodoService::start(mockRuntime); });
            expect(service)->toBeInstanceOf(TodoService);
            expect(service->capabilityDescription)->toBe(std::string("The agent can manage to-do lists with daily recurring and one-off tasks"));
        }
        );
        it(std::string("should stop service gracefully"), [=]() mutable
        {
            auto service = std::async([=]() { TodoService::start(mockRuntime); });
            std::async([=]() { service->stop(); });
            expect(true)->toBe(true);
        }
        );
        it(std::string("should stop service via static method"), [=]() mutable
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
