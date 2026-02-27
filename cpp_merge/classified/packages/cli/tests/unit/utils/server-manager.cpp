#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/unit/utils/server-manager.test.h"

object mockServerState = object{
    object::pair{std::string("process"), as<any>(nullptr)}, 
    object::pair{std::string("isRunning"), false}
};
any mockStartServerProcess = mock([=](auto args = array<string>()) mutable
{
    if (AND((mockServerState["process"]), (mockServerState["isRunning"]))) {
        std::async([=]() { mockStopServerProcess(); });
    }
    console->info(std::string("Starting server..."));
    auto mockProcess = object{
        object::pair{std::string("kill"), mock([=]() mutable
        {
            return true;
        }
        )}, 
        object::pair{std::string("exited"), Promise->resolve(0)}
    };
    mockServerState["process"] = mockProcess;
    mockServerState["isRunning"] = true;
    return Promise->resolve();
}
);
any mockStopServerProcess = mock([=]() mutable
{
    if (OR((!mockServerState["process"]), (!mockServerState["isRunning"]))) {
        return false;
    }
    console->info(std::string("Stopping current server process..."));
    auto killed = mockServerState["process"]["kill"](std::string("SIGTERM"));
    if (!killed) {
        console->warn(std::string("Failed to kill server process, trying force kill..."));
        mockServerState["process"]["kill"](std::string("SIGKILL"));
    }
    mockServerState["process"] = nullptr;
    mockServerState["isRunning"] = false;
    return true;
}
);
any mockRestartServerProcess = mock([=](auto args = array<string>()) mutable
{
    console->info(std::string("Restarting server..."));
    std::async([=]() { mockStopServerProcess(); });
    std::async([=]() { mockStartServerProcess(args); });
}
);
any mockIsServerRunning = mock([=]() mutable
{
    return AND((mockServerState["isRunning"]), (mockServerState["process"] != nullptr));
}
);
any mockGetServerProcess = mock([=]() mutable
{
    return mockServerState["process"];
}
);
any mockExistsSync = mock([=]() mutable
{
    return false;
}
);

