#include "actions.test.h"

void Main(void)
{
    mock->module(std:("../service"), [=]() mutable
    {
        return (object{
            object::pair{std:("StagehandService"), object{
                object::pair{std:("serviceType"), std:("STAGEHAND")}, 
                object::pair{std:("start"), mock()}, 
                object::pair{std:("stop"), mock()}
            }}, 
            object::pair{std:("BrowserSession"), mock()->mockImplementation([=](auto id) mutable
            {
                return (object{
                    object::pair{std:("id"), std:("id")}, 
                    object::pair{std:("createdAt"), std::make_shared<Date>()}
                });
            }
            )}
        });
    }
    );
    mock->module(std:("../websocket-client"), [=]() mutable
    {
        return (object{
            object::pair{std:("StagehandWebSocketClient"), mock()->mockImplementation([=]() mutable
            {
                return (object{
                    object::pair{std:("navigate"), mock()->mockResolvedValue(object{
                        object::pair{std:("url"), std:("https://example.com")}, 
                        object::pair{std:("title"), std:("Example Domain")}
                    })}, 
                    object::pair{std:("getState"), mock()->mockResolvedValue(object{
                        object::pair{std:("url"), std:("https://example.com")}, 
                        object::pair{std:("title"), std:("Example Domain")}, 
                        object::pair{std:("sessionId"), std:("test-session-1")}, 
                        object::pair{std:("createdAt"), std::make_shared<Date>()}
                    })}, 
                    object::pair{std:("goBack"), mock()->mockResolvedValue(object{
                        object::pair{std:("url"), std:("https://previous.com")}, 
                        object::pair{std:("title"), std:("Previous Page")}
                    })}, 
                    object::pair{std:("goForward"), mock()->mockResolvedValue(object{
                        object::pair{std:("url"), std:("https://next.com")}, 
                        object::pair{std:("title"), std:("Next Page")}
                    })}, 
                    object::pair{std:("refresh"), mock()->mockResolvedValue(object{
                        object::pair{std:("url"), std:("https://current.com")}, 
                        object::pair{std:("title"), std:("Current Page")}
                    })}
                });
            }
            )}
        });
    }
    );
    describe(std:("BROWSER_NAVIGATE action"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<any> mockService;
        shared<any> mockClient;
        shared<any> mockSession;
        shared<std::shared_ptr<HandlerCallback>> mockCallback;
        shared<any> browserNavigateAction;
        beforeEach([=]() mutable
        {
            setupLoggerSpies();
            mockSession = std::make_shared<BrowserSession>(std:("test-session-1"));
            mockClient = std::make_shared<StagehandWebSocketClient>(std:("ws://localhost:3456"));
            mockService = object{
                object::pair{std:("getCurrentSession"), mock()->mockResolvedValue(mockSession)}, 
                object::pair{std:("createSession"), mock()->mockResolvedValue(mockSession)}, 
                object::pair{std:("getClient"), mock()->mockReturnValue(mockClient)}, 
                object::pair{std:("isInitialized"), true}
            };
            mockRuntime = createMockRuntime(object{
                object::pair{std:("getService"), mock()->mockReturnValue(mockService)}
            });
            mockCallback = mock();
            browserNavigateAction = stagehandPlugin->actions->find([=](auto action) mutable
            {
                return action["name"] == std:("BROWSER_NAVIGATE");
            }
            );
        }
        );
        describe(std:("validate"), [=]() mutable
        {
            it(std:("should validate when URL is found in message"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Navigate to https://google.com")}, 
                        object::pair{std:("source"), std:("test")}
                    }}
                });
                auto isValid = std::async([=]() { browserNavigateAction["validate"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{})); });
                expect(isValid)->toBe(true);
            }
            );
            it(std:("should validate domain without protocol"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Go to example.com")}, 
                        object::pair{std:("source"), std:("test")}
                    }}
                });
                auto isValid = std::async([=]() { browserNavigateAction["validate"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{})); });
                expect(isValid)->toBe(true);
            }
            );
            it(std:("should validate URL in quotes"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Open "https://example.com" in browser")}, 
                        object::pair{std:("source"), std:("test")}
                    }}
                });
                auto isValid = std::async([=]() { browserNavigateAction["validate"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{})); });
                expect(isValid)->toBe(true);
            }
            );
            it(std:("should not validate when no URL found"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Just regular text without URLs")}, 
                        object::pair{std:("source"), std:("test")}
                    }}
                });
                auto isValid = std::async([=]() { browserNavigateAction["validate"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{})); });
                expect(isValid)->toBe(false);
            }
            );
        }
        );
        describe(std:("handler"), [=]() mutable
        {
            it(std:("should navigate to URL and return success"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Navigate to https://google.com")}, 
                        object::pair{std:("source"), std:("test")}
                    }}
                });
                mockClient["navigate"]["mockResolvedValue"](object{
                    object::pair{std:("url"), std:("https://google.com")}, 
                    object::pair{std:("title"), std:("Google")}
                });
                std::async([=]() { browserNavigateAction["handler"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{}), object{}, mockCallback, array<any>()); });
                expect(mockClient["navigate"])->toHaveBeenCalledWith(std:("test-session-1"), std:("https://google.com"));
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("text"), std:("I've navigated to https://google.com. The page title is: "Google"")}, 
                    object::pair{std:("actions"), array<string>{ std:("BROWSER_NAVIGATE") }}, 
                    object::pair{std:("source"), std:("test")}
                });
            }
            );
            it(std:("should create session if none exists"), [=]() mutable
            {
                mockService["getCurrentSession"]["mockResolvedValue"](undefined);
                auto message = createMockMemory(object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Navigate to https://example.com")}, 
                        object::pair{std:("source"), std:("test")}
                    }}
                });
                std::async([=]() { browserNavigateAction["handler"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{}), object{}, mockCallback, array<any>()); });
                expect(mockService["createSession"])->toHaveBeenCalledWith(expect->stringContaining(std:("session-")));
                expect(mockClient["navigate"])->toHaveBeenCalled();
            }
            );
            it(std:("should handle domain without protocol"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Go to example.com")}, 
                        object::pair{std:("source"), std:("test")}
                    }}
                });
                std::async([=]() { browserNavigateAction["handler"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{}), object{}, mockCallback, array<any>()); });
                expect(mockClient["navigate"])->toHaveBeenCalledWith(std:("test-session-1"), std:("https://example.com"));
            }
            );
            it(std:("should handle error when service not available"), [=]() mutable
            {
                mockRuntime->getService = mock()->mockReturnValue(nullptr);
                auto message = createMockMemory(object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Navigate to https://example.com")}, 
                        object::pair{std:("source"), std:("test")}
                    }}
                });
                std::async([=]() { browserNavigateAction["handler"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{}), object{}, mockCallback, array<any>()); });
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("text"), std:("The browser automation service is not available. Please ensure the Stagehand plugin is properly configured.")}, 
                    object::pair{std:("error"), true}
                });
            }
            );
            it(std:("should handle error when no URL found"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Just some text")}, 
                        object::pair{std:("source"), std:("test")}
                    }}
                });
                std::async([=]() { browserNavigateAction["handler"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{}), object{}, mockCallback, array<any>()); });
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("text"), std:("I couldn't find a URL in your request. Please provide a valid URL to navigate to.")}, 
                    object::pair{std:("error"), true}
                });
            }
            );
            it(std:("should handle navigation errors"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Navigate to https://example.com")}, 
                        object::pair{std:("source"), std:("test")}
                    }}
                });
                mockClient["navigate"]["mockRejectedValue"](std::make_shared<Error>(std:("Navigation failed")));
                std::async([=]() { browserNavigateAction["handler"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{}), object{}, mockCallback, array<any>()); });
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("text"), std:("I couldn't navigate to the requested page. Please check the URL and try again.")}, 
                    object::pair{std:("error"), true}
                });
            }
            );
        }
        );
        describe(std:("examples"), [=]() mutable
        {
            it(std:("should have valid examples"), [=]() mutable
            {
                expect(browserNavigateAction["examples"])->toBeDefined();
                expect(Array->isArray(browserNavigateAction["examples"]))->toBe(true);
                expect(browserNavigateAction["examples"]["length"])->toBeGreaterThan(0);
                browserNavigateAction["examples"]["forEach"]([=](auto example) mutable
                {
                    expect(Array->isArray(example))->toBe(true);
                    expect(const_(example)[0]["name"])->toBeDefined();
                    expect(const_(example)[0]["content"]["text"])->toBeDefined();
                    expect(const_(example)[1]["name"])->toBeDefined();
                    expect(const_(example)[1]["content"]["text"])->toBeDefined();
                    expect(const_(example)[1]["content"]["actions"])->toContain(std:("BROWSER_NAVIGATE"));
                }
                );
            }
            );
        }
        );
        describe(std:("action metadata"), [=]() mutable
        {
            it(std:("should have correct action name"), [=]() mutable
            {
                expect(browserNavigateAction["name"])->toBe(std:("BROWSER_NAVIGATE"));
            }
            );
            it(std:("should have similes"), [=]() mutable
            {
                expect(browserNavigateAction["similes"])->toBeDefined();
                expect(browserNavigateAction["similes"])->toContain(std:("GO_TO_URL"));
                expect(browserNavigateAction["similes"])->toContain(std:("OPEN_WEBSITE"));
            }
            );
            it(std:("should have a description"), [=]() mutable
            {
                expect(browserNavigateAction["description"])->toContain(std:("Navigate"));
            }
            );
        }
        );
    }
    );
}

MAIN
