#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-self-modification/src/__tests__/character-updater.test.h"

void Main(void)
{
    describe(std:("Character Updater"), [=]() mutable
    {
        describe(std:("applyOperationsToCharacter"), [=]() mutable
        {
            shared baseCharacter = object{
                object::pair{std:("name"), std:("TestAgent")}, 
                object::pair{std:("bio"), array<string>{ std:("Original bio") }}, 
                object::pair{std:("lore"), array<string>{ std:("Original lore") }}, 
                object::pair{std:("system"), std:("Original system prompt")}, 
                object::pair{std:("adjectives"), array<string>{ std:("helpful"), std:("friendly") }}, 
                object::pair{std:("topics"), array<string>{ std:("general"), std:("tech") }}, 
                object::pair{std:("style"), object{
                    object::pair{std:("all"), array<string>{ std:("Be helpful") }}, 
                    object::pair{std:("chat"), array<string>{ std:("Be conversational") }}, 
                    object::pair{std:("post"), array<string>{ std:("Be informative") }}
                }}
            };
            it(std:("should add new values to arrays"), [=]() mutable
            {
                auto operations = array<std::shared_ptr<ModificationOperation>>{ object{
                    object::pair{std:("type"), std:("add")}, 
                    object::pair{std:("path"), std:("bio[]")}, 
                    object::pair{std:("value"), std:("New bio entry")}
                }, object{
                    object::pair{std:("type"), std:("add")}, 
                    object::pair{std:("path"), std:("topics[]")}, 
                    object::pair{std:("value"), std:("philosophy")}
                } };
                auto result = applyOperationsToCharacter(baseCharacter, operations);
                expect(result->bio)->toContain(std:("New bio entry"));
                expect(result->bio)->toHaveLength(2);
                expect(result->topics)->toContain(std:("philosophy"));
                expect(result->topics)->toHaveLength(3);
            }
            );
            it(std:("should modify existing values"), [=]() mutable
            {
                auto operations = array<std::shared_ptr<ModificationOperation>>{ object{
                    object::pair{std:("type"), std:("modify")}, 
                    object::pair{std:("path"), std:("system")}, 
                    object::pair{std:("value"), std:("Updated system prompt")}
                }, object{
                    object::pair{std:("type"), std:("modify")}, 
                    object::pair{std:("path"), std:("style.chat[0]")}, 
                    object::pair{std:("value"), std:("Be more engaging")}
                } };
                auto result = applyOperationsToCharacter(baseCharacter, operations);
                expect(result->system)->toBe(std:("Updated system prompt"));
                expect(const_(result->style->chat)[0])->toBe(std:("Be more engaging"));
            }
            );
            it(std:("should delete values"), [=]() mutable
            {
                auto operations = array<std::shared_ptr<ModificationOperation>>{ object{
                    object::pair{std:("type"), std:("delete")}, 
                    object::pair{std:("path"), std:("topics[0]")}
                }, object{
                    object::pair{std:("type"), std:("delete")}, 
                    object::pair{std:("path"), std:("adjectives[1]")}
                } };
                auto result = applyOperationsToCharacter(baseCharacter, operations);
                expect(result->topics)->not->toContain(std:("general"));
                expect(result->topics)->toHaveLength(1);
                expect(result->adjectives)->not->toContain(std:("friendly"));
                expect(result->adjectives)->toHaveLength(1);
            }
            );
            it(std:("should handle complex paths"), [=]() mutable
            {
                auto operations = array<std::shared_ptr<ModificationOperation>>{ object{
                    object::pair{std:("type"), std:("add")}, 
                    object::pair{std:("path"), std:("style.all[]")}, 
                    object::pair{std:("value"), std:("New style guideline")}
                } };
                auto result = applyOperationsToCharacter(baseCharacter, operations);
                expect(result->style->all)->toContain(std:("New style guideline"));
                expect(result->style->all)->toHaveLength(2);
            }
            );
            it(std:("should create arrays if they do not exist"), [=]() mutable
            {
                auto characterWithoutLore = utils::assign(object{
                    , 
                    object::pair{std:("lore"), undefined}
                }, baseCharacter);
                auto operations = array<std::shared_ptr<ModificationOperation>>{ object{
                    object::pair{std:("type"), std:("add")}, 
                    object::pair{std:("path"), std:("lore[]")}, 
                    object::pair{std:("value"), std:("First lore entry")}
                } };
                auto result = applyOperationsToCharacter(characterWithoutLore, operations);
                expect(result->lore)->toEqual(array<string>{ std:("First lore entry") });
            }
            );
            it(std:("should not mutate the original character"), [=]() mutable
            {
                auto operations = array<std::shared_ptr<ModificationOperation>>{ object{
                    object::pair{std:("type"), std:("modify")}, 
                    object::pair{std:("path"), std:("system")}, 
                    object::pair{std:("value"), std:("Modified")}
                } };
                auto original = utils::assign(object{
                }, baseCharacter);
                auto result = applyOperationsToCharacter(baseCharacter, operations);
                expect(baseCharacter)->toEqual(original);
                expect(result)->not->toBe(baseCharacter);
            }
            );
            it(std:("should throw error for invalid operations"), [=]() mutable
            {
                shared operations = array<std::shared_ptr<ModificationOperation>>{ object{
                    object::pair{std:("type"), std:("modify")}, 
                    object::pair{std:("path"), std:("nonexistent.deeply.nested.path")}, 
                    object::pair{std:("value"), std:("value")}
                } };
                expect([=]() mutable
                {
                    return applyOperationsToCharacter(baseCharacter, operations);
                }
                )->toThrow((new RegExp(std:("Failed to apply operatio"))));
            }
            );
        }
        );
        describe(std:("validateCharacterStructure"), [=]() mutable
        {
            it(std:("should validate a proper character structure"), [=]() mutable
            {
                auto validCharacter = object{
                    object::pair{std:("name"), std:("Agent")}, 
                    object::pair{std:("bio"), std:("A helpful agent")}, 
                    object::pair{std:("lore"), array<string>{ std:("Some lore") }}, 
                    object::pair{std:("messageExamples"), array<array<any>>{ array<any>() }}, 
                    object::pair{std:("postExamples"), array<string>{ std:("Example post") }}, 
                    object::pair{std:("topics"), array<string>{ std:("topic1") }}, 
                    object::pair{std:("adjectives"), array<string>{ std:("helpful") }}, 
                    object::pair{std:("style"), object{
                        object::pair{std:("all"), array<string>{ std:("style1") }}, 
                        object::pair{std:("chat"), array<string>{ std:("chat style") }}, 
                        object::pair{std:("post"), array<string>{ std:("post style") }}
                    }}
                };
                expect(validateCharacterStructure(validCharacter))->toBe(true);
            }
            );
            it(std:("should accept bio or array"), [=]() mutable
            {
                auto withStringBio = object{
                    object::pair{std:("name"), std:("Agent")}, 
                    object::pair{std:("bio"), std:("String bio")}
                };
                auto withArrayBio = object{
                    object::pair{std:("name"), std:("Agent")}, 
                    object::pair{std:("bio"), array<string>{ std:("Array"), std:("bio") }}
                };
                expect(validateCharacterStructure(withStringBio))->toBe(true);
                expect(validateCharacterStructure(withArrayBio))->toBe(true);
            }
            );
            it(std:("should reject missing name"), [=]() mutable
            {
                auto noName = object{
                    object::pair{std:("bio"), std:("Has bio but no name")}
                };
                expect(validateCharacterStructure(noName))->toBe(false);
            }
            );
            it(std:("should reject non-array fields that should be arrays"), [=]() mutable
            {
                auto invalidArrays = object{
                    object::pair{std:("name"), std:("Agent")}, 
                    object::pair{std:("topics"), std:("not an array")}, 
                    object::pair{std:("adjectives"), object{
                        object::pair{std:("not"), std:("an array")}
                    }}
                };
                expect(validateCharacterStructure(invalidArrays))->toBe(false);
            }
            );
            it(std:("should validate style structure"), [=]() mutable
            {
                auto invalidStyle = object{
                    object::pair{std:("name"), std:("Agent")}, 
                    object::pair{std:("style"), std:("not an object")}
                };
                expect(validateCharacterStructure(invalidStyle))->toBe(false);
                auto invalidStyleArrays = object{
                    object::pair{std:("name"), std:("Agent")}, 
                    object::pair{std:("style"), object{
                        object::pair{std:("all"), std:("not an array")}, 
                        object::pair{std:("chat"), array<string>{ std:("valid") }}, 
                        object::pair{std:("post"), array<string>{ std:("valid") }}
                    }}
                };
                expect(validateCharacterStructure(invalidStyleArrays))->toBe(false);
            }
            );
            it(std:("should accept missing optional fields"), [=]() mutable
            {
                auto minimal = object{
                    object::pair{std:("name"), std:("Agent")}
                };
                expect(validateCharacterStructure(minimal))->toBe(true);
            }
            );
        }
        );
    }
    );
}

MAIN
