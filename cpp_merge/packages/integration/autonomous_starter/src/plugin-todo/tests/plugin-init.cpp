#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-todo/tests/plugin-init.test.h"

void Main(void)
{
    describe(std:("TodoPlugin Initialization"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<any> taskWorkerId;
        shared<any> registeredTaskWorker;
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            taskWorkerId = undefined;
            registeredTaskWorker = undefined;
            mockRuntime = as<any>(object{
                object::pair{std:("agentId"), as<any>(std:("test-agent"))}, 
                object::pair{std:("getSetting"), vi->fn()}, 
                object::pair{std:("createTask"), vi->fn()->mockImplementation([=]() mutable
                {
                    taskWorkerId = std:("reset-task-id");
                    return taskWorkerId;
                }
                )}, 
                object::pair{std:("registerTaskWorker"), vi->fn()->mockImplementation([=](auto worker) mutable
                {
                    registeredTaskWorker = worker;
                }
                )}, 
                object::pair{std:("getTasks"), vi->fn()}, 
                object::pair{std:("updateTask"), vi->fn()}
            });
        }
        );
        it(std:("should initialize plugin with WORLD_ID"), [=]() mutable
        {
            mockRuntime->getSetting = vi->fn()->mockReturnValue(std:("test-world-id"));
            std::async([=]() { TodoPlugin->init(object{}, mockRuntime); });
            expect(mockRuntime->createTask)->toHaveBeenCalledWith(object{
                object::pair{std:("name"), std:("RESET_DAILY_TASKS")}, 
                object::pair{std:("description"), std:("Resets daily tasks at the start of each day")}, 
                object::pair{std:("tags"), array<string>{ std:("system"), std:("recurring-daily") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("updateInterval"), 24 * 60 * 60 * 1000}
                }}
            });
            expect(mockRuntime->registerTaskWorker)->toHaveBeenCalledWith(object{
                object::pair{std:("name"), std:("RESET_DAILY_TASKS")}, 
                object::pair{std:("validate"), expect->any(Function)}, 
                object::pair{std:("execute"), expect->any(Function)}
            });
        }
        );
        it(std:("should skip initialization without WORLD_ID"), [=]() mutable
        {
            mockRuntime->getSetting = vi->fn()->mockReturnValue(nullptr);
            std::async([=]() { TodoPlugin->init(object{}, mockRuntime); });
            expect(mockRuntime->createTask)->not->toHaveBeenCalled();
            expect(mockRuntime->registerTaskWorker)->not->toHaveBeenCalled();
        }
        );
        describe(std:("Daily Task Reset Worker"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                mockRuntime->getSetting = vi->fn()->mockReturnValue(std:("test-world-id"));
                std::async([=]() { TodoPlugin->init(object{}, mockRuntime); });
            }
            );
            it(std:("should validate task worker"), [=]() mutable
            {
                expect(registeredTaskWorker)->toBeDefined();
                auto isValid = std::async([=]() { registeredTaskWorker["validate"](); });
                expect(isValid)->toBe(true);
            }
            );
            it(std:("should reset completed daily tasks"), [=]() mutable
            {
                auto mockDailyTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                    object::pair{std:("id"), as<any>(std:("task1"))}, 
                    object::pair{std:("name"), std:("Daily Exercise")}, 
                    object::pair{std:("description"), std:("Do pushups")}, 
                    object::pair{std:("tags"), array<string>{ std:("daily"), std:("completed"), std:("TODO") }}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("completedToday"), true}, 
                        object::pair{std:("streak"), 5}
                    }}
                }), as<any>(object{
                    object::pair{std:("id"), as<any>(std:("task2"))}, 
                    object::pair{std:("name"), std:("Daily Reading")}, 
                    object::pair{std:("description"), std:("Read for 30 minutes")}, 
                    object::pair{std:("tags"), array<string>{ std:("daily"), std:("TODO") }}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("completedToday"), false}, 
                        object::pair{std:("streak"), 3}
                    }}
                }), as<any>(object{
                    object::pair{std:("id"), as<any>(std:("task3"))}, 
                    object::pair{std:("name"), std:("One-off Task")}, 
                    object::pair{std:("description"), std:("This should not be reset")}, 
                    object::pair{std:("tags"), array<string>{ std:("one-off"), std:("completed"), std:("TODO") }}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("completedToday"), true}
                    }}
                }) };
                mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockDailyTasks);
                std::async([=]() { registeredTaskWorker["execute"](mockRuntime); });
                expect(mockRuntime->getTasks)->toHaveBeenCalledWith(object{
                    object::pair{std:("tags"), array<string>{ std:("daily"), std:("completed"), std:("TODO") }}
                });
                expect(mockRuntime->updateTask)->toHaveBeenCalled();
                auto firstCall = const_((as<any>(mockRuntime->updateTask))["mock"]["calls"])[0];
                expect(const_(firstCall)[0])->toBe(std:("task1"));
                expect(const_(firstCall)[1]["tags"])->toEqual(array<string>{ std:("daily"), std:("TODO") });
                expect(const_(firstCall)[1]["metadata"]["completedToday"])->toBe(false);
                expect(const_(firstCall)[1]["metadata"]["streak"])->toBe(5);
            }
            );
            it(std:("should handle no daily tasks gracefully"), [=]() mutable
            {
                mockRuntime->getTasks = vi->fn()->mockResolvedValue(array<any>());
                std::async([=]() { registeredTaskWorker["execute"](mockRuntime); });
                expect(mockRuntime->getTasks)->toHaveBeenCalled();
                expect(mockRuntime->updateTask)->not->toHaveBeenCalled();
            }
            );
            it(std:("should handle errors in task reset"), [=]() mutable
            {
                mockRuntime->getTasks = vi->fn()->mockRejectedValue(std::make_shared<Error>(std:("Database error")));
                std::async([=]() { registeredTaskWorker["execute"](mockRuntime); });
                expect(true)->toBe(true);
            }
            );
            it(std:("should handle tasks without metadata"), [=]() mutable
            {
                auto mockDailyTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                    object::pair{std:("id"), as<any>(std:("task1"))}, 
                    object::pair{std:("name"), std:("Daily Task")}, 
                    object::pair{std:("description"), std:("No metadata")}, 
                    object::pair{std:("tags"), array<string>{ std:("daily"), std:("completed"), std:("TODO") }}
                }) };
                mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockDailyTasks);
                std::async([=]() { registeredTaskWorker["execute"](mockRuntime); });
                expect(mockRuntime->updateTask)->not->toHaveBeenCalled();
            }
            );
            it(std:("should preserve other metadata when resetting"), [=]() mutable
            {
                auto mockDailyTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                    object::pair{std:("id"), as<any>(std:("task1"))}, 
                    object::pair{std:("name"), std:("Daily Task")}, 
                    object::pair{std:("description"), std:("Task with extra metadata")}, 
                    object::pair{std:("tags"), array<string>{ std:("daily"), std:("completed"), std:("TODO") }}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("completedToday"), true}, 
                        object::pair{std:("streak"), 10}, 
                        object::pair{std:("customField"), std:("should-be-preserved")}, 
                        object::pair{std:("notes"), std:("User notes")}
                    }}
                }) };
                mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockDailyTasks);
                std::async([=]() { registeredTaskWorker["execute"](mockRuntime); });
                expect(mockRuntime->updateTask)->toHaveBeenCalledWith(std:("task1"), object{
                    object::pair{std:("tags"), array<string>{ std:("daily"), std:("TODO") }}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("completedToday"), false}, 
                        object::pair{std:("streak"), 10}, 
                        object::pair{std:("customField"), std:("should-be-preserved")}, 
                        object::pair{std:("notes"), std:("User notes")}
                    }}
                });
            }
            );
        }
        );
    }
    );
}

MAIN
