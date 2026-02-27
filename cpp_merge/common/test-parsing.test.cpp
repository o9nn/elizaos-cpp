#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-parsing.test.h"

void Main(void)
{
    describe(std:("Parsing"), [=]() mutable
    {
        describe(std:("ActionParser"), [=]() mutable
        {
            it(std:("should parse valid commands"), [=]() mutable
            {
                auto parser = std::make_shared<ActionParser>();
                auto command = object{
                    object::pair{std:("name"), std:("ls")}
                };
                auto [thought, action] = parser->parse(object{
                    object::pair{std:("message"), std:("ls -l")}
                }, array<std::shared_ptr<Command>>{ command });
                expect(thought)->toBe(string_empty);
                expect(action)->toBe(std:("ls -l"));
            }
            );
            it(std:("should throw FormatError for invalid commands"), [=]() mutable
            {
                shared parser = std::make_shared<ActionParser>();
                shared command = object{
                    object::pair{std:("name"), std:("ls")}
                };
                expect([=]() mutable
                {
                    parser->parse(object{
                        object::pair{std:("message"), std:("invalid command")}
                    }, array<std::shared_ptr<Command>>{ command });
                }
                )->toThrow(FormatError);
            }
            );
        }
        );
        describe(std:("ThoughtActionParser"), [=]() mutable
        {
            it(std:("should parse thought and action with code blocks"), [=]() mutable
            {
                auto parser = std::make_shared<ThoughtActionParser>();
                auto modelResponse = std:("Let's look at the files in the current directory.\
"""\
ls -l\
"""");
                auto [thought, action] = parser->parse(object{
                    object::pair{std:("message"), modelResponse}
                }, array<any>());
                expect(thought)->toBe(std:("Let's look at the files in the current directory.\
"));
                expect(action)->toBe(std:("ls -l\
"));
            }
            );
            it(std:("should throw FormatError when no code block"), [=]() mutable
            {
                shared parser = std::make_shared<ThoughtActionParser>();
                expect([=]() mutable
                {
                    parser->parse(object{
                        object::pair{std:("message"), std:("No code block")}
                    }, array<any>());
                }
                )->toThrow(FormatError);
            }
            );
        }
        );
        describe(std:("XMLThoughtActionParser"), [=]() mutable
        {
            it(std:("should parse XML command tags"), [=]() mutable
            {
                auto parser = std::make_shared<XMLThoughtActionParser>();
                auto modelResponse = std:("Let's look at the files in the current directory.\
<command>\
ls -l\
</command>");
                auto [thought, action] = parser->parse(object{
                    object::pair{std:("message"), modelResponse}
                }, array<any>());
                expect(thought)->toBe(std:("Let's look at the files in the current directory."));
                expect(action)->toBe(std:("ls -l"));
            }
            );
            it(std:("should throw FormatError when no command tags"), [=]() mutable
            {
                shared parser = std::make_shared<XMLThoughtActionParser>();
                expect([=]() mutable
                {
                    parser->parse(object{
                        object::pair{std:("message"), std:("No command tags")}
                    }, array<any>());
                }
                )->toThrow(FormatError);
            }
            );
        }
        );
        describe(std:("EditFormat"), [=]() mutable
        {
            it(std:("should parse edit format with code blocks"), [=]() mutable
            {
                auto parser = std::make_shared<EditFormat>();
                auto modelResponse = std:("Let's replace the contents.\
"""\
import os\
os.listdir()\
"""");
                auto [thought, action] = parser->parse(object{
                    object::pair{std:("message"), modelResponse}
                }, array<any>());
                expect(thought)->toBe(std:("Let's replace the contents.\
"));
                expect(action)->toBe(std:("import os\
os.listdir()\
"));
            }
            );
            it(std:("should throw FormatError when no code block"), [=]() mutable
            {
                shared parser = std::make_shared<EditFormat>();
                expect([=]() mutable
                {
                    parser->parse(object{
                        object::pair{std:("message"), std:("No code block")}
                    }, array<any>());
                }
                )->toThrow(FormatError);
            }
            );
        }
        );
        describe(std:("Identity"), [=]() mutable
        {
            it(std:("should return input as is"), [=]() mutable
            {
                auto parser = std::make_shared<Identity>();
                auto modelResponse = std:("Return as is");
                auto [thought, action] = parser->parse(object{
                    object::pair{std:("message"), modelResponse}
                }, array<any>());
                expect(thought)->toBe(modelResponse);
                expect(action)->toBe(modelResponse);
            }
            );
        }
        );
        describe(std:("JsonParser"), [=]() mutable
        {
            it(std:("should parse valid JSON"), [=]() mutable
            {
                auto parser = std::make_shared<JsonParser>();
                auto modelResponse = std:("{"thought": "List files", "command": {"name": "ls", "arguments": {"path": "."}}}");
                auto [thought, action] = parser->parse(object{
                    object::pair{std:("message"), modelResponse}
                }, array<any>());
                expect(thought)->toBe(std:("List files"));
                expect(action)->toBe(std:("ls ."));
            }
            );
            it(std:("should throw FormatError for invalid JSON"), [=]() mutable
            {
                shared parser = std::make_shared<JsonParser>();
                expect([=]() mutable
                {
                    parser->parse(object{
                        object::pair{std:("message"), std:("Not a JSON")}
                    }, array<any>());
                }
                )->toThrow(FormatError);
            }
            );
            it(std:("should throw FormatError for missing keys"), [=]() mutable
            {
                shared parser = std::make_shared<JsonParser>();
                shared missingKeys = std:("{"thought": "Missing command key"}");
                expect([=]() mutable
                {
                    parser->parse(object{
                        object::pair{std:("message"), missingKeys}
                    }, array<any>());
                }
                )->toThrow(FormatError);
            }
            );
        }
        );
        describe(std:("FunctionCallingParser"), [=]() mutable
        {
            it(std:("should parse function calls successfully"), [=]() mutable
            {
                auto parser = std::make_shared<FunctionCallingParser>();
                auto command = object{
                    object::pair{std:("name"), std:("ls")}
                };
                auto modelResponse = object{
                    object::pair{std:("message"), std:("Let's list the files")}, 
                    object::pair{std:("tool_calls"), array<object>{ object{
                        object::pair{std:("function"), object{
                            object::pair{std:("name"), std:("ls")}, 
                            object::pair{std:("arguments"), std:("{}")}
                        }}
                    } }}
                };
                auto [thought, action] = parser->parse(modelResponse, array<std::shared_ptr<Command>>{ command });
                expect(thought)->toBe(std:("Let's list the files"));
                expect(action)->toBe(std:("ls"));
            }
            );
            it(std:("should throw error for missing tool_calls"), [=]() mutable
            {
                shared parser = std::make_shared<FunctionCallingParser>();
                shared command = object{
                    object::pair{std:("name"), std:("ls")}
                };
                expect([=]() mutable
                {
                    parser->parse(object{
                        object::pair{std:("message"), std:("No tool calls")}
                    }, array<std::shared_ptr<Command>>{ command });
                }
                )->toThrow(FormatError);
            }
            );
            it(std:("should throw error for multiple tool calls"), [=]() mutable
            {
                shared parser = std::make_shared<FunctionCallingParser>();
                shared command = object{
                    object::pair{std:("name"), std:("ls")}
                };
                shared multipleCalls = object{
                    object::pair{std:("message"), std:("Multiple calls")}, 
                    object::pair{std:("tool_calls"), array<object>{ object{
                        object::pair{std:("function"), object{
                            object::pair{std:("name"), std:("ls")}, 
                            object::pair{std:("arguments"), std:("{}")}
                        }}
                    }, object{
                        object::pair{std:("function"), object{
                            object::pair{std:("name"), std:("cd")}, 
                            object::pair{std:("arguments"), std:("{}")}
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
            it(std:("should throw error for invalid command"), [=]() mutable
            {
                shared parser = std::make_shared<FunctionCallingParser>();
                shared command = object{
                    object::pair{std:("name"), std:("ls")}
                };
                shared invalidCommand = object{
                    object::pair{std:("message"), std:("Invalid command")}, 
                    object::pair{std:("tool_calls"), array<object>{ object{
                        object::pair{std:("function"), object{
                            object::pair{std:("name"), std:("invalid")}, 
                            object::pair{std:("arguments"), std:("{}")}
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
            it(std:("should throw error for invalid JSON arguments"), [=]() mutable
            {
                shared parser = std::make_shared<FunctionCallingParser>();
                shared command = object{
                    object::pair{std:("name"), std:("ls")}
                };
                shared invalidJson = object{
                    object::pair{std:("message"), std:("Invalid JSON")}, 
                    object::pair{std:("tool_calls"), array<object>{ object{
                        object::pair{std:("function"), object{
                            object::pair{std:("name"), std:("ls")}, 
                            object::pair{std:("arguments"), std:("invalid json")}
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
        describe(std:("FunctionCallingParser error messages"), [=]() mutable
        {
            it(std:("should format error message correctly"), [=]() mutable
            {
                auto parser = std::make_shared<FunctionCallingParser>();
                auto error = std::make_shared<FunctionCallingFormatError>(std:("test"), std:("missing"));
                auto message = parser->formatErrorMessage(error);
                expect(message)->toContain(std:("did not use any tool calls"));
            }
            );
        }
        );
    }
    );
}

MAIN
