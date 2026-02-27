#include "helpers.test.h"

object originalConsoleLog = console->log;
any consoleSpy = mock([=]() mutable
{
}
);

void Main(void)
{
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std:("logger"), object{
                object::pair{std:("info"), mock()}, 
                object::pair{std:("error"), mock()}
            }}
        });
    }
    );
    mock->module(std:("yoctocolors"), [=]() mutable
    {
        return (object{
            object::pair{std:("default"), object{
                object::pair{std:("green"), mock([=](auto text) mutable
                {
                    return std:("[green]") + text + std:("[/green]");
                }
                )}, 
                object::pair{std:("cyan"), mock([=](auto text) mutable
                {
                    return std:("[cyan]") + text + std:("[/cyan]");
                }
                )}
            }}
        });
    }
    );
    console->log = consoleSpy;
    describe(std:("helpers"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
        }
        );
        describe(std:("displayAgent"), [=]() mutable
        {
            it(std:("should display basic agent info"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("username"), std:("test_agent")}
                };
                displayAgent(agent);
            }
            );
            it(std:("should generate username from name if not provided"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std:("name"), std:("Test Agent Name")}
                };
                displayAgent(agent);
            }
            );
            it(std:("should display bio array"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("bio"), array<string>{ std:("Bio line 1"), std:("Bio line 2") }}
                };
                displayAgent(agent);
            }
            );
            it(std:("should display bio string as array"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("bio"), as<any>(std:("Single bio line"))}
                };
                displayAgent(agent);
            }
            );
            it(std:("should display all array sections"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("adjectives"), array<string>{ std:("smart"), std:("funny") }}, 
                    object::pair{std:("topics"), array<string>{ std:("AI"), std:("Tech") }}, 
                    object::pair{std:("plugins"), array<string>{ std:("plugin1"), std:("plugin2") }}, 
                    object::pair{std:("postExamples"), array<string>{ std:("Example 1"), std:("Example 2") }}
                };
                displayAgent(agent);
            }
            );
            it(std:("should display style sections"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("style"), object{
                        object::pair{std:("all"), array<string>{ std:("General style 1"), std:("General style 2") }}, 
                        object::pair{std:("chat"), array<string>{ std:("Chat style 1") }}, 
                        object::pair{std:("post"), array<string>{ std:("Post style 1") }}
                    }}
                };
                displayAgent(agent);
            }
            );
            it(std:("should display message examples"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("messageExamples"), array<array<object>>{ array<object>{ object{
                        object::pair{std:("name"), std:("{{name1}}")}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("Hello")}
                        }}
                    }, object{
                        object::pair{std:("name"), std:("Agent")}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("Hi there")}
                        }}
                    } } }}
                };
                displayAgent(agent);
            }
            );
            it(std:("should use custom title"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std:("name"), std:("Test Agent")}
                };
                displayAgent(agent, std:("Custom Title"));
            }
            );
            it(std:("should handle empty sections gracefully"), [=]() mutable
            {
                auto agent = object{
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("bio"), array<any>()}, 
                    object::pair{std:("topics"), undefined}, 
                    object::pair{std:("adjectives"), array<any>()}
                };
                displayAgent(agent);
            }
            );
        }
        );
        describe(std:("logHeader"), [=]() mutable
        {
            it(std:("should log header with borders"), [=]() mutable
            {
                logHeader(std:("Test Header"));
            }
            );
            it(std:("should add padding around title"), [=]() mutable
            {
                logHeader(std:("Short"));
            }
            );
            it(std:("should create border matching title length"), [=]() mutable
            {
                logHeader(std:("A Very Long Title That Should Have A Long Border"));
                auto greenCalls = colors->green->mock->calls;
                auto borderCalls = greenCalls->filter([=](auto call) mutable
                {
                    return const_(call)[0]->includes(std:("─"));
                }
                );
                expect(borderCalls->length)->toBeGreaterThan(0);
            }
            );
            it(std:("should add newline before header"), [=]() mutable
            {
                logHeader(std:("Test"));
                auto calls = consoleSpy->mock->calls;
                expect(calls->some([=](auto call) mutable
                {
                    return const_(call)[0]->startsWith(std:("\
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