void Main(void)
{
    mock->module(std::string("fs"), [=]() mutable
    {
        return (object{
            object::pair{std::string("existsSync"), mockExistsSync}
        });
    }
    );
    mock->module(std::string("../../../src/commands/dev/utils/server-manager"), [=]() mutable
    {
        shared createServerManager = [=]() mutable
        {
            return (object{
                , 
                , 
                , 
                , 
            });
        };
        shared serverManager = nullptr;
        auto getServerManager = [=]() mutable
        {
            if (!serverManager) {
                serverManager = createServerManager();
            }
            return serverManager;
        };
        return object{
            object::pair{std::string("createServerManager"), std::string("createServerManager")}, 
            object::pair{std::string("getServerManager"), std::string("getServerManager")}, 
            object::pair{std::string("startServer"), mockStartServerProcess}, 
            object::pair{std::string("stopServer"), mockStopServerProcess}, 
            object::pair{std::string("restartServer"), mockRestartServerProcess}, 
            object::pair{std::string("isRunning"), mockIsServerRunning}, 
            object::pair{std::string("getCurrentProcess"), mockGetServerProcess}
        };
    }
    );
    describe(std::string("Server Manager (Functional)"), [=]() mutable
    {
        shared<any> consoleInfoSpy;
        shared<any> consoleWarnSpy;
        shared<any> consoleErrorSpy;
        beforeEach([=]() mutable
        {
            consoleInfoSpy = mock([=]() mutable
            {
            }
            );
            consoleWarnSpy = mock([=]() mutable
            {
            }
            );
            consoleErrorSpy = mock([=]() mutable
            {
            }
            );
            console->info = consoleInfoSpy;
            console->warn = consoleWarnSpy;
            console->error = consoleErrorSpy;
            mockServerState["process"] = nullptr;
            mockServerState["isRunning"] = false;
            mockStartServerProcess->mockClear();
            mockStopServerProcess->mockClear();
            mockRestartServerProcess->mockClear();
            mockIsServerRunning->mockClear();
            mockGetServerProcess->mockClear();
            mockExistsSync->mockClear();
            consoleInfoSpy["mockClear"]();
            consoleWarnSpy["mockClear"]();
            consoleErrorSpy["mockClear"]();
        }
        );
        afterEach([=]() mutable
        {
            mockServerState["process"] = nullptr;
            mockServerState["isRunning"] = false;
        }
        );
        describe(std::string("createServerManager"), [=]() mutable
        {
            it(std::string("should create a server manager with functional interface"), [=]() mutable
            {
                auto manager = createServerManager();
                expect(manager)->toBeDefined();
                expect(type_of(manager->start))->toBe(std::string("function"));
                expect(type_of(manager->stop))->toBe(std::string("function"));
                expect(type_of(manager->restart))->toBe(std::string("function"));
                expect(type_of(manager->isRunning))->toBe(std::string("function"));
                expect(manager->process)->toBeNull();
            }
            );
            it(std::string("should return false for isRunning when no process is active"), [=]() mutable
            {
                auto manager = createServerManager();
                expect(manager->isRunning())->toBe(false);
            }
            );
        }
        );
        describe(std::string("server process management"), [=]() mutable
        {
            it(std::string("should start server process"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->start(); });
                expect(mockStartServerProcess)->toHaveBeenCalledWith(array<any>());
                expect(consoleInfoSpy)->toHaveBeenCalledWith(std::string("Starting server..."));
                expect(manager->isRunning())->toBe(true);
            }
            );
            it(std::string("should start server with arguments"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->start(array<string>{ std::string("--verbose"), std::string("--port"), std::string("3000") }); });
                expect(mockStartServerProcess)->toHaveBeenCalledWith(array<string>{ std::string("--verbose"), std::string("--port"), std::string("3000") });
                expect(consoleInfoSpy)->toHaveBeenCalledWith(std::string("Starting server..."));
            }
            );
            it(std::string("should stop running server process"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->start(); });
                expect(manager->isRunning())->toBe(true);
                auto result = std::async([=]() { manager->stop(); });
                expect(result)->toBe(true);
                expect(mockStopServerProcess)->toHaveBeenCalled();
                expect(consoleInfoSpy)->toHaveBeenCalledWith(std::string("Stopping current server process..."));
                expect(manager->isRunning())->toBe(false);
            }
            );
            it(std::string("should return false when stopping non-running server"), [=]() mutable
            {
                auto manager = createServerManager();
                auto result = std::async([=]() { manager->stop(); });
                expect(result)->toBe(false);
                expect(mockStopServerProcess)->toHaveBeenCalled();
            }
            );
            it(std::string("should restart server process"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->restart(array<string>{ std::string("--debug") }); });
                expect(mockRestartServerProcess)->toHaveBeenCalledWith(array<string>{ std::string("--debug") });
                expect(consoleInfoSpy)->toHaveBeenCalledWith(std::string("Restarting server..."));
            }
            );
            it(std::string("should handle process kill failure"), [=]() mutable
            {
                auto manager = createServerManager();
                auto mockProcess = object{
                    object::pair{std::string("kill"), mock([=]() mutable
                    {
                        return false;
                    }
                    )}
                };
                mockServerState["process"] = mockProcess;
                mockServerState["isRunning"] = true;
                auto result = std::async([=]() { manager->stop(); });
                expect(result)->toBe(true);
                expect(consoleWarnSpy)->toHaveBeenCalledWith(std::string("Failed to kill server process, trying force kill..."));
            }
            );
        }
        );
        describe(std::string("global server manager"), [=]() mutable
        {
            it(std::string("should return same instance from getServerManager"), [=]() mutable
            {
                auto manager1 = getServerManager();
                auto manager2 = getServerManager();
                expect(manager1)->toBe(manager2);
            }
            );
            it(std::string("should work with global server manager"), [=]() mutable
            {
                auto manager = getServerManager();
                std::async([=]() { manager->start(array<string>{ std::string("--test") }); });
                expect(mockStartServerProcess)->toHaveBeenCalledWith(array<string>{ std::string("--test") });
                expect(manager->isRunning())->toBe(true);
            }
            );
        }
        );
        describe(std::string("utility functions"), [=]() mutable
        {
            it(std::string("should start server using startServer function"), [=]() mutable
            {
                std::async([=]() { startServer(array<string>{ std::string("--test") }); });
                expect(mockStartServerProcess)->toHaveBeenCalledWith(array<string>{ std::string("--test") });
                expect(consoleInfoSpy)->toHaveBeenCalledWith(std::string("Starting server..."));
            }
            );
            it(std::string("should stop server using stopServer function"), [=]() mutable
            {
                std::async([=]() { startServer(); });
                auto result = std::async([=]() { stopServer(); });
                expect(result)->toBe(true);
                expect(mockStopServerProcess)->toHaveBeenCalled();
            }
            );
            it(std::string("should restart server using restartServer function"), [=]() mutable
            {
                std::async([=]() { restartServer(array<string>{ std::string("--restart-test") }); });
                expect(mockRestartServerProcess)->toHaveBeenCalledWith(array<string>{ std::string("--restart-test") });
                expect(consoleInfoSpy)->toHaveBeenCalledWith(std::string("Restarting server..."));
            }
            );
            it(std::string("should check running status using isRunning function"), [=]() mutable
            {
                expect(isRunning())->toBe(false);
                std::async([=]() { startServer(); });
                expect(isRunning())->toBe(true);
                std::async([=]() { stopServer(); });
                expect(isRunning())->toBe(false);
            }
            );
            it(std::string("should get current process using getCurrentProcess function"), [=]() mutable
            {
                expect(getCurrentProcess())->toBeNull();
                std::async([=]() { startServer(); });
                expect(getCurrentProcess())->toBeDefined();
                std::async([=]() { stopServer(); });
                expect(getCurrentProcess())->toBeNull();
            }
            );
        }
        );
        describe(std::string("server lifecycle"), [=]() mutable
        {
            it(std::string("should handle server start and stop lifecycle"), [=]() mutable
            {
                auto manager = createServerManager();
                expect(manager->isRunning())->toBe(false);
                expect(manager->process)->toBeNull();
                std::async([=]() { manager->start(array<string>{ std::string("--test") }); });
                expect(manager->isRunning())->toBe(true);
                expect(manager->process)->toBeDefined();
                auto stopped = std::async([=]() { manager->stop(); });
                expect(stopped)->toBe(true);
                expect(manager->isRunning())->toBe(false);
                expect(manager->process)->toBeNull();
            }
            );
            it(std::string("should handle multiple start calls"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->start(array<string>{ std::string("--first") }); });
                expect(mockStartServerProcess)->toHaveBeenCalledTimes(1);
                std::async([=]() { manager->start(array<string>{ std::string("--second") }); });
                expect(mockStartServerProcess)->toHaveBeenCalledTimes(2);
                expect(mockStopServerProcess)->toHaveBeenCalledTimes(1);
            }
            );
            it(std::string("should handle restart without prior start"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->restart(array<string>{ std::string("--restart") }); });
                expect(mockRestartServerProcess)->toHaveBeenCalledWith(array<string>{ std::string("--restart") });
                expect(consoleInfoSpy)->toHaveBeenCalledWith(std::string("Restarting server..."));
            }
            );
        }
        );
        describe(std::string("edge cases"), [=]() mutable
        {
            it(std::string("should handle empty arguments"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->start(); });
                expect(mockStartServerProcess)->toHaveBeenCalledWith(array<any>());
                std::async([=]() { manager->restart(); });
                expect(mockRestartServerProcess)->toHaveBeenCalledWith(array<any>());
            }
            );
            it(std::string("should handle stopping already stopped server"), [=]() mutable
            {
                auto manager = createServerManager();
                auto result1 = std::async([=]() { manager->stop(); });
                expect(result1)->toBe(false);
                auto result2 = std::async([=]() { manager->stop(); });
                expect(result2)->toBe(false);
            }
            );
            it(std::string("should maintain state consistency across operations"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->start(); });
                expect(manager->isRunning())->toBe(true);
                std::async([=]() { manager->restart(array<string>{ std::string("--new-args") }); });
                expect(manager->isRunning())->toBe(true);
                std::async([=]() { manager->stop(); });
                expect(manager->isRunning())->toBe(false);
            }
            );
        }
        );
        describe(std::string("error handling"), [=]() mutable
        {
            it(std::string("should handle process creation errors gracefully"), [=]() mutable
            {
                mockStartServerProcess->mockImplementationOnce([=]() mutable
                {
                    throw any(std::make_shared<Error>(std::string("Process creation failed")));
                }
                );
                auto manager = createServerManager();
                try
                {
                    std::async([=]() { manager->start(); });
                }
                catch (const any& error)
                {
                    expect(error)->toBeDefined();
                }
            }
            );
            it(std::string("should handle process stop errors gracefully"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->start(); });
                mockStopServerProcess->mockImplementationOnce([=]() mutable
                {
                    throw any(std::make_shared<Error>(std::string("Process stop failed")));
                }
                );
                try
                {
                    std::async([=]() { manager->stop(); });
                }
                catch (const any& error)
                {
                    expect(error)->toBeDefined();
                }
            }
            );
        }
        );
    }
    );
}

MAIN
