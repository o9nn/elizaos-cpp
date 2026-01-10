#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-tools.test.h"

void Main(void)
{
    describe(std::string("Command Parsing"), [=]() mutable
    {
        describe(std::string("Signature formats"), [=]() mutable
        {
            it(std::string("should handle default format with no signature"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std::string("name"), std::string("test_cmd")}, 
                    object::pair{std::string("docstring"), std::string("A test command")}, 
                    object::pair{std::string("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                        object::pair{std::string("name"), std::string("arg1")}, 
                        object::pair{std::string("type"), std::string("string")}, 
                        object::pair{std::string("description"), std::string("First argument")}, 
                        object::pair{std::string("required"), true}
                    }), std::make_shared<Argument>(object{
                        object::pair{std::string("name"), std::string("arg2")}, 
                        object::pair{std::string("type"), std::string("integer")}, 
                        object::pair{std::string("description"), std::string("Second argument")}, 
                        object::pair{std::string("required"), false}
                    }) }}
                });
                expect(command->get_invokeFormat())->toBe(std::string("test_cmd {arg1} {arg2} "));
            }
            );
            it(std::string("should handle angle brackets"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std::string("name"), std::string("goto")}, 
                    object::pair{std::string("signature"), std::string("goto <line_number>")}, 
                    object::pair{std::string("docstring"), std::string("moves the window to show line_number")}, 
                    object::pair{std::string("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                        object::pair{std::string("name"), std::string("line_number")}, 
                        object::pair{std::string("type"), std::string("integer")}, 
                        object::pair{std::string("description"), std::string("line number")}, 
                        object::pair{std::string("required"), true}
                    }) }}
                });
                expect(command->get_invokeFormat())->toBe(std::string("goto {line_number}"));
            }
            );
            it(std::string("should handle optional brackets"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std::string("name"), std::string("open")}, 
                    object::pair{std::string("signature"), std::string("open "<path>" [<line_number>]")}, 
                    object::pair{std::string("docstring"), std::string("opens file at path")}, 
                    object::pair{std::string("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                        object::pair{std::string("name"), std::string("path")}, 
                        object::pair{std::string("type"), std::string("string")}, 
                        object::pair{std::string("description"), std::string("file path")}, 
                        object::pair{std::string("required"), true}
                    }), std::make_shared<Argument>(object{
                        object::pair{std::string("name"), std::string("line_number")}, 
                        object::pair{std::string("type"), std::string("integer")}, 
                        object::pair{std::string("description"), std::string("line number")}, 
                        object::pair{std::string("required"), false}
                    }) }}
                });
                expect(command->get_invokeFormat())->toBe(std::string("open "{path}" {line_number}"));
            }
            );
            it(std::string("should handle flag-style arguments"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std::string("name"), std::string("grep")}, 
                    object::pair{std::string("signature"), std::string("grep --pattern <pattern> --file <file>")}, 
                    object::pair{std::string("docstring"), std::string("search for pattern in file")}, 
                    object::pair{std::string("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                        object::pair{std::string("name"), std::string("pattern")}, 
                        object::pair{std::string("type"), std::string("string")}, 
                        object::pair{std::string("description"), std::string("search pattern")}, 
                        object::pair{std::string("required"), true}
                    }), std::make_shared<Argument>(object{
                        object::pair{std::string("name"), std::string("file")}, 
                        object::pair{std::string("type"), std::string("string")}, 
                        object::pair{std::string("description"), std::string("file to search")}, 
                        object::pair{std::string("required"), true}
                    }) }}
                });
                expect(command->get_invokeFormat())->toBe(std::string("grep --pattern {pattern} --file {file}"));
            }
            );
            it(std::string("should handle no arguments"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std::string("name"), std::string("scroll_up")}, 
                    object::pair{std::string("signature"), std::string("scroll_up")}, 
                    object::pair{std::string("docstring"), std::string("scrolls up")}, 
                    object::pair{std::string("arguments"), array<any>()}
                });
                expect(command->get_invokeFormat())->toBe(std::string("scroll_up"));
            }
            );
        }
        );
        describe(std::string("Argument validation"), [=]() mutable
        {
            it(std::string("should require required arguments before optional ones"), [=]() mutable
            {
                expect([=]() mutable
                {
                    std::make_shared<Command>(object{
                        object::pair{std::string("name"), std::string("bad_order")}, 
                        object::pair{std::string("docstring"), std::string("bad argument order")}, 
                        object::pair{std::string("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                            object::pair{std::string("name"), std::string("optional")}, 
                            object::pair{std::string("type"), std::string("string")}, 
                            object::pair{std::string("description"), std::string("optional")}, 
                            object::pair{std::string("required"), false}
                        }), std::make_shared<Argument>(object{
                            object::pair{std::string("name"), std::string("required")}, 
                            object::pair{std::string("type"), std::string("string")}, 
                            object::pair{std::string("description"), std::string("required")}, 
                            object::pair{std::string("required"), true}
                        }) }}
                    });
                }
                )->toThrow((new RegExp(std::string("Required argument.*cannot come after optional argument"))));
            }
            );
            it(std::string("should not allow duplicate argument names"), [=]() mutable
            {
                expect([=]() mutable
                {
                    std::make_shared<Command>(object{
                        object::pair{std::string("name"), std::string("duplicate")}, 
                        object::pair{std::string("docstring"), std::string("duplicate args")}, 
                        object::pair{std::string("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                            object::pair{std::string("name"), std::string("arg1")}, 
                            object::pair{std::string("type"), std::string("string")}, 
                            object::pair{std::string("description"), std::string("first")}, 
                            object::pair{std::string("required"), true}
                        }), std::make_shared<Argument>(object{
                            object::pair{std::string("name"), std::string("arg1")}, 
                            object::pair{std::string("type"), std::string("string")}, 
                            object::pair{std::string("description"), std::string("duplicate")}, 
                            object::pair{std::string("required"), true}
                        }) }}
                    });
                }
                )->toThrow((new RegExp(std::string("Duplicate argument nam"))));
            }
            );
        }
        );
        describe(std::string("Argument name patterns"), [=]() mutable
        {
            it(std::string("should accept valid argument names"), [=]() mutable
            {
                auto validNames = array<string>{ std::string("a"), std::string("x"), std::string("n"), std::string("simple"), std::string("with_underscore"), std::string("with-dash"), std::string("with123numbers"), std::string("_starts_with_underscore") };
                validNames->forEach([=](auto name) mutable
                {
                    auto command = std::make_shared<Command>(object{
                        object::pair{std::string("name"), std::string("test")}, 
                        object::pair{std::string("docstring"), std::string("test")}, 
                        object::pair{std::string("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                            object::pair{std::string("name"), std::string("name")}, 
                            object::pair{std::string("type"), std::string("string")}, 
                            object::pair{std::string("description"), std::string("test")}, 
                            object::pair{std::string("required"), true}
                        }) }}
                    });
                    expect(const_(command->arguments)[0]->name)->toBe(name);
                }
                );
            }
            );
            it(std::string("should reject invalid argument names"), [=]() mutable
            {
                auto invalidNames = array<string>{ std::string("123starts_with_number"), string_empty };
                invalidNames->forEach([=](auto name) mutable
                {
                    expect([=]() mutable
                    {
                        std::make_shared<Command>(object{
                            object::pair{std::string("name"), std::string("test")}, 
                            object::pair{std::string("docstring"), std::string("test")}, 
                            object::pair{std::string("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                                object::pair{std::string("name"), std::string("name")}, 
                                object::pair{std::string("type"), std::string("string")}, 
                                object::pair{std::string("description"), std::string("test")}, 
                                object::pair{std::string("required"), true}
                            }) }}
                        });
                    }
                    )->toThrow((new RegExp(std::string("Invalid argument nam"))));
                }
                );
            }
            );
        }
        );
        describe(std::string("Signature and argument consistency"), [=]() mutable
        {
            it(std::string("should require all arguments to appear in signature"), [=]() mutable
            {
                expect([=]() mutable
                {
                    std::make_shared<Command>(object{
                        object::pair{std::string("name"), std::string("missing_arg")}, 
                        object::pair{std::string("signature"), std::string("missing_arg <existing_arg>")}, 
                        object::pair{std::string("docstring"), std::string("missing argument in signature")}, 
                        object::pair{std::string("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                            object::pair{std::string("name"), std::string("existing_arg")}, 
                            object::pair{std::string("type"), std::string("string")}, 
                            object::pair{std::string("description"), std::string("exists")}, 
                            object::pair{std::string("required"), true}
                        }), std::make_shared<Argument>(object{
                            object::pair{std::string("name"), std::string("missing_arg")}, 
                            object::pair{std::string("type"), std::string("string")}, 
                            object::pair{std::string("description"), std::string("not in signature")}, 
                            object::pair{std::string("required"), true}
                        }) }}
                    });
                }
                )->toThrow((new RegExp(std::string("Missing argument.*in signatur"))));
            }
            );
            it(std::string("should reject extra arguments in signature"), [=]() mutable
            {
                expect([=]() mutable
                {
                    std::make_shared<Command>(object{
                        object::pair{std::string("name"), std::string("extra_arg")}, 
                        object::pair{std::string("signature"), std::string("extra_arg <arg1> <extra>")}, 
                        object::pair{std::string("docstring"), std::string("extra argument in signature")}, 
                        object::pair{std::string("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                            object::pair{std::string("name"), std::string("arg1")}, 
                            object::pair{std::string("type"), std::string("string")}, 
                            object::pair{std::string("description"), std::string("exists")}, 
                            object::pair{std::string("required"), true}
                        }) }}
                    });
                }
                )->toThrow((new RegExp(std::string("Argument names.*do not matc"))));
            }
            );
        }
        );
        describe(std::string("Function calling tool generation"), [=]() mutable
        {
            it(std::string("should generate OpenAI function calling tool"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std::string("name"), std::string("test_function")}, 
                    object::pair{std::string("docstring"), std::string("A test function for OpenAI")}, 
                    object::pair{std::string("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                        object::pair{std::string("name"), std::string("required_arg")}, 
                        object::pair{std::string("type"), std::string("string")}, 
                        object::pair{std::string("description"), std::string("Required string argument")}, 
                        object::pair{std::string("required"), true}
                    }), std::make_shared<Argument>(object{
                        object::pair{std::string("name"), std::string("enum_arg")}, 
                        object::pair{std::string("type"), std::string("string")}, 
                        object::pair{std::string("description"), std::string("Enum argument")}, 
                        object::pair{std::string("required"), true}, 
                        object::pair{std::string("enum"), array<string>{ std::string("option1"), std::string("option2") }}
                    }), std::make_shared<Argument>(object{
                        object::pair{std::string("name"), std::string("optional_arg")}, 
                        object::pair{std::string("type"), std::string("integer")}, 
                        object::pair{std::string("description"), std::string("Optional integer argument")}, 
                        object::pair{std::string("required"), false}
                    }) }}
                });
                auto tool = command->getFunctionCallingTool();
                expect(tool->type)->toBe(std::string("function"));
                expect(tool->function["name"])->toBe(std::string("test_function"));
                expect(tool->function["description"])->toBe(std::string("A test function for OpenAI"));
                auto properties = tool->function["parameters"]["properties"];
                expect(properties["required_arg"]["type"])->toBe(std::string("string"));
                expect(properties["optional_arg"]["type"])->toBe(std::string("integer"));
                expect(properties["enum_arg"]["enum"])->toEqual(array<string>{ std::string("option1"), std::string("option2") });
                auto required = tool->function["parameters"]["required"];
                expect(required)->toContain(std::string("required_arg"));
                expect(required)->toContain(std::string("enum_arg"));
                expect(required)->not->toContain(std::string("optional_arg"));
            }
            );
        }
        );
        describe(std::string("Multi-line commands"), [=]() mutable
        {
            it(std::string("should handle commands with end markers"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std::string("name"), std::string("edit")}, 
                    object::pair{std::string("signature"), std::string("edit <filename>")}, 
                    object::pair{std::string("docstring"), std::string("Edit a file with multi-line content")}, 
                    object::pair{std::string("endName"), std::string("EOF")}, 
                    object::pair{std::string("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                        object::pair{std::string("name"), std::string("filename")}, 
                        object::pair{std::string("type"), std::string("string")}, 
                        object::pair{std::string("description"), std::string("file to edit")}, 
                        object::pair{std::string("required"), true}
                    }) }}
                });
                expect(command->get_invokeFormat())->toBe(std::string("edit {filename}"));
                expect(command->endName)->toBe(std::string("EOF"));
            }
            );
        }
        );
        describe(std::string("Custom argument format"), [=]() mutable
        {
            it(std::string("should handle custom argument formatting"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std::string("name"), std::string("custom_format")}, 
                    object::pair{std::string("docstring"), std::string("Test custom argument formatting")}, 
                    object::pair{std::string("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                        object::pair{std::string("name"), std::string("arg1")}, 
                        object::pair{std::string("type"), std::string("string")}, 
                        object::pair{std::string("description"), std::string("Custom formatted argument")}, 
                        object::pair{std::string("required"), true}, 
                        object::pair{std::string("argumentFormat"), std::string("--{value}")}
                    }) }}
                });
                expect(const_(command->arguments)[0]->argumentFormat)->toBe(std::string("--{value}"));
                expect(command->get_invokeFormat())->toBe(std::string("custom_format {arg1} "));
            }
            );
        }
        );
    }
    );
}

MAIN
