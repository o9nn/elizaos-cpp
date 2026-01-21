#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/__tests__/entities-extra.test.h"

void Main(void)
{
    describe(std::string("entities extra"), [=]() mutable
    {
        it(std::string("createUniqueUuid combines user and agent ids"), [=]() mutable
        {
            auto runtime = as<any>(object{
                object::pair{std::string("agentId"), std::string("agent")}
            });
            auto id = createUniqueUuid(runtime, std::string("user"));
            auto expected = stringToUuid(std::string("user:agent"));
            expect(id)->toBe(expected);
        }
        );
        it(std::string("formatEntities outputs joined string"), [=]() mutable
        {
            auto entities = as<any>(array<any>{ object{
                object::pair{std::string("id"), std::string("1")}, 
                object::pair{std::string("names"), array<string>{ std::string("A") }}, 
                object::pair{std::string("metadata"), object{}}
            }, object{
                object::pair{std::string("id"), std::string("2")}, 
                object::pair{std::string("names"), array<string>{ std::string("B") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("extra"), true}
                }}
            } });
            auto text = formatEntities(object{
                object::pair{std::string("entities"), std::string("entities")}
            });
            expect(text)->toContain(std::string(""A""));
            expect(text)->toContain(std::string("ID: 1"));
            expect(text)->toContain(std::string("ID: 2"));
        }
        );
    }
    );
}

MAIN
