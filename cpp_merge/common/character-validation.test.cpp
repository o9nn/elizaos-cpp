#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/core/src/__tests__/character-validation.test.h"

void Main(void)
{
    describe(std:("Character Schema Validation"), [=]() mutable
    {
        shared validCharacter = object{
            object::pair{std:("name"), std:("Test Character")}, 
            object::pair{std:("bio"), std:("A test character for validation")}, 
            object::pair{std:("messageExamples"), array<any>()}, 
            object::pair{std:("postExamples"), array<any>()}, 
            object::pair{std:("topics"), array<string>{ std:("AI"), std:("Testing") }}, 
            object::pair{std:("adjectives"), array<string>{ std:("helpful"), std:("reliable") }}, 
            object::pair{std:("knowledge"), array<any>()}, 
            object::pair{std:("plugins"), array<any>()}, 
            object::pair{std:("settings"), object{}}, 
            object::pair{std:("style"), object{}}
        };
        shared minimalValidCharacter = object{
            object::pair{std:("name"), std:("Minimal Character")}, 
            object::pair{std:("bio"), std:("Just the basics")}
        };
        describe(std:("validateCharacter"), [=]() mutable
        {
            test(std:("should validate a complete valid character"), [=]() mutable
            {
                auto result = validateCharacter(validCharacter);
                expect(result->success)->toBe(true);
                expect(result->data)->toEqual(validCharacter);
                expect(result->error)->toBeUndefined();
            }
            );
            test(std:("should validate a minimal valid character"), [=]() mutable
            {
                auto result = validateCharacter(minimalValidCharacter);
                expect(result->success)->toBe(true);
                expect(result->data)->toEqual(minimalValidCharacter);
            }
            );
            test(std:("should reject character without name"), [=]() mutable
            {
                auto invalidCharacter = object{
                    object::pair{std:("bio"), std:("No name character")}
                };
                auto result = validateCharacter(invalidCharacter);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std:("Character validation failed"));
                expect(result->error["issues"])->toBeDefined();
            }
            );
            test(std:("should reject character with empty name"), [=]() mutable
            {
                auto invalidCharacter = object{
                    object::pair{std:("name"), string_empty}, 
                    object::pair{std:("bio"), std:("Empty name")}
                };
                auto result = validateCharacter(invalidCharacter);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std:("Character validation failed"));
            }
            );
            test(std:("should reject character without bio"), [=]() mutable
            {
                auto invalidCharacter = object{
                    object::pair{std:("name"), std:("No Bio Character")}
                };
                auto result = validateCharacter(invalidCharacter);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std:("Character validation failed"));
            }
            );
            test(std:("should accept bio array"), [=]() mutable
            {
                auto characterWithArrayBio = object{
                    object::pair{std:("name"), std:("Array Bio Character")}, 
                    object::pair{std:("bio"), array<string>{ std:("First line"), std:("Second line"), std:("Third line") }}
                };
                auto result = validateCharacter(characterWithArrayBio);
                expect(result->success)->toBe(true);
            }
            );
            test(std:("should reject unknown properties in strict mode"), [=]() mutable
            {
                auto characterWithExtra = utils::assign(object{
                    , 
                    object::pair{std:("unknownProperty"), std:("should be rejected")}, 
                    object::pair{std:("anotherUnknown"), 123}
                }, validCharacter);
                auto result = validateCharacter(characterWithExtra);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std:("Unrecognized key"));
            }
            );
            test(std:("should validate optional fields correctly"), [=]() mutable
            {
                auto characterWithOptionals = object{
                    object::pair{std:("name"), std:("Optional Fields Character")}, 
                    object::pair{std:("bio"), std:("Testing optional fields")}, 
                    object::pair{std:("username"), std:("test_user")}, 
                    object::pair{std:("system"), std:("Test system prompt")}, 
                    object::pair{std:("messageExamples"), array<array<object>>{ array<object>{ object{
                        object::pair{std:("name"), std:("user")}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("Hello")}
                        }}
                    }, object{
                        object::pair{std:("name"), std:("assistant")}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("Hi there!")}
                        }}
                    } } }}, 
                    object::pair{std:("postExamples"), array<string>{ std:("Example post 1"), std:("Example post 2") }}, 
                    object::pair{std:("topics"), array<string>{ std:("AI"), std:("Testing"), std:("Validation") }}, 
                    object::pair{std:("adjectives"), array<string>{ std:("helpful"), std:("reliable"), std:("intelligent") }}, 
                    object::pair{std:("knowledge"), array<string>{ std:("knowledge/file1.txt"), object{
                        object::pair{std:("path"), std:("knowledge/file2.txt")}, 
                        object::pair{std:("shared"), true}
                    }, object{
                        object::pair{std:("directory"), std:("knowledge/shared")}, 
                        object::pair{std:("shared"), true}
                    } }}, 
                    object::pair{std:("plugins"), array<string>{ std:("plugin1"), std:("plugin2") }}, 
                    object::pair{std:("settings"), object{
                        object::pair{std:("temperature"), 0.7}, 
                        object::pair{std:("maxTokens"), 1000}, 
                        object::pair{std:("debug"), true}
                    }}, 
                    object::pair{std:("secrets"), object{
                        object::pair{std:("apiKey"), std:("secret-key")}, 
                        object::pair{std:("enabled"), true}
                    }}, 
                    object::pair{std:("style"), object{
                        object::pair{std:("all"), array<string>{ std:("casual"), std:("friendly") }}, 
                        object::pair{std:("chat"), array<string>{ std:("responsive"), std:("helpful") }}, 
                        object::pair{std:("post"), array<string>{ std:("engaging"), std:("informative") }}
                    }}
                };
                auto result = validateCharacter(characterWithOptionals);
                expect(result->success)->toBe(true);
            }
            );
            test(std:("should validate UUID format for id field"), [=]() mutable
            {
                auto characterWithValidUuid = utils::assign(object{
                    , 
                    object::pair{std:("id"), std:("123e4567-e89b-12d3-a456-426614174000")}
                }, validCharacter);
                auto result = validateCharacter(characterWithValidUuid);
                expect(result->success)->toBe(true);
            }
            );
            test(std:("should reject invalid UUID format for id field"), [=]() mutable
            {
                auto characterWithInvalidUuid = utils::assign(object{
                    , 
                    object::pair{std:("id"), std:("invalid-uuid-format")}
                }, validCharacter);
                auto result = validateCharacter(characterWithInvalidUuid);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std:("Invalid UUID format"));
            }
            );
        }
        );
        describe(std:("parseAndValidateCharacter"), [=]() mutable
        {
            test(std:("should parse and validate valid JSON character"), [=]() mutable
            {
                auto jsonString = JSON->stringify(validCharacter);
                auto result = parseAndValidateCharacter(jsonString);
                expect(result->success)->toBe(true);
                expect(result->data)->toEqual(validCharacter);
            }
            );
            test(std:("should handle malformed JSON"), [=]() mutable
            {
                auto malformedJson = std:("{ "name": "Test", "bio": "Test" ");
                auto result = parseAndValidateCharacter(malformedJson);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std:("Invalid JSON"));
            }
            );
            test(std:("should handle JSON with invalid character data"), [=]() mutable
            {
                auto invalidCharacterJson = JSON->stringify(object{
                    object::pair{std:("name"), string_empty}, 
                    object::pair{std:("bio"), std:("Invalid")}
                });
                auto result = parseAndValidateCharacter(invalidCharacterJson);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std:("Character validation failed"));
            }
            );
            test(std:("should handle empty JSON object"), [=]() mutable
            {
                auto emptyJson = std:("{}");
                auto result = parseAndValidateCharacter(emptyJson);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std:("Character validation failed"));
            }
            );
        }
        );
        describe(std:("isValidCharacter"), [=]() mutable
        {
            test(std:("should return true for valid character"), [=]() mutable
            {
                expect(isValidCharacter(validCharacter))->toBe(true);
            }
            );
            test(std:("should return false for invalid character"), [=]() mutable
            {
                auto invalidCharacter = object{
                    object::pair{std:("name"), string_empty}, 
                    object::pair{std:("bio"), std:("Invalid")}
                };
                expect(isValidCharacter(invalidCharacter))->toBe(false);
            }
            );
            test(std:("should return false for non-object input"), [=]() mutable
            {
                expect(isValidCharacter(std:("string")))->toBe(false);
                expect(isValidCharacter(nullptr))->toBe(false);
                expect(isValidCharacter(undefined))->toBe(false);
                expect(isValidCharacter(123))->toBe(false);
            }
            );
        }
        );
        describe(std:("Complex validation scenarios"), [=]() mutable
        {
            test(std:("should validate character with complex knowledge array"), [=]() mutable
            {
                auto characterWithComplexKnowledge = object{
                    object::pair{std:("name"), std:("Knowledge Character")}, 
                    object::pair{std:("bio"), std:("Testing knowledge validation")}, 
                    object::pair{std:("knowledge"), array<string>{ std:("simple/path.txt"), object{
                        object::pair{std:("path"), std:("path/with/config.txt")}, 
                        object::pair{std:("shared"), false}
                    }, object{
                        object::pair{std:("path"), std:("shared/path.txt")}, 
                        object::pair{std:("shared"), true}
                    }, object{
                        object::pair{std:("directory"), std:("knowledge/dir")}
                    }, object{
                        object::pair{std:("directory"), std:("shared/dir")}, 
                        object::pair{std:("shared"), true}
                    } }}
                };
                auto result = validateCharacter(characterWithComplexKnowledge);
                expect(result->success)->toBe(true);
            }
            );
            test(std:("should validate character with complex message examples"), [=]() mutable
            {
                auto characterWithComplexMessages = object{
                    object::pair{std:("name"), std:("Message Character")}, 
                    object::pair{std:("bio"), std:("Testing message validation")}, 
                    object::pair{std:("messageExamples"), array<array<any>>{ array<object>{ object{
                        object::pair{std:("name"), std:("user")}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("Hello, how are you?")}, 
                            object::pair{std:("source"), std:("user")}
                        }}
                    }, object{
                        object::pair{std:("name"), std:("assistant")}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("I am doing well, thank you!")}, 
                            object::pair{std:("source"), std:("assistant")}, 
                            object::pair{std:("attachments"), array<object>{ object{
                                object::pair{std:("type"), std:("text")}, 
                                object::pair{std:("data"), std:("additional info")}
                            } }}
                        }}
                    } }, array<object>{ object{
                        object::pair{std:("name"), std:("user")}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("What is the weather like?")}
                        }}
                    }, object{
                        object::pair{std:("name"), std:("assistant")}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("I would need to check a weather service for current conditions.")}, 
                            object::pair{std:("url"), std:("https://weather.example.com")}
                        }}
                    } } }}
                };
                auto result = validateCharacter(characterWithComplexMessages);
                expect(result->success)->toBe(true);
            }
            );
            test(std:("should validate character with flexible settings"), [=]() mutable
            {
                auto characterWithFlexibleSettings = object{
                    object::pair{std:("name"), std:("Settings Character")}, 
                    object::pair{std:("bio"), std:("Testing settings validation")}, 
                    object::pair{std:("settings"), object{
                        object::pair{std:("temperature"), 0.8}, 
                        object::pair{std:("maxTokens"), 2000}, 
                        object::pair{std:("enableDebug"), true}, 
                        object::pair{std:("model"), std:("gpt-4")}, 
                        object::pair{std:("customConfig"), object{
                            object::pair{std:("nested"), object{
                                object::pair{std:("deeply"), object{
                                    object::pair{std:("value"), std:("test")}
                                }}
                            }}
                        }}, 
                        object::pair{std:("arrayValue"), array<double>{ 1, 2, 3 }}
                    }}, 
                    object::pair{std:("secrets"), object{
                        object::pair{std:("apiKey"), std:("sk-test123")}, 
                        object::pair{std:("secretNumber"), 42}, 
                        object::pair{std:("isEnabled"), true}
                    }}
                };
                auto result = validateCharacter(characterWithFlexibleSettings);
                expect(result->success)->toBe(true);
            }
            );
        }
        );
        describe(std:("Edge cases and error handling"), [=]() mutable
        {
            test(std:("should handle null and undefined inputs"), [=]() mutable
            {
                expect(validateCharacter(nullptr)->success)->toBe(false);
                expect(validateCharacter(undefined)->success)->toBe(false);
            }
            );
            test(std:("should handle non-object inputs"), [=]() mutable
            {
                expect(validateCharacter(std:("string"))->success)->toBe(false);
                expect(validateCharacter(123)->success)->toBe(false);
                expect(validateCharacter(array<any>())->success)->toBe(false);
            }
            );
            test(std:("should provide detailed error information"), [=]() mutable
            {
                auto invalidCharacter = object{
                    object::pair{std:("name"), string_empty}, 
                    object::pair{std:("bio"), 123}, 
                    object::pair{std:("messageExamples"), std:("invalid")}
                };
                auto result = validateCharacter(invalidCharacter);
                expect(result->success)->toBe(false);
                expect(result->error["issues"])->toBeDefined();
                expect(result->error["issues"]->get_length())->toBeGreaterThan(0);
            }
            );
        }
        );
    }
    );
}

MAIN
