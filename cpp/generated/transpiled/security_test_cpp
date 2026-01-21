#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/src/__tests__/security.test.h"

void Main(void)
{
    describe(std::string("UrlValidator"), [=]() mutable
    {
        shared<std::shared_ptr<UrlValidator>> validator;
        beforeEach([=]() mutable
        {
            validator = std::make_shared<UrlValidator>();
        }
        );
        describe(std::string("URL validation"), [=]() mutable
        {
            it(std::string("should validate valid HTTPS URL"), [=]() mutable
            {
                auto result = validator->validateUrl(std::string("https://example.com"));
                expect(result["valid"])->toBe(true);
                expect(result["sanitized"])->toBe(std::string("https://example.com/"));
            }
            );
            it(std::string("should validate valid HTTP URL"), [=]() mutable
            {
                auto result = validator->validateUrl(std::string("http://example.com"));
                expect(result["valid"])->toBe(true);
                expect(result["sanitized"])->toBe(std::string("http://example.com/"));
            }
            );
            it(std::string("should add https:// to URLs without protocol"), [=]() mutable
            {
                auto result = validator->validateUrl(std::string("example.com"));
                expect(result["valid"])->toBe(true);
                expect(result["sanitized"])->toBe(std::string("https://example.com/"));
            }
            );
            it(std::string("should reject invalid URLs"), [=]() mutable
            {
                auto result = validator->validateUrl(std::string("not a url"));
                expect(result["valid"])->toBe(false);
                expect(result["error"])->toBe(std::string("Invalid URL format"));
            }
            );
            it(std::string("should reject URLs that are too long"), [=]() mutable
            {
                auto longUrl = std::string("https://example.com/") + std::string("a")->repeat(7777) + string_empty;
                auto result = validator->validateUrl(longUrl);
                expect(result["valid"])->toBe(false);
                expect(result["error"])->toBe(std::string("URL is too long"));
            }
            );
            it(std::string("should reject file protocol by default"), [=]() mutable
            {
                auto result = validator->validateUrl(std::string("file:///etc/passwd"));
                expect(result["valid"])->toBe(false);
                expect(result["error"])->toBe(std::string("File protocol is not allowed"));
            }
            );
            it(std::string("should allow file protocol when configured"), [=]() mutable
            {
                validator = std::make_shared<UrlValidator>(object{
                    object::pair{std::string("allowFileProtocol"), true}
                });
                auto result = validator->validateUrl(std::string("file:///home/user/file.txt"));
                expect(result["valid"])->toBe(true);
            }
            );
            it(std::string("should reject non-HTTP protocols"), [=]() mutable
            {
                auto result = validator->validateUrl(std::string("ftp://example.com"));
                expect(result["valid"])->toBe(false);
                expect(result["error"])->toBe(std::string("Only HTTP(S) protocols are allowed"));
            }
            );
            it(std::string("should handle localhost URLs"), [=]() mutable
            {
                auto result1 = validator->validateUrl(std::string("http://localhost:7777"));
                expect(result1["valid"])->toBe(true);
                auto result2 = validator->validateUrl(std::string("http://127.0.0.1:8080"));
                expect(result2["valid"])->toBe(true);
            }
            );
            it(std::string("should reject localhost when disabled"), [=]() mutable
            {
                validator = std::make_shared<UrlValidator>(object{
                    object::pair{std::string("allowLocalhost"), false}
                });
                auto result = validator->validateUrl(std::string("http://localhost:7777"));
                expect(result["valid"])->toBe(false);
                expect(result["error"])->toBe(std::string("Localhost URLs are not allowed"));
            }
            );
            it(std::string("should check blocked domains"), [=]() mutable
            {
                auto result = validator->validateUrl(std::string("https://malware.com"));
                expect(result["valid"])->toBe(false);
                expect(result["error"])->toBe(std::string("Domain malware.com is blocked"));
            }
            );
            it(std::string("should enforce allowed domains when configured"), [=]() mutable
            {
                validator = std::make_shared<UrlValidator>(object{
                    object::pair{std::string("allowedDomains"), array<string>{ std::string("github.com"), std::string("example.com") }}
                });
                auto result1 = validator->validateUrl(std::string("https://github.com/user/repo"));
                expect(result1["valid"])->toBe(true);
                auto result2 = validator->validateUrl(std::string("https://google.com"));
                expect(result2["valid"])->toBe(false);
                expect(result2["error"])->toBe(std::string("Domain is not in the allowed list"));
            }
            );
            it(std::string("should allow subdomains of allowed domains"), [=]() mutable
            {
                validator = std::make_shared<UrlValidator>(object{
                    object::pair{std::string("allowedDomains"), array<string>{ std::string("example.com") }}
                });
                auto result = validator->validateUrl(std::string("https://api.example.com"));
                expect(result["valid"])->toBe(true);
            }
            );
        }
        );
        describe(std::string("config updates"), [=]() mutable
        {
            it(std::string("should update configuration"), [=]() mutable
            {
                validator->updateConfig(object{
                    object::pair{std::string("maxUrlLength"), 100}
                });
                auto longUrl = std::string("https://example.com/") + std::string("a")->repeat(100) + string_empty;
                auto result = validator->validateUrl(longUrl);
                expect(result["valid"])->toBe(false);
                expect(result["error"])->toBe(std::string("URL is too long"));
            }
            );
        }
        );
    }
    );
    describe(std::string("InputSanitizer"), [=]() mutable
    {
        describe(std::string("sanitizeText"), [=]() mutable
        {
            it(std::string("should remove HTML tags"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeText(std::string("<script>alert("xss")</script>"));
                expect(result)->toBe(std::string("scriptalert("xss")/script"));
            }
            );
            it(std::string("should remove javascript: protocol"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeText(std::string("javascript:alert("xss")"));
                expect(result)->toBe(std::string("alert("xss")"));
            }
            );
            it(std::string("should remove event handlers"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeText(std::string("onclick=alert("xss")"));
                expect(result)->toBe(std::string("alert("xss")"));
            }
            );
            it(std::string("should trim whitespace"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeText(std::string("  hello world  "));
                expect(result)->toBe(std::string("hello world"));
            }
            );
            it(std::string("should handle case insensitive patterns"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeText(std::string("JAVASCRIPT:alert("xss") OnClick=test"));
                expect(result)->toBe(std::string("alert("xss") test"));
            }
            );
        }
        );
        describe(std::string("sanitizeSelector"), [=]() mutable
        {
            it(std::string("should remove quotes"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeSelector(std::string("button[name="submit"]"));
                expect(result)->toBe(std::string("button[name=submit]"));
            }
            );
            it(std::string("should remove HTML tags"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeSelector(std::string("button<script>alert()</script>"));
                expect(result)->toBe(std::string("buttonscriptalert()/script"));
            }
            );
            it(std::string("should trim whitespace"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeSelector(std::string("  .submit-button  "));
                expect(result)->toBe(std::string(".submit-button"));
            }
            );
        }
        );
        describe(std::string("sanitizeFilePath"), [=]() mutable
        {
            it(std::string("should remove path traversal attempts"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeFilePath(std::string("../../../etc/passwd"));
                expect(result)->toBe(std::string("///etc/passwd"));
            }
            );
            it(std::string("should remove invalid filename characters"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeFilePath(std::string("file<>:"|?*name.txt"));
                expect(result)->toBe(std::string("filename.txt"));
            }
            );
            it(std::string("should trim whitespace"), [=]() mutable
            {
                auto result = InputSanitizer::sanitizeFilePath(std::string("  document.pdf  "));
                expect(result)->toBe(std::string("document.pdf"));
            }
            );
        }
        );
    }
    );
    describe(std::string("validateSecureAction"), [=]() mutable
    {
        shared<std::shared_ptr<UrlValidator>> validator;
        beforeEach([=]() mutable
        {
            validator = std::make_shared<UrlValidator>();
        }
        );
        it(std::string("should pass for valid URLs"), [=]() mutable
        {
            expect([=]() mutable
            {
                return validateSecureAction(std::string("https://example.com"), validator);
            }
            )->not->toThrow();
        }
        );
        it(std::string("should pass for null URLs"), [=]() mutable
        {
            expect([=]() mutable
            {
                return validateSecureAction(nullptr, validator);
            }
            )->not->toThrow();
        }
        );
        it(std::string("should throw BrowserSecurityError for invalid URLs"), [=]() mutable
        {
            expect([=]() mutable
            {
                return validateSecureAction(std::string("https://malware.com"), validator);
            }
            )->toThrow(BrowserSecurityError);
        }
        );
        it(std::string("should include error details"), [=]() mutable
        {
            try
            {
                validateSecureAction(std::string("invalid url"), validator);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(BrowserSecurityError);
                expect((as<std::shared_ptr<BrowserSecurityError>>(error))->details["url"])->toBe(std::string("invalid url"));
                expect((as<std::shared_ptr<BrowserSecurityError>>(error))->details["error"])->toBe(std::string("Invalid URL format"));
            }
        }
        );
    }
    );
    describe(std::string("defaultUrlValidator"), [=]() mutable
    {
        it(std::string("should be an instance of UrlValidator"), [=]() mutable
        {
            expect(defaultUrlValidator)->toBeInstanceOf(UrlValidator);
        }
        );
        it(std::string("should use default configuration"), [=]() mutable
        {
            auto result = defaultUrlValidator->validateUrl(std::string("https://example.com"));
            expect(result["valid"])->toBe(true);
        }
        );
    }
    );
    describe(std::string("RateLimiter"), [=]() mutable
    {
        shared<std::shared_ptr<RateLimiter>> rateLimiter;
        shared<double> now;
        beforeEach([=]() mutable
        {
            rateLimiter = std::make_shared<RateLimiter>(object{
                object::pair{std::string("maxActionsPerMinute"), 2}, 
                object::pair{std::string("maxSessionsPerHour"), 3}
            });
            now = Date->now();
            spyOn(Date, std::string("now"))->mockReturnValue(now);
        }
        );
        describe(std::string("action limits"), [=]() mutable
        {
            it(std::string("should allow actions within limit"), [=]() mutable
            {
                expect(rateLimiter->checkActionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std::string("user1")))->toBe(true);
            }
            );
            it(std::string("should reject actions over limit"), [=]() mutable
            {
                expect(rateLimiter->checkActionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std::string("user1")))->toBe(false);
            }
            );
            it(std::string("should reset after time window"), [=]() mutable
            {
                expect(rateLimiter->checkActionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std::string("user1")))->toBe(false);
                spyOn(Date, std::string("now"))->mockReturnValue(now + 60001);
                expect(rateLimiter->checkActionLimit(std::string("user1")))->toBe(true);
            }
            );
            it(std::string("should track different users separately"), [=]() mutable
            {
                expect(rateLimiter->checkActionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std::string("user2")))->toBe(true);
                expect(rateLimiter->checkActionLimit(std::string("user2")))->toBe(true);
            }
            );
        }
        );
        describe(std::string("session limits"), [=]() mutable
        {
            it(std::string("should allow sessions within limit"), [=]() mutable
            {
                expect(rateLimiter->checkSessionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std::string("user1")))->toBe(true);
            }
            );
            it(std::string("should reject sessions over limit"), [=]() mutable
            {
                expect(rateLimiter->checkSessionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std::string("user1")))->toBe(false);
            }
            );
            it(std::string("should reset after time window"), [=]() mutable
            {
                expect(rateLimiter->checkSessionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std::string("user1")))->toBe(false);
                spyOn(Date, std::string("now"))->mockReturnValue(now + 3600001);
                expect(rateLimiter->checkSessionLimit(std::string("user1")))->toBe(true);
            }
            );
            it(std::string("should track different users separately"), [=]() mutable
            {
                expect(rateLimiter->checkSessionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std::string("user1")))->toBe(true);
                expect(rateLimiter->checkSessionLimit(std::string("user2")))->toBe(true);
            }
            );
        }
        );
    }
    );
}

MAIN
