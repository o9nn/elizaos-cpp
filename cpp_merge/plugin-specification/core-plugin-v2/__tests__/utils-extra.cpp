#include "utils-extra.test.h"

void Main(void)
{
    describe(std:("utils extra"), [=]() mutable
    {
        it(std:("addHeader prepends header when body exists"), [=]() mutable
        {
            expect(addHeader(std:("Head"), std:("Body")))->toBe(std:("Head\
Body\
"));
            expect(addHeader(std:("Head"), string_empty))->toBe(string_empty);
        }
        );
        it(std:("parseKeyValueXml parses simple xml block"), [=]() mutable
        {
            auto xml = std:("<response><key>value</key><actions>a,b</actions><simple>true</simple></response>");
            auto parsed = parseKeyValueXml(xml);
            expect(parsed)->toEqual(object{
                object::pair{std:("key"), std:("value")}, 
                object::pair{std:("actions"), array<string>{ std:("a"), std:("b") }}, 
                object::pair{std:("simple"), true}
            });
        }
        );
        it(std:("safeReplacer handles circular objects"), [=]() mutable
        {
            auto obj = object{
                object::pair{std:("a"), 1}
            };
            obj["self"] = obj;
            auto str = JSON->stringify(obj, safeReplacer());
            expect(str)->toContain(std:("[Circular]"));
        }
        );
        it(std:("validateUuid validates correct uuid and rejects bad values"), [=]() mutable
        {
            auto valid = validateUuid(std:("123e4567-e89b-12d3-a456-426614174000"));
            auto invalid = validateUuid(std:("not-a-uuid"));
            expect(valid)->toBe(std:("123e4567-e89b-12d3-a456-426614174000"));
            expect(invalid)->toBeNull();
        }
        );
    }
    );
}

MAIN
