#include "createTodoAction.test.h"
#include <string>

void Main(void)
{
    describe(std::string("createTodoAction"), [=]() mutable
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
                object::pair{std::string("worldId"), as<std::shared_ptr<UUID>>(std::string("test-world"))}, 
                object::pair{std::string("useModel"), [=]() mutable
                {
                    return Promise->resolve(std::string("<response></response>"));
                }
                }, 
                object::pair{std::string("composeState"), [=]() mutable
                {
                    return Promise->resolve(mockState);
                }
                }, 
                object::pair{std::string("db"), nullptr}, 
                object::pair{std::string("getRoom"), [=]() mutable
                {
                    return Promise->resolve(object{
                        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("room-1"))}, 
                        object::pair{std::string("worldId"), as<std::shared_ptr<UUID>>(std::string("test-world"))}, 
                        object::pair{std::string("source"), std::string("test")}, 
                        object::pair{std::string("type"), ChannelType->DM}
                    });
                }
                }
            });
            mockState = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("text"), string_empty}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("messages"), array<any>()}, 
                    object::pair{std::string("entities"), array<any>()}, 
                    object::pair{std::string("room"), object{
                        object::pair{std::string("id"), std::string("room-1")}, 
                        object::pair{std::string("name"), std::string("Test Room")}, 
                        object::pair{std::string("worldId"), std::string("world-1")}
                    }}
                }}
            };
        };
        it(std::string("should have correct action properties"), [=]() mutable
        {
            expect(createTodoAction->name)->toBe(std::string("CREATE_TODO"));
            expect(createTodoAction->description)->toBeDefined();
            expect(createTodoAction->handler)->toBeInstanceOf(Function);
            expect(createTodoAction->validate)->toBeInstanceOf(Function);
            expect(createTodoAction->examples)->toBeDefined();
            expect(Array->isArray(createTodoAction->examples))->toBe(true);
        }
        );
        it(std::string("should have proper similes"), [=]() mutable
        {
            expect(createTodoAction->similes)->toContain(std::string("ADD_TODO"));
            expect(createTodoAction->similes)->toContain(std::string("NEW_TASK"));
            expect(createTodoAction->similes)->toContain(std::string("ADD_TASK"));
            expect(createTodoAction->similes)->toContain(std::string("CREATE_TASK"));
        }
        );
        it(std::string("should validate correctly"), [=]() mutable
        {
            setupMocks();
            auto message = as<any>(object{
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("user-1"))}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-1"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("create a todo")}
                }}
            });
            auto isValid = std::async([=]() { createTodoAction->validate(mockRuntime, message); });
            expect(type_of(isValid))->toBe(std::string("boolean"));
        }
        );
        it(std::string("should handle missing roomId gracefully"), [=]() mutable
        {
            setupMocks();
            auto message = as<any>(object{
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("user-1"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("create a todo")}, 
                    object::pair{std::string("source"), std::string("test")}
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
        it(std::string("should handle missing entityId gracefully"), [=]() mutable
        {
            setupMocks();
            auto message = as<any>(object{
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-1"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("create a todo")}, 
                    object::pair{std::string("source"), std::string("test")}
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
        it(std::string("should handle empty AI response gracefully"), [=]() mutable
        {
            setupMocks();
            auto message = as<any>(object{
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("user-1"))}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-1"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("create a todo")}, 
                    object::pair{std::string("source"), std::string("test")}
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
        it(std::string("should have proper example structure"), [=]() mutable
        {
            expect(createTodoAction->examples)->toBeDefined();
            expect(Array->isArray(createTodoAction->examples))->toBe(true);
            expect(createTodoAction->examples->length)->toBeGreaterThan(0);
            auto firstExample = const_(createTodoAction->examples)[0];
            expect(Array->isArray(firstExample))->toBe(true);
            expect(firstExample->length)->toBeGreaterThan(0);
            auto firstMessage = const_(firstExample)[0];
            expect(firstMessage)->toHaveProperty(std::string("name"));
            expect(firstMessage)->toHaveProperty(std::string("content"));
        }
        );
    }
    );
}

MAIN
