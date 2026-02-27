#include "reminderService.test.h"

void Main(void)
{
    describe(std:("TodoReminderService"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<TodoReminderService>> service;
        beforeEach([=]() mutable
        {
            mockRuntime = as<any>(object{
                object::pair{std:("agentId"), as<any>(std:("test-agent"))}, 
                object::pair{std:("getTasks"), vi->fn()}, 
                object::pair{std:("updateTask"), vi->fn()}, 
                object::pair{std:("emitEvent"), vi->fn()}, 
                object::pair{std:("getService"), vi->fn()}
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
        it(std:("should have correct service type"), [=]() mutable
        {
            expect(TodoReminderService::serviceType)->toBe(std:("TODO_REMINDER"));
        }
        );
        it(std:("should start service and begin timer"), [=]() mutable
        {
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            expect(service)->toBeInstanceOf(TodoReminderService);
            expect(service->capabilityDescription)->toBe(std:("The agent can send reminders for overdue tasks"));
        }
        );
        it(std:("should stop service and clear timer"), [=]() mutable
        {
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            std::async([=]() { service->stop(); });
            expect(true)->toBe(true);
        }
        );
        it(std:("should start and setup timer for periodic checks"), [=]() mutable
        {
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            expect(service)->toBeInstanceOf(TodoReminderService);
            std::async([=]() { service->checkTasksNow(); });
            expect(mockRuntime->getTasks)->toHaveBeenCalledWith(object{
                object::pair{std:("tags"), array<string>{ std:("one-off") }}
            });
        }
        );
        it(std:("should send reminder for overdue tasks"), [=]() mutable
        {
            auto overdueDate = std::make_shared<Date>(Date->now() - 2 * 24 * 60 * 60 * 1000);
            auto mockTasks = array<std::shared_ptr<Task>>{ as<std::shared_ptr<Task>>(object{
                object::pair{std:("id"), as<any>(std:("task1"))}, 
                object::pair{std:("roomId"), as<any>(std:("room1"))}, 
                object::pair{std:("name"), std:("Overdue task")}, 
                object::pair{std:("description"), std:("Test overdue task")}, 
                object::pair{std:("tags"), array<string>{ std:("one-off") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("dueDate"), overdueDate->toISOString()}
                }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            std::async([=]() { service->checkTasksNow(); });
            expect(mockRuntime->emitEvent)->toHaveBeenCalledWith(std:("MESSAGE_RECEIVED"), expect->objectContaining(object{
                object::pair{std:("message"), expect->objectContaining(object{
                    object::pair{std:("content"), expect->objectContaining(object{
                        object::pair{std:("text"), expect->stringContaining(std:("Task Reminder"))}
                    })}
                })}
            }));
            expect(mockRuntime->updateTask)->toHaveBeenCalledWith(std:("task1"), expect->objectContaining(object{
                object::pair{std:("metadata"), expect->objectContaining(object{
                    object::pair{std:("lastReminderSent"), expect->any(String)}
                })}
            }));
        }
        );
        it(std:("should respect reminder cooldown period"), [=]() mutable
        {
            auto overdueDate = std::make_shared<Date>(Date->now() - 2 * 24 * 60 * 60 * 1000);
            auto recentReminderDate = std::make_shared<Date>(Date->now() - 12 * 60 * 60 * 1000);
            auto mockTasks = array<std::shared_ptr<Task>>{ as<std::shared_ptr<Task>>(object{
                object::pair{std:("id"), as<any>(std:("task1"))}, 
                object::pair{std:("roomId"), as<any>(std:("room1"))}, 
                object::pair{std:("name"), std:("Overdue task with recent reminder")}, 
                object::pair{std:("description"), std:("Test task with recent reminder")}, 
                object::pair{std:("tags"), array<string>{ std:("one-off") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("dueDate"), overdueDate->toISOString()}, 
                    object::pair{std:("lastReminderSent"), recentReminderDate->toISOString()}
                }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            std::async([=]() { service->checkTasksNow(); });
            expect(mockRuntime->emitEvent)->not->toHaveBeenCalled();
            expect(mockRuntime->updateTask)->not->toHaveBeenCalled();
        }
        );
        it(std:("should handle tasks without due dates gracefully"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<std::shared_ptr<Task>>(object{
                object::pair{std:("id"), as<any>(std:("task1"))}, 
                object::pair{std:("roomId"), as<any>(std:("room1"))}, 
                object::pair{std:("name"), std:("Task without due date")}, 
                object::pair{std:("description"), std:("Test task without due date")}, 
                object::pair{std:("tags"), array<string>{ std:("one-off") }}, 
                object::pair{std:("metadata"), object{}}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            std::async([=]() { service->checkTasksNow(); });
            expect(mockRuntime->emitEvent)->not->toHaveBeenCalled();
        }
        );
        it(std:("should handle invalid date formats"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<std::shared_ptr<Task>>(object{
                object::pair{std:("id"), as<any>(std:("task1"))}, 
                object::pair{std:("roomId"), as<any>(std:("room1"))}, 
                object::pair{std:("name"), std:("Task with invalid date")}, 
                object::pair{std:("description"), std:("Test task with invalid date")}, 
                object::pair{std:("tags"), array<string>{ std:("one-off") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("dueDate"), std:("invalid-date")}
                }}
            }), as<std::shared_ptr<Task>>(object{
                object::pair{std:("id"), as<any>(std:("task2"))}, 
                object::pair{std:("roomId"), as<any>(std:("room2"))}, 
                object::pair{std:("name"), std:("Task with null date")}, 
                object::pair{std:("description"), std:("Test task with null date")}, 
                object::pair{std:("tags"), array<string>{ std:("one-off") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("dueDate"), nullptr}
                }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            std::async([=]() { service->checkTasksNow(); });
            expect(mockRuntime->emitEvent)->not->toHaveBeenCalled();
        }
        );
        it(std:("should skip tasks without roomId"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<std::shared_ptr<Task>>(object{
                object::pair{std:("id"), as<any>(std:("task1"))}, 
                object::pair{std:("roomId"), as<any>(undefined)}, 
                object::pair{std:("name"), std:("Task without room")}, 
                object::pair{std:("description"), std:("Test task without room")}, 
                object::pair{std:("tags"), array<string>{ std:("one-off") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("dueDate"), ((std::make_shared<Date>(Date->now() - 24 * 60 * 60 * 1000)))->toISOString()}
                }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            std::async([=]() { service->checkTasksNow(); });
            expect(mockRuntime->emitEvent)->not->toHaveBeenCalled();
        }
        );
        it(std:("should handle errors in checkOverdueTasks gracefully"), [=]() mutable
        {
            mockRuntime->getTasks = vi->fn()->mockRejectedValue(std::make_shared<Error>(std:("Database error")));
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            std::async([=]() { service->checkTasksNow(); });
            expect(true)->toBe(true);
        }
        );
        it(std:("should stop service via static method"), [=]() mutable
        {
            service = std::async([=]() { TodoReminderService::start(mockRuntime); });
            mockRuntime->getService = vi->fn()->mockReturnValue(service);
            auto stopSpy = vi->spyOn(service, std:("stop"));
            std::async([=]() { TodoReminderService::stop(mockRuntime); });
            expect(mockRuntime->getService)->toHaveBeenCalledWith(TodoReminderService::serviceType);
            expect(stopSpy)->toHaveBeenCalled();
        }
        );
    }
    );
}

MAIN
