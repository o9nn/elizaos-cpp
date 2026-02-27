#include "createTodoAction.test.h"

void Main(void)
{
    describe(std:("createTodoAction"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<HandlerCallback>> mockCallback;
        shared<std::shared_ptr<State>> mockState;
        shared setupMocks = [=]() mutable
        {
            mockCallback = [=]() mutable
            {
                return array<any>();
            };
            mockRuntime = createMockRuntime(object{
                object::pair{std:("worldId"), as<std::shared_ptr<UUID>>(std:("test-world"))}, 
                object::pair{std:("useModel"), [=]() mutable
                {
                    return Promise->resolve(std:("<response></response>"));
                }
                }, 
                object::pair{std:("composeState"), [=]() mutable
                {
                    return Promise->resolve(mockState);
                }
                }, 
                object::pair{std:("db"), nullptr}, 
                object::pair{std:("getRoom"), [=]() mutable
                {
                    return Promise->resolve(object{
                        object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("room-1"))}, 
                        object::pair{std:("worldId"), as<std::shared_ptr<UUID>>(std:("test-world"))}, 
                        object::pair{std:("source"), std:("test")}, 
                        object::pair{std:("type"), ChannelType->DM}
                    });
                }
                }
            });
            mockState = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("text"), string_empty}, 
                object::pair{std:("data"), object{
                    object::pair{std:("messages"), array<any>()}, 
                    object::pair{std:("entities"), array<any>()}, 
                    object::pair{std:("room"), object{
                        object::pair{std:("id"), std:("room-1")}, 
                        object::pair{std:("name"), std:("Test Room")}, 
                        object::pair{std:("worldId"), std:("world-1")}
                    }}
                }}
            };
        };
        it(std:("should have correct action properties"), [=]() mutable
        {
            expect(createTodoAction->name)->toBe(std:("CREATE_TODO"));
            expect(createTodoAction->description)->toBeDefined();
            expect(createTodoAction->handler)->toBeInstanceOf(Function);
            expect(createTodoAction->validate)->toBeInstanceOf(Function);
            expect(createTodoAction->examples)->toBeDefined();
            expect(Array->isArray(createTodoAction->examples))->toBe(true);
        }
        );
        it(std:("should have proper similes"), [=]() mutable
        {
            expect(createTodoAction->similes)->toContain(std:("ADD_TODO"));
            expect(createTodoAction->similes)->toContain(std:("NEW_TASK"));
            expect(createTodoAction->similes)->toContain(std:("ADD_TASK"));
            expect(createTodoAction->similes)->toContain(std:("CREATE_TASK"));
        }
        );
        it(std:("should validate correctly"), [=]() mutable
        {
            setupMocks();
            auto message = as<any>(object{
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("user-1"))}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-1"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("create a todo")}
                }}
            });
            auto isValid = std::async([=]() { createTodoAction->validate(mockRuntime, message); });
            expect(type_of(isValid))->toBe(std:("boolean"));
        }
        );
        it(std:("should handle missing roomId gracefully"), [=]() mutable
        {
            setupMocks();
            auto message = as<any>(object{
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("user-1"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("create a todo")}, 
                    object::pair{std:("source"), std:("test")}
                }}
            });
            shared callbackCalled = false;
            auto testCallback = [=]() mutable
            {
                callbackCalled = true;
                return array<any>();
            };
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, testCallback); });
            expect(callbackCalled)->toBe(true);
        }
        );
        it(std:("should handle missing entityId gracefully"), [=]() mutable
        {
            setupMocks();
            auto message = as<any>(object{
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-1"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("create a todo")}, 
                    object::pair{std:("source"), std:("test")}
                }}
            });
            shared callbackCalled = false;
            auto testCallback = [=]() mutable
            {
                callbackCalled = true;
                return array<any>();
            };
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, testCallback); });
            expect(callbackCalled)->toBe(true);
        }
        );
        it(std:("should handle empty AI response gracefully"), [=]() mutable
        {
            setupMocks();
            auto message = as<any>(object{
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("user-1"))}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-1"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("create a todo")}, 
                    object::pair{std:("source"), std:("test")}
                }}
            });
            shared callbackCalled = false;
            auto testCallback = [=]() mutable
            {
                callbackCalled = true;
                return array<any>();
            };
            std::async([=]() { createTodoAction->handler(mockRuntime, message, mockState, object{}, testCallback); });
            expect(callbackCalled)->toBe(true);
        }
        );
        it(std:("should have proper example structure"), [=]() mutable
        {
            expect(createTodoAction->examples)->toBeDefined();
            expect(Array->isArray(createTodoAction->examples))->toBe(true);
            expect(createTodoAction->examples->length)->toBeGreaterThan(0);
            auto firstExample = const_(createTodoAction->examples)[0];
            expect(Array->isArray(firstExample))->toBe(true);
            expect(firstExample->length)->toBeGreaterThan(0);
            auto firstMessage = const_(firstExample)[0];
            expect(firstMessage)->toHaveProperty(std:("name"));
            expect(firstMessage)->toHaveProperty(std:("content"));
        }
        );
    }
    );
}

MAIN
