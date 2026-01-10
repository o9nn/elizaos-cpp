#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-parsing.test.h"

void Main(void)
{
    describe(std::string("Parsing"), [=]() mutable
    {
        describe(std::string("ActionParser"), [=]() mutable
        {
            it(std::string("should parse valid commands"), [=]() mutable
            {
                auto parser = std::make_shared<ActionParser>();
                auto command = object{
                    object::pair{std::string("name"), std::string("ls")}
                };
                auto [thought, action] = parser->parse(object{
                    object::pair{std::string("message"), std::string("ls -l")}
                }, array<std::shared_ptr<Command>>{ command });
                expect(thought)->toBe(string_empty);
                expect(action)->toBe(std::string("ls -l"));
            }
            );
            it(std::string("should throw FormatError for invalid commands"), [=]() mutable
            {
                shared parser = std::make_shared<ActionParser>();
                shared command = object{
                    object::pair{std::string("name"), std::string("ls")}
                };
                expect([=]() mutable
                {
                    parser->parse(object{
                        object::pair{std::string("message"), std::string("invalid command")}
                    }, array<std::shared_ptr<Command>>{ command });
                }
                )->toThrow(FormatError);
            }
            );
        }
        );
        describe(std::string("ThoughtActionParser"), [=]() mutable
        {
            it(std::string("should parse thought and action with code blocks"), [=]() mutable
            {
                auto parser = std::make_shared<ThoughtActionParser>();
                auto modelResponse = std::string("Let's look at the files in the current directory.\
```\
ls -l\
```");
                auto [thought, action] = parser->parse(object{
                    object::pair{std::string("message"), modelResponse}
                }, array<any>());
                expect(thought)->toBe(std::string("Let's look at the files in the current directory.\
"));
                expect(action)->toBe(std::string("ls -l\
"));
            }
            );
            it(std::string("should throw FormatError when no code block"), [=]() mutable
            {
                shared parser = std::make_shared<ThoughtActionParser>();
                expect([=]() mutable
                {
                    parser->parse(object{
                        object::pair{std::string("message"), std::string("No code block")}
                    }, array<any>());
                }
                )->toThrow(FormatError);
            }
            );
        }
        );
        describe(std::string("XMLThoughtActionParser"), [=]() mutable
        {
            it(std::string("should parse XML command tags"), [=]() mutable
            {
                auto parser = std::make_shared<XMLThoughtActionParser>();
                auto modelResponse = std::string("Let's look at the files in the current directory.\
<command>\
ls -l\
</command>");
                auto [thought, action] = parser->parse(object{
                    object::pair{std::string("message"), modelResponse}
                }, array<any>());
                expect(thought)->toBe(std::string("Let's look at the files in the current directory."));
                expect(action)->toBe(std::string("ls -l"));
            }
            );
            it(std::string("should throw FormatError when no command tags"), [=]() mutable
            {
                shared parser = std::make_shared<XMLThoughtActionParser>();
                expect([=]() mutable
                {
                    parser->parse(object{
                        object::pair{std::string("message"), std::string("No command tags")}
                    }, array<any>());
                }
                )->toThrow(FormatError);
            }
            );
        }
        );
        describe(std::string("EditFormat"), [=]() mutable
        {
            it(std::string("should parse edit format with code blocks"), [=]() mutable
            {
                auto parser = std::make_shared<EditFormat>();
                auto modelResponse = std::string("Let's replace the contents.\
```\
import os\
os.listdir()\
```");
                auto [thought, action] = parser->parse(object{
                    object::pair{std::string("message"), modelResponse}
                }, array<any>());
                expect(thought)->toBe(std::string("Let's replace the contents.\
"));
                expect(action)->toBe(std::string("import os\
os.listdir()\
"));
            }
            );
            it(std::string("should throw FormatError when no code block"), [=]() mutable
            {
                shared parser = std::make_shared<EditFormat>();
                expect([=]() mutable
                {
                    parser->parse(object{
                        object::pair{std::string("message"), std::string("No code block")}
                    }, array<any>());
                }
                )->toThrow(FormatError);
            }
            );
        }
        );
        describe(std::string("Identity"), [=]() mutable
        {
            it(std::string("should return input as is"), [=]() mutable
            {
                auto parser = std::make_shared<Identity>();
                auto modelResponse = std::string("Return as is");
                auto [thought, action] = parser->parse(object{
                    object::pair{std::string("message"), modelResponse}
                }, array<any>());
                expect(thought)->toBe(modelResponse);
                expect(action)->toBe(modelResponse);
            }
            );
        }
        );
        describe(std::string("JsonParser"), [=]() mutable
        {
            it(std::string("should parse valid JSON"), [=]() mutable
            {
                auto parser = std::make_shared<JsonParser>();
                auto modelResponse = std::string("{"thought": "List files", "command": {"name": "ls", "arguments": {"path": "."}}}");
                auto [thought, action] = parser->parse(object{
                    object::pair{std::string("message"), modelResponse}
                }, array<any>());
                expect(thought)->toBe(std::string("List files"));
                expect(action)->toBe(std::string("ls ."));
            }
            );
            it(std::string("should throw FormatError for invalid JSON"), [=]() mutable
            {
                shared parser = std::make_shared<JsonParser>();
                expect([=]() mutable
                {
                    parser->parse(object{
                        object::pair{std::string("message"), std::string("Not a JSON")}
                    }, array<any>());
                }
                )->toThrow(FormatError);
            }
            );
            it(std::string("should throw FormatError for missing keys"), [=]() mutable
            {
                shared parser = std::make_shared<JsonParser>();
                shared missingKeys = std::string("{"thought": "Missing command key"}");
                expect([=]() mutable
                {
                    parser->parse(object{
                        object::pair{std::string("message"), missingKeys}
                    }, array<any>());
                }
                )->toThrow(FormatError);
            }
            );
        }
        );
        describe(std::string("FunctionCallingParser"), [=]() mutable
        {
            it(std::string("should parse function calls successfully"), [=]() mutable
            {
                auto parser = std::make_shared<FunctionCallingParser>();
                auto command = object{
                    object::pair{std::string("name"), std::string("ls")}
                };
                auto modelResponse = object{
                    object::pair{std::string("message"), std::string("Let's list the files")}, 
                    object::pair{std::string("tool_calls"), array<object>{ object{
                        object::pair{std::string("function"), object{
                            object::pair{std::string("name"), std::string("ls")}, 
                            object::pair{std::string("arguments"), std::string("{}")}
                        }}
                    } }}
                };
                auto [thought, action] = parser->parse(modelResponse, array<std::shared_ptr<Command>>{ command });
                expect(thought)->toBe(std::string("Let's list the files"));
                expect(action)->toBe(std::string("ls"));
            }
            );
            it(std::string("should throw error for missing tool_calls"), [=]() mutable
            {
                shared parser = std::make_shared<FunctionCallingParser>();
                shared command = object{
                    object::pair{std::string("name"), std::string("ls")}
                };
                expect([=]() mutable
                {
                    parser->parse(object{
                        object::pair{std::string("message"), std::string("No tool calls")}
                    }, array<std::shared_ptr<Command>>{ command });
                }
                )->toThrow(FormatError);
            }
            );
            it(std::string("should throw error for multiple tool calls"), [=]() mutable
            {
                shared parser = std::make_shared<FunctionCallingParser>();
                shared command = object{
                    object::pair{std::string("name"), std::string("ls")}
                };
                shared multipleCalls = object{
                    object::pair{std::string("message"), std::string("Multiple calls")}, 
                    object::pair{std::string("tool_calls"), array<object>{ object{
                        object::pair{std::string("function"), object{
                            object::pair{std::string("name"), std::string("ls")}, 
                            object::pair{std::string("arguments"), std::string("{}")}
                        }}
                    }, object{
                        object::pair{std::string("function"), object{
                            object::pair{std::string("name"), std::string("cd")}, 
                            object::pair{std::string("arguments"), std::string("{}")}
                        }}
                    } }}
                };
                expect([=]() mutable
                {
                    parser->parse(multipleCalls, array<std::shared_ptr<Command>>{ command });
                }
                )->toThrow(FormatError);
            }
            );
            it(std::string("should throw error for invalid command"), [=]() mutable
            {
                shared parser = std::make_shared<FunctionCallingParser>();
                shared command = object{
                    object::pair{std::string("name"), std::string("ls")}
                };
                shared invalidCommand = object{
                    object::pair{std::string("message"), std::string("Invalid command")}, 
                    object::pair{std::string("tool_calls"), array<object>{ object{
                        object::pair{std::string("function"), object{
                            object::pair{std::string("name"), std::string("invalid")}, 
                            object::pair{std::string("arguments"), std::string("{}")}
                        }}
                    } }}
                };
                expect([=]() mutable
                {
                    parser->parse(invalidCommand, array<std::shared_ptr<Command>>{ command });
                }
                )->toThrow(FormatError);
            }
            );
            it(std::string("should throw error for invalid JSON arguments"), [=]() mutable
            {
                shared parser = std::make_shared<FunctionCallingParser>();
                shared command = object{
                    object::pair{std::string("name"), std::string("ls")}
                };
                shared invalidJson = object{
                    object::pair{std::string("message"), std::string("Invalid JSON")}, 
                    object::pair{std::string("tool_calls"), array<object>{ object{
                        object::pair{std::string("function"), object{
                            object::pair{std::string("name"), std::string("ls")}, 
                            object::pair{std::string("arguments"), std::string("invalid json")}
                        }}
                    } }}
                };
                expect([=]() mutable
                {
                    parser->parse(invalidJson, array<std::shared_ptr<Command>>{ command });
                }
                )->toThrow(FormatError);
            }
            );
        }
        );
        describe(std::string("FunctionCallingParser error messages"), [=]() mutable
        {
            it(std::string("should format error message correctly"), [=]() mutable
            {
                auto parser = std::make_shared<FunctionCallingParser>();
                auto error = std::make_shared<FunctionCallingFormatError>(std::string("test"), std::string("missing"));
                auto message = parser->formatErrorMessage(error);
                expect(message)->toContain(std::string("did not use any tool calls"));
            }
            );
        }
        );
    }
    );
}

MAIN
