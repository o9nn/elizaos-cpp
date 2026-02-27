#include "/home/runner/work/elizaos-cpp/elizaos-cpp/src/model/capi/sessions-client.test.h"

void Main(void)
{
    global->fetch = jest->fn();
    describe(std:("SessionsClient Defensive Error Handling"), [=]() mutable
    {
        shared<std::shared_ptr<SessionsClient>> client;
        shared<std::shared_ptr<jest::MockedFunction<fetch>>> mockFetch;
        beforeEach([=]() mutable
        {
            mockFetch = as<std::shared_ptr<jest::MockedFunction<fetch>>>(fetch);
            mockFetch->mockClear();
            client = std::make_shared<SessionsClient>(std:("https://test-endpoint.com/logs"));
        }
        );
        shared sampleLogData = object{
            object::pair{std:("sessionId"), std:("test-session-123")}, 
            object::pair{std:("logs"), array<string>{ std:("Test log entry") }}, 
            object::pair{std:("timestamp"), Date->now()}, 
            object::pair{std:("metadata"), object{
                object::pair{std:("test"), true}
            }}
        };
        describe(std:("404 Error Handling"), [=]() mutable
        {
            it(std:("should handle 404 errors gracefully without throwing"), [=]() mutable
            {
                mockFetch->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), false}, 
                    object::pair{std:("status"), 404}, 
                    object::pair{std:("statusText"), std:("Not Found")}
                }));
                auto consoleSpy = jest->spyOn(console, std:("warn"))->mockImplementation();
                auto result = std::async([=]() { client->uploadSessionLog(sampleLogData); });
                expect(result)->toBe(false);
                expect(consoleSpy)->toHaveBeenCalledWith(std:("Session log endpoint returned 404 – endpoint not found or service unavailable"));
                expect(consoleSpy)->toHaveBeenCalledWith(std:("Continuing execution without failing the workflow..."));
                consoleSpy->mockRestore();
            }
            );
            it(std:("should not retry on 404 errors"), [=]() mutable
            {
                mockFetch->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), false}, 
                    object::pair{std:("status"), 404}, 
                    object::pair{std:("statusText"), std:("Not Found")}
                }));
                std::async([=]() { client->uploadSessionLog(sampleLogData, object{
                    object::pair{std:("retries"), 3}
                }); });
                expect(mockFetch)->toHaveBeenCalledTimes(1);
            }
            );
        }
        );
        describe(std:("Network Error Handling"), [=]() mutable
        {
            it(std:("should handle ECONNREFUSED errors gracefully"), [=]() mutable
            {
                auto networkError = std::make_shared<Error>(std:("Connection refused"));
                (as<any>(networkError))["code"] = std:("ECONNREFUSED");
                mockFetch->mockRejectedValueOnce(networkError);
                auto consoleSpy = jest->spyOn(console, std:("warn"))->mockImplementation();
                auto result = std::async([=]() { client->uploadSessionLog(sampleLogData); });
                expect(result)->toBe(false);
                expect(consoleSpy)->toHaveBeenCalledWith(expect->stringContaining(std:("Network error: Connection refused")));
                consoleSpy->mockRestore();
            }
            );
            it(std:("should handle ENOTFOUND errors gracefully"), [=]() mutable
            {
                auto networkError = std::make_shared<Error>(std:("Host not found"));
                (as<any>(networkError))["code"] = std:("ENOTFOUND");
                mockFetch->mockRejectedValueOnce(networkError);
                auto consoleSpy = jest->spyOn(console, std:("warn"))->mockImplementation();
                auto result = std::async([=]() { client->uploadSessionLog(sampleLogData); });
                expect(result)->toBe(false);
                expect(consoleSpy)->toHaveBeenCalledWith(std:("Continuing execution without failing the workflow..."));
                consoleSpy->mockRestore();
            }
            );
        }
        );
        describe(std:("Server Error Handling"), [=]() mutable
        {
            it(std:("should retry on 500 errors"), [=]() mutable
            {
                mockFetch->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), false}, 
                    object::pair{std:("status"), 500}, 
                    object::pair{std:("statusText"), std:("Internal Server Error")}
                }))->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 200}, 
                    object::pair{std:("statusText"), std:("OK")}
                }));
                auto result = std::async([=]() { client->uploadSessionLog(sampleLogData, object{
                    object::pair{std:("retries"), 2}
                }); });
                expect(result)->toBe(true);
                expect(mockFetch)->toHaveBeenCalledTimes(2);
            }
            );
            it(std:("should exhaust retries and handle gracefully"), [=]() mutable
            {
                mockFetch->mockResolvedValue(as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), false}, 
                    object::pair{std:("status"), 500}, 
                    object::pair{std:("statusText"), std:("Internal Server Error")}
                }));
                auto consoleSpy = jest->spyOn(console, std:("error"))->mockImplementation();
                auto warnSpy = jest->spyOn(console, std:("warn"))->mockImplementation();
                auto result = std::async([=]() { client->uploadSessionLog(sampleLogData, object{
                    object::pair{std:("retries"), 2}
                }); });
                expect(result)->toBe(false);
                expect(mockFetch)->toHaveBeenCalledTimes(2);
                expect(warnSpy)->toHaveBeenCalledWith(std:("Continuing execution despite session log upload failure..."));
                consoleSpy->mockRestore();
                warnSpy->mockRestore();
            }
            );
        }
        );
        describe(std:("Success Scenarios"), [=]() mutable
        {
            it(std:("should upload successfully on first attempt"), [=]() mutable
            {
                mockFetch->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 200}, 
                    object::pair{std:("statusText"), std:("OK")}
                }));
                auto consoleSpy = jest->spyOn(console, std:("log"))->mockImplementation();
                auto result = std::async([=]() { client->uploadSessionLog(sampleLogData); });
                expect(result)->toBe(true);
                expect(consoleSpy)->toHaveBeenCalledWith(std:("Session logs uploaded successfully"));
                consoleSpy->mockRestore();
            }
            );
        }
        );
        describe(std:("Configuration Handling"), [=]() mutable
        {
            it(std:("should handle missing endpoint gracefully"), [=]() mutable
            {
                auto clientWithoutEndpoint = std::make_shared<SessionsClient>(string_empty);
                auto consoleSpy = jest->spyOn(console, std:("warn"))->mockImplementation();
                auto result = std::async([=]() { clientWithoutEndpoint->uploadSessionLog(sampleLogData); });
                expect(result)->toBe(false);
                expect(consoleSpy)->toHaveBeenCalledWith(std:("Session log endpoint not configured - skipping upload"));
                consoleSpy->mockRestore();
            }
            );
        }
        );
        describe(std:("Fail on Error Option"), [=]() mutable
        {
            it(std:("should throw error when failOnError is true and 500 error occurs"), [=]() mutable
            {
                mockFetch->mockResolvedValue(as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), false}, 
                    object::pair{std:("status"), 500}, 
                    object::pair{std:("statusText"), std:("Internal Server Error")}
                }));
                std::async([=]() { expect(client->uploadSessionLog(sampleLogData, object{
                    object::pair{std:("failOnError"), true}, 
                    object::pair{std:("retries"), 1}
                }))->rejects->toThrow(std:("HTTP 500: Internal Server Error")); });
            }
            );
            it(std:("should NOT throw error when failOnError is true and 404 error occurs"), [=]() mutable
            {
                mockFetch->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), false}, 
                    object::pair{std:("status"), 404}, 
                    object::pair{std:("statusText"), std:("Not Found")}
                }));
                auto consoleSpy = jest->spyOn(console, std:("warn"))->mockImplementation();
                auto result = std::async([=]() { client->uploadSessionLog(sampleLogData, object{
                    object::pair{std:("failOnError"), true}
                }); });
                expect(result)->toBe(false);
                consoleSpy->mockRestore();
            }
            );
        }
        );
        describe(std:("Health Check"), [=]() mutable
        {
            it(std:("should return true for healthy endpoint"), [=]() mutable
            {
                mockFetch->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 200}
                }));
                auto result = std::async([=]() { client->healthCheck(); });
                expect(result)->toBe(true);
            }
            );
            it(std:("should return false for unhealthy endpoint"), [=]() mutable
            {
                mockFetch->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std:("ok"), false}, 
                    object::pair{std:("status"), 404}
                }));
                auto result = std::async([=]() { client->healthCheck(); });
                expect(result)->toBe(false);
            }
            );
            it(std:("should return false when endpoint is not configured"), [=]() mutable
            {
                auto clientWithoutEndpoint = std::make_shared<SessionsClient>(string_empty);
                auto result = std::async([=]() { clientWithoutEndpoint->healthCheck(); });
                expect(result)->toBe(false);
            }
            );
        }
        );
    }
    );
    describe(std:("CLI Integration"), [=]() mutable
    {
        it(std:("should handle successful CLI execution"), [=]() mutable
        {
            auto originalModule = require->main;
            auto originalEnv = process->env;
            process->env = utils::assign(object{
                , 
                object::pair{std:("GITHUB_RUN_ID"), std:("test-run-123")}, 
                object::pair{std:("GITHUB_WORKFLOW"), std:("test-workflow")}, 
                object::pair{std:("GITHUB_ACTOR"), std:("test-actor")}
            }, process->env);
            require->main = as<any>(object{
                object::pair{std:("filename"), __filename}
            });
            {
                utils::finally __finally8081_8159([&]() mutable
                {
                    require->main = originalModule;
                    process->env = originalEnv;
                });
                try
                {
                    auto exitSpy = jest->spyOn(process, std:("exit"))->mockImplementation();
                    auto mockFetch = jest->fn()->mockResolvedValue(object{
                        object::pair{std:("ok"), true}, 
                        object::pair{std:("status"), 200}
                    });
                    global->fetch = mockFetch;
                    expect(process->env->GITHUB_RUN_ID)->toBe(std:("test-run-123"));
                    exitSpy->mockRestore();
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        );
    }
    );
}

MAIN
