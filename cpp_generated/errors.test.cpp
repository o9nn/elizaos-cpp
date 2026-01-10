#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/src/__tests__/errors.test.h"

void Main(void)
{
    describe(std::string("Error Classes"), [=]() mutable
    {
        describe(std::string("StagehandError"), [=]() mutable
        {
            it(std::string("should create error with all properties"), [=]() mutable
            {
                auto error = std::make_shared<StagehandError>(std::string("Test error"), std::string("TEST_ERROR"), std::string("User friendly message"), true, object{
                    object::pair{std::string("extra"), std::string("data")}
                });
                expect(error->message)->toBe(std::string("Test error"));
                expect(error->code)->toBe(std::string("TEST_ERROR"));
                expect(error->userMessage)->toBe(std::string("User friendly message"));
                expect(error->recoverable)->toBe(true);
                expect(error->details)->toEqual(object{
                    object::pair{std::string("extra"), std::string("data")}
                });
                expect(error->name)->toBe(std::string("StagehandError"));
            }
            );
            it(std::string("should default recoverable to true"), [=]() mutable
            {
                auto error = std::make_shared<StagehandError>(std::string("Test error"), std::string("TEST_ERROR"), std::string("User friendly message"));
                expect(error->recoverable)->toBe(true);
            }
            );
        }
        );
        describe(std::string("BrowserNavigationError"), [=]() mutable
        {
            it(std::string("should create navigation error with URL"), [=]() mutable
            {
                auto error = std::make_shared<BrowserNavigationError>(std::string("https://example.com"));
                expect(error->code)->toBe(std::string("NAVIGATION_ERROR"));
                expect(error->message)->toContain(std::string("Failed to navigate to https://example.com"));
                expect(error->userMessage)->toContain(std::string("couldn't navigate to the requested page"));
                expect(error->recoverable)->toBe(true);
                expect(error->details["url"])->toBe(std::string("https://example.com"));
            }
            );
            it(std::string("should include original error message"), [=]() mutable
            {
                auto originalError = std::make_shared<Error>(std::string("Network timeout"));
                auto error = std::make_shared<BrowserNavigationError>(std::string("https://example.com"), originalError);
                expect(error->message)->toContain(std::string("Network timeout"));
                expect(error->details["originalError"])->toBe(std::string("Network timeout"));
            }
            );
        }
        );
        describe(std::string("BrowserSessionError"), [=]() mutable
        {
            it(std::string("should create session error"), [=]() mutable
            {
                auto error = std::make_shared<BrowserSessionError>(std::string("Session expired"), object{
                    object::pair{std::string("sessionId"), std::string("123")}
                });
                expect(error->code)->toBe(std::string("BROWSER_SESSION_ERROR"));
                expect(error->message)->toBe(std::string("Session expired"));
                expect(error->userMessage)->toContain(std::string("There was an error with the browser session"));
                expect(error->details["sessionId"])->toBe(std::string("123"));
            }
            );
        }
        );
        describe(std::string("BrowserActionError"), [=]() mutable
        {
            it(std::string("should create action error"), [=]() mutable
            {
                auto error = std::make_shared<BrowserActionError>(std::string("click"), std::string("submit button"));
                expect(error->code)->toBe(std::string("ACTION_ERROR"));
                expect(error->message)->toContain(std::string("Failed to click on submit button"));
                expect(error->userMessage)->toContain(std::string("I couldn't click on the requested element"));
                expect(error->details["action"])->toBe(std::string("click"));
                expect(error->details["target"])->toBe(std::string("submit button"));
            }
            );
            it(std::string("should include original error"), [=]() mutable
            {
                auto originalError = std::make_shared<Error>(std::string("Element not found"));
                auto error = std::make_shared<BrowserActionError>(std::string("type"), std::string("input field"), originalError);
                expect(error->message)->toContain(std::string("Element not found"));
                expect(error->details["originalError"])->toBe(std::string("Element not found"));
            }
            );
        }
        );
        describe(std::string("BrowserSecurityError"), [=]() mutable
        {
            it(std::string("should create security error"), [=]() mutable
            {
                auto error = std::make_shared<BrowserSecurityError>(std::string("Blocked domain"), object{
                    object::pair{std::string("domain"), std::string("malware.com")}
                });
                expect(error->code)->toBe(std::string("SECURITY_ERROR"));
                expect(error->message)->toBe(std::string("Blocked domain"));
                expect(error->userMessage)->toContain(std::string("blocked for security reasons"));
                expect(error->recoverable)->toBe(false);
                expect(error->details["domain"])->toBe(std::string("malware.com"));
            }
            );
        }
        );
        describe(std::string("BrowserServiceNotAvailableError"), [=]() mutable
        {
            it(std::string("should create service not available error"), [=]() mutable
            {
                auto error = std::make_shared<BrowserServiceNotAvailableError>();
                expect(error->code)->toBe(std::string("SERVICE_NOT_AVAILABLE"));
                expect(error->message)->toBe(std::string("Browser service is not available"));
                expect(error->userMessage)->toContain(std::string("The browser automation service is not available"));
                expect(error->recoverable)->toBe(false);
            }
            );
        }
        );
    }
    );
    describe(std::string("handleBrowserError"), [=]() mutable
    {
        it(std::string("should handle StagehandError"), [=]() mutable
        {
            auto callback = mock();
            auto error = std::make_shared<BrowserNavigationError>(std::string("https://example.com"));
            handleBrowserError(error, callback);
            expect(callback)->toHaveBeenCalledWith(object{
                object::pair{std::string("text"), error->userMessage}, 
                object::pair{std::string("error"), true}
            });
        }
        );
        it(std::string("should convert timeout errors"), [=]() mutable
        {
            auto callback = mock();
            auto error = std::make_shared<Error>(std::string("Operation timeout after 5000ms"));
            handleBrowserError(error, callback, std::string("load page"));
            expect(callback)->toHaveBeenCalledWith(object{
                object::pair{std::string("text"), std::string("I encountered an error while trying to load page. Please try again.")}, 
                object::pair{std::string("error"), true}
            });
        }
        );
        it(std::string("should convert navigation errors"), [=]() mutable
        {
            auto callback = mock();
            auto error = std::make_shared<Error>(std::string("Failed to navigate to page"));
            handleBrowserError(error, callback);
            expect(callback)->toHaveBeenCalledWith(object{
                object::pair{std::string("text"), std::string("I encountered an unexpected error. Please try again.")}, 
                object::pair{std::string("error"), true}
            });
        }
        );
        it(std::string("should handle generic errors"), [=]() mutable
        {
            auto callback = mock();
            auto error = std::make_shared<Error>(std::string("Something went wrong"));
            handleBrowserError(error, callback, std::string("perform action"));
            expect(callback)->toHaveBeenCalledWith(object{
                object::pair{std::string("text"), std::string("I encountered an error while trying to perform action. Please try again.")}, 
                object::pair{std::string("error"), true}
            });
        }
        );
        it(std::string("should handle non-Error objects"), [=]() mutable
        {
            auto callback = mock();
            auto error = std::make_shared<Error>(std::string("String error"));
            handleBrowserError(error, callback, std::string("do something"));
            expect(callback)->toHaveBeenCalledWith(object{
                object::pair{std::string("text"), std::string("I encountered an error while trying to do something. Please try again.")}, 
                object::pair{std::string("error"), true}
            });
        }
        );
        it(std::string("should use default action when not provided"), [=]() mutable
        {
            auto callback = mock();
            auto error = std::make_shared<Error>(std::string("Test error"));
            handleBrowserError(error, callback);
            expect(callback)->toHaveBeenCalledWith(object{
                object::pair{std::string("text"), std::string("I encountered an unexpected error. Please try again.")}, 
                object::pair{std::string("error"), true}
            });
        }
        );
    }
    );
}

MAIN
