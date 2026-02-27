#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/__tests__/services/server.test.h"

object originalConsoleError = console->error;
object originalConsoleWarn = console->warn;

void Main(void)
{
    mock->module(std:("../../lib/base-client"), [=]() mutable
    {
        return (object{
            object::pair{std:("BaseApiClient"), mock([=]() mutable
            {
                return (object{
                    object::pair{std:("get"), mock()}, 
                    object::pair{std:("post"), mock()}, 
                    object::pair{std:("put"), mock()}, 
                    object::pair{std:("delete"), mock()}
                });
            }
            )}
        });
    }
    );
    beforeEach([=]() mutable
    {
        console->error = mock([=]() mutable
        {
        }
        );
        console->warn = mock([=]() mutable
        {
        }
        );
    }
    );
    afterAll([=]() mutable
    {
        console->error = originalConsoleError;
        console->warn = originalConsoleWarn;
    }
    );
    describe(std:("ServerService"), [=]() mutable
    {
        shared<std::shared_ptr<ServerService>> serverService;
        shared<any> mockGet;
        shared<any> mockPost;
        shared<any> mockDelete;
        beforeEach([=]() mutable
        {
            mockGet = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
            mockPost = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
            mockDelete = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
            serverService = std::make_shared<ServerService>(object{
                object::pair{std:("baseUrl"), std:("http://localhost:3000")}, 
                object::pair{std:("apiKey"), std:("test-key")}
            });
            (as<any>(serverService))["get"] = mockGet;
            (as<any>(serverService))["post"] = mockPost;
            (as<any>(serverService))["delete"] = mockDelete;
        }
        );
        afterEach([=]() mutable
        {
            mockGet["mockClear"]();
            mockPost["mockClear"]();
            mockDelete["mockClear"]();
        }
        );
        describe(std:("constructor"), [=]() mutable
        {
            it(std:("should initialize with config"), [=]() mutable
            {
                expect(serverService)->toBeInstanceOf(ServerService);
            }
            );
            it(std:("should throw error when config is null"), [=]() mutable
            {
                expect([=]() mutable
                {
                    return std::make_shared<ServerService>(as<any>(nullptr));
                }
                )->toThrow();
            }
            );
            it(std:("should throw error when config is undefined"), [=]() mutable
            {
                expect([=]() mutable
                {
                    return std::make_shared<ServerService>(as<any>(undefined));
                }
                )->toThrow();
            }
            );
        }
        );
        describe(std:("checkHealth"), [=]() mutable
        {
            shared mockHealth = object{
                object::pair{std:("status"), std:("healthy")}, 
                object::pair{std:("timestamp"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}, 
                object::pair{std:("uptime"), 12345}, 
                object::pair{std:("version"), std:("1.0.0")}
            };
            it(std:("should return health status successfully"), [=]() mutable
            {
                mockGet["mockResolvedValue"](mockHealth);
                auto result = std::async([=]() { serverService->checkHealth(); });
                expect(result)->toEqual(mockHealth);
                expect(mockGet)->toHaveBeenCalledWith(std:("/api/server/health"));
                expect(mockGet)->toHaveBeenCalledTimes(1);
            }
            );
            it(std:("should handle health check errors"), [=]() mutable
            {
                auto healthError = std::make_shared<Error>(std:("Health check failed"));
                mockGet["mockRejectedValue"](healthError);
                std::async([=]() { expect(serverService->checkHealth())->rejects->toThrow(std:("Health check failed")); });
                expect(mockGet)->toHaveBeenCalledWith(std:("/api/server/health"));
            }
            );
        }
        );
        describe(std:("ping"), [=]() mutable
        {
            it(std:("should return pong successfully"), [=]() mutable
            {
                auto mockPong = object{
                    object::pair{std:("pong"), true}
                };
                mockGet["mockResolvedValue"](mockPong);
                auto result = std::async([=]() { serverService->ping(); });
                expect(result)->toEqual(mockPong);
                expect(mockGet)->toHaveBeenCalledWith(std:("/api/server/ping"));
            }
            );
            it(std:("should handle ping errors"), [=]() mutable
            {
                mockGet["mockRejectedValue"](std::make_shared<Error>(std:("Ping failed")));
                std::async([=]() { expect(serverService->ping())->rejects->toThrow(std:("Ping failed")); });
            }
            );
        }
        );
        describe(std:("hello"), [=]() mutable
        {
            it(std:("should return hello message successfully"), [=]() mutable
            {
                auto mockHello = object{
                    object::pair{std:("message"), std:("Hello World")}
                };
                mockGet["mockResolvedValue"](mockHello);
                auto result = std::async([=]() { serverService->hello(); });
                expect(result)->toEqual(mockHello);
                expect(mockGet)->toHaveBeenCalledWith(std:("/api/server/hello"));
            }
            );
        }
        );
        describe(std:("getStatus"), [=]() mutable
        {
            shared mockStatus = object{
                object::pair{std:("agents"), object{
                    object::pair{std:("total"), 10}, 
                    object::pair{std:("active"), 5}, 
                    object::pair{std:("inactive"), 5}
                }}, 
                object::pair{std:("memory"), object{
                    object::pair{std:("used"), 500}, 
                    object::pair{std:("total"), 1000}, 
                    object::pair{std:("percentage"), 50}
                }}, 
                object::pair{std:("uptime"), 12345}, 
                object::pair{std:("version"), std:("1.0.0")}
            };
            it(std:("should return server status successfully"), [=]() mutable
            {
                mockGet["mockResolvedValue"](mockStatus);
                auto result = std::async([=]() { serverService->getStatus(); });
                expect(result)->toEqual(mockStatus);
                expect(mockGet)->toHaveBeenCalledWith(std:("/api/server/status"));
            }
            );
        }
        );
        describe(std:("stopServer"), [=]() mutable
        {
            it(std:("should stop server successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("success"), true}
                };
                mockPost["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { serverService->stopServer(); });
                expect(result)->toEqual(mockResponse);
                expect(mockPost)->toHaveBeenCalledWith(std:("/api/server/stop"));
            }
            );
        }
        );
        describe(std:("getDebugInfo"), [=]() mutable
        {
            shared mockDebugInfo = object{
                object::pair{std:("runtime"), object{
                    object::pair{std:("agents"), array<any>()}, 
                    object::pair{std:("connections"), 5}, 
                    object::pair{std:("memory"), object{
                        object::pair{std:("used"), 100}, 
                        object::pair{std:("total"), 1000}
                    }}
                }}, 
                object::pair{std:("environment"), object{
                    object::pair{std:("NODE_ENV"), std:("test")}, 
                    object::pair{std:("VERSION"), std:("1.0.0")}
                }}
            };
            it(std:("should return debug info successfully"), [=]() mutable
            {
                mockGet["mockResolvedValue"](mockDebugInfo);
                auto result = std::async([=]() { serverService->getDebugInfo(); });
                expect(result)->toEqual(mockDebugInfo);
                expect(mockGet)->toHaveBeenCalledWith(std:("/api/server/debug/servers"));
            }
            );
        }
        );
        describe(std:("submitLogs"), [=]() mutable
        {
            shared mockLogs = array<std::shared_ptr<LogSubmitParams>>{ object{
                object::pair{std:("level"), std:("info")}, 
                object::pair{std:("message"), std:("Test log")}, 
                object::pair{std:("source"), std:("test")}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("timestamp"), std:("2024-01-01T00:00:00Z")}
                }}
            } };
            it(std:("should submit logs successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("received"), 1}
                };
                mockPost["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { serverService->submitLogs(mockLogs); });
                expect(result)->toEqual(mockResponse);
                expect(mockPost)->toHaveBeenCalledWith(std:("/api/server/logs"), object{
                    object::pair{std:("logs"), mockLogs}
                });
            }
            );
        }
        );
        describe(std:("clearLogs"), [=]() mutable
        {
            it(std:("should clear logs successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("cleared"), 50}
                };
                mockDelete["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { serverService->clearLogs(); });
                expect(result)->toEqual(mockResponse);
                expect(mockDelete)->toHaveBeenCalledWith(std:("/api/server/logs"));
            }
            );
        }
        );
    }
    );
}

MAIN
