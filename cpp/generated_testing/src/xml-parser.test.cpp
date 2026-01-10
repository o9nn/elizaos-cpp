#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-self-modification/src/__tests__/xml-parser.test.h"

void Main(void)
{
    describe(std::string("XML Parser"), [=]() mutable
    {
        describe(std::string("parseCharacterDiff"), [=]() mutable
        {
            it(std::string("should parse valid XML with all operation types"), [=]() mutable
            {
                auto xml = std::string("\
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
                    object::pair{std::string("type"), std::string("add")}, 
                    object::pair{std::string("path"), std::string("bio[]")}, 
                    object::pair{std::string("value"), std::string("New bio entry")}, 
                    object::pair{std::string("dataType"), std::string("string")}
                });
                expect(const_(diff->operations)[1])->toEqual(object{
                    object::pair{std::string("type"), std::string("modify")}, 
                    object::pair{std::string("path"), std::string("system")}, 
                    object::pair{std::string("value"), std::string("Updated system prompt")}, 
                    object::pair{std::string("dataType"), std::string("string")}
                });
                expect(const_(diff->operations)[2])->toEqual(object{
                    object::pair{std::string("type"), std::string("delete")}, 
                    object::pair{std::string("path"), std::string("topics[0]")}, 
                    object::pair{std::string("value"), undefined}, 
                    object::pair{std::string("dataType"), undefined}
                });
                expect(diff->reasoning)->toBe(std::string("Test modification"));
                expect(diff->timestamp)->toBe(std::string("2024-01-01T00:00:00Z"));
            }
            );
            it(std::string("should handle multiple operations of the same type"), [=]() mutable
            {
                auto xml = std::string("\
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
                    return op["type"] == std::string("add");
                }
                ))->toHaveLength(3);
            }
            );
            it(std::string("should throw error for missing root element"), [=]() mutable
            {
                shared xml = std::string("<invalid>Not a character modification</invalid>");
                expect([=]() mutable
                {
                    return parseCharacterDiff(xml);
                }
                )->toThrow(std::string("Invalid XML: missing character-modification root element"));
            }
            );
            it(std::string("should throw error for missing reasoning"), [=]() mutable
            {
                shared xml = std::string("\
<character-modification>\
  <operations>\
    <add path="bio[]" type="string">New bio</add>\
  </operations>\
</character-modification>");
                expect([=]() mutable
                {
                    return parseCharacterDiff(xml);
                }
                )->toThrow(std::string("Missing or empty reasoning"));
            }
            );
            it(std::string("should throw error for empty reasoning"), [=]() mutable
            {
                shared xml = std::string("\
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
                )->toThrow(std::string("Missing or empty reasoning"));
            }
            );
            it(std::string("should throw error for invalid operation type"), [=]() mutable
            {
                shared xml = std::string("\
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
                )->toThrow(std::string("Invalid operation type: invalidOp"));
            }
            );
            it(std::string("should throw error for missing path attribute"), [=]() mutable
            {
                shared xml = std::string("\
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
                )->toThrow(std::string("Invalid path in add operation"));
            }
            );
            describe(std::string("Security Tests"), [=]() mutable
            {
                it(std::string("should prevent XXE attacks by removing DOCTYPE"), [=]() mutable
                {
                    auto xxeXml = std::string("\
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
                    expect(const_(diff->operations)[0]->value)->toBe(std::string("&xxe;"));
                    expect(const_(diff->operations)[0]->value)->not->toContain(std::string("root:"));
                }
                );
                it(std::string("should remove ENTITY declarations"), [=]() mutable
                {
                    auto entityXml = std::string("\
<!ENTITY test "malicious content">\
<character-modification>\
  <operations>\
    <add path="bio[]" type="string">&test;</add>\
  </operations>\
  <reasoning>Entity test</reasoning>\
</character-modification>");
                    auto diff = parseCharacterDiff(entityXml);
                    expect(const_(diff->operations)[0]->value)->toBe(std::string("&test;"));
                }
                );
                it(std::string("should remove processing instructions"), [=]() mutable
                {
                    auto piXml = std::string("\
<?php echo file_get_contents('/etc/passwd'); ?>\
<character-modification>\
  <operations>\
    <add path="bio[]" type="string">Test</add>\
  </operations>\
  <reasoning>PI test</reasoning>\
</character-modification>");
                    auto diff = parseCharacterDiff(piXml);
                    expect(const_(diff->operations)[0]->value)->toBe(std::string("Test"));
                }
                );
                it(std::string("should escape CDATA content"), [=]() mutable
                {
                    auto cdataXml = std::string("\
<character-modification>\
  <operations>\
    <add path="bio[]" type="string"><![CDATA[<script>alert('xss')</script>]]></add>\
  </operations>\
  <reasoning>CDATA test</reasoning>\
</character-modification>");
                    auto diff = parseCharacterDiff(cdataXml);
                    expect(const_(diff->operations)[0]->value)->toContain(std::string("&lt;script&gt;"));
                    expect(const_(diff->operations)[0]->value)->not->toContain(std::string("<script>"));
                }
                );
                it(std::string("should reject dangerous path patterns"), [=]() mutable
                {
                    shared dangerousPath1 = std::string("\
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
                    )->toThrow(std::string("Dangerous path pattern detected"));
                    shared dangerousPath2 = std::string("\
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
                    )->toThrow(std::string("Dangerous path pattern detected"));
                }
                );
            }
            );
            it(std::string("should handle empty operations gracefully"), [=]() mutable
            {
                auto xml = std::string("\
<character-modification>\
  <operations>\
  </operations>\
  <reasoning>No operations</reasoning>\
</character-modification>");
                auto diff = parseCharacterDiff(xml);
                expect(diff->operations)->toEqual(array<any>());
            }
            );
            it(std::string("should use current timestamp if not provided"), [=]() mutable
            {
                auto xml = std::string("\
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
        describe(std::string("buildCharacterDiffXml"), [=]() mutable
        {
            it(std::string("should build valid XML from diff object"), [=]() mutable
            {
                auto diff = object{
                    object::pair{std::string("operations"), array<object>{ object{
                        object::pair{std::string("type"), std::string("add")}, 
                        object::pair{std::string("path"), std::string("bio[]")}, 
                        object::pair{std::string("value"), std::string("New bio")}, 
                        object::pair{std::string("dataType"), std::string("string")}
                    }, object{
                        object::pair{std::string("type"), std::string("modify")}, 
                        object::pair{std::string("path"), std::string("system")}, 
                        object::pair{std::string("value"), std::string("Updated")}, 
                        object::pair{std::string("dataType"), std::string("string")}
                    }, object{
                        object::pair{std::string("type"), std::string("delete")}, 
                        object::pair{std::string("path"), std::string("topics[0]")}
                    } }}, 
                    object::pair{std::string("reasoning"), std::string("Test build")}, 
                    object::pair{std::string("timestamp"), std::string("2024-01-01T00:00:00Z")}
                };
                auto xml = buildCharacterDiffXml(diff);
                expect(xml)->toContain(std::string("<character-modification>"));
                expect(xml)->toContain(std::string("<add path="bio[]" type="string">New bio</add>"));
                expect(xml)->toContain(std::string("<modify path="system" type="string">Updated</modify>"));
                expect(xml)->toContain(std::string("<delete path="topics[0]"/>"));
                expect(xml)->toContain(std::string("<reasoning>Test build</reasoning>"));
                expect(xml)->toContain(std::string("<timestamp>2024-01-01T00:00:00Z</timestamp>"));
            }
            );
            it(std::string("should omit empty operation categories"), [=]() mutable
            {
                auto diff = object{
                    object::pair{std::string("operations"), array<object>{ object{
                        object::pair{std::string("type"), std::string("add")}, 
                        object::pair{std::string("path"), std::string("bio[]")}, 
                        object::pair{std::string("value"), std::string("New bio")}
                    } }}, 
                    object::pair{std::string("reasoning"), std::string("Only additions")}, 
                    object::pair{std::string("timestamp"), std::string("2024-01-01T00:00:00Z")}
                };
                auto xml = buildCharacterDiffXml(diff);
                expect(xml)->toContain(std::string("<add"));
                expect(xml)->not->toContain(std::string("<modify"));
                expect(xml)->not->toContain(std::string("<delete"));
            }
            );
            it(std::string("should validate reasoning is not empty"), [=]() mutable
            {
                shared diff = object{
                    object::pair{std::string("operations"), array<any>()}, 
                    object::pair{std::string("reasoning"), string_empty}, 
                    object::pair{std::string("timestamp"), std::string("2024-01-01T00:00:00Z")}
                };
                expect([=]() mutable
                {
                    return buildCharacterDiffXml(diff);
                }
                )->toThrow(std::string("Reasoning is required"));
            }
            );
            it(std::string("should validate operations is an array"), [=]() mutable
            {
                shared diff = as<any>(object{
                    object::pair{std::string("operations"), std::string("not an array")}, 
                    object::pair{std::string("reasoning"), std::string("Test")}, 
                    object::pair{std::string("timestamp"), std::string("2024-01-01T00:00:00Z")}
                });
                expect([=]() mutable
                {
                    return buildCharacterDiffXml(diff);
                }
                )->toThrow(std::string("Operations must be an array"));
            }
            );
            it(std::string("should default dataType to string if not specified"), [=]() mutable
            {
                auto diff = object{
                    object::pair{std::string("operations"), array<object>{ object{
                        object::pair{std::string("type"), std::string("add")}, 
                        object::pair{std::string("path"), std::string("bio[]")}, 
                        object::pair{std::string("value"), std::string("No type")}
                    } }}, 
                    object::pair{std::string("reasoning"), std::string("Default type test")}, 
                    object::pair{std::string("timestamp"), std::string("2024-01-01T00:00:00Z")}
                };
                auto xml = buildCharacterDiffXml(diff);
                expect(xml)->toContain(std::string("type="string""));
            }
            );
            it(std::string("should handle build errors gracefully"), [=]() mutable
            {
                shared diff = object{
                    object::pair{std::string("operations"), array<object>{ object{
                        object::pair{std::string("type"), as<any>(std::string("add"))}, 
                        object::pair{std::string("path"), as<any>(nullptr)}, 
                        object::pair{std::string("value"), std::string("Invalid")}
                    } }}, 
                    object::pair{std::string("reasoning"), std::string("Invalid operation")}, 
                    object::pair{std::string("timestamp"), std::string("2024-01-01T00:00:00Z")}
                };
                expect([=]() mutable
                {
                    return buildCharacterDiffXml(diff);
                }
                )->toThrow(std::string("Invalid path in operation"));
            }
            );
        }
        );
        describe(std::string("Round-trip conversion"), [=]() mutable
        {
            it(std::string("should maintain data integrity through parse and build"), [=]() mutable
            {
                shared originalDiff = object{
                    object::pair{std::string("operations"), array<object>{ object{
                        object::pair{std::string("type"), std::string("add")}, 
                        object::pair{std::string("path"), std::string("bio[]")}, 
                        object::pair{std::string("value"), std::string("Test bio")}, 
                        object::pair{std::string("dataType"), std::string("string")}
                    }, object{
                        object::pair{std::string("type"), std::string("modify")}, 
                        object::pair{std::string("path"), std::string("adjectives[0]")}, 
                        object::pair{std::string("value"), std::string("creative")}, 
                        object::pair{std::string("dataType"), std::string("string")}
                    }, object{
                        object::pair{std::string("type"), std::string("delete")}, 
                        object::pair{std::string("path"), std::string("topics[5]")}
                    } }}, 
                    object::pair{std::string("reasoning"), std::string("Round trip test")}, 
                    object::pair{std::string("timestamp"), std::string("2024-01-01T12:00:00Z")}
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
