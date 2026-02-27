#include "utils.test.h"
#include <string>

std::function<string(string)> jsonrepairImpl = [=](auto x) mutable
{
    return x;
};

void Main(void)
{
    vi->mock(std::string("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std::string("logger"), object{
                object::pair{std::string("debug"), vi->fn()}
            }}
        });
    }
    );
    vi->mock(std::string("jsonrepair"), [=]() mutable
    {
        return (object{
            object::pair{std::string("jsonrepair"), [=](auto x) mutable
            {
                return jsonrepairImpl(x);
            }
            }
        });
    }
    );
    describe(std::string("extractAndParseJSON"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            jsonrepairImpl = [=](auto x) mutable
            {
                return x;
            };
        }
        );
        it(std::string("parses valid JSON directly"), [=]() mutable
        {
            auto input = std::string("{"foo": "bar"}");
            expect(extractAndParseJSON(input))->toEqual(object{
                object::pair{std::string("foo"), std::string("bar")}
            });
        }
        );
        it(std::string("repairs and parses broken JSON"), [=]() mutable
        {
            jsonrepairImpl = [=](auto x) mutable
            {
                return std::string("{"foo": "bar"}");
            };
            auto input = std::string("{foo: "bar"}");
            expect(extractAndParseJSON(input))->toEqual(object{
                object::pair{std::string("foo"), std::string("bar")}
            });
        }
        );
        it(std::string("handles JSON with markdown code blocks"), [=]() mutable
        {
            auto input = std::string("{"code": """"js\nconsole.log(1);\n""""}");
            expect(extractAndParseJSON(input))->toEqual(object{
                object::pair{std::string("code"), std::string(""""js\
std::cout << 1 << std::endl;\
"""")}
            });
        }
        );
        it(std::string("returns structured object for thought/message pattern"), [=]() mutable
        {
            auto input = std::string(""thought": "Think!", "message": "Hello"");
            auto result = extractAndParseJSON(input);
            expect(result)->toMatchObject(object{
                object::pair{std::string("type"), std::string("reconstructed_response")}, 
                object::pair{std::string("thought"), std::string("Think!")}, 
                object::pair{std::string("message"), std::string("Hello")}
            });
        }
        );
        it(std::string("returns unstructured_response for unparseable input"), [=]() mutable
        {
            auto input = std::string("Not JSON at all");
            auto result = extractAndParseJSON(input);
            expect(result)->toMatchObject(object{
                object::pair{std::string("type"), std::string("unstructured_response")}, 
                object::pair{std::string("content"), input}
            });
        }
        );
    }
    );
    describe(std::string("ensureReflectionProperties"), [=]() mutable
    {
        it(std::string("adds missing reflection properties when isReflection is true"), [=]() mutable
        {
            auto input = object{
                object::pair{std::string("foo"), std::string("bar")}
            };
            auto result = ensureReflectionProperties(input, true);
            expect(result)->toMatchObject(object{
                object::pair{std::string("foo"), std::string("bar")}, 
                object::pair{std::string("thought"), string_empty}, 
                object::pair{std::string("facts"), array<any>()}, 
                object::pair{std::string("relationships"), array<any>()}
            });
        }
        );
        it(std::string("does not modify object if isReflection is false"), [=]() mutable
        {
            auto input = object{
                object::pair{std::string("foo"), std::string("bar")}
            };
            expect(ensureReflectionProperties(input, false))->toEqual(input);
        }
        );
        it(std::string("preserves existing reflection properties"), [=]() mutable
        {
            auto input = object{
                object::pair{std::string("thought"), std::string("a")}, 
                object::pair{std::string("facts"), array<double>{ 1 }}, 
                object::pair{std::string("relationships"), array<double>{ 2 }}
            };
            auto result = ensureReflectionProperties(input, true);
            expect(result)->toMatchObject(object{
                object::pair{std::string("thought"), std::string("a")}, 
                object::pair{std::string("facts"), array<double>{ 1 }}, 
                object::pair{std::string("relationships"), array<double>{ 2 }}
            });
        }
        );
    }
    );
}

MAIN
