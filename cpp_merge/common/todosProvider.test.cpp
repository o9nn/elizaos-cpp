#include "todosProvider.test.h"

void Main(void)
{
    describe(std:("todosProvider"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<Memory>> mockMessage;
        shared<std::shared_ptr<State>> mockState;
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            mockRuntime = as<any>(object{
                object::pair{std:("agentId"), as<any>(std:("test-agent"))}, 
                object::pair{std:("getTasks"), vi->fn()}, 
                object::pair{std:("getRoom"), vi->fn()->mockResolvedValue(object{
                    object::pair{std:("worldId"), std:("test-world")}
                })}, 
                object::pair{std:("getComponent"), vi->fn()->mockResolvedValue(nullptr)}, 
                object::pair{std:("createComponent"), vi->fn()}
            });
            mockMessage = as<any>(object{
                object::pair{std:("entityId"), as<any>(std:("test-entity"))}, 
                object::pair{std:("roomId"), as<any>(std:("test-room"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("test message")}
                }}
            });
            mockState = as<any>(object{});
        }
        );
        it(std:("should have correct provider properties"), [=]() mutable
        {
            expect(todosProvider->name)->toBe(std:("TODOS"));
            expect(todosProvider->description)->toBe(std:("Information about the user's current tasks, completed tasks, and points"));
            expect(todosProvider->get)->toBeInstanceOf(Function);
        }
        );
        it(std:("should return formatted todos when tasks exist"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                object::pair{std:("id"), as<any>(std:("task1"))}, 
                object::pair{std:("name"), std:("Daily Exercise")}, 
                object::pair{std:("description"), std:("Do 50 pushups")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("daily") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("createdAt"), std:("2024-01-01")}, 
                    object::pair{std:("streak"), 5}, 
                    object::pair{std:("lastCompletedAt"), Date->now() - 24 * 60 * 60 * 1000}
                }}
            }), as<any>(object{
                object::pair{std:("id"), as<any>(std:("task2"))}, 
                object::pair{std:("name"), std:("Finish Report")}, 
                object::pair{std:("description"), std:("Complete quarterly report")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("one-off"), std:("priority-2") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("createdAt"), std:("2024-01-02")}, 
                    object::pair{std:("dueDate"), std:("2024-01-15")}
                }}
            }), as<any>(object{
                object::pair{std:("id"), as<any>(std:("task3"))}, 
                object::pair{std:("name"), std:("Read More Books")}, 
                object::pair{std:("description"), std:("Aspirational goal")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("aspirational") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("createdAt"), std:("2024-01-03")}
                }}
            }), as<any>(object{
                object::pair{std:("id"), as<any>(std:("task4"))}, 
                object::pair{std:("name"), std:("Completed Task")}, 
                object::pair{std:("description"), std:("Already done")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("one-off"), std:("completed") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("createdAt"), std:("2024-01-04")}, 
                    object::pair{std:("completedAt"), std:("2024-01-05")}
                }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            expect(mockRuntime->getTasks)->toHaveBeenCalled();
            expect(result->text)->toContain(std:("User's Todos"));
            expect(result->text)->toContain(std:("Points: 0"));
            expect(result->text)->toContain(std:("Daily Todos"));
            expect(result->text)->toContain(std:("Daily Exercise"));
            expect(result->text)->toContain(std:("daily, streak: 5 days"));
            expect(result->text)->toContain(std:("One-off Todos"));
            expect(result->text)->toContain(std:("Finish Report"));
            expect(result->text)->toContain(std:("P2"));
            expect(result->text)->toContain(std:("due"));
            expect(result->text)->toContain(std:("Aspirational Todos"));
            expect(result->text)->toContain(std:("Read More Books"));
            expect(result->data)->toHaveProperty(std:("userPoints"), 0);
            expect(result->data)->toHaveProperty(std:("dailyTasks"));
            expect(result->data)->toHaveProperty(std:("oneOffTasks"));
            expect(result->data)->toHaveProperty(std:("aspirationalTasks"));
            expect(result->data)->toHaveProperty(std:("completedTasks"));
            expect(result->data->dailyTasks)->toHaveLength(1);
            expect(result->data->oneOffTasks)->toHaveLength(1);
            expect(result->data->aspirationalTasks)->toHaveLength(1);
        }
        );
        it(std:("should return no tasks message when no tasks exist"), [=]() mutable
        {
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(array<any>());
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            expect(result->text)->toContain(std:("No daily todos"));
            expect(result->text)->toContain(std:("No one-off todos"));
            expect(result->text)->toContain(std:("No aspirational todos"));
            expect(result->data)->toEqual(object{
                object::pair{std:("userPoints"), 0}, 
                object::pair{std:("dailyTasks"), array<any>()}, 
                object::pair{std:("oneOffTasks"), array<any>()}, 
                object::pair{std:("aspirationalTasks"), array<any>()}, 
                object::pair{std:("completedTasks"), array<any>()}
            });
        }
        );
        it(std:("should handle overdue tasks"), [=]() mutable
        {
            auto overdueDate = std::make_shared<Date>(Date->now() - 2 * 24 * 60 * 60 * 1000);
            auto mockTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                object::pair{std:("id"), as<any>(std:("task1"))}, 
                object::pair{std:("name"), std:("Overdue Task")}, 
                object::pair{std:("description"), std:("This is overdue")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("one-off") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("dueDate"), overdueDate->toISOString()}
                }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            expect(result->text)->toContain(std:("Overdue Task"));
            expect(result->text)->toContain(std:("due"));
        }
        );
        it(std:("should handle tasks without metadata gracefully"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                object::pair{std:("id"), as<any>(std:("task1"))}, 
                object::pair{std:("name"), std:("Task without metadata")}, 
                object::pair{std:("description"), std:("No metadata")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("one-off") }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            expect(result->text)->toContain(std:("Task without metadata"));
            expect(result->text)->not->toContain(std:("undefined"));
        }
        );
        it(std:("should sort tasks by type correctly"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                object::pair{std:("id"), as<any>(std:("task3"))}, 
                object::pair{std:("name"), std:("Aspirational")}, 
                object::pair{std:("description"), std:("Goal")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("aspirational") }}
            }), as<any>(object{
                object::pair{std:("id"), as<any>(std:("task1"))}, 
                object::pair{std:("name"), std:("Daily")}, 
                object::pair{std:("description"), std:("Daily task")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("daily") }}
            }), as<any>(object{
                object::pair{std:("id"), as<any>(std:("task2"))}, 
                object::pair{std:("name"), std:("One-off")}, 
                object::pair{std:("description"), std:("One-off task")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("one-off") }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            auto dailyIndex = result->text->indexOf(std:("Daily Todos"));
            auto oneOffIndex = result->text->indexOf(std:("One-off Todos"));
            auto aspirationalIndex = result->text->indexOf(std:("Aspirational Todos"));
            expect(dailyIndex)->toBeGreaterThan(-1);
            expect(oneOffIndex)->toBeGreaterThan(-1);
            expect(aspirationalIndex)->toBeGreaterThan(-1);
            expect(dailyIndex)->toBeLessThan(oneOffIndex);
            expect(oneOffIndex)->toBeLessThan(aspirationalIndex);
        }
        );
        it(std:("should handle daily tasks completed today"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                object::pair{std:("id"), as<any>(std:("task1"))}, 
                object::pair{std:("name"), std:("Daily Task")}, 
                object::pair{std:("description"), std:("Completed today")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("daily"), std:("completed") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("streak"), 10}, 
                    object::pair{std:("lastCompletedAt"), Date->now() - 2 * 60 * 60 * 1000}, 
                    object::pair{std:("completedToday"), true}
                }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            expect(result->text)->toContain(std:("No daily todos"));
            expect(result->text)->toContain(std:("Recently Completed"));
        }
        );
        it(std:("should format priority levels correctly"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                object::pair{std:("id"), as<any>(std:("task1"))}, 
                object::pair{std:("name"), std:("High Priority")}, 
                object::pair{std:("description"), std:("Priority 1")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("one-off"), std:("priority-1") }}
            }), as<any>(object{
                object::pair{std:("id"), as<any>(std:("task2"))}, 
                object::pair{std:("name"), std:("Medium Priority")}, 
                object::pair{std:("description"), std:("Priority 3")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("one-off"), std:("priority-3") }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            expect(result->text)->toContain(std:("P1"));
            expect(result->text)->toContain(std:("P3"));
        }
        );
        it(std:("should handle urgent tasks"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                object::pair{std:("id"), as<any>(std:("task1"))}, 
                object::pair{std:("name"), std:("Urgent Task")}, 
                object::pair{std:("description"), std:("This is urgent")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("one-off"), std:("urgent") }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            expect(result->text)->toContain(std:("URGENT"));
        }
        );
    }
    );
}

MAIN
