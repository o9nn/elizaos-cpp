#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/unit/utils/helpers.test.h"

object originalConsoleLog = console->log;
any consoleSpy = mock([=]() mutable
{
}
);

void Main(void)
{
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std::string("logger"), object{
                object::pair{std::string("info"), mock()}, 
                object::pair{std::string("error"), mock()}
            }}
        });
    }
    );
    mock->module(std::string("yoctocolors"), [=]() mutable
    {
        return (object{
            object::pair{std::string("default"), object{
                object::pair{std::string("green"), mock([=](auto text) mutable
                {
                    return std::string("[green]") + text + std::string("[/green]");
                }
                )}, 
                object::pair{std::string("cyan"), mock([=](auto text) mutable
                {
                    return std::string("[cyan]") + text + std::string("[/cyan]");
                }
                )}
            }}
        });
    }
    );
    console->log = consoleSpy;
    describe(std::string("helpers"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
        }
        );
        describe(std::string("displayAgent"), [=]() mutable
        {
            it(std::string("should display basic agent info"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("username"), std::string("test_agent")}
                };
                displayAgent(agent);
            }
            );
            it(std::string("should generate username from name if not provided"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std::string("name"), std::string("Test Agent Name")}
                };
                displayAgent(agent);
            }
            );
            it(std::string("should display bio array"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("bio"), array<string>{ std::string("Bio line 1"), std::string("Bio line 2") }}
                };
                displayAgent(agent);
            }
            );
            it(std::string("should display bio string as array"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("bio"), as<any>(std::string("Single bio line"))}
                };
                displayAgent(agent);
            }
            );
            it(std::string("should display all array sections"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("adjectives"), array<string>{ std::string("smart"), std::string("funny") }}, 
                    object::pair{std::string("topics"), array<string>{ std::string("AI"), std::string("Tech") }}, 
                    object::pair{std::string("plugins"), array<string>{ std::string("plugin1"), std::string("plugin2") }}, 
                    object::pair{std::string("postExamples"), array<string>{ std::string("Example 1"), std::string("Example 2") }}
                };
                displayAgent(agent);
            }
            );
            it(std::string("should display style sections"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("style"), object{
                        object::pair{std::string("all"), array<string>{ std::string("General style 1"), std::string("General style 2") }}, 
                        object::pair{std::string("chat"), array<string>{ std::string("Chat style 1") }}, 
                        object::pair{std::string("post"), array<string>{ std::string("Post style 1") }}
                    }}
                };
                displayAgent(agent);
            }
            );
            it(std::string("should display message examples"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("messageExamples"), array<array<object>>{ array<object>{ object{
                        object::pair{std::string("name"), std::string("{{name1}}")}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("Hello")}
                        }}
                    }, object{
                        object::pair{std::string("name"), std::string("Agent")}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("Hi there")}
                        }}
                    } } }}
                };
                displayAgent(agent);
            }
            );
            it(std::string("should use custom title"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std::string("name"), std::string("Test Agent")}
                };
                displayAgent(agent, std::string("Custom Title"));
            }
            );
            it(std::string("should handle empty sections gracefully"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("bio"), array<any>()}, 
                    object::pair{std::string("topics"), undefined}, 
                    object::pair{std::string("adjectives"), array<any>()}
                };
                displayAgent(agent);
            }
            );
        }
        );
        describe(std::string("logHeader"), [=]() mutable
        {
            it(std::string("should log header with borders"), [=]() mutable
            {
                logHeader(std::string("Test Header"));
            }
            );
            it(std::string("should add padding around title"), [=]() mutable
            {
                logHeader(std::string("Short"));
            }
            );
            it(std::string("should create border matching title length"), [=]() mutable
            {
                logHeader(std::string("A Very Long Title That Should Have A Long Border"));
                auto greenCalls = colors->green->mock->calls;
                auto borderCalls = greenCalls->filter([=](auto call) mutable
                {
                    return const_(call)[0]->includes(std::string("─"));
                }
                );
                expect(borderCalls->length)->toBeGreaterThan(0);
            }
            );
            it(std::string("should add newline before header"), [=]() mutable
            {
                logHeader(std::string("Test"));
                auto calls = consoleSpy->mock->calls;
                expect(calls->some([=](auto call) mutable
                {
                    return const_(call)[0]->startsWith(std::string("\
"));
                }
                ))->toBe(true);
            }
            );
        }
        );
    }
    );
}

MAIN
