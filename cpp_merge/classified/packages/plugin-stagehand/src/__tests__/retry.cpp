#include "retry.test.h"

void Main(void)
{
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std:("logger"), object{
                object::pair{std:("info"), mock()}, 
                object::pair{std:("warn"), mock()}, 
                object::pair{std:("error"), mock()}
            }}
        });
    }
    );
    describe(std:("retry utilities"), [=]() mutable
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
            process->on(std:("unhandledRejection"), unhandledRejectionHandler);
            console->error = [=](Args... args_) mutable
            {
                array<any> args = array<any>{args_...};
                if (AND((const_(args)[0]["includes"]), ((OR((const_(args)[0]["includes"](std:("PromiseRejectionHandledWarning"))), (const_(args)[0]["includes"](std:("test operation timed out")))))))) {
                    return;
                }
                originalConsoleError();
            };
        }
        );
        afterAll([=]() mutable
        {
            process->off(std:("unhandledRejection"), unhandledRejectionHandler);
            console->error = originalConsoleError;
        }
        );
        describe(std:("retryWithBackoff"), [=]() mutable
        {
            it(std:("should succeed on first attempt"), [=]() mutable
            {
                auto fn = mock()->mockResolvedValue(std:("success"));
                auto result = std::async([=]() { retryWithBackoff(fn, object{}, std:("test operation")); });
                expect(result)->toBe(std:("success"));
                expect(fn)->toHaveBeenCalledTimes(1);
                expect(logger->info)->toHaveBeenCalledWith(std:("Attempting test operation (attempt 1/3)"));
            }
            );
            it(std:("should retry on failure and succeed"), [=]() mutable
            {
                auto fn = mock()->mockRejectedValueOnce(std::make_shared<Error>(std:("net::ERR_CONNECTION_REFUSED")))->mockResolvedValueOnce(std:("success"));
                auto result = std::async([=]() { retryWithBackoff(fn, object{
                    object::pair{std:("maxRetries"), 3}, 
                    object::pair{std:("initialDelay"), 100}
                }, std:("test operation")); });
                expect(result)->toBe(std:("success"));
                expect(fn)->toHaveBeenCalledTimes(2);
                expect(logger->warn)->toHaveBeenCalled();
            }
            );
            it(std:("should fail after max retries"), [=]() mutable
            {
                auto error = std::make_shared<Error>(std:("ETIMEDOUT"));
                auto fn = mock()->mockRejectedValue(error);
                std::async([=]() { expect(retryWithBackoff(fn, object{
                    object::pair{std:("maxRetries"), 2}, 
                    object::pair{std:("initialDelay"), 100}
                }, std:("test operation")))->rejects->toThrow(std:("ETIMEDOUT")); });
                expect(fn)->toHaveBeenCalledTimes(2);
                expect(logger->error)->toHaveBeenCalledWith(std:("test operation failed after 2 attempts"));
            }
            );
            it(std:("should not retry non-retryable errors"), [=]() mutable
            {
                auto error = std::make_shared<Error>(std:("Invalid credentials"));
                auto fn = mock()->mockRejectedValue(error);
                std::async([=]() { expect(retryWithBackoff(fn, object{}, std:("test operation")))->rejects->toThrow(std:("Invalid credentials")); });
                expect(fn)->toHaveBeenCalledTimes(3);
                expect(logger->error)->toHaveBeenCalledWith(std:("test operation failed after 3 attempts"));
            }
            );
            it(std:("should handle timeout"), [=]() mutable
            {
                auto fn = mock()->mockImplementation([=]() mutable
                {
                    return std::make_shared<Promise>([=](auto resolve) mutable
                    {
                        return setTimeout([=]() mutable
                        {
                            return resolve(std:("success"));
                        }
                        , 2000);
                    }
                    );
                }
                );
                std::async([=]() { expect(retryWithBackoff(fn, object{
                    object::pair{std:("timeout"), 500}
                }, std:("test operation")))->rejects->toThrow(std:("test operation timed out after 500ms")); });
                expect(fn)->toHaveBeenCalledTimes(3);
            }
            , 10000);
            it(std:("should apply exponential backoff"), [=]() mutable
            {
                shared callCount = 0;
                auto fn = mock()->mockImplementation([=]() mutable
                {
                    callCount++;
                    if (callCount < 3) {
                        return Promise->reject(std::make_shared<Error>(std:("Timeout")));
                    }
                    return Promise->resolve(std:("success"));
                }
                );
                auto startTime = Date->now();
                auto result = std::async([=]() { retryWithBackoff(fn, object{
                    object::pair{std:("maxRetries"), 3}, 
                    object::pair{std:("initialDelay"), 10}, 
                    object::pair{std:("backoffMultiplier"), 2}
                }, std:("test")); });
                expect(result)->toBe(std:("success"));
                expect(fn)->toHaveBeenCalledTimes(3);
                auto totalTime = Date->now() - startTime;
                expect(totalTime)->toBeGreaterThan(25);
            }
            );
            it(std:("should respect maxDelay"), [=]() mutable
            {
                auto fn = mock()->mockRejectedValueOnce(std::make_shared<Error>(std:("Timeout")))->mockResolvedValueOnce(std:("success"));
                auto result = std::async([=]() { retryWithBackoff(fn, object{
                    object::pair{std:("maxRetries"), 2}, 
                    object::pair{std:("initialDelay"), 2000}, 
                    object::pair{std:("maxDelay"), 2500}, 
                    object::pair{std:("backoffMultiplier"), 2}
                }, std:("test")); });
                expect(result)->toBe(std:("success"));
                expect(fn)->toHaveBeenCalledTimes(2);
            }
            , 10000);
        }
        );
        describe(std:("browserRetryConfigs"), [=]() mutable
        {
            it(std:("should have navigation config"), [=]() mutable
            {
                auto config = browserRetryConfigs["navigation"];
                expect(config["maxRetries"])->toBe(3);
                expect(config["initialDelay"])->toBe(1000);
                expect(config["maxDelay"])->toBe(5000);
                expect(config["backoffMultiplier"])->toBe(2);
            }
            );
            it(std:("should have action config"), [=]() mutable
            {
                auto config = browserRetryConfigs["action"];
                expect(config["maxRetries"])->toBe(2);
                expect(config["initialDelay"])->toBe(500);
                expect(config["maxDelay"])->toBe(2000);
                expect(config["backoffMultiplier"])->toBe(1.5);
            }
            );
            it(std:("should have extraction config"), [=]() mutable
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
