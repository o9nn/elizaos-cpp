#include "loader.test.h"

string TEST_MULTI_CHARACTER_URL = std:("https://raw.githubusercontent.com/elizaos/eliza/refs/heads/develop/packages/cli/tests/test-characters/multi-chars.json");
any mockFs = as<any>(fs);

void Main(void)
{
    mock->module(std:("node:fs"), [=]() mutable
    {
        return (object{
            object::pair{std:("existsSync"), mock([=]() mutable
            {
                return true;
            }
            )}, 
            object::pair{std:("readFileSync"), mock([=]() mutable
            {
                return std:("{}");
            }
            )}, 
            object::pair{std:("statSync"), mock([=]() mutable
            {
                return (object{
                    object::pair{std:("isDirectory"), [=]() mutable
                    {
                        return true;
                    }
                    }
                });
            }
            )}, 
            object::pair{std:("writeFileSync"), mock()}, 
            object::pair{std:("promises"), object{
                object::pair{std:("readFile"), mock([=]() mutable
                {
                    return std:("{}");
                }
                )}, 
                object::pair{std:("writeFile"), mock()}, 
                object::pair{std:("mkdir"), mock()}
            }}
        });
    }
    );
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std:("logger"), object{
                object::pair{std:("error"), mock()}, 
                object::pair{std:("warn"), mock()}, 
                object::pair{std:("info"), mock()}, 
                object::pair{std:("debug"), mock()}
            }}
        });
    }
    );
    describe(std:("Character Loader"), [=]() mutable
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
        describe(std:("tryLoadFile"), [=]() mutable
        {
            test(std:("should load file content successfully"), [=]() mutable
            {
                shared fileContent = std:("test file content");
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    return fileContent;
                }
                );
                auto result = tryLoadFile(std:("/path/to/file.json"));
                expect(result)->toBe(fileContent);
            }
            );
            test(std:("should throw error when file loading fails"), [=]() mutable
            {
                shared error = std::make_shared<Error>(std:("File not found"));
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    throw any(error);
                }
                );
                expect([=]() mutable
                {
                    return tryLoadFile(std:("/nonexistent/file.json"));
                }
                )->toThrow(std:("Error loading file /nonexistent/file.json: Error: File not found"));
            }
            );
        }
        );
        describe(std:("loadCharacter"), [=]() mutable
        {
            test(std:("should load and validate character from file"), [=]() mutable
            {
                shared characterJson = JSON->stringify(validCharacter);
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    return characterJson;
                }
                );
                auto result = std::async([=]() { loadCharacter(std:("/path/to/character.json")); });
                expect(result)->toEqual(expect->objectContaining(validCharacter));
            }
            );
            test(std:("should throw error for non-existent file"), [=]() mutable
            {
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    throw any(std::make_shared<Error>(std:("ENOENT: no such file or directory")));
                }
                );
                std::async([=]() { expect(loadCharacter(std:("/nonexistent/character.json")))->rejects->toThrow(std:("Error loading file /nonexistent/character.json")); });
            }
            );
            test(std:("should throw error for invalid JSON"), [=]() mutable
            {
                shared invalidJson = std:("{ "name": "Test", "bio": "Test" ");
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    return invalidJson;
                }
                );
                std::async([=]() { expect(loadCharacter(std:("/path/to/invalid.json")))->rejects->toThrow(std:("Invalid JSON")); });
            }
            );
            test(std:("should throw error for invalid character data"), [=]() mutable
            {
                shared invalidCharacter = JSON->stringify(object{
                    object::pair{std:("name"), string_empty}, 
                    object::pair{std:("bio"), std:("Invalid")}
                });
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    return invalidCharacter;
                }
                );
                std::async([=]() { expect(loadCharacter(std:("/path/to/invalid-character.json")))->rejects->toThrow(std:("Character validation failed")); });
            }
            );
            test(std:("should throw error for missing required fields"), [=]() mutable
            {
                shared incompleteCharacter = JSON->stringify(object{
                    object::pair{std:("name"), std:("Test")}
                });
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    return incompleteCharacter;
                }
                );
                std::async([=]() { expect(loadCharacter(std:("/path/to/incomplete.json")))->rejects->toThrow(std:("Character validation failed")); });
            }
            );
        }
        );
        describe(std:("jsonToCharacter"), [=]() mutable
        {
            test(std:("should validate and return character"), [=]() mutable
            {
                auto result = std::async([=]() { jsonToCharacter(validCharacter); });
                expect(result)->toEqual(expect->objectContaining(validCharacter));
            }
            );
            test(std:("should throw error for invalid character data"), [=]() mutable
            {
                auto invalidCharacter = object{
                    object::pair{std:("name"), string_empty}, 
                    object::pair{std:("bio"), std:("Invalid")}
                };
                std::async([=]() { expect(jsonToCharacter(invalidCharacter))->rejects->toThrow(std:("Character validation failed")); });
            }
            );
            test(std:("should handle environment-based settings"), [=]() mutable
            {
                auto originalEnv = process->env;
                process->env = utils::assign(object{
                    , 
                    object::pair{std:("CHARACTER.TEST_CHARACTER.API_KEY"), std:("test-key")}, 
                    object::pair{std:("CHARACTER.TEST_CHARACTER.DEBUG"), std:("true")}
                }, originalEnv);
                auto characterWithName = utils::assign(object{
                    , 
                    object::pair{std:("name"), std:("Test Character")}
                }, validCharacter);
                auto result = std::async([=]() { jsonToCharacter(characterWithName); });
                expect(result->secrets)->toEqual(expect->objectContaining(object{
                    object::pair{std:("API_KEY"), std:("test-key")}, 
                    object::pair{std:("DEBUG"), std:("true")}
                }));
                process->env = originalEnv;
            }
            );
            test(std:("should handle character without environment settings"), [=]() mutable
            {
                auto originalEnv = process->env;
                process->env = object{};
                auto result = std::async([=]() { jsonToCharacter(validCharacter); });
                expect(result)->toEqual(expect->objectContaining(validCharacter));
                process->env = originalEnv;
            }
            );
            test(std:("should validate character with complex data structures"), [=]() mutable
            {
                auto complexCharacter = object{
                    object::pair{std:("name"), std:("Complex Character")}, 
                    object::pair{std:("bio"), array<string>{ std:("Multi-line"), std:("bio description") }}, 
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
                    object::pair{std:("knowledge"), array<string>{ std:("simple/path.txt"), object{
                        object::pair{std:("path"), std:("complex/path.txt")}, 
                        object::pair{std:("shared"), true}
                    }, object{
                        object::pair{std:("directory"), std:("knowledge/dir")}, 
                        object::pair{std:("shared"), false}
                    } }}, 
                    object::pair{std:("settings"), object{
                        object::pair{std:("temperature"), 0.7}, 
                        object::pair{std:("nested"), object{
                            object::pair{std:("deeply"), object{
                                object::pair{std:("value"), std:("test")}
                            }}
                        }}
                    }}, 
                    object::pair{std:("style"), object{
                        object::pair{std:("all"), array<string>{ std:("casual") }}, 
                        object::pair{std:("chat"), array<string>{ std:("responsive") }}, 
                        object::pair{std:("post"), array<string>{ std:("engaging") }}
                    }}
                };
                auto result = std::async([=]() { jsonToCharacter(complexCharacter); });
                expect(result)->toEqual(expect->objectContaining(complexCharacter));
            }
            );
        }
        );
        describe(std:("loadCharactersFromUrl"), [=]() mutable
        {
            shared mockFetch = mock();
            global->fetch = mockFetch;
            beforeEach([=]() mutable
            {
                mockFetch;
            }
            );
            test(std:("should load single character from URL"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 200}, 
                    object::pair{std:("json"), [=]() mutable
                    {
                        return Promise->resolve(validCharacter);
                    }
                    }
                };
                mockFetch->mockResolvedValue(mockResponse);
                auto result = std::async([=]() { loadCharactersFromUrl(std:("https://example.com/character.json")); });
                expect(result)->toHaveLength(1);
                expect(const_(result)[0])->toEqual(expect->objectContaining(validCharacter));
            }
            );
            test(std:("should load multiple characters from URL"), [=]() mutable
            {
                shared characters = array<any>{ validCharacter, utils::assign(object{
                    , 
                    object::pair{std:("name"), std:("Second Character")}
                }, validCharacter) };
                auto mockResponse = object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 200}, 
                    object::pair{std:("json"), [=]() mutable
                    {
                        return Promise->resolve(characters);
                    }
                    }
                };
                mockFetch->mockResolvedValue(mockResponse);
                auto result = std::async([=]() { loadCharactersFromUrl(TEST_MULTI_CHARACTER_URL); });
                expect(result)->toHaveLength(2);
                expect(const_(result)[0])->toEqual(expect->objectContaining(validCharacter));
                expect(const_(result)[1]->name)->toBe(std:("Second Character"));
            }
            );
            test(std:("should handle HTTP errors"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("ok"), false}, 
                    object::pair{std:("status"), 404}, 
                    object::pair{std:("statusText"), std:("Not Found")}
                };
                mockFetch->mockResolvedValue(mockResponse);
                std::async([=]() { expect(loadCharactersFromUrl(std:("https://example.com/notfound.json")))->rejects->toThrow(std:("HTTP error 404: Not Found")); });
            }
            );
            test(std:("should handle network errors"), [=]() mutable
            {
                mockFetch->mockRejectedValue(std::make_shared<TypeError>(std:("Network error")));
                std::async([=]() { expect(loadCharactersFromUrl(std:("https://example.com/character.json")))->rejects->toThrow(std:("Failed to fetch character from URL")); });
            }
            );
            test(std:("should handle invalid JSON response"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 200}, 
                    object::pair{std:("json"), [=]() mutable
                    {
                        return Promise->reject(std::make_shared<SyntaxError>(std:("Unexpected token")));
                    }
                    }
                };
                mockFetch->mockResolvedValue(mockResponse);
                std::async([=]() { expect(loadCharactersFromUrl(std:("https://example.com/invalid.json")))->rejects->toThrow(std:("Invalid JSON response from URL")); });
            }
            );
            test(std:("should handle invalid character data from URL"), [=]() mutable
            {
                shared invalidCharacter = object{
                    object::pair{std:("name"), string_empty}, 
                    object::pair{std:("bio"), std:("Invalid")}
                };
                auto mockResponse = object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 200}, 
                    object::pair{std:("json"), [=]() mutable
                    {
                        return Promise->resolve(invalidCharacter);
                    }
                    }
                };
                mockFetch->mockResolvedValue(mockResponse);
                std::async([=]() { expect(loadCharactersFromUrl(std:("https://example.com/invalid-character.json")))->rejects->toThrow(std:("Invalid character data from URL")); });
            }
            );
            test(std:("should handle validation errors for array of characters"), [=]() mutable
            {
                shared characters = array<any>{ validCharacter, object{
                    object::pair{std:("name"), string_empty}, 
                    object::pair{std:("bio"), std:("Invalid")}
                } };
                auto mockResponse = object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 200}, 
                    object::pair{std:("json"), [=]() mutable
                    {
                        return Promise->resolve(characters);
                    }
                    }
                };
                mockFetch->mockResolvedValue(mockResponse);
                std::async([=]() { expect(loadCharactersFromUrl(std:("https://example.com/mixed-characters.json")))->rejects->toThrow(std:("Character validation failed")); });
            }
            );
        }
        );
        describe(std:("Error handling and logging"), [=]() mutable
        {
            test(std:("should provide detailed validation error messages"), [=]() mutable
            {
                shared characterMissingName = JSON->stringify(object{
                    object::pair{std:("bio"), std:("No name")}
                });
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    return characterMissingName;
                }
                );
                try
                {
                    std::async([=]() { loadCharacter(std:("/path/to/no-name.json")); });
                    expect->fail(std:("Should have thrown an error"));
                }
                catch (const any& error)
                {
                    expect(error["message"])->toContain(std:("Character validation failed"));
                    expect(error["message"])->toContain(std:("name"));
                }
            }
            );
        }
        );
    }
    );
}

MAIN
