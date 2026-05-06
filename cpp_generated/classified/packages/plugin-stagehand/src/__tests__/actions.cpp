#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/src/__tests__/actions.test.h"

void Main(void)
{
    mock->module(std::string("../service"), [=]() mutable
    {
        return (object{
            object::pair{std::string("StagehandService"), object{
                object::pair{std::string("serviceType"), std::string("STAGEHAND")}, 
                object::pair{std::string("start"), mock()}, 
                object::pair{std::string("stop"), mock()}
            }}, 
            object::pair{std::string("BrowserSession"), mock()->mockImplementation([=](auto id) mutable
            {
                return (object{
                    object::pair{std::string("id"), std::string("id")}, 
                    object::pair{std::string("createdAt"), std::make_shared<Date>()}
                });
            }
            )}
        });
    }
    );
    mock->module(std::string("../websocket-client"), [=]() mutable
    {
        return (object{
            object::pair{std::string("StagehandWebSocketClient"), mock()->mockImplementation([=]() mutable
            {
                return (object{
                    object::pair{std::string("navigate"), mock()->mockResolvedValue(object{
                        object::pair{std::string("url"), std::string("https://example.com")}, 
                        object::pair{std::string("title"), std::string("Example Domain")}
                    })}, 
                    object::pair{std::string("getState"), mock()->mockResolvedValue(object{
                        object::pair{std::string("url"), std::string("https://example.com")}, 
                        object::pair{std::string("title"), std::string("Example Domain")}, 
                        object::pair{std::string("sessionId"), std::string("test-session-1")}, 
                        object::pair{std::string("createdAt"), std::make_shared<Date>()}
                    })}, 
                    object::pair{std::string("goBack"), mock()->mockResolvedValue(object{
                        object::pair{std::string("url"), std::string("https://previous.com")}, 
                        object::pair{std::string("title"), std::string("Previous Page")}
                    })}, 
                    object::pair{std::string("goForward"), mock()->mockResolvedValue(object{
                        object::pair{std::string("url"), std::string("https://next.com")}, 
                        object::pair{std::string("title"), std::string("Next Page")}
                    })}, 
                    object::pair{std::string("refresh"), mock()->mockResolvedValue(object{
                        object::pair{std::string("url"), std::string("https://current.com")}, 
                        object::pair{std::string("title"), std::string("Current Page")}
                    })}
                });
            }
            )}
        });
    }
    );
    describe(std::string("BROWSER_NAVIGATE action"), [=]() mutable
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
            mockSession = std::make_shared<BrowserSession>(std::string("test-session-1"));
            mockClient = std::make_shared<StagehandWebSocketClient>(std::string("ws://localhost:3456"));
            mockService = object{
                object::pair{std::string("getCurrentSession"), mock()->mockResolvedValue(mockSession)}, 
                object::pair{std::string("createSession"), mock()->mockResolvedValue(mockSession)}, 
                object::pair{std::string("getClient"), mock()->mockReturnValue(mockClient)}, 
                object::pair{std::string("isInitialized"), true}
            };
            mockRuntime = createMockRuntime(object{
                object::pair{std::string("getService"), mock()->mockReturnValue(mockService)}
            });
            mockCallback = mock();
            browserNavigateAction = stagehandPlugin->actions->find([=](auto action) mutable
            {
                return action["name"] == std::string("BROWSER_NAVIGATE");
            }
            );
        }
        );
        describe(std::string("validate"), [=]() mutable
        {
            it(std::string("should validate when URL is found in message"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Navigate to https://google.com")}, 
                        object::pair{std::string("source"), std::string("test")}
                    }}
                });
                auto isValid = std::async([=]() { browserNavigateAction["validate"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{})); });
                expect(isValid)->toBe(true);
            }
            );
            it(std::string("should validate domain without protocol"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Go to example.com")}, 
                        object::pair{std::string("source"), std::string("test")}
                    }}
                });
                auto isValid = std::async([=]() { browserNavigateAction["validate"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{})); });
                expect(isValid)->toBe(true);
            }
            );
            it(std::string("should validate URL in quotes"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Open "https://example.com" in browser")}, 
                        object::pair{std::string("source"), std::string("test")}
                    }}
                });
                auto isValid = std::async([=]() { browserNavigateAction["validate"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{})); });
                expect(isValid)->toBe(true);
            }
            );
            it(std::string("should not validate when no URL found"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Just regular text without URLs")}, 
                        object::pair{std::string("source"), std::string("test")}
                    }}
                });
                auto isValid = std::async([=]() { browserNavigateAction["validate"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{})); });
                expect(isValid)->toBe(false);
            }
            );
        }
        );
        describe(std::string("handler"), [=]() mutable
        {
            it(std::string("should navigate to URL and return success"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Navigate to https://google.com")}, 
                        object::pair{std::string("source"), std::string("test")}
                    }}
                });
                mockClient["navigate"]["mockResolvedValue"](object{
                    object::pair{std::string("url"), std::string("https://google.com")}, 
                    object::pair{std::string("title"), std::string("Google")}
                });
                std::async([=]() { browserNavigateAction["handler"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{}), object{}, mockCallback, array<any>()); });
                expect(mockClient["navigate"])->toHaveBeenCalledWith(std::string("test-session-1"), std::string("https://google.com"));
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("text"), std::string("I've navigated to https://google.com. The page title is: "Google"")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("BROWSER_NAVIGATE") }}, 
                    object::pair{std::string("source"), std::string("test")}
                });
            }
            );
            it(std::string("should create session if none exists"), [=]() mutable
            {
                mockService["getCurrentSession"]["mockResolvedValue"](undefined);
                auto message = createMockMemory(object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Navigate to https://example.com")}, 
                        object::pair{std::string("source"), std::string("test")}
                    }}
                });
                std::async([=]() { browserNavigateAction["handler"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{}), object{}, mockCallback, array<any>()); });
                expect(mockService["createSession"])->toHaveBeenCalledWith(expect->stringContaining(std::string("session-")));
                expect(mockClient["navigate"])->toHaveBeenCalled();
            }
            );
            it(std::string("should handle domain without protocol"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Go to example.com")}, 
                        object::pair{std::string("source"), std::string("test")}
                    }}
                });
                std::async([=]() { browserNavigateAction["handler"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{}), object{}, mockCallback, array<any>()); });
                expect(mockClient["navigate"])->toHaveBeenCalledWith(std::string("test-session-1"), std::string("https://example.com"));
            }
            );
            it(std::string("should handle error when service not available"), [=]() mutable
            {
                mockRuntime->getService = mock()->mockReturnValue(nullptr);
                auto message = createMockMemory(object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Navigate to https://example.com")}, 
                        object::pair{std::string("source"), std::string("test")}
                    }}
                });
                std::async([=]() { browserNavigateAction["handler"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{}), object{}, mockCallback, array<any>()); });
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("text"), std::string("The browser automation service is not available. Please ensure the Stagehand plugin is properly configured.")}, 
                    object::pair{std::string("error"), true}
                });
            }
            );
            it(std::string("should handle error when no URL found"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Just some text")}, 
                        object::pair{std::string("source"), std::string("test")}
                    }}
                });
                std::async([=]() { browserNavigateAction["handler"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{}), object{}, mockCallback, array<any>()); });
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("text"), std::string("I couldn't find a URL in your request. Please provide a valid URL to navigate to.")}, 
                    object::pair{std::string("error"), true}
                });
            }
            );
            it(std::string("should handle navigation errors"), [=]() mutable
            {
                auto message = createMockMemory(object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Navigate to https://example.com")}, 
                        object::pair{std::string("source"), std::string("test")}
                    }}
                });
                mockClient["navigate"]["mockRejectedValue"](std::make_shared<Error>(std::string("Navigation failed")));
                std::async([=]() { browserNavigateAction["handler"](mockRuntime, as<std::shared_ptr<Memory>>(message), as<std::shared_ptr<State>>(object{}), object{}, mockCallback, array<any>()); });
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("text"), std::string("I couldn't navigate to the requested page. Please check the URL and try again.")}, 
                    object::pair{std::string("error"), true}
                });
            }
            );
        }
        );
        describe(std::string("examples"), [=]() mutable
        {
            it(std::string("should have valid examples"), [=]() mutable
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
                    expect(const_(example)[1]["content"]["actions"])->toContain(std::string("BROWSER_NAVIGATE"));
                }
                );
            }
            );
        }
        );
        describe(std::string("action metadata"), [=]() mutable
        {
            it(std::string("should have correct action name"), [=]() mutable
            {
                expect(browserNavigateAction["name"])->toBe(std::string("BROWSER_NAVIGATE"));
            }
            );
            it(std::string("should have similes"), [=]() mutable
            {
                expect(browserNavigateAction["similes"])->toBeDefined();
                expect(browserNavigateAction["similes"])->toContain(std::string("GO_TO_URL"));
                expect(browserNavigateAction["similes"])->toContain(std::string("OPEN_WEBSITE"));
            }
            );
            it(std::string("should have a description"), [=]() mutable
            {
                expect(browserNavigateAction["description"])->toContain(std::string("Navigate"));
            }
            );
        }
        );
    }
    );
}

MAIN
