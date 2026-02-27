#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/src/tests/todosProvider.test.h"

void Main(void)
{
    describe(std:("todosProvider"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<State>> mockState;
        shared setupMocks = [=]() mutable
        {
            mockRuntime = createMockRuntime(object{
                object::pair{std:("getRoom"), [=](auto _roomId) mutable
                {
                    return Promise->resolve(object{
                        object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("room-1"))}, 
                        object::pair{std:("source"), std:("TEST")}, 
                        object::pair{std:("type"), ChannelType->DM}, 
                        object::pair{std:("worldId"), as<std::shared_ptr<UUID>>(std:("world-1"))}
                    });
                }
                }, 
                object::pair{std:("db"), nullptr}
            });
            mockState = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("text"), string_empty}, 
                object::pair{std:("data"), object{
                    object::pair{std:("room"), object{
                        object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("room-1"))}, 
                        object::pair{std:("name"), std:("Test Room")}, 
                        object::pair{std:("worldId"), as<std::shared_ptr<UUID>>(std:("world-1"))}
                    }}
                }}
            };
        };
        it(std:("should have correct provider properties"), [=]() mutable
        {
            expect(todosProvider->name)->toBe(std:("TODOS"));
            expect(todosProvider->description)->toBeDefined();
            expect(todosProvider->get)->toBeInstanceOf(Function);
        }
        );
        it(std:("should handle no database gracefully"), [=]() mutable
        {
            setupMocks();
            auto message = as<any>(object{
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("user-1"))}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-1"))}
            });
            auto result = std::async([=]() { todosProvider->get(mockRuntime, message, mockState); });
            expect(result->text)->toBeDefined();
            expect(type_of(result->text))->toBe(std:("string"));
            expect(result->text->length)->toBeGreaterThan(0);
        }
        );
        it(std:("should return proper structure"), [=]() mutable
        {
            setupMocks();
            auto message = as<any>(object{
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("user-1"))}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-1"))}
            });
            auto result = std::async([=]() { todosProvider->get(mockRuntime, message, mockState); });
            expect(result)->toHaveProperty(std:("text"));
            expect(type_of(result->text))->toBe(std:("string"));
        }
        );
        it(std:("should handle missing entityId gracefully"), [=]() mutable
        {
            setupMocks();
            auto message = as<any>(object{
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("room-1"))}
            });
            auto result = std::async([=]() { todosProvider->get(mockRuntime, message, mockState); });
            expect(result->text)->toBeDefined();
            expect(type_of(result->text))->toBe(std:("string"));
        }
        );
    }
    );
}

MAIN
