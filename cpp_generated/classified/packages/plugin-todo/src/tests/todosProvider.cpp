#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/src/tests/todosProvider.test.h"

void Main(void)
{
    describe(std::string("todosProvider"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<State>> mockState;
        shared setupMocks = [=]() mutable
        {
            mockRuntime = createMockRuntime(object{
                object::pair{std::string("getRoom"), [=](auto _roomId) mutable
                {
                    return Promise->resolve(object{
                        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("room-1"))}, 
                        object::pair{std::string("source"), std::string("TEST")}, 
                        object::pair{std::string("type"), ChannelType->DM}, 
                        object::pair{std::string("worldId"), as<std::shared_ptr<UUID>>(std::string("world-1"))}
                    });
                }
                }, 
                object::pair{std::string("db"), nullptr}
            });
            mockState = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("text"), string_empty}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("room"), object{
                        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("room-1"))}, 
                        object::pair{std::string("name"), std::string("Test Room")}, 
                        object::pair{std::string("worldId"), as<std::shared_ptr<UUID>>(std::string("world-1"))}
                    }}
                }}
            };
        };
        it(std::string("should have correct provider properties"), [=]() mutable
        {
            expect(todosProvider->name)->toBe(std::string("TODOS"));
            expect(todosProvider->description)->toBeDefined();
            expect(todosProvider->get)->toBeInstanceOf(Function);
        }
        );
        it(std::string("should handle no database gracefully"), [=]() mutable
        {
            setupMocks();
            auto message = as<any>(object{
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("user-1"))}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-1"))}
            });
            auto result = std::async([=]() { todosProvider->get(mockRuntime, message, mockState); });
            expect(result->text)->toBeDefined();
            expect(type_of(result->text))->toBe(std::string("string"));
            expect(result->text->length)->toBeGreaterThan(0);
        }
        );
        it(std::string("should return proper structure"), [=]() mutable
        {
            setupMocks();
            auto message = as<any>(object{
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("user-1"))}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-1"))}
            });
            auto result = std::async([=]() { todosProvider->get(mockRuntime, message, mockState); });
            expect(result)->toHaveProperty(std::string("text"));
            expect(type_of(result->text))->toBe(std::string("string"));
        }
        );
        it(std::string("should handle missing entityId gracefully"), [=]() mutable
        {
            setupMocks();
            auto message = as<any>(object{
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("room-1"))}
            });
            auto result = std::async([=]() { todosProvider->get(mockRuntime, message, mockState); });
            expect(result->text)->toBeDefined();
            expect(type_of(result->text))->toBe(std::string("string"));
        }
        );
    }
    );
}

MAIN
