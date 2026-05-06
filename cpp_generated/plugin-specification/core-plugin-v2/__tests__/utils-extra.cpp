#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/__tests__/utils-extra.test.h"

void Main(void)
{
    describe(std::string("utils extra"), [=]() mutable
    {
        it(std::string("addHeader prepends header when body exists"), [=]() mutable
        {
            expect(addHeader(std::string("Head"), std::string("Body")))->toBe(std::string("Head\
Body\
"));
            expect(addHeader(std::string("Head"), string_empty))->toBe(string_empty);
        }
        );
        it(std::string("parseKeyValueXml parses simple xml block"), [=]() mutable
        {
            auto xml = std::string("<response><key>value</key><actions>a,b</actions><simple>true</simple></response>");
            auto parsed = parseKeyValueXml(xml);
            expect(parsed)->toEqual(object{
                object::pair{std::string("key"), std::string("value")}, 
                object::pair{std::string("actions"), array<string>{ std::string("a"), std::string("b") }}, 
                object::pair{std::string("simple"), true}
            });
        }
        );
        it(std::string("safeReplacer handles circular objects"), [=]() mutable
        {
            auto obj = object{
                object::pair{std::string("a"), 1}
            };
            obj["self"] = obj;
            auto str = JSON->stringify(obj, safeReplacer());
            expect(str)->toContain(std::string("[Circular]"));
        }
        );
        it(std::string("validateUuid validates correct uuid and rejects bad values"), [=]() mutable
        {
            auto valid = validateUuid(std::string("123e4567-e89b-12d3-a456-426614174000"));
            auto invalid = validateUuid(std::string("not-a-uuid"));
            expect(valid)->toBe(std::string("123e4567-e89b-12d3-a456-426614174000"));
            expect(invalid)->toBeNull();
        }
        );
    }
    );
}

MAIN
