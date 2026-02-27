#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-self-modification/src/__tests__/xml-parser.test.h"

void Main(void)
{
    describe(std:("XML Parser"), [=]() mutable
    {
        describe(std:("parseCharacterDiff"), [=]() mutable
        {
            it(std:("should parse valid XML with all operation types"), [=]() mutable
            {
                auto xml = std:("\
<character-modification>\
  <operations>\
    <add path="bio[]" type="string">New bio entry</add>\
    <modify path="system" type="string">Updated system prompt</modify>\
    <delete path="topics[0]" />\
  </operations>\
  <reasoning>Test modification</reasoning>\
  <timestamp>2024-01-01T00:00:00Z</timestamp>\
</character-modification>");
                auto diff = parseCharacterDiff(xml);
                expect(diff->operations)->toHaveLength(3);
                expect(const_(diff->operations)[0])->toEqual(object{
                    object::pair{std:("type"), std:("add")}, 
                    object::pair{std:("path"), std:("bio[]")}, 
                    object::pair{std:("value"), std:("New bio entry")}, 
                    object::pair{std:("dataType"), std:("string")}
                });
                expect(const_(diff->operations)[1])->toEqual(object{
                    object::pair{std:("type"), std:("modify")}, 
                    object::pair{std:("path"), std:("system")}, 
                    object::pair{std:("value"), std:("Updated system prompt")}, 
                    object::pair{std:("dataType"), std:("string")}
                });
                expect(const_(diff->operations)[2])->toEqual(object{
                    object::pair{std:("type"), std:("delete")}, 
                    object::pair{std:("path"), std:("topics[0]")}, 
                    object::pair{std:("value"), undefined}, 
                    object::pair{std:("dataType"), undefined}
                });
                expect(diff->reasoning)->toBe(std:("Test modification"));
                expect(diff->timestamp)->toBe(std:("2024-01-01T00:00:00Z"));
            }
            );
            it(std:("should handle multiple operations of the same type"), [=]() mutable
            {
                auto xml = std:("\
<character-modification>\
  <operations>\
    <add path="bio[]" type="string">First bio</add>\
    <add path="bio[]" type="string">Second bio</add>\
    <add path="topics[]" type="string">New topic</add>\
  </operations>\
  <reasoning>Multiple additions</reasoning>\
</character-modification>");
                auto diff = parseCharacterDiff(xml);
                expect(diff->operations)->toHaveLength(3);
                expect(diff->operations->filter([=](auto op) mutable
                {
                    return op["type"] == std:("add");
                }
                ))->toHaveLength(3);
            }
            );
            it(std:("should throw error for missing root element"), [=]() mutable
            {
                shared xml = std:("<invalid>Not a character modification</invalid>");
                expect([=]() mutable
                {
                    return parseCharacterDiff(xml);
                }
                )->toThrow(std:("Invalid XML: missing character-modification root element"));
            }
            );
            it(std:("should throw error for missing reasoning"), [=]() mutable
            {
                shared xml = std:("\
<character-modification>\
  <operations>\
    <add path="bio[]" type="string">New bio</add>\
  </operations>\
</character-modification>");
                expect([=]() mutable
                {
                    return parseCharacterDiff(xml);
                }
                )->toThrow(std:("Missing or empty reasoning"));
            }
            );
            it(std:("should throw error for empty reasoning"), [=]() mutable
            {
                shared xml = std:("\
<character-modification>\
  <operations>\
    <add path="bio[]" type="string">New bio</add>\
  </operations>\
  <reasoning>   </reasoning>\
</character-modification>");
                expect([=]() mutable
                {
                    return parseCharacterDiff(xml);
                }
                )->toThrow(std:("Missing or empty reasoning"));
            }
            );
            it(std:("should throw error for invalid operation type"), [=]() mutable
            {
                shared xml = std:("\
<character-modification>\
  <operations>\
    <invalidOp path="bio[]">Test</invalidOp>\
  </operations>\
  <reasoning>Test</reasoning>\
</character-modification>");
                expect([=]() mutable
                {
                    return parseCharacterDiff(xml);
                }
                )->toThrow(std:("Invalid operation type: invalidOp"));
            }
            );
            it(std:("should throw error for missing path attribute"), [=]() mutable
            {
                shared xml = std:("\
<character-modification>\
  <operations>\
    <add type="string">No path</add>\
  </operations>\
  <reasoning>Test</reasoning>\
</character-modification>");
                expect([=]() mutable
                {
                    return parseCharacterDiff(xml);
                }
                )->toThrow(std:("Invalid path in add operation"));
            }
            );
            describe(std:("Security Tests"), [=]() mutable
            {
                it(std:("should prevent XXE attacks by removing DOCTYPE"), [=]() mutable
                {
                    auto xxeXml = std:("\
<!DOCTYPE foo [\
<!ENTITY xxe SYSTEM "file:///etc/passwd">\
]>\
<character-modification>\
  <operations>\
    <add path="bio[]" type="string">&xxe;</add>\
  </operations>\
  <reasoning>XXE attempt</reasoning>\
</character-modification>");
                    auto diff = parseCharacterDiff(xxeXml);
                    expect(const_(diff->operations)[0]->value)->toBe(std:("&xxe;"));
                    expect(const_(diff->operations)[0]->value)->not->toContain(std:("root:"));
                }
                );
                it(std:("should remove ENTITY declarations"), [=]() mutable
                {
                    auto entityXml = std:("\
<!ENTITY test "malicious content">\
<character-modification>\
  <operations>\
    <add path="bio[]" type="string">&test;</add>\
  </operations>\
  <reasoning>Entity test</reasoning>\
</character-modification>");
                    auto diff = parseCharacterDiff(entityXml);
                    expect(const_(diff->operations)[0]->value)->toBe(std:("&test;"));
                }
                );
                it(std:("should remove processing instructions"), [=]() mutable
                {
                    auto piXml = std:("\
<?php echo file_get_contents('/etc/passwd'); ?>\
<character-modification>\
  <operations>\
    <add path="bio[]" type="string">Test</add>\
  </operations>\
  <reasoning>PI test</reasoning>\
</character-modification>");
                    auto diff = parseCharacterDiff(piXml);
                    expect(const_(diff->operations)[0]->value)->toBe(std:("Test"));
                }
                );
                it(std:("should escape CDATA content"), [=]() mutable
                {
                    auto cdataXml = std:("\
<character-modification>\
  <operations>\
    <add path="bio[]" type="string"><![CDATA[<script>alert('xss')</script>]]></add>\
  </operations>\
  <reasoning>CDATA test</reasoning>\
</character-modification>");
                    auto diff = parseCharacterDiff(cdataXml);
                    expect(const_(diff->operations)[0]->value)->toContain(std:("&lt;script&gt;"));
                    expect(const_(diff->operations)[0]->value)->not->toContain(std:("<script>"));
                }
                );
                it(std:("should reject dangerous path patterns"), [=]() mutable
                {
                    shared dangerousPath1 = std:("\
<character-modification>\
  <operations>\
    <add path="../../../etc/passwd" type="string">Traversal</add>\
  </operations>\
  <reasoning>Path traversal</reasoning>\
</character-modification>");
                    expect([=]() mutable
                    {
                        return parseCharacterDiff(dangerousPath1);
                    }
                    )->toThrow(std:("Dangerous path pattern detected"));
                    shared dangerousPath2 = std:("\
<character-modification>\
  <operations>\
    <add path="bio//../../admin" type="string">Double slash</add>\
  </operations>\
  <reasoning>Double slash</reasoning>\
</character-modification>");
                    expect([=]() mutable
                    {
                        return parseCharacterDiff(dangerousPath2);
                    }
                    )->toThrow(std:("Dangerous path pattern detected"));
                }
                );
            }
            );
            it(std:("should handle empty operations gracefully"), [=]() mutable
            {
                auto xml = std:("\
<character-modification>\
  <operations>\
  </operations>\
  <reasoning>No operations</reasoning>\
</character-modification>");
                auto diff = parseCharacterDiff(xml);
                expect(diff->operations)->toEqual(array<any>());
            }
            );
            it(std:("should use current timestamp if not provided"), [=]() mutable
            {
                auto xml = std:("\
<character-modification>\
  <operations>\
    <add path="bio[]" type="string">Test</add>\
  </operations>\
  <reasoning>Test</reasoning>\
</character-modification>");
                auto beforeParse = std::make_shared<Date>();
                auto diff = parseCharacterDiff(xml);
                auto afterParse = std::make_shared<Date>();
                auto timestamp = std::make_shared<Date>(diff->timestamp);
                expect(timestamp->getTime())->toBeGreaterThanOrEqual(beforeParse->getTime());
                expect(timestamp->getTime())->toBeLessThanOrEqual(afterParse->getTime());
            }
            );
        }
        );
        describe(std:("buildCharacterDiffXml"), [=]() mutable
        {
            it(std:("should build valid XML from diff object"), [=]() mutable
            {
                auto diff = object{
                    object::pair{std:("operations"), array<object>{ object{
                        object::pair{std:("type"), std:("add")}, 
                        object::pair{std:("path"), std:("bio[]")}, 
                        object::pair{std:("value"), std:("New bio")}, 
                        object::pair{std:("dataType"), std:("string")}
                    }, object{
                        object::pair{std:("type"), std:("modify")}, 
                        object::pair{std:("path"), std:("system")}, 
                        object::pair{std:("value"), std:("Updated")}, 
                        object::pair{std:("dataType"), std:("string")}
                    }, object{
                        object::pair{std:("type"), std:("delete")}, 
                        object::pair{std:("path"), std:("topics[0]")}
                    } }}, 
                    object::pair{std:("reasoning"), std:("Test build")}, 
                    object::pair{std:("timestamp"), std:("2024-01-01T00:00:00Z")}
                };
                auto xml = buildCharacterDiffXml(diff);
                expect(xml)->toContain(std:("<character-modification>"));
                expect(xml)->toContain(std:("<add path="bio[]" type="string">New bio</add>"));
                expect(xml)->toContain(std:("<modify path="system" type="string">Updated</modify>"));
                expect(xml)->toContain(std:("<delete path="topics[0]"/>"));
                expect(xml)->toContain(std:("<reasoning>Test build</reasoning>"));
                expect(xml)->toContain(std:("<timestamp>2024-01-01T00:00:00Z</timestamp>"));
            }
            );
            it(std:("should omit empty operation categories"), [=]() mutable
            {
                auto diff = object{
                    object::pair{std:("operations"), array<object>{ object{
                        object::pair{std:("type"), std:("add")}, 
                        object::pair{std:("path"), std:("bio[]")}, 
                        object::pair{std:("value"), std:("New bio")}
                    } }}, 
                    object::pair{std:("reasoning"), std:("Only additions")}, 
                    object::pair{std:("timestamp"), std:("2024-01-01T00:00:00Z")}
                };
                auto xml = buildCharacterDiffXml(diff);
                expect(xml)->toContain(std:("<add"));
                expect(xml)->not->toContain(std:("<modify"));
                expect(xml)->not->toContain(std:("<delete"));
            }
            );
            it(std:("should validate reasoning is not empty"), [=]() mutable
            {
                shared diff = object{
                    object::pair{std:("operations"), array<any>()}, 
                    object::pair{std:("reasoning"), string_empty}, 
                    object::pair{std:("timestamp"), std:("2024-01-01T00:00:00Z")}
                };
                expect([=]() mutable
                {
                    return buildCharacterDiffXml(diff);
                }
                )->toThrow(std:("Reasoning is required"));
            }
            );
            it(std:("should validate operations is an array"), [=]() mutable
            {
                shared diff = as<any>(object{
                    object::pair{std:("operations"), std:("not an array")}, 
                    object::pair{std:("reasoning"), std:("Test")}, 
                    object::pair{std:("timestamp"), std:("2024-01-01T00:00:00Z")}
                });
                expect([=]() mutable
                {
                    return buildCharacterDiffXml(diff);
                }
                )->toThrow(std:("Operations must be an array"));
            }
            );
            it(std:("should default dataType to string if not specified"), [=]() mutable
            {
                auto diff = object{
                    object::pair{std:("operations"), array<object>{ object{
                        object::pair{std:("type"), std:("add")}, 
                        object::pair{std:("path"), std:("bio[]")}, 
                        object::pair{std:("value"), std:("No type")}
                    } }}, 
                    object::pair{std:("reasoning"), std:("Default type test")}, 
                    object::pair{std:("timestamp"), std:("2024-01-01T00:00:00Z")}
                };
                auto xml = buildCharacterDiffXml(diff);
                expect(xml)->toContain(std:("type="string""));
            }
            );
            it(std:("should handle build errors gracefully"), [=]() mutable
            {
                shared diff = object{
                    object::pair{std:("operations"), array<object>{ object{
                        object::pair{std:("type"), as<any>(std:("add"))}, 
                        object::pair{std:("path"), as<any>(nullptr)}, 
                        object::pair{std:("value"), std:("Invalid")}
                    } }}, 
                    object::pair{std:("reasoning"), std:("Invalid operation")}, 
                    object::pair{std:("timestamp"), std:("2024-01-01T00:00:00Z")}
                };
                expect([=]() mutable
                {
                    return buildCharacterDiffXml(diff);
                }
                )->toThrow(std:("Invalid path in operation"));
            }
            );
        }
        );
        describe(std:("Round-trip conversion"), [=]() mutable
        {
            it(std:("should maintain data integrity through parse and build"), [=]() mutable
            {
                shared originalDiff = object{
                    object::pair{std:("operations"), array<object>{ object{
                        object::pair{std:("type"), std:("add")}, 
                        object::pair{std:("path"), std:("bio[]")}, 
                        object::pair{std:("value"), std:("Test bio")}, 
                        object::pair{std:("dataType"), std:("string")}
                    }, object{
                        object::pair{std:("type"), std:("modify")}, 
                        object::pair{std:("path"), std:("adjectives[0]")}, 
                        object::pair{std:("value"), std:("creative")}, 
                        object::pair{std:("dataType"), std:("string")}
                    }, object{
                        object::pair{std:("type"), std:("delete")}, 
                        object::pair{std:("path"), std:("topics[5]")}
                    } }}, 
                    object::pair{std:("reasoning"), std:("Round trip test")}, 
                    object::pair{std:("timestamp"), std:("2024-01-01T12:00:00Z")}
                };
                auto xml = buildCharacterDiffXml(originalDiff);
                auto parsedDiff = parseCharacterDiff(xml);
                expect(parsedDiff->operations)->toHaveLength(3);
                expect(parsedDiff->reasoning)->toBe(originalDiff->reasoning);
                expect(parsedDiff->timestamp)->toBe(originalDiff->timestamp);
                parsedDiff->operations->forEach([=](auto op, auto i) mutable
                {
                    expect(op["type"])->toBe(const_(originalDiff->operations)[i]->type);
                    expect(op["path"])->toBe(const_(originalDiff->operations)[i]->path);
                    expect(op["value"])->toBe(const_(originalDiff->operations)[i]->value);
                    expect(op["dataType"])->toBe(const_(originalDiff->operations)[i]->dataType);
                }
                );
            }
            );
        }
        );
    }
    );
}

MAIN
