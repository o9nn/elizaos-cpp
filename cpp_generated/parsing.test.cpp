#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/__tests__/parsing.test.h"

void Main(void)
{
    describe(std::string("Parsing Module"), [=]() mutable
    {
        describe(std::string("parseBooleanFromText"), [=]() mutable
        {
            it(std::string("should parse exact YES/NO matches"), [=]() mutable
            {
                expect(parseBooleanFromText(std::string("YES")))->toBe(true);
                expect(parseBooleanFromText(std::string("NO")))->toBe(false);
            }
            );
            it(std::string("should handle case insensitive input"), [=]() mutable
            {
                expect(parseBooleanFromText(std::string("yes")))->toBe(true);
                expect(parseBooleanFromText(std::string("no")))->toBe(false);
            }
            );
            it(std::string("should return null for invalid input"), [=]() mutable
            {
                expect(parseBooleanFromText(string_empty))->toBe(false);
                expect(parseBooleanFromText(std::string("maybe")))->toBe(false);
                expect(parseBooleanFromText(std::string("YES NO")))->toBe(false);
            }
            );
        }
        );
        describe(std::string("parseJSONObjectFromText"), [=]() mutable
        {
            it(std::string("should parse JSON object from code block"), [=]() mutable
            {
                auto input = std::string("```json\
{"key": "value", "number": 42}\
```");
                expect(parseJSONObjectFromText(input))->toEqual(object{
                    object::pair{std::string("key"), std::string("value")}, 
                    object::pair{std::string("number"), std::string("42")}
                });
            }
            );
            it(std::string("should parse JSON object without code block"), [=]() mutable
            {
                auto input = std::string("{"key": "value", "number": 42}");
                expect(parseJSONObjectFromText(input))->toEqual(object{
                    object::pair{std::string("key"), std::string("value")}, 
                    object::pair{std::string("number"), std::string("42")}
                });
            }
            );
            it(std::string("should parse JSON objects containing array values"), [=]() mutable
            {
                auto input = std::string("{"key": ["item1", "item2", "item3"]}");
                expect(parseJSONObjectFromText(input))->toEqual(object{
                    object::pair{std::string("key"), array<string>{ std::string("item1"), std::string("item2"), std::string("item3") }}
                });
            }
            );
            it(std::string("should handle empty objects"), [=]() mutable
            {
                expect(parseJSONObjectFromText(std::string("```json\
{}\
```")))->toEqual(object{});
                expect(parseJSONObjectFromText(std::string("{}")))->toEqual(object{});
            }
            );
            it(std::string("should return null for invalid JSON"), [=]() mutable
            {
                expect(parseJSONObjectFromText(std::string("invalid")))->toBe(nullptr);
                expect(parseJSONObjectFromText(std::string("{invalid}")))->toBe(nullptr);
                expect(parseJSONObjectFromText(std::string("```json\
{invalid}\
```")))->toBe(nullptr);
            }
            );
        }
        );
    }
    );
}

MAIN
