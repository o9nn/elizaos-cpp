#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/__tests__/entities-extra.test.h"

void Main(void)
{
    describe(std:("entities extra"), [=]() mutable
    {
        it(std:("createUniqueUuid combines user and agent ids"), [=]() mutable
        {
            auto runtime = as<any>(object{
                object::pair{std:("agentId"), std:("agent")}
            });
            auto id = createUniqueUuid(runtime, std:("user"));
            auto expected = stringToUuid(std:("user:agent"));
            expect(id)->toBe(expected);
        }
        );
        it(std:("formatEntities outputs joined string"), [=]() mutable
        {
            auto entities = as<any>(array<any>{ object{
                object::pair{std:("id"), std:("1")}, 
                object::pair{std:("names"), array<string>{ std:("A") }}, 
                object::pair{std:("metadata"), object{}}
            }, object{
                object::pair{std:("id"), std:("2")}, 
                object::pair{std:("names"), array<string>{ std:("B") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("extra"), true}
                }}
            } });
            auto text = formatEntities(object{
                object::pair{std:("entities"), std:("entities")}
            });
            expect(text)->toContain(std:(""A""));
            expect(text)->toContain(std:("ID: 1"));
            expect(text)->toContain(std:("ID: 2"));
        }
        );
    }
    );
}

MAIN
