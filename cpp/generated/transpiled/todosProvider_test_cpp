#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-todo/tests/todosProvider.test.h"

void Main(void)
{
    describe(std::string("todosProvider"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<Memory>> mockMessage;
        shared<std::shared_ptr<State>> mockState;
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            mockRuntime = as<any>(object{
                object::pair{std::string("agentId"), as<any>(std::string("test-agent"))}, 
                object::pair{std::string("getTasks"), vi->fn()}, 
                object::pair{std::string("getRoom"), vi->fn()->mockResolvedValue(object{
                    object::pair{std::string("worldId"), std::string("test-world")}
                })}, 
                object::pair{std::string("getComponent"), vi->fn()->mockResolvedValue(nullptr)}, 
                object::pair{std::string("createComponent"), vi->fn()}
            });
            mockMessage = as<any>(object{
                object::pair{std::string("entityId"), as<any>(std::string("test-entity"))}, 
                object::pair{std::string("roomId"), as<any>(std::string("test-room"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("test message")}
                }}
            });
            mockState = as<any>(object{});
        }
        );
        it(std::string("should have correct provider properties"), [=]() mutable
        {
            expect(todosProvider->name)->toBe(std::string("TODOS"));
            expect(todosProvider->description)->toBe(std::string("Information about the user's current tasks, completed tasks, and points"));
            expect(todosProvider->get)->toBeInstanceOf(Function);
        }
        );
        it(std::string("should return formatted todos when tasks exist"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                object::pair{std::string("id"), as<any>(std::string("task1"))}, 
                object::pair{std::string("name"), std::string("Daily Exercise")}, 
                object::pair{std::string("description"), std::string("Do 50 pushups")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("daily") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("createdAt"), std::string("2024-01-01")}, 
                    object::pair{std::string("streak"), 5}, 
                    object::pair{std::string("lastCompletedAt"), Date->now() - 24 * 60 * 60 * 1000}
                }}
            }), as<any>(object{
                object::pair{std::string("id"), as<any>(std::string("task2"))}, 
                object::pair{std::string("name"), std::string("Finish Report")}, 
                object::pair{std::string("description"), std::string("Complete quarterly report")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("one-off"), std::string("priority-2") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("createdAt"), std::string("2024-01-02")}, 
                    object::pair{std::string("dueDate"), std::string("2024-01-15")}
                }}
            }), as<any>(object{
                object::pair{std::string("id"), as<any>(std::string("task3"))}, 
                object::pair{std::string("name"), std::string("Read More Books")}, 
                object::pair{std::string("description"), std::string("Aspirational goal")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("aspirational") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("createdAt"), std::string("2024-01-03")}
                }}
            }), as<any>(object{
                object::pair{std::string("id"), as<any>(std::string("task4"))}, 
                object::pair{std::string("name"), std::string("Completed Task")}, 
                object::pair{std::string("description"), std::string("Already done")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("one-off"), std::string("completed") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("createdAt"), std::string("2024-01-04")}, 
                    object::pair{std::string("completedAt"), std::string("2024-01-05")}
                }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            expect(mockRuntime->getTasks)->toHaveBeenCalled();
            expect(result->text)->toContain(std::string("User's Todos"));
            expect(result->text)->toContain(std::string("Points: 0"));
            expect(result->text)->toContain(std::string("Daily Todos"));
            expect(result->text)->toContain(std::string("Daily Exercise"));
            expect(result->text)->toContain(std::string("daily, streak: 5 days"));
            expect(result->text)->toContain(std::string("One-off Todos"));
            expect(result->text)->toContain(std::string("Finish Report"));
            expect(result->text)->toContain(std::string("P2"));
            expect(result->text)->toContain(std::string("due"));
            expect(result->text)->toContain(std::string("Aspirational Todos"));
            expect(result->text)->toContain(std::string("Read More Books"));
            expect(result->data)->toHaveProperty(std::string("userPoints"), 0);
            expect(result->data)->toHaveProperty(std::string("dailyTasks"));
            expect(result->data)->toHaveProperty(std::string("oneOffTasks"));
            expect(result->data)->toHaveProperty(std::string("aspirationalTasks"));
            expect(result->data)->toHaveProperty(std::string("completedTasks"));
            expect(result->data->dailyTasks)->toHaveLength(1);
            expect(result->data->oneOffTasks)->toHaveLength(1);
            expect(result->data->aspirationalTasks)->toHaveLength(1);
        }
        );
        it(std::string("should return no tasks message when no tasks exist"), [=]() mutable
        {
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(array<any>());
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            expect(result->text)->toContain(std::string("No daily todos"));
            expect(result->text)->toContain(std::string("No one-off todos"));
            expect(result->text)->toContain(std::string("No aspirational todos"));
            expect(result->data)->toEqual(object{
                object::pair{std::string("userPoints"), 0}, 
                object::pair{std::string("dailyTasks"), array<any>()}, 
                object::pair{std::string("oneOffTasks"), array<any>()}, 
                object::pair{std::string("aspirationalTasks"), array<any>()}, 
                object::pair{std::string("completedTasks"), array<any>()}
            });
        }
        );
        it(std::string("should handle overdue tasks"), [=]() mutable
        {
            auto overdueDate = std::make_shared<Date>(Date->now() - 2 * 24 * 60 * 60 * 1000);
            auto mockTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                object::pair{std::string("id"), as<any>(std::string("task1"))}, 
                object::pair{std::string("name"), std::string("Overdue Task")}, 
                object::pair{std::string("description"), std::string("This is overdue")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("one-off") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("dueDate"), overdueDate->toISOString()}
                }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            expect(result->text)->toContain(std::string("Overdue Task"));
            expect(result->text)->toContain(std::string("due"));
        }
        );
        it(std::string("should handle tasks without metadata gracefully"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                object::pair{std::string("id"), as<any>(std::string("task1"))}, 
                object::pair{std::string("name"), std::string("Task without metadata")}, 
                object::pair{std::string("description"), std::string("No metadata")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("one-off") }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            expect(result->text)->toContain(std::string("Task without metadata"));
            expect(result->text)->not->toContain(std::string("undefined"));
        }
        );
        it(std::string("should sort tasks by type correctly"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                object::pair{std::string("id"), as<any>(std::string("task3"))}, 
                object::pair{std::string("name"), std::string("Aspirational")}, 
                object::pair{std::string("description"), std::string("Goal")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("aspirational") }}
            }), as<any>(object{
                object::pair{std::string("id"), as<any>(std::string("task1"))}, 
                object::pair{std::string("name"), std::string("Daily")}, 
                object::pair{std::string("description"), std::string("Daily task")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("daily") }}
            }), as<any>(object{
                object::pair{std::string("id"), as<any>(std::string("task2"))}, 
                object::pair{std::string("name"), std::string("One-off")}, 
                object::pair{std::string("description"), std::string("One-off task")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("one-off") }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            auto dailyIndex = result->text->indexOf(std::string("Daily Todos"));
            auto oneOffIndex = result->text->indexOf(std::string("One-off Todos"));
            auto aspirationalIndex = result->text->indexOf(std::string("Aspirational Todos"));
            expect(dailyIndex)->toBeGreaterThan(-1);
            expect(oneOffIndex)->toBeGreaterThan(-1);
            expect(aspirationalIndex)->toBeGreaterThan(-1);
            expect(dailyIndex)->toBeLessThan(oneOffIndex);
            expect(oneOffIndex)->toBeLessThan(aspirationalIndex);
        }
        );
        it(std::string("should handle daily tasks completed today"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                object::pair{std::string("id"), as<any>(std::string("task1"))}, 
                object::pair{std::string("name"), std::string("Daily Task")}, 
                object::pair{std::string("description"), std::string("Completed today")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("daily"), std::string("completed") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("streak"), 10}, 
                    object::pair{std::string("lastCompletedAt"), Date->now() - 2 * 60 * 60 * 1000}, 
                    object::pair{std::string("completedToday"), true}
                }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            expect(result->text)->toContain(std::string("No daily todos"));
            expect(result->text)->toContain(std::string("Recently Completed"));
        }
        );
        it(std::string("should format priority levels correctly"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                object::pair{std::string("id"), as<any>(std::string("task1"))}, 
                object::pair{std::string("name"), std::string("High Priority")}, 
                object::pair{std::string("description"), std::string("Priority 1")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("one-off"), std::string("priority-1") }}
            }), as<any>(object{
                object::pair{std::string("id"), as<any>(std::string("task2"))}, 
                object::pair{std::string("name"), std::string("Medium Priority")}, 
                object::pair{std::string("description"), std::string("Priority 3")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("one-off"), std::string("priority-3") }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            expect(result->text)->toContain(std::string("P1"));
            expect(result->text)->toContain(std::string("P3"));
        }
        );
        it(std::string("should handle urgent tasks"), [=]() mutable
        {
            auto mockTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                object::pair{std::string("id"), as<any>(std::string("task1"))}, 
                object::pair{std::string("name"), std::string("Urgent Task")}, 
                object::pair{std::string("description"), std::string("This is urgent")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("one-off"), std::string("urgent") }}
            }) };
            mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockTasks);
            auto result = std::async([=]() { todosProvider->get(mockRuntime, mockMessage, mockState); });
            expect(result->text)->toContain(std::string("URGENT"));
        }
        );
    }
    );
}

MAIN
