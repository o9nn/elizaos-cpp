#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/unit/utils/server-manager.test.h"

object mockServerState = object{
    object::pair{std:("process"), as<any>(nullptr)}, 
    object::pair{std:("isRunning"), false}
};
any mockStartServerProcess = mock([=](auto args = array<string>()) mutable
{
    if (AND((mockServerState["process"]), (mockServerState["isRunning"]))) {
        std::async([=]() { mockStopServerProcess(); });
    }
    console->info(std:("Starting server..."));
    auto mockProcess = object{
        object::pair{std:("kill"), mock([=]() mutable
        {
            return true;
        }
        )}, 
        object::pair{std:("exited"), Promise->resolve(0)}
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
    console->info(std:("Stopping current server process..."));
    auto killed = mockServerState["process"]["kill"](std:("SIGTERM"));
    if (!killed) {
        console->warn(std:("Failed to kill server process, trying force kill..."));
        mockServerState["process"]["kill"](std:("SIGKILL"));
    }
    mockServerState["process"] = nullptr;
    mockServerState["isRunning"] = false;
    return true;
}
);
any mockRestartServerProcess = mock([=](auto args = array<string>()) mutable
{
    console->info(std:("Restarting server..."));
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
    mock->module(std:("fs"), [=]() mutable
    {
        return (object{
            object::pair{std:("existsSync"), mockExistsSync}
        });
    }
    );
    mock->module(std:("../../../src/commands/dev/utils/server-manager"), [=]() mutable
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
            object::pair{std:("createServerManager"), std:("createServerManager")}, 
            object::pair{std:("getServerManager"), std:("getServerManager")}, 
            object::pair{std:("startServer"), mockStartServerProcess}, 
            object::pair{std:("stopServer"), mockStopServerProcess}, 
            object::pair{std:("restartServer"), mockRestartServerProcess}, 
            object::pair{std:("isRunning"), mockIsServerRunning}, 
            object::pair{std:("getCurrentProcess"), mockGetServerProcess}
        };
    }
    );
    describe(std:("Server Manager (Functional)"), [=]() mutable
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
        describe(std:("createServerManager"), [=]() mutable
        {
            it(std:("should create a server manager with functional interface"), [=]() mutable
            {
                auto manager = createServerManager();
                expect(manager)->toBeDefined();
                expect(type_of(manager->start))->toBe(std:("function"));
                expect(type_of(manager->stop))->toBe(std:("function"));
                expect(type_of(manager->restart))->toBe(std:("function"));
                expect(type_of(manager->isRunning))->toBe(std:("function"));
                expect(manager->process)->toBeNull();
            }
            );
            it(std:("should return false for isRunning when no process is active"), [=]() mutable
            {
                auto manager = createServerManager();
                expect(manager->isRunning())->toBe(false);
            }
            );
        }
        );
        describe(std:("server process management"), [=]() mutable
        {
            it(std:("should start server process"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->start(); });
                expect(mockStartServerProcess)->toHaveBeenCalledWith(array<any>());
                expect(consoleInfoSpy)->toHaveBeenCalledWith(std:("Starting server..."));
                expect(manager->isRunning())->toBe(true);
            }
            );
            it(std:("should start server with arguments"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->start(array<string>{ std:("--verbose"), std:("--port"), std:("3000") }); });
                expect(mockStartServerProcess)->toHaveBeenCalledWith(array<string>{ std:("--verbose"), std:("--port"), std:("3000") });
                expect(consoleInfoSpy)->toHaveBeenCalledWith(std:("Starting server..."));
            }
            );
            it(std:("should stop running server process"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->start(); });
                expect(manager->isRunning())->toBe(true);
                auto result = std::async([=]() { manager->stop(); });
                expect(result)->toBe(true);
                expect(mockStopServerProcess)->toHaveBeenCalled();
                expect(consoleInfoSpy)->toHaveBeenCalledWith(std:("Stopping current server process..."));
                expect(manager->isRunning())->toBe(false);
            }
            );
            it(std:("should return false when stopping non-running server"), [=]() mutable
            {
                auto manager = createServerManager();
                auto result = std::async([=]() { manager->stop(); });
                expect(result)->toBe(false);
                expect(mockStopServerProcess)->toHaveBeenCalled();
            }
            );
            it(std:("should restart server process"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->restart(array<string>{ std:("--debug") }); });
                expect(mockRestartServerProcess)->toHaveBeenCalledWith(array<string>{ std:("--debug") });
                expect(consoleInfoSpy)->toHaveBeenCalledWith(std:("Restarting server..."));
            }
            );
            it(std:("should handle process kill failure"), [=]() mutable
            {
                auto manager = createServerManager();
                auto mockProcess = object{
                    object::pair{std:("kill"), mock([=]() mutable
                    {
                        return false;
                    }
                    )}
                };
                mockServerState["process"] = mockProcess;
                mockServerState["isRunning"] = true;
                auto result = std::async([=]() { manager->stop(); });
                expect(result)->toBe(true);
                expect(consoleWarnSpy)->toHaveBeenCalledWith(std:("Failed to kill server process, trying force kill..."));
            }
            );
        }
        );
        describe(std:("global server manager"), [=]() mutable
        {
            it(std:("should return same instance from getServerManager"), [=]() mutable
            {
                auto manager1 = getServerManager();
                auto manager2 = getServerManager();
                expect(manager1)->toBe(manager2);
            }
            );
            it(std:("should work with global server manager"), [=]() mutable
            {
                auto manager = getServerManager();
                std::async([=]() { manager->start(array<string>{ std:("--test") }); });
                expect(mockStartServerProcess)->toHaveBeenCalledWith(array<string>{ std:("--test") });
                expect(manager->isRunning())->toBe(true);
            }
            );
        }
        );
        describe(std:("utility functions"), [=]() mutable
        {
            it(std:("should start server using startServer function"), [=]() mutable
            {
                std::async([=]() { startServer(array<string>{ std:("--test") }); });
                expect(mockStartServerProcess)->toHaveBeenCalledWith(array<string>{ std:("--test") });
                expect(consoleInfoSpy)->toHaveBeenCalledWith(std:("Starting server..."));
            }
            );
            it(std:("should stop server using stopServer function"), [=]() mutable
            {
                std::async([=]() { startServer(); });
                auto result = std::async([=]() { stopServer(); });
                expect(result)->toBe(true);
                expect(mockStopServerProcess)->toHaveBeenCalled();
            }
            );
            it(std:("should restart server using restartServer function"), [=]() mutable
            {
                std::async([=]() { restartServer(array<string>{ std:("--restart-test") }); });
                expect(mockRestartServerProcess)->toHaveBeenCalledWith(array<string>{ std:("--restart-test") });
                expect(consoleInfoSpy)->toHaveBeenCalledWith(std:("Restarting server..."));
            }
            );
            it(std:("should check running status using isRunning function"), [=]() mutable
            {
                expect(isRunning())->toBe(false);
                std::async([=]() { startServer(); });
                expect(isRunning())->toBe(true);
                std::async([=]() { stopServer(); });
                expect(isRunning())->toBe(false);
            }
            );
            it(std:("should get current process using getCurrentProcess function"), [=]() mutable
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
        describe(std:("server lifecycle"), [=]() mutable
        {
            it(std:("should handle server start and stop lifecycle"), [=]() mutable
            {
                auto manager = createServerManager();
                expect(manager->isRunning())->toBe(false);
                expect(manager->process)->toBeNull();
                std::async([=]() { manager->start(array<string>{ std:("--test") }); });
                expect(manager->isRunning())->toBe(true);
                expect(manager->process)->toBeDefined();
                auto stopped = std::async([=]() { manager->stop(); });
                expect(stopped)->toBe(true);
                expect(manager->isRunning())->toBe(false);
                expect(manager->process)->toBeNull();
            }
            );
            it(std:("should handle multiple start calls"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->start(array<string>{ std:("--first") }); });
                expect(mockStartServerProcess)->toHaveBeenCalledTimes(1);
                std::async([=]() { manager->start(array<string>{ std:("--second") }); });
                expect(mockStartServerProcess)->toHaveBeenCalledTimes(2);
                expect(mockStopServerProcess)->toHaveBeenCalledTimes(1);
            }
            );
            it(std:("should handle restart without prior start"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->restart(array<string>{ std:("--restart") }); });
                expect(mockRestartServerProcess)->toHaveBeenCalledWith(array<string>{ std:("--restart") });
                expect(consoleInfoSpy)->toHaveBeenCalledWith(std:("Restarting server..."));
            }
            );
        }
        );
        describe(std:("edge cases"), [=]() mutable
        {
            it(std:("should handle empty arguments"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->start(); });
                expect(mockStartServerProcess)->toHaveBeenCalledWith(array<any>());
                std::async([=]() { manager->restart(); });
                expect(mockRestartServerProcess)->toHaveBeenCalledWith(array<any>());
            }
            );
            it(std:("should handle stopping already stopped server"), [=]() mutable
            {
                auto manager = createServerManager();
                auto result1 = std::async([=]() { manager->stop(); });
                expect(result1)->toBe(false);
                auto result2 = std::async([=]() { manager->stop(); });
                expect(result2)->toBe(false);
            }
            );
            it(std:("should maintain state consistency across operations"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->start(); });
                expect(manager->isRunning())->toBe(true);
                std::async([=]() { manager->restart(array<string>{ std:("--new-args") }); });
                expect(manager->isRunning())->toBe(true);
                std::async([=]() { manager->stop(); });
                expect(manager->isRunning())->toBe(false);
            }
            );
        }
        );
        describe(std:("error handling"), [=]() mutable
        {
            it(std:("should handle process creation errors gracefully"), [=]() mutable
            {
                mockStartServerProcess->mockImplementationOnce([=]() mutable
                {
                    throw any(std::make_shared<Error>(std:("Process creation failed")));
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
            it(std:("should handle process stop errors gracefully"), [=]() mutable
            {
                auto manager = createServerManager();
                std::async([=]() { manager->start(); });
                mockStopServerProcess->mockImplementationOnce([=]() mutable
                {
                    throw any(std::make_shared<Error>(std:("Process stop failed")));
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
