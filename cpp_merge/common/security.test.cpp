#include "security.test.h"

void Main(void)
{
    describe(std:("UrlValidator"), [=]() mutable
    {
        shared<std::shared_ptr<UrlValidator>> validator;
        beforeEach([=]() mutable
        {
            validator = std::make_shared<UrlValidator>();
        }
        );
        describe(std:("URL validation"), [=]() mutable
        {
            it(std:("should validate valid HTTPS URL"), [=]() mutable
            {
                auto result = validator->validateUrl(std:("https://example.com"));
                expect(result["valid"])->toBe(true);
                expect(result["sanitized"])->toBe(std:("https://example.com/"));
            }
            );
            it(std:("should validate valid HTTP URL"), [=]() mutable
            {
                auto result = validator->validateUrl(std:("http://example.com"));
                expect(result["valid"])->toBe(true);
                expect(result["sanitized"])->toBe(std:("http://example.com/"));
            }
            );
            it(std:("should add https:// to URLs without protocol"), [=]() mutable
            {
                auto result = validator->validateUrl(std:("example.com"));
                expect(result["valid"])->toBe(true);
                expect(result["sanitized"])->toBe(std:("https://example.com/"));
            }
            );
            it(std:("should reject invalid URLs"), [=]() mutable
            {
                auto result = validator->validateUrl(std:("not a url"));
                expect(result["valid"])->toBe(false);
                expect(result["error"])->toBe(std:("Invalid URL format"));
            }
            );
            it(std:("should reject URLs that are too long"), [=]() mutable
            {
                auto longUrl = std:("https://example.com/") + std:("a")->repeat(7777) + string_empty;
                auto result = validator->validateUrl(longUrl);
                expect(result["valid"])->toBe(false);
                expect(result["error"])->toBe(std:("URL is too long"));
            }
            );
            it(std:("should reject file protocol by default"), [=]() mutable
            {
                auto result = validator->validateUrl(std:("file:///etc/passwd"));
                expect(result["valid"])->toBe(false);
                expect(result["error"])->toBe(std:("File protocol is not allowed"));
            }
            );
            it(std:("should allow file protocol when configured"), [=]() mutable
            {
                validator = std::make_shared<UrlValidator>(object{
                    object::pair{std:("allowFileProtocol"), true}
                });
                auto result = validator->validateUrl(std:("file:///home/user/file.txt"));
                expect(result["valid"])->toBe(true);
            }
            );
            it(std:("should reject non-HTTP protocols"), [=]() mutable
            {
                auto result = validator->validateUrl(std:("ftp://example.com"));
                expect(result["valid"])->toBe(false);
                expect(result["error"])->toBe(std:("Only HTTP(S) protocols are allowed"));
            }
            );
            it(std:("should handle localhost URLs"), [=]() mutable
            {
                auto result1 = validator->validateUrl(std:("http://localhost:7777"));
                expect(result1["valid"])->toBe(true);
                auto result2 = validator->validateUrl(std:("http://127.0.0.1:8080"));
                expect(result2["valid"])->toBe(true);
            }
            );
            it(std:("should reject localhost when disabled"), [=]() mutable
            {
                validator = std::make_shared<UrlValidator>(object{
                    object::pair{std:("allowLocalhost"), false}
                });
                auto result = validator->validateUrl(std:("http://localhost:7777"));
                expect(result["valid"])->toBe(false);
                expect(result["error"])->toBe(std:("Localhost URLs are not allowed"));
            }
            );
            it(std:("should check blocked domains"), [=]() mutable
            {
                auto result = validator->validateUrl(std:("https://malware.com"));
                expect(result["valid"])->toBe(false);
                expect(result["error"])->toBe(std:("Domain malware.com is blocked"));
            }
            );
            it(std:("should enforce allowed domains when configured"), [=]() mutable
            {
                validator = std::make_shared<UrlValidator>(object{
                    object::pair{std:("allowedDomains"), array<string>{ std:("github.com"), std:("example.com") }}
                });
                auto result1 = validator->validateUrl(std:("https://github.com/user/repo"));
                expect(result1["valid"])->toBe(true);
                auto result2 = validator->validateUrl(std:("https://google.com"));
                expect(result2["valid"])->toBe(false);
                expect(result2["error"])->toBe(std:("Domain is not in the allowed list"));
            }
            );
            it(std:("should allow subdomains of allowed domains"), [=]() mutable
            {
                validator = std::make_shared<UrlValidator>(object{
                    object::pair{std:("allowedDomains"), array<string>{ std:("example.com") }}
                });
                auto result = validator->validateUrl(std:("https://api.example.com"));
                expect(result["valid"])->toBe(true);
            }
            );
        }
        );
        describe(std:("config updates"), [=]() mutable
        {
            it(std:("should update configuration"), [=]() mutable
            {
                validator->updateConfig(object{
                    object::pair{std:("maxUrlLength"), 100}
                });
                auto longUrl = std:("https://example.com/") + std:("a")->repeat(100) + string_empty;
                auto result = validator->validateUrl(longUrl);
                expect(result["valid"])->toBe(false);
                expect(result["error"])->toBe(std:("URL is too long"));
            }
            );
        }
        );
    }
    );
    describe(std:("InputSanitizer"), [=]() mutable
    {
        describe(std:("sanitizeText"), [=]() mutable
        {
            it(std:("should remove HTML tags"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeText(std:("<script>alert("xss")</script>"));
                expect(result)->toBe(std:("scriptalert("xss")/script"));
            }
            );
            it(std:("should remove javascript: protocol"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeText(std:("javascript:alert("xss")"));
                expect(result)->toBe(std:("alert("xss")"));
            }
            );
            it(std:("should remove event handlers"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeText(std:("onclick=alert("xss")"));
                expect(result)->toBe(std:("alert("xss")"));
            }
            );
            it(std:("should trim whitespace"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeText(std:("  hello world  "));
                expect(result)->toBe(std:("hello world"));
            }
            );
            it(std:("should handle case insensitive patterns"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeText(std:("JAVASCRIPT:alert("xss") OnClick=test"));
                expect(result)->toBe(std:("alert("xss") test"));
            }
            );
        }
        );
        describe(std:("sanitizeSelector"), [=]() mutable
        {
            it(std:("should remove quotes"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeSelector(std:("button[name="submit"]"));
                expect(result)->toBe(std:("button[name=submit]"));
            }
            );
            it(std:("should remove HTML tags"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeSelector(std:("button<script>alert()</script>"));
                expect(result)->toBe(std:("buttonscriptalert()/script"));
            }
            );
            it(std:("should trim whitespace"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeSelector(std:("  .submit-button  "));
                expect(result)->toBe(std:(".submit-button"));
            }
            );
        }
        );
        describe(std:("sanitizeFilePath"), [=]() mutable
        {
            it(std:("should remove path traversal attempts"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeFilePath(std:("../../../etc/passwd"));
                expect(result)->toBe(std:("///etc/passwd"));
            }
            );
            it(std:("should remove invalid filename characters"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeFilePath(std:("file<>:"|?*name.txt"));
                expect(result)->toBe(std:("filename.txt"));
            }
            );
            it(std:("should trim whitespace"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeFilePath(std:("  document.pdf  "));
                expect(result)->toBe(std:("document.pdf"));
            }
            );
        }
        );
    }
    );
    describe(std:("validateSecureAction"), [=]() mutable
    {
        shared<std::shared_ptr<UrlValidator>> validator;
        beforeEach([=]() mutable
        {
            validator = std::make_shared<UrlValidator>();
        }
        );
        it(std:("should pass for valid URLs"), [=]() mutable
        {
            expect([=]() mutable
            {
                return validateSecureAction(std:("https://example.com"), validator);
            }
            )->not->toThrow();
        }
        );
        it(std:("should pass for null URLs"), [=]() mutable
        {
            expect([=]() mutable
            {
                return validateSecureAction(nullptr, validator);
            }
            )->not->toThrow();
        }
        );
        it(std:("should throw BrowserSecurityError for invalid URLs"), [=]() mutable
        {
            expect([=]() mutable
            {
                return validateSecureAction(std:("https://malware.com"), validator);
            }
            )->toThrow(BrowserSecurityError);
        }
        );
        it(std:("should include error details"), [=]() mutable
        {
            try
            {
                validateSecureAction(std:("invalid url"), validator);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(BrowserSecurityError);
                expect((as<std::shared_ptr<BrowserSecurityError>>(error))->details["url"])->toBe(std:("invalid url"));
                expect((as<std::shared_ptr<BrowserSecurityError>>(error))->details["error"])->toBe(std:("Invalid URL format"));
            }
        }
        );
    }
    );
    describe(std:("defaultUrlValidator"), [=]() mutable
    {
        it(std:("should be an instance of UrlValidator"), [=]() mutable
        {
            expect(defaultUrlValidator)->toBeInstanceOf(UrlValidator);
        }
        );
        it(std:("should use default configuration"), [=]() mutable
        {
            auto result = defaultUrlValidator->validateUrl(std:("https://example.com"));
            expect(result["valid"])->toBe(true);
        }
        );
    }
    );
    describe(std:("RateLimiter"), [=]() mutable
    {
        shared<std::shared_ptr<RateLimiter>> rateLimiter;
        shared<double> now;
        beforeEach([=]() mutable
        {
            rateLimiter = std::make_shared<RateLimiter>(object{
                object::pair{std:("maxActionsPerMinute"), 2}, 
                object::pair{std:("maxSessionsPerHour"), 3}
            });
            now = Date->now();
            spyOn(Date, std:("now"))->mockReturnValue(now);
        }
        );
        describe(std:("action limits"), [=]() mutable
        {
            it(std:("should allow actions within limit"), [=]() mutable
            {
                expect(rateLimiter->checkActionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std:("user1")))->toBe(true);
            }
            );
            it(std:("should reject actions over limit"), [=]() mutable
            {
                expect(rateLimiter->checkActionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std:("user1")))->toBe(false);
            }
            );
            it(std:("should reset after time window"), [=]() mutable
            {
                expect(rateLimiter->checkActionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std:("user1")))->toBe(false);
                spyOn(Date, std:("now"))->mockReturnValue(now + 60001);
                expect(rateLimiter->checkActionLimit(std:("user1")))->toBe(true);
            }
            );
            it(std:("should track different users separately"), [=]() mutable
            {
                expect(rateLimiter->checkActionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std:("user2")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std:("user2")))->toBe(true);
            }
            );
        }
        );
        describe(std:("session limits"), [=]() mutable
        {
            it(std:("should allow sessions within limit"), [=]() mutable
            {
                expect(rateLimiter->checkSessionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std:("user1")))->toBe(true);
            }
            );
            it(std:("should reject sessions over limit"), [=]() mutable
            {
                expect(rateLimiter->checkSessionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std:("user1")))->toBe(false);
            }
            );
            it(std:("should reset after time window"), [=]() mutable
            {
                expect(rateLimiter->checkSessionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std:("user1")))->toBe(false);
                spyOn(Date, std:("now"))->mockReturnValue(now + 3600001);
                expect(rateLimiter->checkSessionLimit(std:("user1")))->toBe(true);
            }
            );
            it(std:("should track different users separately"), [=]() mutable
            {
                expect(rateLimiter->checkSessionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std:("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std:("user2")))->toBe(true);
            }
            );
        }
        );
    }
    );
}

MAIN
