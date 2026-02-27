#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/src/__tests__/provider.test.h"

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
                    object::pair{std::string("getState"), mock()->mockResolvedValue(object{
                        object::pair{std::string("url"), std::string("https://example.com")}, 
                        object::pair{std::string("title"), std::string("Test Page Title")}, 
                        object::pair{std::string("sessionId"), std::string("test-session-1")}, 
                        object::pair{std::string("createdAt"), std::make_shared<Date>()}
                    })}
                });
            }
            )}
        });
    }
    );
    describe(std::string("BROWSER_STATE provider"), [=]() mutable
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
            mockSession = std::make_shared<BrowserSession>(std::string("test-session-1"));
            mockClient = std::make_shared<StagehandWebSocketClient>(std::string("ws://localhost:3456"));
            mockService = object{
                object::pair{std::string("getCurrentSession"), mock()->mockResolvedValue(mockSession)}, 
                object::pair{std::string("getClient"), mock()->mockReturnValue(mockClient)}, 
                object::pair{std::string("isInitialized"), true}
            };
            mockRuntime = createMockRuntime(object{
                object::pair{std::string("getService"), mock()->mockReturnValue(mockService)}
            });
            spyOn(logger, std::string("error"))->mockImplementation([=]() mutable
            {
            }
            );
            browserStateProvider = stagehandPlugin->providers->find([=](auto p) mutable
            {
                return p["name"] == std::string("BROWSER_STATE");
            }
            );
        }
        );
        describe(std::string("get method"), [=]() mutable
        {
            it(std::string("should return current session information when session exists"), [=]() mutable
            {
                auto result = std::async([=]() { browserStateProvider["get"](mockRuntime, mockMessage, mockState); });
                expect(result["text"])->toBe(std::string("Current browser page: "Test Page Title" at https://example.com"));
                expect(result["values"])->toEqual(object{
                    object::pair{std::string("hasSession"), true}, 
                    object::pair{std::string("url"), std::string("https://example.com")}, 
                    object::pair{std::string("title"), std::string("Test Page Title")}
                });
                expect(result["data"]["sessionId"])->toBe(std::string("test-session-1"));
                expect(result["data"]["createdAt"])->toBeInstanceOf(Date);
            }
            );
            it(std::string("should return no session message when no session exists"), [=]() mutable
            {
                mockService["getCurrentSession"]["mockResolvedValue"](undefined);
                auto result = std::async([=]() { browserStateProvider["get"](mockRuntime, mockMessage, mockState); });
                expect(result["text"])->toBe(std::string("No active browser session"));
                expect(result["values"])->toEqual(object{
                    object::pair{std::string("hasSession"), false}
                });
                expect(result["data"])->toEqual(object{});
            }
            );
            it(std::string("should handle errors gracefully when getting page info fails"), [=]() mutable
            {
                mockClient["getState"]["mockRejectedValue"](std::make_shared<Error>(std::string("Page error")));
                auto result = std::async([=]() { browserStateProvider["get"](mockRuntime, mockMessage, mockState); });
                expect(result["text"])->toBe(std::string("Error getting browser state"));
                expect(result["values"])->toEqual(object{
                    object::pair{std::string("hasSession"), true}, 
                    object::pair{std::string("error"), true}
                });
                expect(logger->error)->toHaveBeenCalledWith(std::string("Error getting browser state:"), expect->any(Error));
            }
            );
            it(std::string("should work when service is not available"), [=]() mutable
            {
                mockRuntime->getService = mock()->mockReturnValue(nullptr);
                auto result = std::async([=]() { browserStateProvider["get"](mockRuntime, mockMessage, mockState); });
                expect(result["text"])->toBe(std::string("No active browser session"));
                expect(result["values"]["hasSession"])->toBe(false);
            }
            );
            it(std::string("should provide session creation time"), [=]() mutable
            {
                auto testDate = std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"));
                mockSession["createdAt"] = testDate;
                auto result = std::async([=]() { browserStateProvider["get"](mockRuntime, mockMessage, mockState); });
                expect(result["data"]["createdAt"])->toEqual(testDate);
            }
            );
            it(std::string("should handle URL without title"), [=]() mutable
            {
                mockClient["getState"]["mockResolvedValue"](object{
                    object::pair{std::string("url"), std::string("https://example.com")}, 
                    object::pair{std::string("title"), string_empty}, 
                    object::pair{std::string("sessionId"), std::string("test-session-1")}, 
                    object::pair{std::string("createdAt"), std::make_shared<Date>()}
                });
                auto result = std::async([=]() { browserStateProvider["get"](mockRuntime, mockMessage, mockState); });
                expect(result["text"])->toBe(std::string("Current browser page: "" at https://example.com"));
            }
            );
        }
        );
        describe(std::string("provider structure"), [=]() mutable
        {
            it(std::string("should have correct name"), [=]() mutable
            {
                expect(browserStateProvider["name"])->toBe(std::string("BROWSER_STATE"));
            }
            );
            it(std::string("should have a description"), [=]() mutable
            {
                expect(browserStateProvider["description"])->toContain(std::string("browser state information"));
            }
            );
            it(std::string("should have get method"), [=]() mutable
            {
                expect(type_of(browserStateProvider["get"]))->toBe(std::string("function"));
            }
            );
        }
        );
    }
    );
}

MAIN
