#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/unit/utils/loader.test.h"

string TEST_MULTI_CHARACTER_URL = std::string("https://raw.githubusercontent.com/elizaOS/eliza/refs/heads/develop/packages/cli/tests/test-characters/multi-chars.json");
any mockFs = as<any>(fs);

void Main(void)
{
    mock->module(std::string("node:fs"), [=]() mutable
    {
        return (object{
            object::pair{std::string("existsSync"), mock([=]() mutable
            {
                return true;
            }
            )}, 
            object::pair{std::string("readFileSync"), mock([=]() mutable
            {
                return std::string("{}");
            }
            )}, 
            object::pair{std::string("statSync"), mock([=]() mutable
            {
                return (object{
                    object::pair{std::string("isDirectory"), [=]() mutable
                    {
                        return true;
                    }
                    }
                });
            }
            )}, 
            object::pair{std::string("writeFileSync"), mock()}, 
            object::pair{std::string("promises"), object{
                object::pair{std::string("readFile"), mock([=]() mutable
                {
                    return std::string("{}");
                }
                )}, 
                object::pair{std::string("writeFile"), mock()}, 
                object::pair{std::string("mkdir"), mock()}
            }}
        });
    }
    );
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std::string("logger"), object{
                object::pair{std::string("error"), mock()}, 
                object::pair{std::string("warn"), mock()}, 
                object::pair{std::string("info"), mock()}, 
                object::pair{std::string("debug"), mock()}
            }}
        });
    }
    );
    describe(std::string("Character Loader"), [=]() mutable
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
        describe(std::string("tryLoadFile"), [=]() mutable
        {
            test(std::string("should load file content successfully"), [=]() mutable
            {
                shared fileContent = std::string("test file content");
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    return fileContent;
                }
                );
                auto result = tryLoadFile(std::string("/path/to/file.json"));
                expect(result)->toBe(fileContent);
            }
            );
            test(std::string("should throw error when file loading fails"), [=]() mutable
            {
                shared error = std::make_shared<Error>(std::string("File not found"));
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    throw any(error);
                }
                );
                expect([=]() mutable
                {
                    return tryLoadFile(std::string("/nonexistent/file.json"));
                }
                )->toThrow(std::string("Error loading file /nonexistent/file.json: Error: File not found"));
            }
            );
        }
        );
        describe(std::string("loadCharacter"), [=]() mutable
        {
            test(std::string("should load and validate character from file"), [=]() mutable
            {
                shared characterJson = JSON->stringify(validCharacter);
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    return characterJson;
                }
                );
                auto result = std::async([=]() { loadCharacter(std::string("/path/to/character.json")); });
                expect(result)->toEqual(expect->objectContaining(validCharacter));
            }
            );
            test(std::string("should throw error for non-existent file"), [=]() mutable
            {
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    throw any(std::make_shared<Error>(std::string("ENOENT: no such file or directory")));
                }
                );
                std::async([=]() { expect(loadCharacter(std::string("/nonexistent/character.json")))->rejects->toThrow(std::string("Error loading file /nonexistent/character.json")); });
            }
            );
            test(std::string("should throw error for invalid JSON"), [=]() mutable
            {
                shared invalidJson = std::string("{ "name": "Test", "bio": "Test" ");
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    return invalidJson;
                }
                );
                std::async([=]() { expect(loadCharacter(std::string("/path/to/invalid.json")))->rejects->toThrow(std::string("Invalid JSON")); });
            }
            );
            test(std::string("should throw error for invalid character data"), [=]() mutable
            {
                shared invalidCharacter = JSON->stringify(object{
                    object::pair{std::string("name"), string_empty}, 
                    object::pair{std::string("bio"), std::string("Invalid")}
                });
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    return invalidCharacter;
                }
                );
                std::async([=]() { expect(loadCharacter(std::string("/path/to/invalid-character.json")))->rejects->toThrow(std::string("Character validation failed")); });
            }
            );
            test(std::string("should throw error for missing required fields"), [=]() mutable
            {
                shared incompleteCharacter = JSON->stringify(object{
                    object::pair{std::string("name"), std::string("Test")}
                });
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    return incompleteCharacter;
                }
                );
                std::async([=]() { expect(loadCharacter(std::string("/path/to/incomplete.json")))->rejects->toThrow(std::string("Character validation failed")); });
            }
            );
        }
        );
        describe(std::string("jsonToCharacter"), [=]() mutable
        {
            test(std::string("should validate and return character"), [=]() mutable
            {
                auto result = std::async([=]() { jsonToCharacter(validCharacter); });
                expect(result)->toEqual(expect->objectContaining(validCharacter));
            }
            );
            test(std::string("should throw error for invalid character data"), [=]() mutable
            {
                auto invalidCharacter = object{
                    object::pair{std::string("name"), string_empty}, 
                    object::pair{std::string("bio"), std::string("Invalid")}
                };
                std::async([=]() { expect(jsonToCharacter(invalidCharacter))->rejects->toThrow(std::string("Character validation failed")); });
            }
            );
            test(std::string("should handle environment-based settings"), [=]() mutable
            {
                auto originalEnv = process->env;
                process->env = utils::assign(object{
                    , 
                    object::pair{std::string("CHARACTER.TEST_CHARACTER.API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("CHARACTER.TEST_CHARACTER.DEBUG"), std::string("true")}
                }, originalEnv);
                auto characterWithName = utils::assign(object{
                    , 
                    object::pair{std::string("name"), std::string("Test Character")}
                }, validCharacter);
                auto result = std::async([=]() { jsonToCharacter(characterWithName); });
                expect(result->secrets)->toEqual(expect->objectContaining(object{
                    object::pair{std::string("API_KEY"), std::string("test-key")}, 
                    object::pair{std::string("DEBUG"), std::string("true")}
                }));
                process->env = originalEnv;
            }
            );
            test(std::string("should handle character without environment settings"), [=]() mutable
            {
                auto originalEnv = process->env;
                process->env = object{};
                auto result = std::async([=]() { jsonToCharacter(validCharacter); });
                expect(result)->toEqual(expect->objectContaining(validCharacter));
                process->env = originalEnv;
            }
            );
            test(std::string("should validate character with complex data structures"), [=]() mutable
            {
                auto complexCharacter = object{
                    object::pair{std::string("name"), std::string("Complex Character")}, 
                    object::pair{std::string("bio"), array<string>{ std::string("Multi-line"), std::string("bio description") }}, 
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
                    object::pair{std::string("knowledge"), array<string>{ std::string("simple/path.txt"), object{
                        object::pair{std::string("path"), std::string("complex/path.txt")}, 
                        object::pair{std::string("shared"), true}
                    }, object{
                        object::pair{std::string("directory"), std::string("knowledge/dir")}, 
                        object::pair{std::string("shared"), false}
                    } }}, 
                    object::pair{std::string("settings"), object{
                        object::pair{std::string("temperature"), 0.7}, 
                        object::pair{std::string("nested"), object{
                            object::pair{std::string("deeply"), object{
                                object::pair{std::string("value"), std::string("test")}
                            }}
                        }}
                    }}, 
                    object::pair{std::string("style"), object{
                        object::pair{std::string("all"), array<string>{ std::string("casual") }}, 
                        object::pair{std::string("chat"), array<string>{ std::string("responsive") }}, 
                        object::pair{std::string("post"), array<string>{ std::string("engaging") }}
                    }}
                };
                auto result = std::async([=]() { jsonToCharacter(complexCharacter); });
                expect(result)->toEqual(expect->objectContaining(complexCharacter));
            }
            );
        }
        );
        describe(std::string("loadCharactersFromUrl"), [=]() mutable
        {
            shared mockFetch = mock();
            global->fetch = mockFetch;
            beforeEach([=]() mutable
            {
                mockFetch;
            }
            );
            test(std::string("should load single character from URL"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 200}, 
                    object::pair{std::string("json"), [=]() mutable
                    {
                        return Promise->resolve(validCharacter);
                    }
                    }
                };
                mockFetch->mockResolvedValue(mockResponse);
                auto result = std::async([=]() { loadCharactersFromUrl(std::string("https://example.com/character.json")); });
                expect(result)->toHaveLength(1);
                expect(const_(result)[0])->toEqual(expect->objectContaining(validCharacter));
            }
            );
            test(std::string("should load multiple characters from URL"), [=]() mutable
            {
                shared characters = array<any>{ validCharacter, utils::assign(object{
                    , 
                    object::pair{std::string("name"), std::string("Second Character")}
                }, validCharacter) };
                auto mockResponse = object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 200}, 
                    object::pair{std::string("json"), [=]() mutable
                    {
                        return Promise->resolve(characters);
                    }
                    }
                };
                mockFetch->mockResolvedValue(mockResponse);
                auto result = std::async([=]() { loadCharactersFromUrl(TEST_MULTI_CHARACTER_URL); });
                expect(result)->toHaveLength(2);
                expect(const_(result)[0])->toEqual(expect->objectContaining(validCharacter));
                expect(const_(result)[1]->name)->toBe(std::string("Second Character"));
            }
            );
            test(std::string("should handle HTTP errors"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("ok"), false}, 
                    object::pair{std::string("status"), 404}, 
                    object::pair{std::string("statusText"), std::string("Not Found")}
                };
                mockFetch->mockResolvedValue(mockResponse);
                std::async([=]() { expect(loadCharactersFromUrl(std::string("https://example.com/notfound.json")))->rejects->toThrow(std::string("HTTP error 404: Not Found")); });
            }
            );
            test(std::string("should handle network errors"), [=]() mutable
            {
                mockFetch->mockRejectedValue(std::make_shared<TypeError>(std::string("Network error")));
                std::async([=]() { expect(loadCharactersFromUrl(std::string("https://example.com/character.json")))->rejects->toThrow(std::string("Failed to fetch character from URL")); });
            }
            );
            test(std::string("should handle invalid JSON response"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 200}, 
                    object::pair{std::string("json"), [=]() mutable
                    {
                        return Promise->reject(std::make_shared<SyntaxError>(std::string("Unexpected token")));
                    }
                    }
                };
                mockFetch->mockResolvedValue(mockResponse);
                std::async([=]() { expect(loadCharactersFromUrl(std::string("https://example.com/invalid.json")))->rejects->toThrow(std::string("Invalid JSON response from URL")); });
            }
            );
            test(std::string("should handle invalid character data from URL"), [=]() mutable
            {
                shared invalidCharacter = object{
                    object::pair{std::string("name"), string_empty}, 
                    object::pair{std::string("bio"), std::string("Invalid")}
                };
                auto mockResponse = object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 200}, 
                    object::pair{std::string("json"), [=]() mutable
                    {
                        return Promise->resolve(invalidCharacter);
                    }
                    }
                };
                mockFetch->mockResolvedValue(mockResponse);
                std::async([=]() { expect(loadCharactersFromUrl(std::string("https://example.com/invalid-character.json")))->rejects->toThrow(std::string("Invalid character data from URL")); });
            }
            );
            test(std::string("should handle validation errors for array of characters"), [=]() mutable
            {
                shared characters = array<any>{ validCharacter, object{
                    object::pair{std::string("name"), string_empty}, 
                    object::pair{std::string("bio"), std::string("Invalid")}
                } };
                auto mockResponse = object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 200}, 
                    object::pair{std::string("json"), [=]() mutable
                    {
                        return Promise->resolve(characters);
                    }
                    }
                };
                mockFetch->mockResolvedValue(mockResponse);
                std::async([=]() { expect(loadCharactersFromUrl(std::string("https://example.com/mixed-characters.json")))->rejects->toThrow(std::string("Character validation failed")); });
            }
            );
        }
        );
        describe(std::string("Error handling and logging"), [=]() mutable
        {
            test(std::string("should provide detailed validation error messages"), [=]() mutable
            {
                shared characterMissingName = JSON->stringify(object{
                    object::pair{std::string("bio"), std::string("No name")}
                });
                mockFs["readFileSync"]["mockImplementation"]([=]() mutable
                {
                    return characterMissingName;
                }
                );
                try
                {
                    std::async([=]() { loadCharacter(std::string("/path/to/no-name.json")); });
                    expect->fail(std::string("Should have thrown an error"));
                }
                catch (const any& error)
                {
                    expect(error["message"])->toContain(std::string("Character validation failed"));
                    expect(error["message"])->toContain(std::string("name"));
                }
            }
            );
        }
        );
    }
    );
}

MAIN
