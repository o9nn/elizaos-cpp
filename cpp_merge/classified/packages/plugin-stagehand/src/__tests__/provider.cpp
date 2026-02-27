#include "provider.test.h"

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
                    object::pair{std:("getState"), mock()->mockResolvedValue(object{
                        object::pair{std:("url"), std:("https://example.com")}, 
                        object::pair{std:("title"), std:("Test Page Title")}, 
                        object::pair{std:("sessionId"), std:("test-session-1")}, 
                        object::pair{std:("createdAt"), std::make_shared<Date>()}
                    })}
                });
            }
            )}
        });
    }
    );
    describe(std:("BROWSER_STATE provider"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<any> mockService;
        shared<any> mockClient;
        shared<any> mockSession;
        shared<std::shared_ptr<Memory>> mockMessage;
        shared<std::shared_ptr<State>> mockState;
        shared<any> browserStateProvider;
        beforeEach([=]() mutable
        {
            mockMessage = createMockMemory();
            mockState = createMockState();
            mockSession = std::make_shared<BrowserSession>(std:("test-session-1"));
            mockClient = std::make_shared<StagehandWebSocketClient>(std:("ws://localhost:3456"));
            mockService = object{
                object::pair{std:("getCurrentSession"), mock()->mockResolvedValue(mockSession)}, 
                object::pair{std:("getClient"), mock()->mockReturnValue(mockClient)}, 
                object::pair{std:("isInitialized"), true}
            };
            mockRuntime = createMockRuntime(object{
                object::pair{std:("getService"), mock()->mockReturnValue(mockService)}
            });
            spyOn(logger, std:("error"))->mockImplementation([=]() mutable
            {
            }
            );
            browserStateProvider = stagehandPlugin->providers->find([=](auto p) mutable
            {
                return p["name"] == std:("BROWSER_STATE");
            }
            );
        }
        );
        describe(std:("get method"), [=]() mutable
        {
            it(std:("should return current session information when session exists"), [=]() mutable
            {
                auto result = std::async([=]() { browserStateProvider["get"](mockRuntime, mockMessage, mockState); });
                expect(result["text"])->toBe(std:("Current browser page: "Test Page Title" at https://example.com"));
                expect(result["values"])->toEqual(object{
                    object::pair{std:("hasSession"), true}, 
                    object::pair{std:("url"), std:("https://example.com")}, 
                    object::pair{std:("title"), std:("Test Page Title")}
                });
                expect(result["data"]["sessionId"])->toBe(std:("test-session-1"));
                expect(result["data"]["createdAt"])->toBeInstanceOf(Date);
            }
            );
            it(std:("should return no session message when no session exists"), [=]() mutable
            {
                mockService["getCurrentSession"]["mockResolvedValue"](undefined);
                auto result = std::async([=]() { browserStateProvider["get"](mockRuntime, mockMessage, mockState); });
                expect(result["text"])->toBe(std:("No active browser session"));
                expect(result["values"])->toEqual(object{
                    object::pair{std:("hasSession"), false}
                });
                expect(result["data"])->toEqual(object{});
            }
            );
            it(std:("should handle errors gracefully when getting page info fails"), [=]() mutable
            {
                mockClient["getState"]["mockRejectedValue"](std::make_shared<Error>(std:("Page error")));
                auto result = std::async([=]() { browserStateProvider["get"](mockRuntime, mockMessage, mockState); });
                expect(result["text"])->toBe(std:("Error getting browser state"));
                expect(result["values"])->toEqual(object{
                    object::pair{std:("hasSession"), true}, 
                    object::pair{std:("error"), true}
                });
                expect(logger->error)->toHaveBeenCalledWith(std:("Error getting browser state:"), expect->any(Error));
            }
            );
            it(std:("should work when service is not available"), [=]() mutable
            {
                mockRuntime->getService = mock()->mockReturnValue(nullptr);
                auto result = std::async([=]() { browserStateProvider["get"](mockRuntime, mockMessage, mockState); });
                expect(result["text"])->toBe(std:("No active browser session"));
                expect(result["values"]["hasSession"])->toBe(false);
            }
            );
            it(std:("should provide session creation time"), [=]() mutable
            {
                auto testDate = std::make_shared<Date>(std:("2024-01-01T00:00:00Z"));
                mockSession["createdAt"] = testDate;
                auto result = std::async([=]() { browserStateProvider["get"](mockRuntime, mockMessage, mockState); });
                expect(result["data"]["createdAt"])->toEqual(testDate);
            }
            );
            it(std:("should handle URL without title"), [=]() mutable
            {
                mockClient["getState"]["mockResolvedValue"](object{
                    object::pair{std:("url"), std:("https://example.com")}, 
                    object::pair{std:("title"), string_empty}, 
                    object::pair{std:("sessionId"), std:("test-session-1")}, 
                    object::pair{std:("createdAt"), std::make_shared<Date>()}
                });
                auto result = std::async([=]() { browserStateProvider["get"](mockRuntime, mockMessage, mockState); });
                expect(result["text"])->toBe(std:("Current browser page: "" at https://example.com"));
            }
            );
        }
        );
        describe(std:("provider structure"), [=]() mutable
        {
            it(std:("should have correct name"), [=]() mutable
            {
                expect(browserStateProvider["name"])->toBe(std:("BROWSER_STATE"));
            }
            );
            it(std:("should have a description"), [=]() mutable
            {
                expect(browserStateProvider["description"])->toContain(std:("browser state information"));
            }
            );
            it(std:("should have get method"), [=]() mutable
            {
                expect(type_of(browserStateProvider["get"]))->toBe(std:("function"));
            }
            );
        }
        );
    }
    );
}

MAIN
