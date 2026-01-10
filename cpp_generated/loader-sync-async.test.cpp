#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/unit/utils/loader-sync-async.test.h"

void Main(void)
{
    mock->module(std::string("@/src/utils/module-loader"), [=]() mutable
    {
        return (object{
            object::pair{std::string("loadModule"), mock([=]() mutable
            {
                return (object{
                    object::pair{std::string("tryLoadFile"), [=](auto filePath) mutable
                    {
                        if (filePath == std::string("/test/file.txt")) {
                            return std::string("test content");
                        }
                        if (filePath == std::string("/empty/file.txt")) {
                            return nullptr;
                        }
                        throw any(std::make_shared<Error>(std::string("Error loading file ") + filePath + std::string(": File not found")));
                    }
                    }, 
                    object::pair{std::string("hasValidRemoteUrls"), [=]() mutable
                    {
                        auto urls = process->env->REMOTE_CHARACTER_URLS;
                        return AND((AND((urls), (urls != string_empty))), (urls->toLowerCase()->startsWith(std::string("http"))));
                    }
                    }
                });
            }
            )}, 
            object::pair{std::string("loadModuleSync"), mock([=]() mutable
            {
                return (object{
                    object::pair{std::string("tryLoadFile"), [=](auto filePath) mutable
                    {
                        if (filePath == std::string("/test/file.txt")) {
                            return std::string("test content");
                        }
                        if (filePath == std::string("/empty/file.txt")) {
                            return nullptr;
                        }
                        throw any(std::make_shared<Error>(std::string("Error loading file ") + filePath + std::string(": File not found")));
                    }
                    }, 
                    object::pair{std::string("hasValidRemoteUrls"), [=]() mutable
                    {
                        auto urls = process->env->REMOTE_CHARACTER_URLS;
                        return AND((AND((urls), (urls != string_empty))), (urls->toLowerCase()->startsWith(std::string("http"))));
                    }
                    }
                });
            }
            )}
        });
    }
    );
    describe(std::string("loader utils - synchronous and asynchronous functions"), [=]() mutable
    {
        describe(std::string("tryLoadFile"), [=]() mutable
        {
            describe(std::string("synchronous version"), [=]() mutable
            {
                test(std::string("should load file content successfully"), [=]() mutable
                {
                    auto result = tryLoadFile(std::string("/test/file.txt"));
                    expect(result)->toBe(std::string("test content"));
                    expect(type_of(result))->toBe(std::string("string"));
                }
                );
                test(std::string("should return null for empty file"), [=]() mutable
                {
                    auto result = tryLoadFile(std::string("/empty/file.txt"));
                    expect(result)->toBeNull();
                }
                );
                test(std::string("should throw error when file loading fails"), [=]() mutable
                {
                    expect([=]() mutable
                    {
                        return tryLoadFile(std::string("/nonexistent/file.txt"));
                    }
                    )->toThrow(std::string("Error loading file /nonexistent/file.txt: File not found"));
                }
                );
                test(std::string("should not return a Promise"), [=]() mutable
                {
                    auto result = tryLoadFile(std::string("/test/file.txt"));
                    expect(result)->not->toHaveProperty(std::string("then"));
                    expect(result)->not->toHaveProperty(std::string("catch"));
                    expect(result)->not->toBeInstanceOf(Promise);
                }
                );
            }
            );
            describe(std::string("asynchronous version"), [=]() mutable
            {
                test(std::string("should load file content successfully"), [=]() mutable
                {
                    auto result = std::async([=]() { tryLoadFileAsync(std::string("/test/file.txt")); });
                    expect(result)->toBe(std::string("test content"));
                    expect(type_of(result))->toBe(std::string("string"));
                }
                );
                test(std::string("should return null for empty file"), [=]() mutable
                {
                    auto result = std::async([=]() { tryLoadFileAsync(std::string("/empty/file.txt")); });
                    expect(result)->toBeNull();
                }
                );
                test(std::string("should throw error when file loading fails"), [=]() mutable
                {
                    std::async([=]() { expect(tryLoadFileAsync(std::string("/nonexistent/file.txt")))->rejects->toThrow(std::string("Error loading file /nonexistent/file.txt: File not found")); });
                }
                );
                test(std::string("should return a Promise"), [=]() mutable
                {
                    auto resultPromise = tryLoadFileAsync(std::string("/test/file.txt"));
                    expect(resultPromise)->toHaveProperty(std::string("then"));
                    expect(resultPromise)->toHaveProperty(std::string("catch"));
                    expect(resultPromise)->toBeInstanceOf(Promise);
                }
                );
            }
            );
        }
        );
        describe(std::string("hasValidRemoteUrls"), [=]() mutable
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
            describe(std::string("synchronous version"), [=]() mutable
            {
                test(std::string("should return true for valid HTTP URLs"), [=]() mutable
                {
                    process->env->REMOTE_CHARACTER_URLS = std::string("https://example.com/character.json");
                    auto result = hasValidRemoteUrls();
                    expect(result)->toBe(true);
                    expect(type_of(result))->toBe(std::string("boolean"));
                }
                );
                test(std::string("should return true for valid HTTP URLs (lowercase)"), [=]() mutable
                {
                    process->env->REMOTE_CHARACTER_URLS = std::string("http://example.com/character.json");
                    auto result = hasValidRemoteUrls();
                    expect(result)->toBe(true);
                }
                );
                test(std::string("should return false for empty URLs"), [=]() mutable
                {
                    process->env->REMOTE_CHARACTER_URLS = string_empty;
                    auto result = hasValidRemoteUrls();
                    expect(result)->toBe(false);
                }
                );
                test(std::string("should return false for non-HTTP URLs"), [=]() mutable
                {
                    process->env->REMOTE_CHARACTER_URLS = std::string("file:///local/path.json");
                    auto result = hasValidRemoteUrls();
                    expect(result)->toBe(false);
                }
                );
                test(std::string("should return false when environment variable not set"), [=]() mutable
                {
                    auto result = hasValidRemoteUrls();
                    expect(result)->toBe(false);
                }
                );
                test(std::string("should not return a Promise"), [=]() mutable
                {
                    process->env->REMOTE_CHARACTER_URLS = std::string("https://example.com/character.json");
                    auto result = hasValidRemoteUrls();
                    expect(result)->not->toHaveProperty(std::string("then"));
                    expect(result)->not->toHaveProperty(std::string("catch"));
                    expect(result)->not->toBeInstanceOf(Promise);
                    expect(type_of(result))->toBe(std::string("boolean"));
                }
                );
            }
            );
            describe(std::string("asynchronous version"), [=]() mutable
            {
                test(std::string("should return true for valid HTTP URLs"), [=]() mutable
                {
                    process->env->REMOTE_CHARACTER_URLS = std::string("https://example.com/character.json");
                    auto result = std::async([=]() { hasValidRemoteUrlsAsync(); });
                    expect(result)->toBe(true);
                    expect(type_of(result))->toBe(std::string("boolean"));
                }
                );
                test(std::string("should return true for valid HTTP URLs (lowercase)"), [=]() mutable
                {
                    process->env->REMOTE_CHARACTER_URLS = std::string("http://example.com/character.json");
                    auto result = std::async([=]() { hasValidRemoteUrlsAsync(); });
                    expect(result)->toBe(true);
                }
                );
                test(std::string("should return false for empty URLs"), [=]() mutable
                {
                    process->env->REMOTE_CHARACTER_URLS = string_empty;
                    auto result = std::async([=]() { hasValidRemoteUrlsAsync(); });
                    expect(result)->toBe(false);
                }
                );
                test(std::string("should return false for non-HTTP URLs"), [=]() mutable
                {
                    process->env->REMOTE_CHARACTER_URLS = std::string("file:///local/path.json");
                    auto result = std::async([=]() { hasValidRemoteUrlsAsync(); });
                    expect(result)->toBe(false);
                }
                );
                test(std::string("should return false when environment variable not set"), [=]() mutable
                {
                    auto result = std::async([=]() { hasValidRemoteUrlsAsync(); });
                    expect(result)->toBe(false);
                }
                );
                test(std::string("should return a Promise"), [=]() mutable
                {
                    process->env->REMOTE_CHARACTER_URLS = std::string("https://example.com/character.json");
                    auto resultPromise = hasValidRemoteUrlsAsync();
                    expect(resultPromise)->toHaveProperty(std::string("then"));
                    expect(resultPromise)->toHaveProperty(std::string("catch"));
                    expect(resultPromise)->toBeInstanceOf(Promise);
                }
                );
            }
            );
        }
        );
        describe(std::string("backward compatibility"), [=]() mutable
        {
            test(std::string("synchronous functions should maintain the same API as before"), [=]() mutable
            {
                process->env->REMOTE_CHARACTER_URLS = std::string("https://example.com/character.json");
                auto urlResult = hasValidRemoteUrls();
                expect(urlResult)->toBe(true);
                auto fileResult = tryLoadFile(std::string("/test/file.txt"));
                expect(fileResult)->toBe(std::string("test content"));
                expect(urlResult == true)->toBe(true);
                expect(fileResult == std::string("test content"))->toBe(true);
            }
            );
            test(std::string("async functions should be available for new code"), [=]() mutable
            {
                process->env->REMOTE_CHARACTER_URLS = std::string("https://example.com/character.json");
                auto urlResult = std::async([=]() { hasValidRemoteUrlsAsync(); });
                expect(urlResult)->toBe(true);
                auto fileResult = std::async([=]() { tryLoadFileAsync(std::string("/test/file.txt")); });
                expect(fileResult)->toBe(std::string("test content"));
            }
            );
        }
        );
    }
    );
}

MAIN
