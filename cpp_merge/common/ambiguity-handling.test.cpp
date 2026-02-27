#include "ambiguity-handling.test.h"

Content handleAmbiguity(Content responseContent)
{
    if (AND((responseContent["actions"]), (responseContent["actions"]->get_length() > 1))) {
        shared isIgnoreAction = [=](auto action) mutable
        {
            return AND((type_of(action) == std:("string")), (action->toUpperCase() == std:("IGNORE")));
        };
        auto hasIgnoreAction = responseContent["actions"]->some(isIgnoreAction);
        if (hasIgnoreAction) {
            if (OR((!responseContent["text"]), (responseContent["text"]->trim() == string_empty))) {
                responseContent["actions"] = array<string>{ std:("IGNORE") };
            } else {
                auto filteredActions = responseContent["actions"]->filter([=](auto action) mutable
                {
                    return !isIgnoreAction(action);
                }
                );
                if (filteredActions->get_length() == 0) {
                    responseContent["actions"] = array<string>{ std:("REPLY") };
                } else {
                    responseContent["actions"] = filteredActions;
                }
            }
        }
    }
    auto isSimple = AND((AND((AND((responseContent["actions"]->get_length() == 1), (type_of(const_(responseContent["actions"])[0]) == std:("string")))), (const_(responseContent["actions"])[0]->toUpperCase() == std:("REPLY")))), ((OR((!responseContent["providers"]), (responseContent["providers"]->get_length() == 0)))));
    responseContent["simple"] = isSimple;
    return responseContent;
};



void Main(void)
{
    describe(std:("LLM Ambiguity Handling"), [=]() mutable
    {
        describe(std:("Bug 1: Empty Actions Array Fix"), [=]() mutable
        {
            it(std:("should handle multiple IGNORE actions with text present"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std:("text"), std:("Hello world")}, 
                    object::pair{std:("actions"), array<string>{ std:("IGNORE"), std:("IGNORE") }}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<string>{ std:("REPLY") });
                expect(result["simple"])->toBe(true);
            }
            );
            it(std:("should handle mixed actions with IGNORE and text present"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std:("text"), std:("Hello world")}, 
                    object::pair{std:("actions"), array<string>{ std:("REPLY"), std:("IGNORE"), std:("SEND_MESSAGE") }}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<string>{ std:("REPLY"), std:("SEND_MESSAGE") });
                expect(result["simple"])->toBe(false);
            }
            );
            it(std:("should handle IGNORE with no text"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std:("text"), string_empty}, 
                    object::pair{std:("actions"), array<string>{ std:("REPLY"), std:("IGNORE"), std:("SEND_MESSAGE") }}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<string>{ std:("IGNORE") });
                expect(result["simple"])->toBe(false);
            }
            );
        }
        );
        describe(std:("Bug 2: Runtime Error Fix"), [=]() mutable
        {
            it(std:("should handle non-string actions gracefully"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std:("text"), std:("Hello world")}, 
                    object::pair{std:("actions"), array<std::nullptr_t>{ nullptr, undefined, 123, std:("IGNORE"), std:("REPLY") }}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<std::nullptr_t>{ nullptr, undefined, 123, std:("REPLY") });
                expect(result["simple"])->toBe(false);
            }
            );
            it(std:("should handle mixed string/non-string actions with IGNORE"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std:("text"), std:("Hello world")}, 
                    object::pair{std:("actions"), array<std::nullptr_t>{ nullptr, std:("IGNORE"), undefined, std:("REPLY") }}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<std::nullptr_t>{ nullptr, undefined, std:("REPLY") });
                expect(result["simple"])->toBe(false);
            }
            );
            it(std:("should handle case-insensitive IGNORE detection"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std:("text"), std:("Hello world")}, 
                    object::pair{std:("actions"), array<string>{ std:("ignore"), std:("Ignore"), std:("IGNORE"), std:("REPLY") }}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<string>{ std:("REPLY") });
                expect(result["simple"])->toBe(true);
            }
            );
        }
        );
        describe(std:("Edge Cases"), [=]() mutable
        {
            it(std:("should handle single action (no ambiguity)"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std:("text"), std:("Hello world")}, 
                    object::pair{std:("actions"), array<string>{ std:("REPLY") }}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<string>{ std:("REPLY") });
                expect(result["simple"])->toBe(true);
            }
            );
            it(std:("should handle no actions"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std:("text"), std:("Hello world")}, 
                    object::pair{std:("actions"), array<any>()}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toEqual(array<any>());
                expect(result["simple"])->toBe(false);
            }
            );
            it(std:("should handle undefined actions"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std:("text"), std:("Hello world")}
                };
                auto result = handleAmbiguity(responseContent);
                expect(result["actions"])->toBeUndefined();
                expect(result["simple"])->toBe(false);
            }
            );
            it(std:("should handle all non-string actions"), [=]() mutable
            {
                auto responseContent = object{
                    object::pair{std:("text"), std:("Hello world")}, 
                    object::pair{std:("actions"), array<std::nullptr_t>{ nullptr, undefined, 123, object{} }}
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
