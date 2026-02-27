#include "utils.test.h"

std::function<string(string)> jsonrepairImpl = [=](auto x) mutable
{
    return x;
};

void Main(void)
{
    vi->mock(std:("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std:("logger"), object{
                object::pair{std:("debug"), vi->fn()}
            }}
        });
    }
    );
    vi->mock(std:("jsonrepair"), [=]() mutable
    {
        return (object{
            object::pair{std:("jsonrepair"), [=](auto x) mutable
            {
                return jsonrepairImpl(x);
            }
            }
        });
    }
    );
    describe(std:("extractAndParseJSON"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            jsonrepairImpl = [=](auto x) mutable
            {
                return x;
            };
        }
        );
        it(std:("parses valid JSON directly"), [=]() mutable
        {
            auto input = std:("{"foo": "bar"}");
            expect(extractAndParseJSON(input))->toEqual(object{
                object::pair{std:("foo"), std:("bar")}
            });
        }
        );
        it(std:("repairs and parses broken JSON"), [=]() mutable
        {
            jsonrepairImpl = [=](auto x) mutable
            {
                return std:("{"foo": "bar"}");
            };
            auto input = std:("{foo: "bar"}");
            expect(extractAndParseJSON(input))->toEqual(object{
                object::pair{std:("foo"), std:("bar")}
            });
        }
        );
        it(std:("handles JSON with markdown code blocks"), [=]() mutable
        {
            auto input = std:("{"code": """"js\nconsole.log(1);\n""""}");
            expect(extractAndParseJSON(input))->toEqual(object{
                object::pair{std:("code"), std:(""""js\
std::cout << 1 << std::endl;\
"""")}
            });
        }
        );
        it(std:("returns structured object for thought/message pattern"), [=]() mutable
        {
            auto input = std:(""thought": "Think!", "message": "Hello"");
            auto result = extractAndParseJSON(input);
            expect(result)->toMatchObject(object{
                object::pair{std:("type"), std:("reconstructed_response")}, 
                object::pair{std:("thought"), std:("Think!")}, 
                object::pair{std:("message"), std:("Hello")}
            });
        }
        );
        it(std:("returns unstructured_response for unparseable input"), [=]() mutable
        {
            auto input = std:("Not JSON at all");
            auto result = extractAndParseJSON(input);
            expect(result)->toMatchObject(object{
                object::pair{std:("type"), std:("unstructured_response")}, 
                object::pair{std:("content"), input}
            });
        }
        );
    }
    );
    describe(std:("ensureReflectionProperties"), [=]() mutable
    {
        it(std:("adds missing reflection properties when isReflection is true"), [=]() mutable
        {
            auto input = object{
                object::pair{std:("foo"), std:("bar")}
            };
            auto result = ensureReflectionProperties(input, true);
            expect(result)->toMatchObject(object{
                object::pair{std:("foo"), std:("bar")}, 
                object::pair{std:("thought"), string_empty}, 
                object::pair{std:("facts"), array<any>()}, 
                object::pair{std:("relationships"), array<any>()}
            });
        }
        );
        it(std:("does not modify object if isReflection is false"), [=]() mutable
        {
            auto input = object{
                object::pair{std:("foo"), std:("bar")}
            };
            expect(ensureReflectionProperties(input, false))->toEqual(input);
        }
        );
        it(std:("preserves existing reflection properties"), [=]() mutable
        {
            auto input = object{
                object::pair{std:("thought"), std:("a")}, 
                object::pair{std:("facts"), array<double>{ 1 }}, 
                object::pair{std:("relationships"), array<double>{ 2 }}
            };
            auto result = ensureReflectionProperties(input, true);
            expect(result)->toMatchObject(object{
                object::pair{std:("thought"), std:("a")}, 
                object::pair{std:("facts"), array<double>{ 1 }}, 
                object::pair{std:("relationships"), array<double>{ 2 }}
            });
        }
        );
    }
    );
}

MAIN
