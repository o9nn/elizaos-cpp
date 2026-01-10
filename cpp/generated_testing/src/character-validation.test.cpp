#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/core/src/__tests__/character-validation.test.h"

void Main(void)
{
    describe(std::string("Character Schema Validation"), [=]() mutable
    {
        shared validCharacter = object{
            object::pair{std::string("name"), std::string("Test Character")}, 
            object::pair{std::string("bio"), std::string("A test character for validation")}, 
            object::pair{std::string("messageExamples"), array<any>()}, 
            object::pair{std::string("postExamples"), array<any>()}, 
            object::pair{std::string("topics"), array<string>{ std::string("AI"), std::string("Testing") }}, 
            object::pair{std::string("adjectives"), array<string>{ std::string("helpful"), std::string("reliable") }}, 
            object::pair{std::string("knowledge"), array<any>()}, 
            object::pair{std::string("plugins"), array<any>()}, 
            object::pair{std::string("settings"), object{}}, 
            object::pair{std::string("style"), object{}}
        };
        shared minimalValidCharacter = object{
            object::pair{std::string("name"), std::string("Minimal Character")}, 
            object::pair{std::string("bio"), std::string("Just the basics")}
        };
        describe(std::string("validateCharacter"), [=]() mutable
        {
            test(std::string("should validate a complete valid character"), [=]() mutable
            {
                auto result = validateCharacter(validCharacter);
                expect(result->success)->toBe(true);
                expect(result->data)->toEqual(validCharacter);
                expect(result->error)->toBeUndefined();
            }
            );
            test(std::string("should validate a minimal valid character"), [=]() mutable
            {
                auto result = validateCharacter(minimalValidCharacter);
                expect(result->success)->toBe(true);
                expect(result->data)->toEqual(minimalValidCharacter);
            }
            );
            test(std::string("should reject character without name"), [=]() mutable
            {
                auto invalidCharacter = object{
                    object::pair{std::string("bio"), std::string("No name character")}
                };
                auto result = validateCharacter(invalidCharacter);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std::string("Character validation failed"));
                expect(result->error["issues"])->toBeDefined();
            }
            );
            test(std::string("should reject character with empty name"), [=]() mutable
            {
                auto invalidCharacter = object{
                    object::pair{std::string("name"), string_empty}, 
                    object::pair{std::string("bio"), std::string("Empty name")}
                };
                auto result = validateCharacter(invalidCharacter);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std::string("Character validation failed"));
            }
            );
            test(std::string("should reject character without bio"), [=]() mutable
            {
                auto invalidCharacter = object{
                    object::pair{std::string("name"), std::string("No Bio Character")}
                };
                auto result = validateCharacter(invalidCharacter);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std::string("Character validation failed"));
            }
            );
            test(std::string("should accept bio as string array"), [=]() mutable
            {
                auto characterWithArrayBio = object{
                    object::pair{std::string("name"), std::string("Array Bio Character")}, 
                    object::pair{std::string("bio"), array<string>{ std::string("First line"), std::string("Second line"), std::string("Third line") }}
                };
                auto result = validateCharacter(characterWithArrayBio);
                expect(result->success)->toBe(true);
            }
            );
            test(std::string("should reject unknown properties in strict mode"), [=]() mutable
            {
                auto characterWithExtra = utils::assign(object{
                    , 
                    object::pair{std::string("unknownProperty"), std::string("should be rejected")}, 
                    object::pair{std::string("anotherUnknown"), 123}
                }, validCharacter);
                auto result = validateCharacter(characterWithExtra);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std::string("Unrecognized key"));
            }
            );
            test(std::string("should validate optional fields correctly"), [=]() mutable
            {
                auto characterWithOptionals = object{
                    object::pair{std::string("name"), std::string("Optional Fields Character")}, 
                    object::pair{std::string("bio"), std::string("Testing optional fields")}, 
                    object::pair{std::string("username"), std::string("test_user")}, 
                    object::pair{std::string("system"), std::string("Test system prompt")}, 
                    object::pair{std::string("messageExamples"), array<array<object>>{ array<object>{ object{
                        object::pair{std::string("name"), std::string("user")}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("Hello")}
                        }}
                    }, object{
                        object::pair{std::string("name"), std::string("assistant")}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("Hi there!")}
                        }}
                    } } }}, 
                    object::pair{std::string("postExamples"), array<string>{ std::string("Example post 1"), std::string("Example post 2") }}, 
                    object::pair{std::string("topics"), array<string>{ std::string("AI"), std::string("Testing"), std::string("Validation") }}, 
                    object::pair{std::string("adjectives"), array<string>{ std::string("helpful"), std::string("reliable"), std::string("intelligent") }}, 
                    object::pair{std::string("knowledge"), array<string>{ std::string("knowledge/file1.txt"), object{
                        object::pair{std::string("path"), std::string("knowledge/file2.txt")}, 
                        object::pair{std::string("shared"), true}
                    }, object{
                        object::pair{std::string("directory"), std::string("knowledge/shared")}, 
                        object::pair{std::string("shared"), true}
                    } }}, 
                    object::pair{std::string("plugins"), array<string>{ std::string("plugin1"), std::string("plugin2") }}, 
                    object::pair{std::string("settings"), object{
                        object::pair{std::string("temperature"), 0.7}, 
                        object::pair{std::string("maxTokens"), 1000}, 
                        object::pair{std::string("debug"), true}
                    }}, 
                    object::pair{std::string("secrets"), object{
                        object::pair{std::string("apiKey"), std::string("secret-key")}, 
                        object::pair{std::string("enabled"), true}
                    }}, 
                    object::pair{std::string("style"), object{
                        object::pair{std::string("all"), array<string>{ std::string("casual"), std::string("friendly") }}, 
                        object::pair{std::string("chat"), array<string>{ std::string("responsive"), std::string("helpful") }}, 
                        object::pair{std::string("post"), array<string>{ std::string("engaging"), std::string("informative") }}
                    }}
                };
                auto result = validateCharacter(characterWithOptionals);
                expect(result->success)->toBe(true);
            }
            );
            test(std::string("should validate UUID format for id field"), [=]() mutable
            {
                auto characterWithValidUuid = utils::assign(object{
                    , 
                    object::pair{std::string("id"), std::string("123e4567-e89b-12d3-a456-426614174000")}
                }, validCharacter);
                auto result = validateCharacter(characterWithValidUuid);
                expect(result->success)->toBe(true);
            }
            );
            test(std::string("should reject invalid UUID format for id field"), [=]() mutable
            {
                auto characterWithInvalidUuid = utils::assign(object{
                    , 
                    object::pair{std::string("id"), std::string("invalid-uuid-format")}
                }, validCharacter);
                auto result = validateCharacter(characterWithInvalidUuid);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std::string("Invalid UUID format"));
            }
            );
        }
        );
        describe(std::string("parseAndValidateCharacter"), [=]() mutable
        {
            test(std::string("should parse and validate valid JSON character"), [=]() mutable
            {
                auto jsonString = JSON->stringify(validCharacter);
                auto result = parseAndValidateCharacter(jsonString);
                expect(result->success)->toBe(true);
                expect(result->data)->toEqual(validCharacter);
            }
            );
            test(std::string("should handle malformed JSON"), [=]() mutable
            {
                auto malformedJson = std::string("{ "name": "Test", "bio": "Test" ");
                auto result = parseAndValidateCharacter(malformedJson);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std::string("Invalid JSON"));
            }
            );
            test(std::string("should handle JSON with invalid character data"), [=]() mutable
            {
                auto invalidCharacterJson = JSON->stringify(object{
                    object::pair{std::string("name"), string_empty}, 
                    object::pair{std::string("bio"), std::string("Invalid")}
                });
                auto result = parseAndValidateCharacter(invalidCharacterJson);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std::string("Character validation failed"));
            }
            );
            test(std::string("should handle empty JSON object"), [=]() mutable
            {
                auto emptyJson = std::string("{}");
                auto result = parseAndValidateCharacter(emptyJson);
                expect(result->success)->toBe(false);
                expect(result->error["message"])->toContain(std::string("Character validation failed"));
            }
            );
        }
        );
        describe(std::string("isValidCharacter"), [=]() mutable
        {
            test(std::string("should return true for valid character"), [=]() mutable
            {
                expect(isValidCharacter(validCharacter))->toBe(true);
            }
            );
            test(std::string("should return false for invalid character"), [=]() mutable
            {
                auto invalidCharacter = object{
                    object::pair{std::string("name"), string_empty}, 
                    object::pair{std::string("bio"), std::string("Invalid")}
                };
                expect(isValidCharacter(invalidCharacter))->toBe(false);
            }
            );
            test(std::string("should return false for non-object input"), [=]() mutable
            {
                expect(isValidCharacter(std::string("string")))->toBe(false);
                expect(isValidCharacter(nullptr))->toBe(false);
                expect(isValidCharacter(undefined))->toBe(false);
                expect(isValidCharacter(123))->toBe(false);
            }
            );
        }
        );
        describe(std::string("Complex validation scenarios"), [=]() mutable
        {
            test(std::string("should validate character with complex knowledge array"), [=]() mutable
            {
                auto characterWithComplexKnowledge = object{
                    object::pair{std::string("name"), std::string("Knowledge Character")}, 
                    object::pair{std::string("bio"), std::string("Testing knowledge validation")}, 
                    object::pair{std::string("knowledge"), array<string>{ std::string("simple/path.txt"), object{
                        object::pair{std::string("path"), std::string("path/with/config.txt")}, 
                        object::pair{std::string("shared"), false}
                    }, object{
                        object::pair{std::string("path"), std::string("shared/path.txt")}, 
                        object::pair{std::string("shared"), true}
                    }, object{
                        object::pair{std::string("directory"), std::string("knowledge/dir")}
                    }, object{
                        object::pair{std::string("directory"), std::string("shared/dir")}, 
                        object::pair{std::string("shared"), true}
                    } }}
                };
                auto result = validateCharacter(characterWithComplexKnowledge);
                expect(result->success)->toBe(true);
            }
            );
            test(std::string("should validate character with complex message examples"), [=]() mutable
            {
                auto characterWithComplexMessages = object{
                    object::pair{std::string("name"), std::string("Message Character")}, 
                    object::pair{std::string("bio"), std::string("Testing message validation")}, 
                    object::pair{std::string("messageExamples"), array<array<any>>{ array<object>{ object{
                        object::pair{std::string("name"), std::string("user")}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("Hello, how are you?")}, 
                            object::pair{std::string("source"), std::string("user")}
                        }}
                    }, object{
                        object::pair{std::string("name"), std::string("assistant")}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("I am doing well, thank you!")}, 
                            object::pair{std::string("source"), std::string("assistant")}, 
                            object::pair{std::string("attachments"), array<object>{ object{
                                object::pair{std::string("type"), std::string("text")}, 
                                object::pair{std::string("data"), std::string("additional info")}
                            } }}
                        }}
                    } }, array<object>{ object{
                        object::pair{std::string("name"), std::string("user")}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("What is the weather like?")}
                        }}
                    }, object{
                        object::pair{std::string("name"), std::string("assistant")}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("I would need to check a weather service for current conditions.")}, 
                            object::pair{std::string("url"), std::string("https://weather.example.com")}
                        }}
                    } } }}
                };
                auto result = validateCharacter(characterWithComplexMessages);
                expect(result->success)->toBe(true);
            }
            );
            test(std::string("should validate character with flexible settings"), [=]() mutable
            {
                auto characterWithFlexibleSettings = object{
                    object::pair{std::string("name"), std::string("Settings Character")}, 
                    object::pair{std::string("bio"), std::string("Testing settings validation")}, 
                    object::pair{std::string("settings"), object{
                        object::pair{std::string("temperature"), 0.8}, 
                        object::pair{std::string("maxTokens"), 2000}, 
                        object::pair{std::string("enableDebug"), true}, 
                        object::pair{std::string("model"), std::string("gpt-4")}, 
                        object::pair{std::string("customConfig"), object{
                            object::pair{std::string("nested"), object{
                                object::pair{std::string("deeply"), object{
                                    object::pair{std::string("value"), std::string("test")}
                                }}
                            }}
                        }}, 
                        object::pair{std::string("arrayValue"), array<double>{ 1, 2, 3 }}
                    }}, 
                    object::pair{std::string("secrets"), object{
                        object::pair{std::string("apiKey"), std::string("sk-test123")}, 
                        object::pair{std::string("secretNumber"), 42}, 
                        object::pair{std::string("isEnabled"), true}
                    }}
                };
                auto result = validateCharacter(characterWithFlexibleSettings);
                expect(result->success)->toBe(true);
            }
            );
        }
        );
        describe(std::string("Edge cases and error handling"), [=]() mutable
        {
            test(std::string("should handle null and undefined inputs"), [=]() mutable
            {
                expect(validateCharacter(nullptr)->success)->toBe(false);
                expect(validateCharacter(undefined)->success)->toBe(false);
            }
            );
            test(std::string("should handle non-object inputs"), [=]() mutable
            {
                expect(validateCharacter(std::string("string"))->success)->toBe(false);
                expect(validateCharacter(123)->success)->toBe(false);
                expect(validateCharacter(array<any>())->success)->toBe(false);
            }
            );
            test(std::string("should provide detailed error information"), [=]() mutable
            {
                auto invalidCharacter = object{
                    object::pair{std::string("name"), string_empty}, 
                    object::pair{std::string("bio"), 123}, 
                    object::pair{std::string("messageExamples"), std::string("invalid")}
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
