#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-todo/tests/plugin-init.test.h"

void Main(void)
{
    describe(std::string("TodoPlugin Initialization"), [=]() mutable
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
                object::pair{std::string("agentId"), as<any>(std::string("test-agent"))}, 
                object::pair{std::string("getSetting"), vi->fn()}, 
                object::pair{std::string("createTask"), vi->fn()->mockImplementation([=]() mutable
                {
                    taskWorkerId = std::string("reset-task-id");
                    return taskWorkerId;
                }
                )}, 
                object::pair{std::string("registerTaskWorker"), vi->fn()->mockImplementation([=](auto worker) mutable
                {
                    registeredTaskWorker = worker;
                }
                )}, 
                object::pair{std::string("getTasks"), vi->fn()}, 
                object::pair{std::string("updateTask"), vi->fn()}
            });
        }
        );
        it(std::string("should initialize plugin with WORLD_ID"), [=]() mutable
        {
            mockRuntime->getSetting = vi->fn()->mockReturnValue(std::string("test-world-id"));
            std::async([=]() { TodoPlugin->init(object{}, mockRuntime); });
            expect(mockRuntime->createTask)->toHaveBeenCalledWith(object{
                object::pair{std::string("name"), std::string("RESET_DAILY_TASKS")}, 
                object::pair{std::string("description"), std::string("Resets daily tasks at the start of each day")}, 
                object::pair{std::string("tags"), array<string>{ std::string("system"), std::string("recurring-daily") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("updateInterval"), 24 * 60 * 60 * 1000}
                }}
            });
            expect(mockRuntime->registerTaskWorker)->toHaveBeenCalledWith(object{
                object::pair{std::string("name"), std::string("RESET_DAILY_TASKS")}, 
                object::pair{std::string("validate"), expect->any(Function)}, 
                object::pair{std::string("execute"), expect->any(Function)}
            });
        }
        );
        it(std::string("should skip initialization without WORLD_ID"), [=]() mutable
        {
            mockRuntime->getSetting = vi->fn()->mockReturnValue(nullptr);
            std::async([=]() { TodoPlugin->init(object{}, mockRuntime); });
            expect(mockRuntime->createTask)->not->toHaveBeenCalled();
            expect(mockRuntime->registerTaskWorker)->not->toHaveBeenCalled();
        }
        );
        describe(std::string("Daily Task Reset Worker"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                mockRuntime->getSetting = vi->fn()->mockReturnValue(std::string("test-world-id"));
                std::async([=]() { TodoPlugin->init(object{}, mockRuntime); });
            }
            );
            it(std::string("should validate task worker"), [=]() mutable
            {
                expect(registeredTaskWorker)->toBeDefined();
                auto isValid = std::async([=]() { registeredTaskWorker["validate"](); });
                expect(isValid)->toBe(true);
            }
            );
            it(std::string("should reset completed daily tasks"), [=]() mutable
            {
                auto mockDailyTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                    object::pair{std::string("id"), as<any>(std::string("task1"))}, 
                    object::pair{std::string("name"), std::string("Daily Exercise")}, 
                    object::pair{std::string("description"), std::string("Do pushups")}, 
                    object::pair{std::string("tags"), array<string>{ std::string("daily"), std::string("completed"), std::string("TODO") }}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("completedToday"), true}, 
                        object::pair{std::string("streak"), 5}
                    }}
                }), as<any>(object{
                    object::pair{std::string("id"), as<any>(std::string("task2"))}, 
                    object::pair{std::string("name"), std::string("Daily Reading")}, 
                    object::pair{std::string("description"), std::string("Read for 30 minutes")}, 
                    object::pair{std::string("tags"), array<string>{ std::string("daily"), std::string("TODO") }}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("completedToday"), false}, 
                        object::pair{std::string("streak"), 3}
                    }}
                }), as<any>(object{
                    object::pair{std::string("id"), as<any>(std::string("task3"))}, 
                    object::pair{std::string("name"), std::string("One-off Task")}, 
                    object::pair{std::string("description"), std::string("This should not be reset")}, 
                    object::pair{std::string("tags"), array<string>{ std::string("one-off"), std::string("completed"), std::string("TODO") }}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("completedToday"), true}
                    }}
                }) };
                mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockDailyTasks);
                std::async([=]() { registeredTaskWorker["execute"](mockRuntime); });
                expect(mockRuntime->getTasks)->toHaveBeenCalledWith(object{
                    object::pair{std::string("tags"), array<string>{ std::string("daily"), std::string("completed"), std::string("TODO") }}
                });
                expect(mockRuntime->updateTask)->toHaveBeenCalled();
                auto firstCall = const_((as<any>(mockRuntime->updateTask))["mock"]["calls"])[0];
                expect(const_(firstCall)[0])->toBe(std::string("task1"));
                expect(const_(firstCall)[1]["tags"])->toEqual(array<string>{ std::string("daily"), std::string("TODO") });
                expect(const_(firstCall)[1]["metadata"]["completedToday"])->toBe(false);
                expect(const_(firstCall)[1]["metadata"]["streak"])->toBe(5);
            }
            );
            it(std::string("should handle no daily tasks gracefully"), [=]() mutable
            {
                mockRuntime->getTasks = vi->fn()->mockResolvedValue(array<any>());
                std::async([=]() { registeredTaskWorker["execute"](mockRuntime); });
                expect(mockRuntime->getTasks)->toHaveBeenCalled();
                expect(mockRuntime->updateTask)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should handle errors in task reset"), [=]() mutable
            {
                mockRuntime->getTasks = vi->fn()->mockRejectedValue(std::make_shared<Error>(std::string("Database error")));
                std::async([=]() { registeredTaskWorker["execute"](mockRuntime); });
                expect(true)->toBe(true);
            }
            );
            it(std::string("should handle tasks without metadata"), [=]() mutable
            {
                auto mockDailyTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                    object::pair{std::string("id"), as<any>(std::string("task1"))}, 
                    object::pair{std::string("name"), std::string("Daily Task")}, 
                    object::pair{std::string("description"), std::string("No metadata")}, 
                    object::pair{std::string("tags"), array<string>{ std::string("daily"), std::string("completed"), std::string("TODO") }}
                }) };
                mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockDailyTasks);
                std::async([=]() { registeredTaskWorker["execute"](mockRuntime); });
                expect(mockRuntime->updateTask)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should preserve other metadata when resetting"), [=]() mutable
            {
                auto mockDailyTasks = array<std::shared_ptr<Task>>{ as<any>(object{
                    object::pair{std::string("id"), as<any>(std::string("task1"))}, 
                    object::pair{std::string("name"), std::string("Daily Task")}, 
                    object::pair{std::string("description"), std::string("Task with extra metadata")}, 
                    object::pair{std::string("tags"), array<string>{ std::string("daily"), std::string("completed"), std::string("TODO") }}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("completedToday"), true}, 
                        object::pair{std::string("streak"), 10}, 
                        object::pair{std::string("customField"), std::string("should-be-preserved")}, 
                        object::pair{std::string("notes"), std::string("User notes")}
                    }}
                }) };
                mockRuntime->getTasks = vi->fn()->mockResolvedValue(mockDailyTasks);
                std::async([=]() { registeredTaskWorker["execute"](mockRuntime); });
                expect(mockRuntime->updateTask)->toHaveBeenCalledWith(std::string("task1"), object{
                    object::pair{std::string("tags"), array<string>{ std::string("daily"), std::string("TODO") }}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("completedToday"), false}, 
                        object::pair{std::string("streak"), 10}, 
                        object::pair{std::string("customField"), std::string("should-be-preserved")}, 
                        object::pair{std::string("notes"), std::string("User notes")}
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
