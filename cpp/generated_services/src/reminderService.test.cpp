#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-todo/tests/reminderService.test.h"

void Main(void)
{
    describe(std::string("TodoReminderService"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<TodoReminderService>> service;
        beforeEach([=]() mutable
        {
            mockRuntime = as<any>(object{
                object::pair{std::string("agentId"), as<any>(std::string("test-agent"))}, 
                object::pair{std::string("getTasks"), vi->fn()}, 
                object::pair{std::string("updateTask"), vi->fn()}, 
                object::pair{std::string("emitEvent"), vi->fn()}, 
                object::pair{std::string("getService"), vi->fn()}
            });
        }
        );
        afterEach([=]() mutable
        {
            if (service) {
                std::async([=]() { service->stop(); });
            }
        }
        );
        it(std::string("should have correct service type"), [=]() mutable
        {
            expect(TodoReminderService::serviceType)->toBe(std::string("TODO_REMINDER"));
        }
        );
        it(std::string("should start service and begin timer"), [=]() mutable
        {
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            expect(service)->toBeInstanceOf(TodoReminderService);
            expect(service->capabilityDescription)->toBe(std::string("The agent can send reminders for overdue tasks"));
        }
        );
        it(std::string("should stop service and clear timer"), [=]() mutable
        {
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            std::async([=]() { service->stop(); });
            expect(true)->toBe(true);
        }
        );
        it(std::string("should start and setup timer for periodic checks"), [=]() mutable
        {
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            expect(service)->toBeInstanceOf(TodoReminderService);
            std::async([=]() { service->checkTasksNow(); });
            expect(mockRuntime->getTasks)->toHaveBeenCalledWith(object{
                object::pair{std::string("tags"), array<string>{ std::string("one-off") }}
            });
        }
        );
        it(std::string("should send reminder for overdue tasks"), [=]() mutable
        {
            auto overdueDate = std::make_shared<Date>(Date->now() - 2 * 24 * 60 * 60 * 1000);
            auto mockTasks = array<std::shared_ptr<Task>>{ as<std::shared_ptr<Task>>(object{
                object::pair{std::string("id"), as<any>(std::string("task1"))}, 
                object::pair{std::string("roomId"), as<any>(std::string("room1"))}, 
                object::pair{std::string("name"), std::string("Overdue task")}, 
                object::pair{std::string("description"), std::string("Test overdue task")}, 
                object::pair{std::string("tags"), array<string>{ std::string("one-off") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("dueDate"), overdueDate->toISOString()}
                }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            std::async([=]() { service->checkTasksNow(); });
            expect(mockRuntime->emitEvent)->toHaveBeenCalledWith(std::string("MESSAGE_RECEIVED"), expect->objectContaining(object{
                object::pair{std::string("message"), expect->objectContaining(object{
                    object::pair{std::string("content"), expect->objectContaining(object{
                        object::pair{std::string("text"), expect->stringContaining(std::string("Task Reminder"))}
                    })}
                })}
            }));
            expect(mockRuntime->updateTask)->toHaveBeenCalledWith(std::string("task1"), expect->objectContaining(object{
                object::pair{std::string("metadata"), expect->objectContaining(object{
                    object::pair{std::string("lastReminderSent"), expect->any(String)}
                })}
            }));
        }
        );
        it(std::string("should respect reminder cooldown period"), [=]() mutable
        {
            auto overdueDate = std::make_shared<Date>(Date->now() - 2 * 24 * 60 * 60 * 1000);
            auto recentReminderDate = std::make_shared<Date>(Date->now() - 12 * 60 * 60 * 1000);
            auto mockTasks = array<std::shared_ptr<Task>>{ as<std::shared_ptr<Task>>(object{
                object::pair{std::string("id"), as<any>(std::string("task1"))}, 
                object::pair{std::string("roomId"), as<any>(std::string("room1"))}, 
                object::pair{std::string("name"), std::string("Overdue task with recent reminder")}, 
                object::pair{std::string("description"), std::string("Test task with recent reminder")}, 
                object::pair{std::string("tags"), array<string>{ std::string("one-off") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("dueDate"), overdueDate->toISOString()}, 
                    object::pair{std::string("lastReminderSent"), recentReminderDate->toISOString()}
                }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            std::async([=]() { service->checkTasksNow(); });
            expect(mockRuntime->emitEvent)->not->toHaveBeenCalled();
            expect(mockRuntime->updateTask)->not->toHaveBeenCalled();
        }
        );
        it(std::string("should handle tasks without due dates gracefully"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<std::shared_ptr<Task>>(object{
                object::pair{std::string("id"), as<any>(std::string("task1"))}, 
                object::pair{std::string("roomId"), as<any>(std::string("room1"))}, 
                object::pair{std::string("name"), std::string("Task without due date")}, 
                object::pair{std::string("description"), std::string("Test task without due date")}, 
                object::pair{std::string("tags"), array<string>{ std::string("one-off") }}, 
                object::pair{std::string("metadata"), object{}}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            std::async([=]() { service->checkTasksNow(); });
            expect(mockRuntime->emitEvent)->not->toHaveBeenCalled();
        }
        );
        it(std::string("should handle invalid date formats"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<std::shared_ptr<Task>>(object{
                object::pair{std::string("id"), as<any>(std::string("task1"))}, 
                object::pair{std::string("roomId"), as<any>(std::string("room1"))}, 
                object::pair{std::string("name"), std::string("Task with invalid date")}, 
                object::pair{std::string("description"), std::string("Test task with invalid date")}, 
                object::pair{std::string("tags"), array<string>{ std::string("one-off") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("dueDate"), std::string("invalid-date")}
                }}
            }), as<std::shared_ptr<Task>>(object{
                object::pair{std::string("id"), as<any>(std::string("task2"))}, 
                object::pair{std::string("roomId"), as<any>(std::string("room2"))}, 
                object::pair{std::string("name"), std::string("Task with null date")}, 
                object::pair{std::string("description"), std::string("Test task with null date")}, 
                object::pair{std::string("tags"), array<string>{ std::string("one-off") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("dueDate"), nullptr}
                }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            std::async([=]() { service->checkTasksNow(); });
            expect(mockRuntime->emitEvent)->not->toHaveBeenCalled();
        }
        );
        it(std::string("should skip tasks without roomId"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<std::shared_ptr<Task>>(object{
                object::pair{std::string("id"), as<any>(std::string("task1"))}, 
                object::pair{std::string("roomId"), as<any>(undefined)}, 
                object::pair{std::string("name"), std::string("Task without room")}, 
                object::pair{std::string("description"), std::string("Test task without room")}, 
                object::pair{std::string("tags"), array<string>{ std::string("one-off") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("dueDate"), ((std::make_shared<Date>(Date->now() - 24 * 60 * 60 * 1000)))->toISOString()}
                }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            std::async([=]() { service->checkTasksNow(); });
            expect(mockRuntime->emitEvent)->not->toHaveBeenCalled();
        }
        );
        it(std::string("should handle errors in checkOverdueTasks gracefully"), [=]() mutable
        {
            mockRuntime->getTasks = vi->fn()->mockRejectedValue(std::make_shared<Error>(std::string("Database error")));
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            std::async([=]() { service->checkTasksNow(); });
            expect(true)->toBe(true);
        }
        );
        it(std::string("should stop service via static method"), [=]() mutable
        {
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            mockRuntime->getService = vi->fn()->mockReturnValue(service);
            auto stopSpy = vi->spyOn(service, std::string("stop"));
            std::async([=]() { TodoReminderService::stop(mockRuntime); });
            expect(mockRuntime->getService)->toHaveBeenCalledWith(TodoReminderService::serviceType);
            expect(stopSpy)->toHaveBeenCalled();
        }
        );
    }
    );
}

MAIN
