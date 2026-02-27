#include "loader.test.h"
#include <string>

void Main(void)
{
    describe(std::string("loader utils"), [=]() mutable
    {
        describe(std::string("hasValidRemoteUrlsAsync"), [=]() mutable
        {
            shared originalEnv = process->env->REMOTE_CHARACTER_URLS;
            beforeEach([=]() mutable
            {
                process->env.Delete("REMOTE_CHARACTER_URLS");
            }
            );
            afterEach([=]() mutable
            {
                if (originalEnv != std::nullopt) {
                    process->env->REMOTE_CHARACTER_URLS = originalEnv;
                } else {
                    process->env.Delete("REMOTE_CHARACTER_URLS");
                }
            }
            );
            it(std::string("should return true for valid HTTP URLs"), [=]() mutable
            {
                process->env->REMOTE_CHARACTER_URLS = std::string("https://example.com/character.json");
                auto result = std::async([=]() { hasValidRemoteUrlsAsync(); });
                expect(type_of(result))->toBe(std::string("boolean"));
                expect(result)->toBe(true);
            }
            );
            it(std::string("should return false for empty URLs"), [=]() mutable
            {
                process->env->REMOTE_CHARACTER_URLS = string_empty;
                auto result = std::async([=]() { hasValidRemoteUrlsAsync(); });
                expect(type_of(result))->toBe(std::string("boolean"));
                expect(result)->toBe(false);
            }
            );
            it(std::string("should return false for non-HTTP URLs"), [=]() mutable
            {
                process->env->REMOTE_CHARACTER_URLS = std::string("file:///local/path.json");
                auto result = std::async([=]() { hasValidRemoteUrlsAsync(); });
                expect(type_of(result))->toBe(std::string("boolean"));
                expect(result)->toBe(false);
            }
            );
            it(std::string("should return false when environment variable not set"), [=]() mutable
            {
                auto result = std::async([=]() { hasValidRemoteUrlsAsync(); });
                expect(type_of(result))->toBe(std::string("boolean"));
                expect(result)->toBe(false);
            }
            );
            it(std::string("should be asynchronous and return a Promise"), [=]() mutable
            {
                process->env->REMOTE_CHARACTER_URLS = std::string("https://example.com/character.json");
                auto resultPromise = hasValidRemoteUrlsAsync();
                expect(resultPromise)->toHaveProperty(std::string("then"));
                expect(resultPromise)->toHaveProperty(std::string("catch"));
                expect(resultPromise)->toBeInstanceOf(Promise);
                auto result = std::async([=]() { resultPromise; });
                expect(type_of(result))->toBe(std::string("boolean"));
            }
            );
        }
        );
    }
    );
}

MAIN
