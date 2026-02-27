#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v1/src/__tests__/action.test.h"

std::function<any(double)> createTestUUID = [=](auto num) mutable
{
    return std:("00000000-0000-0000-0000-") + num->toString()->padStart(12, std:("0")) + string_empty;
};

void Main(void)
{
    describe(std:("Action adapter"), [=]() mutable
    {
        shared mockHandler = [=]() mutable
        {
            return (object{
                object::pair{std:("success"), true}
            });
        };
        shared mockValidator = [=]() mutable
        {
            return true;
        };
        it(std:("should convert from v2 action to v1 action correctly"), [=]() mutable
        {
            auto actionV2 = object{
                object::pair{std:("name"), std:("test-action")}, 
                object::pair{std:("description"), std:("A test action")}, 
                object::pair{std:("similes"), array<string>{ std:("similar-action"), std:("another-action") }}, 
                object::pair{std:("examples"), array<array<object>>{ array<object>{ object{
                    object::pair{std:("name"), std:("testuser")}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("test command")}
                    }}
                } } }}, 
                object::pair{std:("handler"), mockHandler}, 
                object::pair{std:("validate"), mockValidator}
            };
            auto actionV1 = fromV2Action(as<any>(actionV2));
            expect(actionV1->name)->toBe(std:("test-action"));
            expect(actionV1->description)->toBe(std:("A test action"));
            expect(actionV1->similes)->toEqual(array<string>{ std:("similar-action"), std:("another-action") });
            expect(actionV1->examples)->toHaveLength(1);
            expect(const_(const_(actionV1->examples)[0])[0]->user)->toBe(std:("testuser"));
            expect(const_(const_(actionV1->examples)[0])[0]->content->text)->toBe(std:("test command"));
            expect(actionV1->suppressInitialMessage)->toBe(false);
            expect(type_of(actionV1->handler))->toBe(std:("function"));
            expect(type_of(actionV1->validate))->toBe(std:("function"));
        }
        );
        it(std:("should convert from v1 action to v2 action correctly"), [=]() mutable
        {
            auto actionV1 = object{
                object::pair{std:("name"), std:("test-action")}, 
                object::pair{std:("description"), std:("A test action")}, 
                object::pair{std:("similes"), array<string>{ std:("similar-action") }}, 
                object::pair{std:("examples"), array<array<object>>{ array<object>{ object{
                    object::pair{std:("user"), std:("testuser")}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("test command")}, 
                        object::pair{std:("action"), std:("test")}
                    }}
                } } }}, 
                object::pair{std:("handler"), mockHandler}, 
                object::pair{std:("validate"), mockValidator}, 
                object::pair{std:("suppressInitialMessage"), true}
            };
            auto actionV2 = toV2Action(actionV1);
            expect(actionV2->name)->toBe(std:("test-action"));
            expect(actionV2->description)->toBe(std:("A test action"));
            expect(actionV2->similes)->toEqual(array<string>{ std:("similar-action") });
            expect(const_(const_(actionV2->examples)[0])[0]->name)->toBe(std:("testuser"));
            expect(const_(const_(actionV2->examples)[0])[0]->content->text)->toBe(std:("test command"));
            expect(const_(const_(actionV2->examples)[0])[0]->content->actions)->toEqual(array<string>{ std:("test") });
            expect(type_of(actionV2->handler))->toBe(std:("function"));
            expect(type_of(actionV2->validate))->toBe(std:("function"));
        }
        );
        it(std:("should handle empty similes correctly"), [=]() mutable
        {
            auto actionV2WithoutSimiles = object{
                object::pair{std:("name"), std:("test-action")}, 
                object::pair{std:("description"), std:("A test action")}, 
                object::pair{std:("handler"), mockHandler}, 
                object::pair{std:("validate"), mockValidator}
            };
            auto actionV1 = fromV2Action(as<any>(actionV2WithoutSimiles));
            expect(actionV1->similes)->toEqual(array<any>());
        }
        );
        it(std:("should handle round-trip conversion"), [=]() mutable
        {
            auto originalAction = object{
                object::pair{std:("name"), std:("round-trip-action")}, 
                object::pair{std:("description"), std:("Tests round-trip conversion")}, 
                object::pair{std:("similes"), array<string>{ std:("test1"), std:("test2") }}, 
                object::pair{std:("examples"), array<array<object>>{ array<object>{ object{
                    object::pair{std:("user"), std:("alice")}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("hello world")}, 
                        object::pair{std:("action"), std:("greet")}
                    }}
                } } }}, 
                object::pair{std:("handler"), mockHandler}, 
                object::pair{std:("validate"), mockValidator}, 
                object::pair{std:("suppressInitialMessage"), false}
            };
            auto actionV2 = toV2Action(originalAction);
            auto convertedBack = fromV2Action(as<any>(actionV2));
            expect(convertedBack->name)->toBe(originalAction->name);
            expect(convertedBack->description)->toBe(originalAction->description);
            expect(convertedBack->similes)->toEqual(originalAction->similes);
            expect(const_(const_(convertedBack->examples)[0])[0]->user)->toBe(std:("alice"));
            expect(const_(const_(convertedBack->examples)[0])[0]->content->text)->toBe(std:("hello world"));
        }
        );
    }
    );
}

MAIN
