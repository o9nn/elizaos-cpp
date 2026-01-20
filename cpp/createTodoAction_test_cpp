#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-todo/tests/createTodoAction.test.h"

void Main(void)
{
    describe(std::string("createTodoAction"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<HandlerCallback>> mockCallback;
        shared<std::shared_ptr<State>> mockState;
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            mockRuntime = as<any>(object{
                object::pair{std::string("agentId"), as<any>(std::string("test-agent"))}, 
                object::pair{std::string("getTasks"), vi->fn()->mockResolvedValue(array<any>())}, 
                object::pair{std::string("createTask"), vi->fn()->mockResolvedValue(std::string("new-task-id"))}, 
                object::pair{std::string("getRoom"), vi->fn()->mockResolvedValue(object{
                    object::pair{std::string("worldId"), std::string("test-world")}
                })}, 
                object::pair{std::string("ensureConnection"), vi->fn()}, 
                object::pair{std::string("composeState"), vi->fn()->mockImplementation([=]() mutable
                {
                    return mockState;
                }
                )}, 
                object::pair{std::string("useModel"), vi->fn()}
            });
            mockCallback = vi->fn();
            mockState = as<any>(object{
                object::pair{std::string("data"), object{
                    object::pair{std::string("tasks"), array<any>()}, 
                    object::pair{std::string("room"), object{
                        object::pair{std::string("worldId"), std::string("test-world")}
                    }}, 
                    object::pair{std::string("messages"), array<any>()}, 
                    object::pair{std::string("entities"), array<any>()}
                }}
            });
        }
        );
        it(std::string("should have correct action properties"), [=]() mutable
        {
            expect(createTodoAction->name)->toBe(std::string("CREATE_TODO"));
            expect(createTodoAction->similes)->toContain(std::string("ADD_TODO"));
            expect(createTodoAction->similes)->toContain(std::string("NEW_TASK"));
            expect(createTodoAction->description)->toContain(std::string("Creates a new todo item"));
            expect(createTodoAction->validate)->toBeInstanceOf(Function);
            expect(createTodoAction->handler)->toBeInstanceOf(Function);
            expect(createTodoAction->examples)->toHaveLength(3);
        }
        );
        it(std::string("should validate always return true"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Add todo")}
                }}
            });
            auto result = std::async([=]() { createTodoAction->validate(mockRuntime, message); });
            expect(result)->toBe(true);
        }
        );
        it(std::string("should create a daily todo successfully"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Add daily task to do 50 pushups")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("roomId"), as<any>(std::string("room1"))}, 
                object::pair{std::string("entityId"), as<any>(std::string("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std::string("\
      <response>\
        <name>Do 50 pushups</name>\
        <description>Daily exercise routine</description>\
        <taskType>daily</taskType>\
        <recurring>daily</recurring>\
      </response>\
    "));
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockRuntime->createTask)->toHaveBeenCalledWith(object{
                object::pair{std::string("name"), std::string("Do 50 pushups")}, 
                object::pair{std::string("description"), std::string("Daily exercise routine")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("daily"), std::string("recurring-daily") }}, 
                object::pair{std::string("metadata"), expect->objectContaining(object{
                    object::pair{std::string("createdAt"), expect->any(String)}, 
                    object::pair{std::string("description"), std::string("Daily exercise routine")}, 
                    object::pair{std::string("streak"), 0}
                })}, 
                object::pair{std::string("roomId"), std::string("room1")}, 
                object::pair{std::string("worldId"), std::string("test-world")}, 
                object::pair{std::string("entityId"), std::string("entity1")}
            });
            expect(mockCallback)->toHaveBeenCalledWith(object{
                object::pair{std::string("text"), expect->stringContaining(std::string("Added new daily task: "Do 50 pushups""))}, 
                object::pair{std::string("actions"), array<string>{ std::string("CREATE_TODO_SUCCESS") }}, 
                object::pair{std::string("source"), std::string("test")}
            });
        }
        );
        it(std::string("should create a one-off todo with due date"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Add todo to finish taxes by April 15")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("roomId"), as<any>(std::string("room1"))}, 
                object::pair{std::string("entityId"), as<any>(std::string("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std::string("\
      <response>\
        <name>Finish taxes</name>\
        <description>Complete tax filing</description>\
        <taskType>one-off</taskType>\
        <priority>2</priority>\
        <urgent>false</urgent>\
        <dueDate>2024-04-15</dueDate>\
      </response>\
    "));
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockRuntime->createTask)->toHaveBeenCalledWith(object{
                object::pair{std::string("name"), std::string("Finish taxes")}, 
                object::pair{std::string("description"), std::string("Complete tax filing")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("one-off"), std::string("priority-2") }}, 
                object::pair{std::string("metadata"), expect->objectContaining(object{
                    object::pair{std::string("createdAt"), expect->any(String)}, 
                    object::pair{std::string("description"), std::string("Complete tax filing")}, 
                    object::pair{std::string("dueDate"), std::string("2024-04-15")}
                })}, 
                object::pair{std::string("roomId"), std::string("room1")}, 
                object::pair{std::string("worldId"), std::string("test-world")}, 
                object::pair{std::string("entityId"), std::string("entity1")}
            });
            expect(mockCallback)->toHaveBeenCalledWith(object{
                object::pair{std::string("text"), expect->stringMatching((new RegExp(std::string("Added new one-off task.*Finish taxes.*Priority 2.*Due"))))}, 
                object::pair{std::string("actions"), array<string>{ std::string("CREATE_TODO_SUCCESS") }}, 
                object::pair{std::string("source"), std::string("test")}
            });
        }
        );
        it(std::string("should create an aspirational todo"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Add goal to read more books")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("roomId"), as<any>(std::string("room1"))}, 
                object::pair{std::string("entityId"), as<any>(std::string("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std::string("\
      <response>\
        <name>Read more books</name>\
        <taskType>aspirational</taskType>\
      </response>\
    "));
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockRuntime->createTask)->toHaveBeenCalledWith(object{
                object::pair{std::string("name"), std::string("Read more books")}, 
                object::pair{std::string("description"), std::string("Read more books")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("aspirational") }}, 
                object::pair{std::string("metadata"), expect->objectContaining(object{
                    object::pair{std::string("createdAt"), expect->any(String)}
                })}, 
                object::pair{std::string("roomId"), std::string("room1")}, 
                object::pair{std::string("worldId"), std::string("test-world")}, 
                object::pair{std::string("entityId"), std::string("entity1")}
            });
            expect(mockCallback)->toHaveBeenCalledWith(object{
                object::pair{std::string("text"), expect->stringMatching((new RegExp(std::string("Added new aspirational goal.*Read more book"))))}, 
                object::pair{std::string("actions"), array<string>{ std::string("CREATE_TODO_SUCCESS") }}, 
                object::pair{std::string("source"), std::string("test")}
            });
        }
        );
        it(std::string("should detect and reject duplicate todos"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Add task to clean house")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("roomId"), as<any>(std::string("room1"))}, 
                object::pair{std::string("entityId"), as<any>(std::string("entity1"))}
            });
            mockState->data->tasks = array<object>{ object{
                object::pair{std::string("id"), std::string("existing-task")}, 
                object::pair{std::string("name"), std::string("Clean house")}, 
                object::pair{std::string("description"), std::string("Clean the entire house")}, 
                object::pair{std::string("tags"), array<string>{ std::string("TODO"), std::string("one-off") }}
            } };
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std::string("\
      <response>\
        <name>Clean house</name>\
        <taskType>one-off</taskType>\
      </response>\
    "));
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockRuntime->createTask)->not->toHaveBeenCalled();
            expect(mockCallback)->toHaveBeenCalledWith(object{
                object::pair{std::string("text"), expect->stringContaining(std::string("already have an active task named "Clean house""))}, 
                object::pair{std::string("actions"), array<string>{ std::string("CREATE_TODO_DUPLICATE") }}, 
                object::pair{std::string("source"), std::string("test")}
            });
        }
        );
        it(std::string("should handle extraction failure gracefully"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Invalid todo request")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("roomId"), as<any>(std::string("room1"))}, 
                object::pair{std::string("entityId"), as<any>(std::string("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std::string("invalid XML response"));
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalledWith(object{
                object::pair{std::string("text"), expect->stringContaining(std::string("couldn't understand the details"))}, 
                object::pair{std::string("actions"), array<string>{ std::string("CREATE_TODO_FAILED") }}, 
                object::pair{std::string("source"), std::string("test")}
            });
        }
        );
        it(std::string("should handle confirmation messages without creating todo"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("yes")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("roomId"), as<any>(std::string("room1"))}, 
                object::pair{std::string("entityId"), as<any>(std::string("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std::string("\
      <response>\
        <is_confirmation>true</is_confirmation>\
      </response>\
    "));
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockRuntime->createTask)->not->toHaveBeenCalled();
            expect(mockCallback)->toHaveBeenCalledWith(object{
                object::pair{std::string("text"), expect->stringContaining(std::string("couldn't understand the details"))}, 
                object::pair{std::string("actions"), array<string>{ std::string("CREATE_TODO_FAILED") }}, 
                object::pair{std::string("source"), std::string("test")}
            });
        }
        );
        it(std::string("should handle createTask failure"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Add task to test failure")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("roomId"), as<any>(std::string("room1"))}, 
                object::pair{std::string("entityId"), as<any>(std::string("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std::string("\
      <response>\
        <name>Test failure</name>\
        <taskType>one-off</taskType>\
      </response>\
    "));
            mockRuntime->createTask = vi->fn()->mockResolvedValue(nullptr);
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalledWith(object{
                object::pair{std::string("text"), expect->stringContaining(std::string("encountered an error"))}, 
                object::pair{std::string("actions"), array<string>{ std::string("CREATE_TODO_FAILED") }}, 
                object::pair{std::string("source"), std::string("test")}
            });
        }
        );
        it(std::string("should handle urgent one-off tasks"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Add urgent task")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("roomId"), as<any>(std::string("room1"))}, 
                object::pair{std::string("entityId"), as<any>(std::string("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std::string("\
      <response>\
        <name>Urgent task</name>\
        <taskType>one-off</taskType>\
        <priority>1</priority>\
        <urgent>true</urgent>\
      </response>\
    "));
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockRuntime->createTask)->toHaveBeenCalledWith(expect->objectContaining(object{
                object::pair{std::string("tags"), expect->arrayContaining(array<string>{ std::string("TODO"), std::string("one-off"), std::string("priority-1"), std::string("urgent") })}
            }));
        }
        );
        it(std::string("should use default priority when not specified"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Add task without priority")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("roomId"), as<any>(std::string("room1"))}, 
                object::pair{std::string("entityId"), as<any>(std::string("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std::string("\
      <response>\
        <name>Task without priority</name>\
        <taskType>one-off</taskType>\
      </response>\
    "));
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockRuntime->createTask)->toHaveBeenCalledWith(expect->objectContaining(object{
                object::pair{std::string("tags"), expect->arrayContaining(array<string>{ std::string("TODO"), std::string("one-off"), std::string("priority-3") })}
            }));
        }
        );
    }
    );
}

MAIN
