#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-self-modification/src/__tests__/character-updater.test.h"

void Main(void)
{
    describe(std::string("Character Updater"), [=]() mutable
    {
        describe(std::string("applyOperationsToCharacter"), [=]() mutable
        {
            shared baseCharacter = object{
                object::pair{std::string("name"), std::string("TestAgent")}, 
                object::pair{std::string("bio"), array<string>{ std::string("Original bio") }}, 
                object::pair{std::string("lore"), array<string>{ std::string("Original lore") }}, 
                object::pair{std::string("system"), std::string("Original system prompt")}, 
                object::pair{std::string("adjectives"), array<string>{ std::string("helpful"), std::string("friendly") }}, 
                object::pair{std::string("topics"), array<string>{ std::string("general"), std::string("tech") }}, 
                object::pair{std::string("style"), object{
                    object::pair{std::string("all"), array<string>{ std::string("Be helpful") }}, 
                    object::pair{std::string("chat"), array<string>{ std::string("Be conversational") }}, 
                    object::pair{std::string("post"), array<string>{ std::string("Be informative") }}
                }}
            };
            it(std::string("should add new values to arrays"), [=]() mutable
            {
                auto operations = array<std::shared_ptr<ModificationOperation>>{ object{
                    object::pair{std::string("type"), std::string("add")}, 
                    object::pair{std::string("path"), std::string("bio[]")}, 
                    object::pair{std::string("value"), std::string("New bio entry")}
                }, object{
                    object::pair{std::string("type"), std::string("add")}, 
                    object::pair{std::string("path"), std::string("topics[]")}, 
                    object::pair{std::string("value"), std::string("philosophy")}
                } };
                auto result = applyOperationsToCharacter(baseCharacter, operations);
                expect(result->bio)->toContain(std::string("New bio entry"));
                expect(result->bio)->toHaveLength(2);
                expect(result->topics)->toContain(std::string("philosophy"));
                expect(result->topics)->toHaveLength(3);
            }
            );
            it(std::string("should modify existing values"), [=]() mutable
            {
                auto operations = array<std::shared_ptr<ModificationOperation>>{ object{
                    object::pair{std::string("type"), std::string("modify")}, 
                    object::pair{std::string("path"), std::string("system")}, 
                    object::pair{std::string("value"), std::string("Updated system prompt")}
                }, object{
                    object::pair{std::string("type"), std::string("modify")}, 
                    object::pair{std::string("path"), std::string("style.chat[0]")}, 
                    object::pair{std::string("value"), std::string("Be more engaging")}
                } };
                auto result = applyOperationsToCharacter(baseCharacter, operations);
                expect(result->system)->toBe(std::string("Updated system prompt"));
                expect(const_(result->style->chat)[0])->toBe(std::string("Be more engaging"));
            }
            );
            it(std::string("should delete values"), [=]() mutable
            {
                auto operations = array<std::shared_ptr<ModificationOperation>>{ object{
                    object::pair{std::string("type"), std::string("delete")}, 
                    object::pair{std::string("path"), std::string("topics[0]")}
                }, object{
                    object::pair{std::string("type"), std::string("delete")}, 
                    object::pair{std::string("path"), std::string("adjectives[1]")}
                } };
                auto result = applyOperationsToCharacter(baseCharacter, operations);
                expect(result->topics)->not->toContain(std::string("general"));
                expect(result->topics)->toHaveLength(1);
                expect(result->adjectives)->not->toContain(std::string("friendly"));
                expect(result->adjectives)->toHaveLength(1);
            }
            );
            it(std::string("should handle complex paths"), [=]() mutable
            {
                auto operations = array<std::shared_ptr<ModificationOperation>>{ object{
                    object::pair{std::string("type"), std::string("add")}, 
                    object::pair{std::string("path"), std::string("style.all[]")}, 
                    object::pair{std::string("value"), std::string("New style guideline")}
                } };
                auto result = applyOperationsToCharacter(baseCharacter, operations);
                expect(result->style->all)->toContain(std::string("New style guideline"));
                expect(result->style->all)->toHaveLength(2);
            }
            );
            it(std::string("should create arrays if they do not exist"), [=]() mutable
            {
                auto characterWithoutLore = utils::assign(object{
                    , 
                    object::pair{std::string("lore"), undefined}
                }, baseCharacter);
                auto operations = array<std::shared_ptr<ModificationOperation>>{ object{
                    object::pair{std::string("type"), std::string("add")}, 
                    object::pair{std::string("path"), std::string("lore[]")}, 
                    object::pair{std::string("value"), std::string("First lore entry")}
                } };
                auto result = applyOperationsToCharacter(characterWithoutLore, operations);
                expect(result->lore)->toEqual(array<string>{ std::string("First lore entry") });
            }
            );
            it(std::string("should not mutate the original character"), [=]() mutable
            {
                auto operations = array<std::shared_ptr<ModificationOperation>>{ object{
                    object::pair{std::string("type"), std::string("modify")}, 
                    object::pair{std::string("path"), std::string("system")}, 
                    object::pair{std::string("value"), std::string("Modified")}
                } };
                auto original = utils::assign(object{
                }, baseCharacter);
                auto result = applyOperationsToCharacter(baseCharacter, operations);
                expect(baseCharacter)->toEqual(original);
                expect(result)->not->toBe(baseCharacter);
            }
            );
            it(std::string("should throw error for invalid operations"), [=]() mutable
            {
                shared operations = array<std::shared_ptr<ModificationOperation>>{ object{
                    object::pair{std::string("type"), std::string("modify")}, 
                    object::pair{std::string("path"), std::string("nonexistent.deeply.nested.path")}, 
                    object::pair{std::string("value"), std::string("value")}
                } };
                expect([=]() mutable
                {
                    return applyOperationsToCharacter(baseCharacter, operations);
                }
                )->toThrow((new RegExp(std::string("Failed to apply operatio"))));
            }
            );
        }
        );
        describe(std::string("validateCharacterStructure"), [=]() mutable
        {
            it(std::string("should validate a proper character structure"), [=]() mutable
            {
                auto validCharacter = object{
                    object::pair{std::string("name"), std::string("Agent")}, 
                    object::pair{std::string("bio"), std::string("A helpful agent")}, 
                    object::pair{std::string("lore"), array<string>{ std::string("Some lore") }}, 
                    object::pair{std::string("messageExamples"), array<array<any>>{ array<any>() }}, 
                    object::pair{std::string("postExamples"), array<string>{ std::string("Example post") }}, 
                    object::pair{std::string("topics"), array<string>{ std::string("topic1") }}, 
                    object::pair{std::string("adjectives"), array<string>{ std::string("helpful") }}, 
                    object::pair{std::string("style"), object{
                        object::pair{std::string("all"), array<string>{ std::string("style1") }}, 
                        object::pair{std::string("chat"), array<string>{ std::string("chat style") }}, 
                        object::pair{std::string("post"), array<string>{ std::string("post style") }}
                    }}
                };
                expect(validateCharacterStructure(validCharacter))->toBe(true);
            }
            );
            it(std::string("should accept bio as string or array"), [=]() mutable
            {
                auto withStringBio = object{
                    object::pair{std::string("name"), std::string("Agent")}, 
                    object::pair{std::string("bio"), std::string("String bio")}
                };
                auto withArrayBio = object{
                    object::pair{std::string("name"), std::string("Agent")}, 
                    object::pair{std::string("bio"), array<string>{ std::string("Array"), std::string("bio") }}
                };
                expect(validateCharacterStructure(withStringBio))->toBe(true);
                expect(validateCharacterStructure(withArrayBio))->toBe(true);
            }
            );
            it(std::string("should reject missing name"), [=]() mutable
            {
                auto noName = object{
                    object::pair{std::string("bio"), std::string("Has bio but no name")}
                };
                expect(validateCharacterStructure(noName))->toBe(false);
            }
            );
            it(std::string("should reject non-array fields that should be arrays"), [=]() mutable
            {
                auto invalidArrays = object{
                    object::pair{std::string("name"), std::string("Agent")}, 
                    object::pair{std::string("topics"), std::string("not an array")}, 
                    object::pair{std::string("adjectives"), object{
                        object::pair{std::string("not"), std::string("an array")}
                    }}
                };
                expect(validateCharacterStructure(invalidArrays))->toBe(false);
            }
            );
            it(std::string("should validate style structure"), [=]() mutable
            {
                auto invalidStyle = object{
                    object::pair{std::string("name"), std::string("Agent")}, 
                    object::pair{std::string("style"), std::string("not an object")}
                };
                expect(validateCharacterStructure(invalidStyle))->toBe(false);
                auto invalidStyleArrays = object{
                    object::pair{std::string("name"), std::string("Agent")}, 
                    object::pair{std::string("style"), object{
                        object::pair{std::string("all"), std::string("not an array")}, 
                        object::pair{std::string("chat"), array<string>{ std::string("valid") }}, 
                        object::pair{std::string("post"), array<string>{ std::string("valid") }}
                    }}
                };
                expect(validateCharacterStructure(invalidStyleArrays))->toBe(false);
            }
            );
            it(std::string("should accept missing optional fields"), [=]() mutable
            {
                auto minimal = object{
                    object::pair{std::string("name"), std::string("Agent")}
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
