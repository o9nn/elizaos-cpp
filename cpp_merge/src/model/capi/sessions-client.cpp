#include "/home/runner/work/elizaos-cpp/elizaos-cpp/src/model/capi/sessions-client.test.h"

void Main(void)
{
    global->fetch = jest->fn();
    describe(std::string("SessionsClient Defensive Error Handling"), [=]() mutable
    {
        shared<std::shared_ptr<SessionsClient>> client;
        shared<std::shared_ptr<jest::MockedFunction<fetch>>> mockFetch;
        beforeEach([=]() mutable
        {
            mockFetch = as<std::shared_ptr<jest::MockedFunction<fetch>>>(fetch);
            mockFetch->mockClear();
            client = std::make_shared<SessionsClient>(std::string("https://test-endpoint.com/logs"));
        }
        );
        shared sampleLogData = object{
            object::pair{std::string("sessionId"), std::string("test-session-123")}, 
            object::pair{std::string("logs"), array<string>{ std::string("Test log entry") }}, 
            object::pair{std::string("timestamp"), Date->now()}, 
            object::pair{std::string("metadata"), object{
                object::pair{std::string("test"), true}
            }}
        };
        describe(std::string("404 Error Handling"), [=]() mutable
        {
            it(std::string("should handle 404 errors gracefully without throwing"), [=]() mutable
            {
                mockFetch->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), false}, 
                    object::pair{std::string("status"), 404}, 
                    object::pair{std::string("statusText"), std::string("Not Found")}
                }));
                auto consoleSpy = jest->spyOn(console, std::string("warn"))->mockImplementation();
                auto result = std::async([=]() { client->uploadSessionLog(sampleLogData); });
                expect(result)->toBe(false);
                expect(consoleSpy)->toHaveBeenCalledWith(std::string("Session log endpoint returned 404 – endpoint not found or service unavailable"));
                expect(consoleSpy)->toHaveBeenCalledWith(std::string("Continuing execution without failing the workflow..."));
                consoleSpy->mockRestore();
            }
            );
            it(std::string("should not retry on 404 errors"), [=]() mutable
            {
                mockFetch->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), false}, 
                    object::pair{std::string("status"), 404}, 
                    object::pair{std::string("statusText"), std::string("Not Found")}
                }));
                std::async([=]() { client->uploadSessionLog(sampleLogData, object{
                    object::pair{std::string("retries"), 3}
                }); });
                expect(mockFetch)->toHaveBeenCalledTimes(1);
            }
            );
        }
        );
        describe(std::string("Network Error Handling"), [=]() mutable
        {
            it(std::string("should handle ECONNREFUSED errors gracefully"), [=]() mutable
            {
                auto networkError = std::make_shared<Error>(std::string("Connection refused"));
                (as<any>(networkError))["code"] = std::string("ECONNREFUSED");
                mockFetch->mockRejectedValueOnce(networkError);
                auto consoleSpy = jest->spyOn(console, std::string("warn"))->mockImplementation();
                auto result = std::async([=]() { client->uploadSessionLog(sampleLogData); });
                expect(result)->toBe(false);
                expect(consoleSpy)->toHaveBeenCalledWith(expect->stringContaining(std::string("Network error: Connection refused")));
                consoleSpy->mockRestore();
            }
            );
            it(std::string("should handle ENOTFOUND errors gracefully"), [=]() mutable
            {
                auto networkError = std::make_shared<Error>(std::string("Host not found"));
                (as<any>(networkError))["code"] = std::string("ENOTFOUND");
                mockFetch->mockRejectedValueOnce(networkError);
                auto consoleSpy = jest->spyOn(console, std::string("warn"))->mockImplementation();
                auto result = std::async([=]() { client->uploadSessionLog(sampleLogData); });
                expect(result)->toBe(false);
                expect(consoleSpy)->toHaveBeenCalledWith(std::string("Continuing execution without failing the workflow..."));
                consoleSpy->mockRestore();
            }
            );
        }
        );
        describe(std::string("Server Error Handling"), [=]() mutable
        {
            it(std::string("should retry on 500 errors"), [=]() mutable
            {
                mockFetch->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), false}, 
                    object::pair{std::string("status"), 500}, 
                    object::pair{std::string("statusText"), std::string("Internal Server Error")}
                }))->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 200}, 
                    object::pair{std::string("statusText"), std::string("OK")}
                }));
                auto result = std::async([=]() { client->uploadSessionLog(sampleLogData, object{
                    object::pair{std::string("retries"), 2}
                }); });
                expect(result)->toBe(true);
                expect(mockFetch)->toHaveBeenCalledTimes(2);
            }
            );
            it(std::string("should exhaust retries and handle gracefully"), [=]() mutable
            {
                mockFetch->mockResolvedValue(as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), false}, 
                    object::pair{std::string("status"), 500}, 
                    object::pair{std::string("statusText"), std::string("Internal Server Error")}
                }));
                auto consoleSpy = jest->spyOn(console, std::string("error"))->mockImplementation();
                auto warnSpy = jest->spyOn(console, std::string("warn"))->mockImplementation();
                auto result = std::async([=]() { client->uploadSessionLog(sampleLogData, object{
                    object::pair{std::string("retries"), 2}
                }); });
                expect(result)->toBe(false);
                expect(mockFetch)->toHaveBeenCalledTimes(2);
                expect(warnSpy)->toHaveBeenCalledWith(std::string("Continuing execution despite session log upload failure..."));
                consoleSpy->mockRestore();
                warnSpy->mockRestore();
            }
            );
        }
        );
        describe(std::string("Success Scenarios"), [=]() mutable
        {
            it(std::string("should upload successfully on first attempt"), [=]() mutable
            {
                mockFetch->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 200}, 
                    object::pair{std::string("statusText"), std::string("OK")}
                }));
                auto consoleSpy = jest->spyOn(console, std::string("log"))->mockImplementation();
                auto result = std::async([=]() { client->uploadSessionLog(sampleLogData); });
                expect(result)->toBe(true);
                expect(consoleSpy)->toHaveBeenCalledWith(std::string("Session logs uploaded successfully"));
                consoleSpy->mockRestore();
            }
            );
        }
        );
        describe(std::string("Configuration Handling"), [=]() mutable
        {
            it(std::string("should handle missing endpoint gracefully"), [=]() mutable
            {
                auto clientWithoutEndpoint = std::make_shared<SessionsClient>(string_empty);
                auto consoleSpy = jest->spyOn(console, std::string("warn"))->mockImplementation();
                auto result = std::async([=]() { clientWithoutEndpoint->uploadSessionLog(sampleLogData); });
                expect(result)->toBe(false);
                expect(consoleSpy)->toHaveBeenCalledWith(std::string("Session log endpoint not configured - skipping upload"));
                consoleSpy->mockRestore();
            }
            );
        }
        );
        describe(std::string("Fail on Error Option"), [=]() mutable
        {
            it(std::string("should throw error when failOnError is true and 500 error occurs"), [=]() mutable
            {
                mockFetch->mockResolvedValue(as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), false}, 
                    object::pair{std::string("status"), 500}, 
                    object::pair{std::string("statusText"), std::string("Internal Server Error")}
                }));
                std::async([=]() { expect(client->uploadSessionLog(sampleLogData, object{
                    object::pair{std::string("failOnError"), true}, 
                    object::pair{std::string("retries"), 1}
                }))->rejects->toThrow(std::string("HTTP 500: Internal Server Error")); });
            }
            );
            it(std::string("should NOT throw error when failOnError is true and 404 error occurs"), [=]() mutable
            {
                mockFetch->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), false}, 
                    object::pair{std::string("status"), 404}, 
                    object::pair{std::string("statusText"), std::string("Not Found")}
                }));
                auto consoleSpy = jest->spyOn(console, std::string("warn"))->mockImplementation();
                auto result = std::async([=]() { client->uploadSessionLog(sampleLogData, object{
                    object::pair{std::string("failOnError"), true}
                }); });
                expect(result)->toBe(false);
                consoleSpy->mockRestore();
            }
            );
        }
        );
        describe(std::string("Health Check"), [=]() mutable
        {
            it(std::string("should return true for healthy endpoint"), [=]() mutable
            {
                mockFetch->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), true}, 
                    object::pair{std::string("status"), 200}
                }));
                auto result = std::async([=]() { client->healthCheck(); });
                expect(result)->toBe(true);
            }
            );
            it(std::string("should return false for unhealthy endpoint"), [=]() mutable
            {
                mockFetch->mockResolvedValueOnce(as<std::shared_ptr<Response>>(object{
                    object::pair{std::string("ok"), false}, 
                    object::pair{std::string("status"), 404}
                }));
                auto result = std::async([=]() { client->healthCheck(); });
                expect(result)->toBe(false);
            }
            );
            it(std::string("should return false when endpoint is not configured"), [=]() mutable
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
    describe(std::string("CLI Integration"), [=]() mutable
    {
        it(std::string("should handle successful CLI execution"), [=]() mutable
        {
            auto originalModule = require->main;
            auto originalEnv = process->env;
            process->env = utils::assign(object{
                , 
                object::pair{std::string("GITHUB_RUN_ID"), std::string("test-run-123")}, 
                object::pair{std::string("GITHUB_WORKFLOW"), std::string("test-workflow")}, 
                object::pair{std::string("GITHUB_ACTOR"), std::string("test-actor")}
            }, process->env);
            require->main = as<any>(object{
                object::pair{std::string("filename"), __filename}
            });
            {
                utils::finally __finally8081_8159([&]() mutable
                {
                    require->main = originalModule;
                    process->env = originalEnv;
                });
                try
                {
                    auto exitSpy = jest->spyOn(process, std::string("exit"))->mockImplementation();
                    auto mockFetch = jest->fn()->mockResolvedValue(object{
                        object::pair{std::string("ok"), true}, 
                        object::pair{std::string("status"), 200}
                    });
                    global->fetch = mockFetch;
                    expect(process->env->GITHUB_RUN_ID)->toBe(std::string("test-run-123"));
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
