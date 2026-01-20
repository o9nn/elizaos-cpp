#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/__tests__/services/server.test.h"

object originalConsoleError = console->error;
object originalConsoleWarn = console->warn;

void Main(void)
{
    mock->module(std::string("../../lib/base-client"), [=]() mutable
    {
        return (object{
            object::pair{std::string("BaseApiClient"), mock([=]() mutable
            {
                return (object{
                    object::pair{std::string("get"), mock()}, 
                    object::pair{std::string("post"), mock()}, 
                    object::pair{std::string("put"), mock()}, 
                    object::pair{std::string("delete"), mock()}
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
    describe(std::string("ServerService"), [=]() mutable
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
                object::pair{std::string("baseUrl"), std::string("http://localhost:3000")}, 
                object::pair{std::string("apiKey"), std::string("test-key")}
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
        describe(std::string("constructor"), [=]() mutable
        {
            it(std::string("should initialize with config"), [=]() mutable
            {
                expect(serverService)->toBeInstanceOf(ServerService);
            }
            );
            it(std::string("should throw error when config is null"), [=]() mutable
            {
                expect([=]() mutable
                {
                    return std::make_shared<ServerService>(as<any>(nullptr));
                }
                )->toThrow();
            }
            );
            it(std::string("should throw error when config is undefined"), [=]() mutable
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
        describe(std::string("checkHealth"), [=]() mutable
        {
            shared mockHealth = object{
                object::pair{std::string("status"), std::string("healthy")}, 
                object::pair{std::string("timestamp"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}, 
                object::pair{std::string("uptime"), 12345}, 
                object::pair{std::string("version"), std::string("1.0.0")}
            };
            it(std::string("should return health status successfully"), [=]() mutable
            {
                mockGet["mockResolvedValue"](mockHealth);
                auto result = std::async([=]() { serverService->checkHealth(); });
                expect(result)->toEqual(mockHealth);
                expect(mockGet)->toHaveBeenCalledWith(std::string("/api/server/health"));
                expect(mockGet)->toHaveBeenCalledTimes(1);
            }
            );
            it(std::string("should handle health check errors"), [=]() mutable
            {
                auto healthError = std::make_shared<Error>(std::string("Health check failed"));
                mockGet["mockRejectedValue"](healthError);
                std::async([=]() { expect(serverService->checkHealth())->rejects->toThrow(std::string("Health check failed")); });
                expect(mockGet)->toHaveBeenCalledWith(std::string("/api/server/health"));
            }
            );
        }
        );
        describe(std::string("ping"), [=]() mutable
        {
            it(std::string("should return pong successfully"), [=]() mutable
            {
                auto mockPong = object{
                    object::pair{std::string("pong"), true}
                };
                mockGet["mockResolvedValue"](mockPong);
                auto result = std::async([=]() { serverService->ping(); });
                expect(result)->toEqual(mockPong);
                expect(mockGet)->toHaveBeenCalledWith(std::string("/api/server/ping"));
            }
            );
            it(std::string("should handle ping errors"), [=]() mutable
            {
                mockGet["mockRejectedValue"](std::make_shared<Error>(std::string("Ping failed")));
                std::async([=]() { expect(serverService->ping())->rejects->toThrow(std::string("Ping failed")); });
            }
            );
        }
        );
        describe(std::string("hello"), [=]() mutable
        {
            it(std::string("should return hello message successfully"), [=]() mutable
            {
                auto mockHello = object{
                    object::pair{std::string("message"), std::string("Hello World")}
                };
                mockGet["mockResolvedValue"](mockHello);
                auto result = std::async([=]() { serverService->hello(); });
                expect(result)->toEqual(mockHello);
                expect(mockGet)->toHaveBeenCalledWith(std::string("/api/server/hello"));
            }
            );
        }
        );
        describe(std::string("getStatus"), [=]() mutable
        {
            shared mockStatus = object{
                object::pair{std::string("agents"), object{
                    object::pair{std::string("total"), 10}, 
                    object::pair{std::string("active"), 5}, 
                    object::pair{std::string("inactive"), 5}
                }}, 
                object::pair{std::string("memory"), object{
                    object::pair{std::string("used"), 500}, 
                    object::pair{std::string("total"), 1000}, 
                    object::pair{std::string("percentage"), 50}
                }}, 
                object::pair{std::string("uptime"), 12345}, 
                object::pair{std::string("version"), std::string("1.0.0")}
            };
            it(std::string("should return server status successfully"), [=]() mutable
            {
                mockGet["mockResolvedValue"](mockStatus);
                auto result = std::async([=]() { serverService->getStatus(); });
                expect(result)->toEqual(mockStatus);
                expect(mockGet)->toHaveBeenCalledWith(std::string("/api/server/status"));
            }
            );
        }
        );
        describe(std::string("stopServer"), [=]() mutable
        {
            it(std::string("should stop server successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("success"), true}
                };
                mockPost["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { serverService->stopServer(); });
                expect(result)->toEqual(mockResponse);
                expect(mockPost)->toHaveBeenCalledWith(std::string("/api/server/stop"));
            }
            );
        }
        );
        describe(std::string("getDebugInfo"), [=]() mutable
        {
            shared mockDebugInfo = object{
                object::pair{std::string("runtime"), object{
                    object::pair{std::string("agents"), array<any>()}, 
                    object::pair{std::string("connections"), 5}, 
                    object::pair{std::string("memory"), object{
                        object::pair{std::string("used"), 100}, 
                        object::pair{std::string("total"), 1000}
                    }}
                }}, 
                object::pair{std::string("environment"), object{
                    object::pair{std::string("NODE_ENV"), std::string("test")}, 
                    object::pair{std::string("VERSION"), std::string("1.0.0")}
                }}
            };
            it(std::string("should return debug info successfully"), [=]() mutable
            {
                mockGet["mockResolvedValue"](mockDebugInfo);
                auto result = std::async([=]() { serverService->getDebugInfo(); });
                expect(result)->toEqual(mockDebugInfo);
                expect(mockGet)->toHaveBeenCalledWith(std::string("/api/server/debug/servers"));
            }
            );
        }
        );
        describe(std::string("submitLogs"), [=]() mutable
        {
            shared mockLogs = array<std::shared_ptr<LogSubmitParams>>{ object{
                object::pair{std::string("level"), std::string("info")}, 
                object::pair{std::string("message"), std::string("Test log")}, 
                object::pair{std::string("source"), std::string("test")}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("timestamp"), std::string("2024-01-01T00:00:00Z")}
                }}
            } };
            it(std::string("should submit logs successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("received"), 1}
                };
                mockPost["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { serverService->submitLogs(mockLogs); });
                expect(result)->toEqual(mockResponse);
                expect(mockPost)->toHaveBeenCalledWith(std::string("/api/server/logs"), object{
                    object::pair{std::string("logs"), mockLogs}
                });
            }
            );
        }
        );
        describe(std::string("clearLogs"), [=]() mutable
        {
            it(std::string("should clear logs successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("cleared"), 50}
                };
                mockDelete["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { serverService->clearLogs(); });
                expect(result)->toEqual(mockResponse);
                expect(mockDelete)->toHaveBeenCalledWith(std::string("/api/server/logs"));
            }
            );
        }
        );
    }
    );
}

MAIN
