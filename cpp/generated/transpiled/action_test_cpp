#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v1/src/__tests__/action.test.h"

std::function<any(double)> createTestUUID = [=](auto num) mutable
{
    return std::string("00000000-0000-0000-0000-") + num->toString()->padStart(12, std::string("0")) + string_empty;
};

void Main(void)
{
    describe(std::string("Action adapter"), [=]() mutable
    {
        shared mockHandler = [=]() mutable
        {
            return (object{
                object::pair{std::string("success"), true}
            });
        };
        shared mockValidator = [=]() mutable
        {
            return true;
        };
        it(std::string("should convert from v2 action to v1 action correctly"), [=]() mutable
        {
            auto actionV2 = object{
                object::pair{std::string("name"), std::string("test-action")}, 
                object::pair{std::string("description"), std::string("A test action")}, 
                object::pair{std::string("similes"), array<string>{ std::string("similar-action"), std::string("another-action") }}, 
                object::pair{std::string("examples"), array<array<object>>{ array<object>{ object{
                    object::pair{std::string("name"), std::string("testuser")}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("test command")}
                    }}
                } } }}, 
                object::pair{std::string("handler"), mockHandler}, 
                object::pair{std::string("validate"), mockValidator}
            };
            auto actionV1 = fromV2Action(as<any>(actionV2));
            expect(actionV1->name)->toBe(std::string("test-action"));
            expect(actionV1->description)->toBe(std::string("A test action"));
            expect(actionV1->similes)->toEqual(array<string>{ std::string("similar-action"), std::string("another-action") });
            expect(actionV1->examples)->toHaveLength(1);
            expect(const_(const_(actionV1->examples)[0])[0]->user)->toBe(std::string("testuser"));
            expect(const_(const_(actionV1->examples)[0])[0]->content->text)->toBe(std::string("test command"));
            expect(actionV1->suppressInitialMessage)->toBe(false);
            expect(type_of(actionV1->handler))->toBe(std::string("function"));
            expect(type_of(actionV1->validate))->toBe(std::string("function"));
        }
        );
        it(std::string("should convert from v1 action to v2 action correctly"), [=]() mutable
        {
            auto actionV1 = object{
                object::pair{std::string("name"), std::string("test-action")}, 
                object::pair{std::string("description"), std::string("A test action")}, 
                object::pair{std::string("similes"), array<string>{ std::string("similar-action") }}, 
                object::pair{std::string("examples"), array<array<object>>{ array<object>{ object{
                    object::pair{std::string("user"), std::string("testuser")}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("test command")}, 
                        object::pair{std::string("action"), std::string("test")}
                    }}
                } } }}, 
                object::pair{std::string("handler"), mockHandler}, 
                object::pair{std::string("validate"), mockValidator}, 
                object::pair{std::string("suppressInitialMessage"), true}
            };
            auto actionV2 = toV2Action(actionV1);
            expect(actionV2->name)->toBe(std::string("test-action"));
            expect(actionV2->description)->toBe(std::string("A test action"));
            expect(actionV2->similes)->toEqual(array<string>{ std::string("similar-action") });
            expect(const_(const_(actionV2->examples)[0])[0]->name)->toBe(std::string("testuser"));
            expect(const_(const_(actionV2->examples)[0])[0]->content->text)->toBe(std::string("test command"));
            expect(const_(const_(actionV2->examples)[0])[0]->content->actions)->toEqual(array<string>{ std::string("test") });
            expect(type_of(actionV2->handler))->toBe(std::string("function"));
            expect(type_of(actionV2->validate))->toBe(std::string("function"));
        }
        );
        it(std::string("should handle empty similes correctly"), [=]() mutable
        {
            auto actionV2WithoutSimiles = object{
                object::pair{std::string("name"), std::string("test-action")}, 
                object::pair{std::string("description"), std::string("A test action")}, 
                object::pair{std::string("handler"), mockHandler}, 
                object::pair{std::string("validate"), mockValidator}
            };
            auto actionV1 = fromV2Action(as<any>(actionV2WithoutSimiles));
            expect(actionV1->similes)->toEqual(array<any>());
        }
        );
        it(std::string("should handle round-trip conversion"), [=]() mutable
        {
            auto originalAction = object{
                object::pair{std::string("name"), std::string("round-trip-action")}, 
                object::pair{std::string("description"), std::string("Tests round-trip conversion")}, 
                object::pair{std::string("similes"), array<string>{ std::string("test1"), std::string("test2") }}, 
                object::pair{std::string("examples"), array<array<object>>{ array<object>{ object{
                    object::pair{std::string("user"), std::string("alice")}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("hello world")}, 
                        object::pair{std::string("action"), std::string("greet")}
                    }}
                } } }}, 
                object::pair{std::string("handler"), mockHandler}, 
                object::pair{std::string("validate"), mockValidator}, 
                object::pair{std::string("suppressInitialMessage"), false}
            };
            auto actionV2 = toV2Action(originalAction);
            auto convertedBack = fromV2Action(as<any>(actionV2));
            expect(convertedBack->name)->toBe(originalAction->name);
            expect(convertedBack->description)->toBe(originalAction->description);
            expect(convertedBack->similes)->toEqual(originalAction->similes);
            expect(const_(const_(convertedBack->examples)[0])[0]->user)->toBe(std::string("alice"));
            expect(const_(const_(convertedBack->examples)[0])[0]->content->text)->toBe(std::string("hello world"));
        }
        );
    }
    );
}

MAIN
