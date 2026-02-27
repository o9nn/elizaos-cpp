#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/commands/start/utils/__tests__/loader.test.h"

void Main(void)
{
    describe(std:("loader utils"), [=]() mutable
    {
        describe(std:("hasValidRemoteUrlsAsync"), [=]() mutable
        {
            shared originalEnv = process->env->REMOTE_CHARACTER_URLS;
            beforeEach([=]() mutable
            {
                process->env.Delete("REMOTE_CHARACTER_URLS");
            }
            );
            afterEach([=]() mutable
            {
                if (originalEnv != undefined) {
                    process->env->REMOTE_CHARACTER_URLS = originalEnv;
                } else {
                    process->env.Delete("REMOTE_CHARACTER_URLS");
                }
            }
            );
            it(std:("should return true for valid HTTP URLs"), [=]() mutable
            {
                process->env->REMOTE_CHARACTER_URLS = std:("https://example.com/character.json");
                auto result = std::async([=]() { hasValidRemoteUrlsAsync(); });
                expect(type_of(result))->toBe(std:("boolean"));
                expect(result)->toBe(true);
            }
            );
            it(std:("should return false for empty URLs"), [=]() mutable
            {
                process->env->REMOTE_CHARACTER_URLS = string_empty;
                auto result = std::async([=]() { hasValidRemoteUrlsAsync(); });
                expect(type_of(result))->toBe(std:("boolean"));
                expect(result)->toBe(false);
            }
            );
            it(std:("should return false for non-HTTP URLs"), [=]() mutable
            {
                process->env->REMOTE_CHARACTER_URLS = std:("file:///local/path.json");
                auto result = std::async([=]() { hasValidRemoteUrlsAsync(); });
                expect(type_of(result))->toBe(std:("boolean"));
                expect(result)->toBe(false);
            }
            );
            it(std:("should return false when environment variable not set"), [=]() mutable
            {
                auto result = std::async([=]() { hasValidRemoteUrlsAsync(); });
                expect(type_of(result))->toBe(std:("boolean"));
                expect(result)->toBe(false);
            }
            );
            it(std:("should be asynchronous and return a Promise"), [=]() mutable
            {
                process->env->REMOTE_CHARACTER_URLS = std:("https://example.com/character.json");
                auto resultPromise = hasValidRemoteUrlsAsync();
                expect(resultPromise)->toHaveProperty(std:("then"));
                expect(resultPromise)->toHaveProperty(std:("catch"));
                expect(resultPromise)->toBeInstanceOf(Promise);
                auto result = std::async([=]() { resultPromise; });
                expect(type_of(result))->toBe(std:("boolean"));
            }
            );
        }
        );
    }
    );
}

MAIN
