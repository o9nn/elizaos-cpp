#include "errors.test.h"

void Main(void)
{
    describe(std:("Error Classes"), [=]() mutable
    {
        describe(std:("StagehandError"), [=]() mutable
        {
            it(std:("should create error with all properties"), [=]() mutable
            {
                auto error = std::make_shared<StagehandError>(std:("Test error"), std:("TEST_ERROR"), std:("User friendly message"), true, object{
                    object::pair{std:("extra"), std:("data")}
                });
                expect(error->message)->toBe(std:("Test error"));
                expect(error->code)->toBe(std:("TEST_ERROR"));
                expect(error->userMessage)->toBe(std:("User friendly message"));
                expect(error->recoverable)->toBe(true);
                expect(error->details)->toEqual(object{
                    object::pair{std:("extra"), std:("data")}
                });
                expect(error->name)->toBe(std:("StagehandError"));
            }
            );
            it(std:("should default recoverable to true"), [=]() mutable
            {
                auto error = std::make_shared<StagehandError>(std:("Test error"), std:("TEST_ERROR"), std:("User friendly message"));
                expect(error->recoverable)->toBe(true);
            }
            );
        }
        );
        describe(std:("BrowserNavigationError"), [=]() mutable
        {
            it(std:("should create navigation error with URL"), [=]() mutable
            {
                auto error = std::make_shared<BrowserNavigationError>(std:("https://example.com"));
                expect(error->code)->toBe(std:("NAVIGATION_ERROR"));
                expect(error->message)->toContain(std:("Failed to navigate to https://example.com"));
                expect(error->userMessage)->toContain(std:("couldn't navigate to the requested page"));
                expect(error->recoverable)->toBe(true);
                expect(error->details["url"])->toBe(std:("https://example.com"));
            }
            );
            it(std:("should include original error message"), [=]() mutable
            {
                auto originalError = std::make_shared<Error>(std:("Network timeout"));
                auto error = std::make_shared<BrowserNavigationError>(std:("https://example.com"), originalError);
                expect(error->message)->toContain(std:("Network timeout"));
                expect(error->details["originalError"])->toBe(std:("Network timeout"));
            }
            );
        }
        );
        describe(std:("BrowserSessionError"), [=]() mutable
        {
            it(std:("should create session error"), [=]() mutable
            {
                auto error = std::make_shared<BrowserSessionError>(std:("Session expired"), object{
                    object::pair{std:("sessionId"), std:("123")}
                });
                expect(error->code)->toBe(std:("BROWSER_SESSION_ERROR"));
                expect(error->message)->toBe(std:("Session expired"));
                expect(error->userMessage)->toContain(std:("There was an error with the browser session"));
                expect(error->details["sessionId"])->toBe(std:("123"));
            }
            );
        }
        );
        describe(std:("BrowserActionError"), [=]() mutable
        {
            it(std:("should create action error"), [=]() mutable
            {
                auto error = std::make_shared<BrowserActionError>(std:("click"), std:("submit button"));
                expect(error->code)->toBe(std:("ACTION_ERROR"));
                expect(error->message)->toContain(std:("Failed to click on submit button"));
                expect(error->userMessage)->toContain(std:("I couldn't click on the requested element"));
                expect(error->details["action"])->toBe(std:("click"));
                expect(error->details["target"])->toBe(std:("submit button"));
            }
            );
            it(std:("should include original error"), [=]() mutable
            {
                auto originalError = std::make_shared<Error>(std:("Element not found"));
                auto error = std::make_shared<BrowserActionError>(std:("type"), std:("input field"), originalError);
                expect(error->message)->toContain(std:("Element not found"));
                expect(error->details["originalError"])->toBe(std:("Element not found"));
            }
            );
        }
        );
        describe(std:("BrowserSecurityError"), [=]() mutable
        {
            it(std:("should create security error"), [=]() mutable
            {
                auto error = std::make_shared<BrowserSecurityError>(std:("Blocked domain"), object{
                    object::pair{std:("domain"), std:("malware.com")}
                });
                expect(error->code)->toBe(std:("SECURITY_ERROR"));
                expect(error->message)->toBe(std:("Blocked domain"));
                expect(error->userMessage)->toContain(std:("blocked for security reasons"));
                expect(error->recoverable)->toBe(false);
                expect(error->details["domain"])->toBe(std:("malware.com"));
            }
            );
        }
        );
        describe(std:("BrowserServiceNotAvailableError"), [=]() mutable
        {
            it(std:("should create service not available error"), [=]() mutable
            {
                auto error = std::make_shared<BrowserServiceNotAvailableError>();
                expect(error->code)->toBe(std:("SERVICE_NOT_AVAILABLE"));
                expect(error->message)->toBe(std:("Browser service is not available"));
                expect(error->userMessage)->toContain(std:("The browser automation service is not available"));
                expect(error->recoverable)->toBe(false);
            }
            );
        }
        );
    }
    );
    describe(std:("handleBrowserError"), [=]() mutable
    {
        it(std:("should handle StagehandError"), [=]() mutable
        {
            auto callback = mock();
            auto error = std::make_shared<BrowserNavigationError>(std:("https://example.com"));
            handleBrowserError(error, callback);
            expect(callback)->toHaveBeenCalledWith(object{
                object::pair{std:("text"), error->userMessage}, 
                object::pair{std:("error"), true}
            });
        }
        );
        it(std:("should convert timeout errors"), [=]() mutable
        {
            auto callback = mock();
            auto error = std::make_shared<Error>(std:("Operation timeout after 5000ms"));
            handleBrowserError(error, callback, std:("load page"));
            expect(callback)->toHaveBeenCalledWith(object{
                object::pair{std:("text"), std:("I encountered an error while trying to load page. Please try again.")}, 
                object::pair{std:("error"), true}
            });
        }
        );
        it(std:("should convert navigation errors"), [=]() mutable
        {
            auto callback = mock();
            auto error = std::make_shared<Error>(std:("Failed to navigate to page"));
            handleBrowserError(error, callback);
            expect(callback)->toHaveBeenCalledWith(object{
                object::pair{std:("text"), std:("I encountered an unexpected error. Please try again.")}, 
                object::pair{std:("error"), true}
            });
        }
        );
        it(std:("should handle generic errors"), [=]() mutable
        {
            auto callback = mock();
            auto error = std::make_shared<Error>(std:("Something went wrong"));
            handleBrowserError(error, callback, std:("perform action"));
            expect(callback)->toHaveBeenCalledWith(object{
                object::pair{std:("text"), std:("I encountered an error while trying to perform action. Please try again.")}, 
                object::pair{std:("error"), true}
            });
        }
        );
        it(std:("should handle non-Error objects"), [=]() mutable
        {
            auto callback = mock();
            auto error = std::make_shared<Error>(std:("String error"));
            handleBrowserError(error, callback, std:("do something"));
            expect(callback)->toHaveBeenCalledWith(object{
                object::pair{std:("text"), std:("I encountered an error while trying to do something. Please try again.")}, 
                object::pair{std:("error"), true}
            });
        }
        );
        it(std:("should use default action when not provided"), [=]() mutable
        {
            auto callback = mock();
            auto error = std::make_shared<Error>(std:("Test error"));
            handleBrowserError(error, callback);
            expect(callback)->toHaveBeenCalledWith(object{
                object::pair{std:("text"), std:("I encountered an unexpected error. Please try again.")}, 
                object::pair{std:("error"), true}
            });
        }
        );
    }
    );
}

MAIN
