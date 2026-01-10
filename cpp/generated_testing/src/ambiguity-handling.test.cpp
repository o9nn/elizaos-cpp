#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-bootstrap/src/__tests__/ambiguity-handling.test.h"

Content handleAmbiguity(Content responseContent)
{
    if (AND((responseContent["actions"]), (responseContent["actions"]->get_length() > 1))) {
        shared isIgnoreAction = [=](auto action) mutable
        {
            return AND((type_of(action) == std::string("string")), (action->toUpperCase() == std::string("IGNORE")));
        };
        auto hasIgnoreAction = responseContent["actions"]->some(isIgnoreAction);
        if (hasIgnoreAction) {
            if (OR((!responseContent["text"]), (responseContent["text"]->trim() == string_empty))) {
                responseContent["actions"] = array<string>{ std::string("IGNORE") };
            } else {
                auto filteredActions = responseContent["actions"]->filter([=](auto action) mutable
                {
                    return !isIgnoreAction(action);
                }
                );
                if (filteredActions->get_length() == 0) {
                    responseContent["actions"] = array<string>{ std::string("REPLY") };
                } else {
                    responseContent["actions"] = filteredActions;
                }
            }
        }
    }
    auto isSimple = AND((AND((AND((responseContent["actions"]->get_length() == 1), (type_of(const_(responseContent["actions"])[0]) == std::string("string")))), (const_(responseContent["actions"])[0]->toUpperCase() == std::string("REPLY")))), ((OR((!responseContent["providers"]), (responseContent["providers"]->get_length() == 0)))));
    responseContent["simple"] = isSimple;
    return responseContent;
};



void Main(void)
{
    describe(std::string("LLM Ambiguity Handling"), [=]() mutable
    {
        describe(std::string("Bug 1: Empty Actions Array Fix"), [=]() mutable
        {
            it(std::string("should handle multiple IGNORE actions with text present"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std::string("text"), std::string("Hello world")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("IGNORE"), std::string("IGNORE") }}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<string>{ std::string("REPLY") });
                expect(result["simple"])->toBe(true);
            }
            );
            it(std::string("should handle mixed actions with IGNORE and text present"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std::string("text"), std::string("Hello world")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("REPLY"), std::string("IGNORE"), std::string("SEND_MESSAGE") }}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<string>{ std::string("REPLY"), std::string("SEND_MESSAGE") });
                expect(result["simple"])->toBe(false);
            }
            );
            it(std::string("should handle IGNORE with no text"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std::string("text"), string_empty}, 
                    object::pair{std::string("actions"), array<string>{ std::string("REPLY"), std::string("IGNORE"), std::string("SEND_MESSAGE") }}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<string>{ std::string("IGNORE") });
                expect(result["simple"])->toBe(false);
            }
            );
        }
        );
        describe(std::string("Bug 2: Runtime Error Fix"), [=]() mutable
        {
            it(std::string("should handle non-string actions gracefully"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std::string("text"), std::string("Hello world")}, 
                    object::pair{std::string("actions"), array<std::nullptr_t>{ nullptr, undefined, 123, std::string("IGNORE"), std::string("REPLY") }}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<std::nullptr_t>{ nullptr, undefined, 123, std::string("REPLY") });
                expect(result["simple"])->toBe(false);
            }
            );
            it(std::string("should handle mixed string/non-string actions with IGNORE"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std::string("text"), std::string("Hello world")}, 
                    object::pair{std::string("actions"), array<std::nullptr_t>{ nullptr, std::string("IGNORE"), undefined, std::string("REPLY") }}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<std::nullptr_t>{ nullptr, undefined, std::string("REPLY") });
                expect(result["simple"])->toBe(false);
            }
            );
            it(std::string("should handle case-insensitive IGNORE detection"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std::string("text"), std::string("Hello world")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("ignore"), std::string("Ignore"), std::string("IGNORE"), std::string("REPLY") }}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<string>{ std::string("REPLY") });
                expect(result["simple"])->toBe(true);
            }
            );
        }
        );
        describe(std::string("Edge Cases"), [=]() mutable
        {
            it(std::string("should handle single action (no ambiguity)"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std::string("text"), std::string("Hello world")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("REPLY") }}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<string>{ std::string("REPLY") });
                expect(result["simple"])->toBe(true);
            }
            );
            it(std::string("should handle no actions"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std::string("text"), std::string("Hello world")}, 
                    object::pair{std::string("actions"), array<any>()}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<any>());
                expect(result["simple"])->toBe(false);
            }
            );
            it(std::string("should handle undefined actions"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std::string("text"), std::string("Hello world")}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toBeUndefined();
                expect(result["simple"])->toBe(false);
            }
            );
            it(std::string("should handle all non-string actions"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std::string("text"), std::string("Hello world")}, 
                    object::pair{std::string("actions"), array<std::nullptr_t>{ nullptr, undefined, 123, object{} }}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<std::nullptr_t>{ nullptr, undefined, 123, object{} });
                expect(result["simple"])->toBe(false);
            }
            );
        }
        );
    }
    );
}

MAIN
