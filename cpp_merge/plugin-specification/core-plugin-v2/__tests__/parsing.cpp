#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/__tests__/parsing.test.h"

void Main(void)
{
    describe(std:("Parsing Module"), [=]() mutable
    {
        describe(std:("parseBooleanFromText"), [=]() mutable
        {
            it(std:("should parse exact YES/NO matches"), [=]() mutable
            {
                expect(parseBooleanFromText(std:("YES")))->toBe(true);
                expect(parseBooleanFromText(std:("NO")))->toBe(false);
            }
            );
            it(std:("should handle case insensitive input"), [=]() mutable
            {
                expect(parseBooleanFromText(std:("yes")))->toBe(true);
                expect(parseBooleanFromText(std:("no")))->toBe(false);
            }
            );
            it(std:("should return null for invalid input"), [=]() mutable
            {
                expect(parseBooleanFromText(string_empty))->toBe(false);
                expect(parseBooleanFromText(std:("maybe")))->toBe(false);
                expect(parseBooleanFromText(std:("YES NO")))->toBe(false);
            }
            );
        }
        );
        describe(std:("parseJSONObjectFromText"), [=]() mutable
        {
            it(std:("should parse JSON object from code block"), [=]() mutable
            {
                auto input = std:(""""json\
{"key": "value", "number": 42}\
"""");
                expect(parseJSONObjectFromText(input))->toEqual(object{
                    object::pair{std:("key"), std:("value")}, 
                    object::pair{std:("number"), std:("42")}
                });
            }
            );
            it(std:("should parse JSON object without code block"), [=]() mutable
            {
                auto input = std:("{"key": "value", "number": 42}");
                expect(parseJSONObjectFromText(input))->toEqual(object{
                    object::pair{std:("key"), std:("value")}, 
                    object::pair{std:("number"), std:("42")}
                });
            }
            );
            it(std:("should parse JSON objects containing array values"), [=]() mutable
            {
                auto input = std:("{"key": ["item1", "item2", "item3"]}");
                expect(parseJSONObjectFromText(input))->toEqual(object{
                    object::pair{std:("key"), array<string>{ std:("item1"), std:("item2"), std:("item3") }}
                });
            }
            );
            it(std:("should handle empty objects"), [=]() mutable
            {
                expect(parseJSONObjectFromText(std:(""""json\
{}\
"""")))->toEqual(object{});
                expect(parseJSONObjectFromText(std:("{}")))->toEqual(object{});
            }
            );
            it(std:("should return null for invalid JSON"), [=]() mutable
            {
                expect(parseJSONObjectFromText(std:("invalid")))->toBe(nullptr);
                expect(parseJSONObjectFromText(std:("{invalid}")))->toBe(nullptr);
                expect(parseJSONObjectFromText(std:(""""json\
{invalid}\
"""")))->toBe(nullptr);
            }
            );
        }
        );
    }
    );
}

MAIN
