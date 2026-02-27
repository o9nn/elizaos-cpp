#include "createTodoAction.test.h"

void Main(void)
{
    describe(std:("createTodoAction"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<HandlerCallback>> mockCallback;
        shared<std::shared_ptr<State>> mockState;
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            mockRuntime = as<any>(object{
                object::pair{std:("agentId"), as<any>(std:("test-agent"))}, 
                object::pair{std:("getTasks"), vi->fn()->mockResolvedValue(array<any>())}, 
                object::pair{std:("createTask"), vi->fn()->mockResolvedValue(std:("new-task-id"))}, 
                object::pair{std:("getRoom"), vi->fn()->mockResolvedValue(object{
                    object::pair{std:("worldId"), std:("test-world")}
                })}, 
                object::pair{std:("ensureConnection"), vi->fn()}, 
                object::pair{std:("composeState"), vi->fn()->mockImplementation([=]() mutable
                {
                    return mockState;
                }
                )}, 
                object::pair{std:("useModel"), vi->fn()}
            });
            mockCallback = vi->fn();
            mockState = as<any>(object{
                object::pair{std:("data"), object{
                    object::pair{std:("tasks"), array<any>()}, 
                    object::pair{std:("room"), object{
                        object::pair{std:("worldId"), std:("test-world")}
                    }}, 
                    object::pair{std:("messages"), array<any>()}, 
                    object::pair{std:("entities"), array<any>()}
                }}
            });
        }
        );
        it(std:("should have correct action properties"), [=]() mutable
        {
            expect(createTodoAction->name)->toBe(std:("CREATE_TODO"));
            expect(createTodoAction->similes)->toContain(std:("ADD_TODO"));
            expect(createTodoAction->similes)->toContain(std:("NEW_TASK"));
            expect(createTodoAction->description)->toContain(std:("Creates a new todo item"));
            expect(createTodoAction->validate)->toBeInstanceOf(Function);
            expect(createTodoAction->handler)->toBeInstanceOf(Function);
            expect(createTodoAction->examples)->toHaveLength(3);
        }
        );
        it(std:("should validate always return true"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Add todo")}
                }}
            });
            auto result = std::async([=]() { createTodoAction->validate(mockRuntime, message); });
            expect(result)->toBe(true);
        }
        );
        it(std:("should create a daily todo successfully"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Add daily task to do 50 pushups")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("roomId"), as<any>(std:("room1"))}, 
                object::pair{std:("entityId"), as<any>(std:("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std:("\
      <response>\
        <name>Do 50 pushups</name>\
        <description>Daily exercise routine</description>\
        <taskType>daily</taskType>\
        <recurring>daily</recurring>\
      </response>\
    "));
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockRuntime->createTask)->toHaveBeenCalledWith(object{
                object::pair{std:("name"), std:("Do 50 pushups")}, 
                object::pair{std:("description"), std:("Daily exercise routine")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("daily"), std:("recurring-daily") }}, 
                object::pair{std:("metadata"), expect->objectContaining(object{
                    object::pair{std:("createdAt"), expect->any(String)}, 
                    object::pair{std:("description"), std:("Daily exercise routine")}, 
                    object::pair{std:("streak"), 0}
                })}, 
                object::pair{std:("roomId"), std:("room1")}, 
                object::pair{std:("worldId"), std:("test-world")}, 
                object::pair{std:("entityId"), std:("entity1")}
            });
            expect(mockCallback)->toHaveBeenCalledWith(object{
                object::pair{std:("text"), expect->stringContaining(std:("Added new daily task: "Do 50 pushups""))}, 
                object::pair{std:("actions"), array<string>{ std:("CREATE_TODO_SUCCESS") }}, 
                object::pair{std:("source"), std:("test")}
            });
        }
        );
        it(std:("should create a one-off todo with due date"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Add todo to finish taxes by April 15")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("roomId"), as<any>(std:("room1"))}, 
                object::pair{std:("entityId"), as<any>(std:("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std:("\
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
                object::pair{std:("name"), std:("Finish taxes")}, 
                object::pair{std:("description"), std:("Complete tax filing")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("one-off"), std:("priority-2") }}, 
                object::pair{std:("metadata"), expect->objectContaining(object{
                    object::pair{std:("createdAt"), expect->any(String)}, 
                    object::pair{std:("description"), std:("Complete tax filing")}, 
                    object::pair{std:("dueDate"), std:("2024-04-15")}
                })}, 
                object::pair{std:("roomId"), std:("room1")}, 
                object::pair{std:("worldId"), std:("test-world")}, 
                object::pair{std:("entityId"), std:("entity1")}
            });
            expect(mockCallback)->toHaveBeenCalledWith(object{
                object::pair{std:("text"), expect->stringMatching((new RegExp(std:("Added new one-off task.*Finish taxes.*Priority 2.*Due"))))}, 
                object::pair{std:("actions"), array<string>{ std:("CREATE_TODO_SUCCESS") }}, 
                object::pair{std:("source"), std:("test")}
            });
        }
        );
        it(std:("should create an aspirational todo"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Add goal to read more books")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("roomId"), as<any>(std:("room1"))}, 
                object::pair{std:("entityId"), as<any>(std:("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std:("\
      <response>\
        <name>Read more books</name>\
        <taskType>aspirational</taskType>\
      </response>\
    "));
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockRuntime->createTask)->toHaveBeenCalledWith(object{
                object::pair{std:("name"), std:("Read more books")}, 
                object::pair{std:("description"), std:("Read more books")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("aspirational") }}, 
                object::pair{std:("metadata"), expect->objectContaining(object{
                    object::pair{std:("createdAt"), expect->any(String)}
                })}, 
                object::pair{std:("roomId"), std:("room1")}, 
                object::pair{std:("worldId"), std:("test-world")}, 
                object::pair{std:("entityId"), std:("entity1")}
            });
            expect(mockCallback)->toHaveBeenCalledWith(object{
                object::pair{std:("text"), expect->stringMatching((new RegExp(std:("Added new aspirational goal.*Read more book"))))}, 
                object::pair{std:("actions"), array<string>{ std:("CREATE_TODO_SUCCESS") }}, 
                object::pair{std:("source"), std:("test")}
            });
        }
        );
        it(std:("should detect and reject duplicate todos"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Add task to clean house")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("roomId"), as<any>(std:("room1"))}, 
                object::pair{std:("entityId"), as<any>(std:("entity1"))}
            });
            mockState->data->tasks = array<object>{ object{
                object::pair{std:("id"), std:("existing-task")}, 
                object::pair{std:("name"), std:("Clean house")}, 
                object::pair{std:("description"), std:("Clean the entire house")}, 
                object::pair{std:("tags"), array<string>{ std:("TODO"), std:("one-off") }}
            } };
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std:("\
      <response>\
        <name>Clean house</name>\
        <taskType>one-off</taskType>\
      </response>\
    "));
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockRuntime->createTask)->not->toHaveBeenCalled();
            expect(mockCallback)->toHaveBeenCalledWith(object{
                object::pair{std:("text"), expect->stringContaining(std:("already have an active task named "Clean house""))}, 
                object::pair{std:("actions"), array<string>{ std:("CREATE_TODO_DUPLICATE") }}, 
                object::pair{std:("source"), std:("test")}
            });
        }
        );
        it(std:("should handle extraction failure gracefully"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Invalid todo request")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("roomId"), as<any>(std:("room1"))}, 
                object::pair{std:("entityId"), as<any>(std:("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std:("invalid XML response"));
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalledWith(object{
                object::pair{std:("text"), expect->stringContaining(std:("couldn't understand the details"))}, 
                object::pair{std:("actions"), array<string>{ std:("CREATE_TODO_FAILED") }}, 
                object::pair{std:("source"), std:("test")}
            });
        }
        );
        it(std:("should handle confirmation messages without creating todo"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("yes")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("roomId"), as<any>(std:("room1"))}, 
                object::pair{std:("entityId"), as<any>(std:("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std:("\
      <response>\
        <is_confirmation>true</is_confirmation>\
      </response>\
    "));
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockRuntime->createTask)->not->toHaveBeenCalled();
            expect(mockCallback)->toHaveBeenCalledWith(object{
                object::pair{std:("text"), expect->stringContaining(std:("couldn't understand the details"))}, 
                object::pair{std:("actions"), array<string>{ std:("CREATE_TODO_FAILED") }}, 
                object::pair{std:("source"), std:("test")}
            });
        }
        );
        it(std:("should handle createTask failure"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Add task to test failure")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("roomId"), as<any>(std:("room1"))}, 
                object::pair{std:("entityId"), as<any>(std:("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std:("\
      <response>\
        <name>Test failure</name>\
        <taskType>one-off</taskType>\
      </response>\
    "));
            mockRuntime->createTask = vi->fn()->mockResolvedValue(nullptr);
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalledWith(object{
                object::pair{std:("text"), expect->stringContaining(std:("encountered an error"))}, 
                object::pair{std:("actions"), array<string>{ std:("CREATE_TODO_FAILED") }}, 
                object::pair{std:("source"), std:("test")}
            });
        }
        );
        it(std:("should handle urgent one-off tasks"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Add urgent task")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("roomId"), as<any>(std:("room1"))}, 
                object::pair{std:("entityId"), as<any>(std:("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std:("\
      <response>\
        <name>Urgent task</name>\
        <taskType>one-off</taskType>\
        <priority>1</priority>\
        <urgent>true</urgent>\
      </response>\
    "));
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockRuntime->createTask)->toHaveBeenCalledWith(expect->objectContaining(object{
                object::pair{std:("tags"), expect->arrayContaining(array<string>{ std:("TODO"), std:("one-off"), std:("priority-1"), std:("urgent") })}
            }));
        }
        );
        it(std:("should use default priority when not specified"), [=]() mutable
        {
            auto message = as<any>(object{
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Add task without priority")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("roomId"), as<any>(std:("room1"))}, 
                object::pair{std:("entityId"), as<any>(std:("entity1"))}
            });
            mockRuntime->useModel = vi->fn()->mockResolvedValue(std:("\
      <response>\
        <name>Task without priority</name>\
        <taskType>one-off</taskType>\
      </response>\
    "));
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, mockCallback); });
            expect(mockRuntime->createTask)->toHaveBeenCalledWith(expect->objectContaining(object{
                object::pair{std:("tags"), expect->arrayContaining(array<string>{ std:("TODO"), std:("one-off"), std:("priority-3") })}
            }));
        }
        );
    }
    );
}

MAIN
