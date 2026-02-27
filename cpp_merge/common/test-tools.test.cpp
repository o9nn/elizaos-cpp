#include "test-tools.test.h"

void Main(void)
{
    describe(std:("Command Parsing"), [=]() mutable
    {
        describe(std:("Signature formats"), [=]() mutable
        {
            it(std:("should handle default format with no signature"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std:("name"), std:("test_cmd")}, 
                    object::pair{std:("docstring"), std:("A test command")}, 
                    object::pair{std:("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                        object::pair{std:("name"), std:("arg1")}, 
                        object::pair{std:("type"), std:("string")}, 
                        object::pair{std:("description"), std:("First argument")}, 
                        object::pair{std:("required"), true}
                    }), std::make_shared<Argument>(object{
                        object::pair{std:("name"), std:("arg2")}, 
                        object::pair{std:("type"), std:("integer")}, 
                        object::pair{std:("description"), std:("Second argument")}, 
                        object::pair{std:("required"), false}
                    }) }}
                });
                expect(command->get_invokeFormat())->toBe(std:("test_cmd {arg1} {arg2} "));
            }
            );
            it(std:("should handle angle brackets"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std:("name"), std:("goto")}, 
                    object::pair{std:("signature"), std:("goto <line_number>")}, 
                    object::pair{std:("docstring"), std:("moves the window to show line_number")}, 
                    object::pair{std:("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                        object::pair{std:("name"), std:("line_number")}, 
                        object::pair{std:("type"), std:("integer")}, 
                        object::pair{std:("description"), std:("line number")}, 
                        object::pair{std:("required"), true}
                    }) }}
                });
                expect(command->get_invokeFormat())->toBe(std:("goto {line_number}"));
            }
            );
            it(std:("should handle optional brackets"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std:("name"), std:("open")}, 
                    object::pair{std:("signature"), std:("open "<path>" [<line_number>]")}, 
                    object::pair{std:("docstring"), std:("opens file at path")}, 
                    object::pair{std:("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                        object::pair{std:("name"), std:("path")}, 
                        object::pair{std:("type"), std:("string")}, 
                        object::pair{std:("description"), std:("file path")}, 
                        object::pair{std:("required"), true}
                    }), std::make_shared<Argument>(object{
                        object::pair{std:("name"), std:("line_number")}, 
                        object::pair{std:("type"), std:("integer")}, 
                        object::pair{std:("description"), std:("line number")}, 
                        object::pair{std:("required"), false}
                    }) }}
                });
                expect(command->get_invokeFormat())->toBe(std:("open "{path}" {line_number}"));
            }
            );
            it(std:("should handle flag-style arguments"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std:("name"), std:("grep")}, 
                    object::pair{std:("signature"), std:("grep --pattern <pattern> --file <file>")}, 
                    object::pair{std:("docstring"), std:("search for pattern in file")}, 
                    object::pair{std:("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                        object::pair{std:("name"), std:("pattern")}, 
                        object::pair{std:("type"), std:("string")}, 
                        object::pair{std:("description"), std:("search pattern")}, 
                        object::pair{std:("required"), true}
                    }), std::make_shared<Argument>(object{
                        object::pair{std:("name"), std:("file")}, 
                        object::pair{std:("type"), std:("string")}, 
                        object::pair{std:("description"), std:("file to search")}, 
                        object::pair{std:("required"), true}
                    }) }}
                });
                expect(command->get_invokeFormat())->toBe(std:("grep --pattern {pattern} --file {file}"));
            }
            );
            it(std:("should handle no arguments"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std:("name"), std:("scroll_up")}, 
                    object::pair{std:("signature"), std:("scroll_up")}, 
                    object::pair{std:("docstring"), std:("scrolls up")}, 
                    object::pair{std:("arguments"), array<any>()}
                });
                expect(command->get_invokeFormat())->toBe(std:("scroll_up"));
            }
            );
        }
        );
        describe(std:("Argument validation"), [=]() mutable
        {
            it(std:("should require required arguments before optional ones"), [=]() mutable
            {
                expect([=]() mutable
                {
                    std::make_shared<Command>(object{
                        object::pair{std:("name"), std:("bad_order")}, 
                        object::pair{std:("docstring"), std:("bad argument order")}, 
                        object::pair{std:("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                            object::pair{std:("name"), std:("optional")}, 
                            object::pair{std:("type"), std:("string")}, 
                            object::pair{std:("description"), std:("optional")}, 
                            object::pair{std:("required"), false}
                        }), std::make_shared<Argument>(object{
                            object::pair{std:("name"), std:("required")}, 
                            object::pair{std:("type"), std:("string")}, 
                            object::pair{std:("description"), std:("required")}, 
                            object::pair{std:("required"), true}
                        }) }}
                    });
                }
                )->toThrow((new RegExp(std:("Required argument.*cannot come after optional argument"))));
            }
            );
            it(std:("should not allow duplicate argument names"), [=]() mutable
            {
                expect([=]() mutable
                {
                    std::make_shared<Command>(object{
                        object::pair{std:("name"), std:("duplicate")}, 
                        object::pair{std:("docstring"), std:("duplicate args")}, 
                        object::pair{std:("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                            object::pair{std:("name"), std:("arg1")}, 
                            object::pair{std:("type"), std:("string")}, 
                            object::pair{std:("description"), std:("first")}, 
                            object::pair{std:("required"), true}
                        }), std::make_shared<Argument>(object{
                            object::pair{std:("name"), std:("arg1")}, 
                            object::pair{std:("type"), std:("string")}, 
                            object::pair{std:("description"), std:("duplicate")}, 
                            object::pair{std:("required"), true}
                        }) }}
                    });
                }
                )->toThrow((new RegExp(std:("Duplicate argument nam"))));
            }
            );
        }
        );
        describe(std:("Argument name patterns"), [=]() mutable
        {
            it(std:("should accept valid argument names"), [=]() mutable
            {
                auto validNames = array<string>{ std:("a"), std:("x"), std:("n"), std:("simple"), std:("with_underscore"), std:("with-dash"), std:("with123numbers"), std:("_starts_with_underscore") };
                validNames->forEach([=](auto name) mutable
                {
                    auto command = std::make_shared<Command>(object{
                        object::pair{std:("name"), std:("test")}, 
                        object::pair{std:("docstring"), std:("test")}, 
                        object::pair{std:("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                            object::pair{std:("name"), std:("name")}, 
                            object::pair{std:("type"), std:("string")}, 
                            object::pair{std:("description"), std:("test")}, 
                            object::pair{std:("required"), true}
                        }) }}
                    });
                    expect(const_(command->arguments)[0]->name)->toBe(name);
                }
                );
            }
            );
            it(std:("should reject invalid argument names"), [=]() mutable
            {
                auto invalidNames = array<string>{ std:("123starts_with_number"), string_empty };
                invalidNames->forEach([=](auto name) mutable
                {
                    expect([=]() mutable
                    {
                        std::make_shared<Command>(object{
                            object::pair{std:("name"), std:("test")}, 
                            object::pair{std:("docstring"), std:("test")}, 
                            object::pair{std:("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                                object::pair{std:("name"), std:("name")}, 
                                object::pair{std:("type"), std:("string")}, 
                                object::pair{std:("description"), std:("test")}, 
                                object::pair{std:("required"), true}
                            }) }}
                        });
                    }
                    )->toThrow((new RegExp(std:("Invalid argument nam"))));
                }
                );
            }
            );
        }
        );
        describe(std:("Signature and argument consistency"), [=]() mutable
        {
            it(std:("should require all arguments to appear in signature"), [=]() mutable
            {
                expect([=]() mutable
                {
                    std::make_shared<Command>(object{
                        object::pair{std:("name"), std:("missing_arg")}, 
                        object::pair{std:("signature"), std:("missing_arg <existing_arg>")}, 
                        object::pair{std:("docstring"), std:("missing argument in signature")}, 
                        object::pair{std:("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                            object::pair{std:("name"), std:("existing_arg")}, 
                            object::pair{std:("type"), std:("string")}, 
                            object::pair{std:("description"), std:("exists")}, 
                            object::pair{std:("required"), true}
                        }), std::make_shared<Argument>(object{
                            object::pair{std:("name"), std:("missing_arg")}, 
                            object::pair{std:("type"), std:("string")}, 
                            object::pair{std:("description"), std:("not in signature")}, 
                            object::pair{std:("required"), true}
                        }) }}
                    });
                }
                )->toThrow((new RegExp(std:("Missing argument.*in signatur"))));
            }
            );
            it(std:("should reject extra arguments in signature"), [=]() mutable
            {
                expect([=]() mutable
                {
                    std::make_shared<Command>(object{
                        object::pair{std:("name"), std:("extra_arg")}, 
                        object::pair{std:("signature"), std:("extra_arg <arg1> <extra>")}, 
                        object::pair{std:("docstring"), std:("extra argument in signature")}, 
                        object::pair{std:("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                            object::pair{std:("name"), std:("arg1")}, 
                            object::pair{std:("type"), std:("string")}, 
                            object::pair{std:("description"), std:("exists")}, 
                            object::pair{std:("required"), true}
                        }) }}
                    });
                }
                )->toThrow((new RegExp(std:("Argument names.*do not matc"))));
            }
            );
        }
        );
        describe(std:("Function calling tool generation"), [=]() mutable
        {
            it(std:("should generate OpenAI function calling tool"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std:("name"), std:("test_function")}, 
                    object::pair{std:("docstring"), std:("A test function for OpenAI")}, 
                    object::pair{std:("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                        object::pair{std:("name"), std:("required_arg")}, 
                        object::pair{std:("type"), std:("string")}, 
                        object::pair{std:("description"), std:("Required string argument")}, 
                        object::pair{std:("required"), true}
                    }), std::make_shared<Argument>(object{
                        object::pair{std:("name"), std:("enum_arg")}, 
                        object::pair{std:("type"), std:("string")}, 
                        object::pair{std:("description"), std:("Enum argument")}, 
                        object::pair{std:("required"), true}, 
                        object::pair{std:("enum"), array<string>{ std:("option1"), std:("option2") }}
                    }), std::make_shared<Argument>(object{
                        object::pair{std:("name"), std:("optional_arg")}, 
                        object::pair{std:("type"), std:("integer")}, 
                        object::pair{std:("description"), std:("Optional integer argument")}, 
                        object::pair{std:("required"), false}
                    }) }}
                });
                auto tool = command->getFunctionCallingTool();
                expect(tool->type)->toBe(std:("function"));
                expect(tool->function["name"])->toBe(std:("test_function"));
                expect(tool->function["description"])->toBe(std:("A test function for OpenAI"));
                auto properties = tool->function["parameters"]["properties"];
                expect(properties["required_arg"]["type"])->toBe(std:("string"));
                expect(properties["optional_arg"]["type"])->toBe(std:("integer"));
                expect(properties["enum_arg"]["enum"])->toEqual(array<string>{ std:("option1"), std:("option2") });
                auto required = tool->function["parameters"]["required"];
                expect(required)->toContain(std:("required_arg"));
                expect(required)->toContain(std:("enum_arg"));
                expect(required)->not->toContain(std:("optional_arg"));
            }
            );
        }
        );
        describe(std:("Multi-line commands"), [=]() mutable
        {
            it(std:("should handle commands with end markers"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std:("name"), std:("edit")}, 
                    object::pair{std:("signature"), std:("edit <filename>")}, 
                    object::pair{std:("docstring"), std:("Edit a file with multi-line content")}, 
                    object::pair{std:("endName"), std:("EOF")}, 
                    object::pair{std:("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                        object::pair{std:("name"), std:("filename")}, 
                        object::pair{std:("type"), std:("string")}, 
                        object::pair{std:("description"), std:("file to edit")}, 
                        object::pair{std:("required"), true}
                    }) }}
                });
                expect(command->get_invokeFormat())->toBe(std:("edit {filename}"));
                expect(command->endName)->toBe(std:("EOF"));
            }
            );
        }
        );
        describe(std:("Custom argument format"), [=]() mutable
        {
            it(std:("should handle custom argument formatting"), [=]() mutable
            {
                auto command = std::make_shared<Command>(object{
                    object::pair{std:("name"), std:("custom_format")}, 
                    object::pair{std:("docstring"), std:("Test custom argument formatting")}, 
                    object::pair{std:("arguments"), array<std::shared_ptr<ArgumentImpl>>{ std::make_shared<Argument>(object{
                        object::pair{std:("name"), std:("arg1")}, 
                        object::pair{std:("type"), std:("string")}, 
                        object::pair{std:("description"), std:("Custom formatted argument")}, 
                        object::pair{std:("required"), true}, 
                        object::pair{std:("argumentFormat"), std:("--{value}")}
                    }) }}
                });
                expect(const_(command->arguments)[0]->argumentFormat)->toBe(std:("--{value}"));
                expect(command->get_invokeFormat())->toBe(std:("custom_format {arg1} "));
            }
            );
        }
        );
    }
    );
}

MAIN
