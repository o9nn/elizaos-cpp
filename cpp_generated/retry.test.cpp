#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/src/__tests__/retry.test.h"

void Main(void)
{
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std::string("logger"), object{
                object::pair{std::string("info"), mock()}, 
                object::pair{std::string("warn"), mock()}, 
                object::pair{std::string("error"), mock()}
            }}
        });
    }
    );
    describe(std::string("retry utilities"), [=]() mutable
    {
        shared unhandledRejections = array<any>();
        beforeEach([=]() mutable
        {
            mock->restore();
            unhandledRejections = array<any>();
        }
        );
        afterEach([=]() mutable
        {
            unhandledRejections = array<any>();
        }
        );
        shared originalConsoleError = console->error;
        shared unhandledRejectionHandler = [=](auto reason) mutable
        {
            unhandledRejections->push(reason);
        };
        beforeAll([=]() mutable
        {
            process->on(std::string("unhandledRejection"), unhandledRejectionHandler);
            console->error = [=](Args... args_) mutable
            {
                array<any> args = array<any>{args_...};
                if (AND((const_(args)[0]["includes"]), ((OR((const_(args)[0]["includes"](std::string("PromiseRejectionHandledWarning"))), (const_(args)[0]["includes"](std::string("test operation timed out")))))))) {
                    return;
                }
                originalConsoleError();
            };
        }
        );
        afterAll([=]() mutable
        {
            process->off(std::string("unhandledRejection"), unhandledRejectionHandler);
            console->error = originalConsoleError;
        }
        );
        describe(std::string("retryWithBackoff"), [=]() mutable
        {
            it(std::string("should succeed on first attempt"), [=]() mutable
            {
                auto fn = mock()->mockResolvedValue(std::string("success"));
                auto result = std::async([=]() { retryWithBackoff(fn, object{}, std::string("test operation")); });
                expect(result)->toBe(std::string("success"));
                expect(fn)->toHaveBeenCalledTimes(1);
                expect(logger->info)->toHaveBeenCalledWith(std::string("Attempting test operation (attempt 1/3)"));
            }
            );
            it(std::string("should retry on failure and succeed"), [=]() mutable
            {
                auto fn = mock()->mockRejectedValueOnce(std::make_shared<Error>(std::string("net::ERR_CONNECTION_REFUSED")))->mockResolvedValueOnce(std::string("success"));
                auto result = std::async([=]() { retryWithBackoff(fn, object{
                    object::pair{std::string("maxRetries"), 3}, 
                    object::pair{std::string("initialDelay"), 100}
                }, std::string("test operation")); });
                expect(result)->toBe(std::string("success"));
                expect(fn)->toHaveBeenCalledTimes(2);
                expect(logger->warn)->toHaveBeenCalled();
            }
            );
            it(std::string("should fail after max retries"), [=]() mutable
            {
                auto error = std::make_shared<Error>(std::string("ETIMEDOUT"));
                auto fn = mock()->mockRejectedValue(error);
                std::async([=]() { expect(retryWithBackoff(fn, object{
                    object::pair{std::string("maxRetries"), 2}, 
                    object::pair{std::string("initialDelay"), 100}
                }, std::string("test operation")))->rejects->toThrow(std::string("ETIMEDOUT")); });
                expect(fn)->toHaveBeenCalledTimes(2);
                expect(logger->error)->toHaveBeenCalledWith(std::string("test operation failed after 2 attempts"));
            }
            );
            it(std::string("should not retry non-retryable errors"), [=]() mutable
            {
                auto error = std::make_shared<Error>(std::string("Invalid credentials"));
                auto fn = mock()->mockRejectedValue(error);
                std::async([=]() { expect(retryWithBackoff(fn, object{}, std::string("test operation")))->rejects->toThrow(std::string("Invalid credentials")); });
                expect(fn)->toHaveBeenCalledTimes(3);
                expect(logger->error)->toHaveBeenCalledWith(std::string("test operation failed after 3 attempts"));
            }
            );
            it(std::string("should handle timeout"), [=]() mutable
            {
                auto fn = mock()->mockImplementation([=]() mutable
                {
                    return std::make_shared<Promise>([=](auto resolve) mutable
                    {
                        return setTimeout([=]() mutable
                        {
                            return resolve(std::string("success"));
                        }
                        , 2000);
                    }
                    );
                }
                );
                std::async([=]() { expect(retryWithBackoff(fn, object{
                    object::pair{std::string("timeout"), 500}
                }, std::string("test operation")))->rejects->toThrow(std::string("test operation timed out after 500ms")); });
                expect(fn)->toHaveBeenCalledTimes(3);
            }
            , 10000);
            it(std::string("should apply exponential backoff"), [=]() mutable
            {
                shared callCount = 0;
                auto fn = mock()->mockImplementation([=]() mutable
                {
                    callCount++;
                    if (callCount < 3) {
                        return Promise->reject(std::make_shared<Error>(std::string("Timeout")));
                    }
                    return Promise->resolve(std::string("success"));
                }
                );
                auto startTime = Date->now();
                auto result = std::async([=]() { retryWithBackoff(fn, object{
                    object::pair{std::string("maxRetries"), 3}, 
                    object::pair{std::string("initialDelay"), 10}, 
                    object::pair{std::string("backoffMultiplier"), 2}
                }, std::string("test")); });
                expect(result)->toBe(std::string("success"));
                expect(fn)->toHaveBeenCalledTimes(3);
                auto totalTime = Date->now() - startTime;
                expect(totalTime)->toBeGreaterThan(25);
            }
            );
            it(std::string("should respect maxDelay"), [=]() mutable
            {
                auto fn = mock()->mockRejectedValueOnce(std::make_shared<Error>(std::string("Timeout")))->mockResolvedValueOnce(std::string("success"));
                auto result = std::async([=]() { retryWithBackoff(fn, object{
                    object::pair{std::string("maxRetries"), 2}, 
                    object::pair{std::string("initialDelay"), 2000}, 
                    object::pair{std::string("maxDelay"), 2500}, 
                    object::pair{std::string("backoffMultiplier"), 2}
                }, std::string("test")); });
                expect(result)->toBe(std::string("success"));
                expect(fn)->toHaveBeenCalledTimes(2);
            }
            , 10000);
        }
        );
        describe(std::string("browserRetryConfigs"), [=]() mutable
        {
            it(std::string("should have navigation config"), [=]() mutable
            {
                auto config = browserRetryConfigs["navigation"];
                expect(config["maxRetries"])->toBe(3);
                expect(config["initialDelay"])->toBe(1000);
                expect(config["maxDelay"])->toBe(5000);
                expect(config["backoffMultiplier"])->toBe(2);
            }
            );
            it(std::string("should have action config"), [=]() mutable
            {
                auto config = browserRetryConfigs["action"];
                expect(config["maxRetries"])->toBe(2);
                expect(config["initialDelay"])->toBe(500);
                expect(config["maxDelay"])->toBe(2000);
                expect(config["backoffMultiplier"])->toBe(1.5);
            }
            );
            it(std::string("should have extraction config"), [=]() mutable
            {
                auto config = browserRetryConfigs["extraction"];
                expect(config["maxRetries"])->toBe(2);
                expect(config["initialDelay"])->toBe(500);
                expect(config["maxDelay"])->toBe(3000);
                expect(config["backoffMultiplier"])->toBe(2);
            }
            );
        }
        );
    }
    );
}

MAIN
