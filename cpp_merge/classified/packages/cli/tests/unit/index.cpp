#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/unit/index.test.h"
#include <string>
#include <vector>

void Main(void)
{
    describe(std:("CLI argument parsing logic"), [=]() mutable
    {
        it(std:("should detect --no-emoji flag in std::vector<std::string>()"), [=]() mutable
        {
            auto testArgv = array<string>{ std:("node"), std:("elizaos"), std:("--no-emoji") };
            auto hasNoEmojiFlag = testArgv->includes(std:("--no-emoji"));
            expect(hasNoEmojiFlag)->toBe(true);
        }
        );
        it(std:("should detect --no-auto-install flag in std::vector<std::string>()"), [=]() mutable
        {
            auto testArgv = array<string>{ std:("node"), std:("elizaos"), std:("--no-auto-install") };
            auto hasNoAutoInstallFlag = testArgv->includes(std:("--no-auto-install"));
            expect(hasNoAutoInstallFlag)->toBe(true);
        }
        );
        it(std:("should detect when no arguments are provided"), [=]() mutable
        {
            auto testArgv = array<string>{ std:("node"), std:("elizaos") };
            auto hasNoArgs = testArgv->get_length() == 2;
            expect(hasNoArgs)->toBe(true);
        }
        );
        it(std:("should detect update command"), [=]() mutable
        {
            auto testArgv = array<string>{ std:("node"), std:("elizaos"), std:("update") };
            auto args = testArgv->slice(2);
            auto isUpdateCommand = args->includes(std:("update"));
            expect(isUpdateCommand)->toBe(true);
        }
        );
        it(std:("should detect when banner should be shown"), [=]() mutable
        {
            auto testArgv = array<string>{ std:("node"), std:("elizaos") };
            auto args = testArgv->slice(2);
            auto willShowBanner = args->get_length() == 0;
            expect(willShowBanner)->toBe(true);
        }
        );
        it(std:("should not show banner when command is provided"), [=]() mutable
        {
            auto testArgv = array<string>{ std:("node"), std:("elizaos"), std:("start") };
            auto args = testArgv->slice(2);
            auto willShowBanner = args->get_length() == 0;
            expect(willShowBanner)->toBe(false);
        }
        );
    }
    );
    describe(std:("Signal handling"), [=]() mutable
    {
        shared<> originalExit;
        shared<ReturnType<mock>> mockExit;
        shared<any> mockLogger;
        shared<ReturnType<mock>> mockStopServer;
        shared<object> shutdownState;
        beforeEach([=]() mutable
        {
            shutdownState = object{
                object::pair{std:("isShuttingDown"), false}, 
            };
            originalExit = process->exit;
            mockExit = mock([=](auto code = undefined) mutable
            {
                throw any(std::make_shared<Error>(std:("Process exit called with code: ") + code + string_empty));
            }
            );
            process->exit = as<any>(mockExit);
            mockLogger = object{
                object::pair{std:("info"), mock()}, 
                object::pair{std:("error"), mock()}, 
                object::pair{std:("debug"), mock()}
            };
            mockStopServer = mock([=]() mutable
            {
                return true;
            }
            );
        }
        );
        afterEach([=]() mutable
        {
            process->exit = originalExit;
            mockExit["mockRestore"]();
            mockLogger["info"]["mockRestore"]();
            mockLogger["error"]["mockRestore"]();
            mockLogger["debug"]["mockRestore"]();
            mockStopServer["mockRestore"]();
        }
        );
        auto testGracefulShutdown = [=](auto signal, auto expectedExitCode) mutable
        {
            if (!shutdownState["tryInitiateShutdown"]()) {
                mockLogger["debug"](std:("Ignoring ") + signal + std:(" - shutdown already in progress"));
                return std::shared_ptr<Promise<void>>();
            }
            mockLogger["info"](std:("Received ") + signal + std:(", shutting down gracefully..."));
            try
            {
                auto serverWasStopped = std::async([=]() { mockStopServer(); });
                if (serverWasStopped) {
                    mockLogger["info"](std:("Server stopped successfully"));
                }
            }
            catch (const any& error)
            {
                auto errorMessage = (is<Error>(error)) ? error->message : String(error);
                mockLogger["error"](std:("Error stopping server: ") + errorMessage + string_empty);
                mockLogger["debug"](std:("Full error details:"), error);
            }
            auto exitCode = (signal == std:("SIGINT")) ? any(130) ((signal == std:("SIGTERM")) ? 143 : 0);
            expect(exitCode)->toBe(expectedExitCode);
            try
            {
                process->exit(exitCode);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(Error);
                expect((as<std::shared_ptr<Error>>(error))->message)->toBe(std:("Process exit called with code: ") + exitCode + string_empty);
            }
        };

        it(std:("should handle SIGINT gracefully and exit with code 130"), [=]() mutable
        {
            std::async([=]() { testGracefulShutdown(std:("SIGINT"), 130); });
            expect(mockLogger["info"])->toHaveBeenCalledWith(std:("Received SIGINT, shutting down gracefully..."));
            expect(mockLogger["info"])->toHaveBeenCalledWith(std:("Server stopped successfully"));
            expect(mockStopServer)->toHaveBeenCalled();
            expect(mockExit)->toHaveBeenCalledWith(130);
        }
        );
        it(std:("should handle SIGTERM gracefully and exit with code 143"), [=]() mutable
        {
            std::async([=]() { testGracefulShutdown(std:("SIGTERM"), 143); });
            expect(mockLogger["info"])->toHaveBeenCalledWith(std:("Received SIGTERM, shutting down gracefully..."));
            expect(mockLogger["info"])->toHaveBeenCalledWith(std:("Server stopped successfully"));
            expect(mockStopServer)->toHaveBeenCalled();
            expect(mockExit)->toHaveBeenCalledWith(143);
        }
        );
        it(std:("should handle server stop errors gracefully"), [=]() mutable
        {
            auto testError = std::make_shared<Error>(std:("Server stop failed"));
            mockStopServer["mockRejectedValue"](testError);
            if (!shutdownState["tryInitiateShutdown"]()) {
                mockLogger["debug"](std:("Ignoring SIGINT - shutdown already in progress"));
                return std::shared_ptr<Promise<void>>();
            }
            mockLogger["info"](std:("Received SIGINT, shutting down gracefully..."));
            try
            {
                auto serverWasStopped = std::async([=]() { mockStopServer(); });
                if (serverWasStopped) {
                    mockLogger["info"](std:("Server stopped successfully"));
                }
            }
            catch (const any& error)
            {
                auto errorMessage = (is<Error>(error)) ? error->message : String(error);
                mockLogger["error"](std:("Error stopping server: ") + errorMessage + string_empty);
                mockLogger["debug"](std:("Full error details:"), error);
            }
            try
            {
                process->exit(130);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(Error);
                expect((as<std::shared_ptr<Error>>(error))->message)->toBe(std:("Process exit called with code: 130"));
            }
            expect(mockLogger["error"])->toHaveBeenCalledWith(std:("Error stopping server: Server stop failed"));
            expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Full error details:"), testError);
            expect(mockExit)->toHaveBeenCalledWith(130);
        }
        );
        it(std:("should handle non-Error objects in catch block"), [=]() mutable
        {
            auto testErrorObject = object{
                object::pair{std:("message"), std:("Non-error object")}
            };
            mockStopServer["mockRejectedValue"](testErrorObject);
            if (!shutdownState["tryInitiateShutdown"]()) {
                mockLogger["debug"](std:("Ignoring SIGINT - shutdown already in progress"));
                return std::shared_ptr<Promise<void>>();
            }
            mockLogger["info"](std:("Received SIGINT, shutting down gracefully..."));
            try
            {
                auto serverWasStopped = std::async([=]() { mockStopServer(); });
                if (serverWasStopped) {
                    mockLogger["info"](std:("Server stopped successfully"));
                }
            }
            catch (const any& error)
            {
                auto errorMessage = (is<Error>(error)) ? error->message : String(error);
                mockLogger["error"](std:("Error stopping server: ") + errorMessage + string_empty);
                mockLogger["debug"](std:("Full error details:"), error);
            }
            try
            {
                process->exit(130);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(Error);
                expect((as<std::shared_ptr<Error>>(error))->message)->toBe(std:("Process exit called with code: 130"));
            }
            expect(mockLogger["error"])->toHaveBeenCalledWith(std:("Error stopping server: [object Object]"));
            expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Full error details:"), testErrorObject);
        }
        );
        it(std:("should prevent multiple concurrent shutdown attempts"), [=]() mutable
        {
            auto firstShutdown = [=]() mutable
            {
                if (!shutdownState["tryInitiateShutdown"]()) {
                    mockLogger["debug"](std:("Ignoring SIGINT - shutdown already in progress"));
                    return std::shared_ptr<Promise<void>>();
                }
                mockLogger["info"](std:("Received SIGINT, shutting down gracefully..."));
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 100);
                }
                ); });
                std::async([=]() { mockStopServer(); });
                mockLogger["info"](std:("Server stopped successfully"));
                try
                {
                    process->exit(130);
                }
                catch (const any& error)
                {
                }
            };
            auto secondShutdown = [=]() mutable
            {
                if (!shutdownState["tryInitiateShutdown"]()) {
                    mockLogger["debug"](std:("Ignoring SIGTERM - shutdown already in progress"));
                    return std::shared_ptr<Promise<void>>();
                }
                mockLogger["info"](std:("Received SIGTERM, shutting down gracefully..."));
                std::async([=]() { mockStopServer(); });
                mockLogger["info"](std:("Server stopped successfully"));
                try
                {
                    process->exit(143);
                }
                catch (const any& error)
                {
                }
            };
            std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<void>>, std::shared_ptr<Promise<void>>>{ firstShutdown(), secondShutdown() }); });
            expect(mockLogger["info"])->toHaveBeenCalledWith(std:("Received SIGINT, shutting down gracefully..."));
            expect(mockLogger["info"])->toHaveBeenCalledWith(std:("Server stopped successfully"));
            expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Ignoring SIGTERM - shutdown already in progress"));
            expect(mockStopServer)->toHaveBeenCalledTimes(1);
        }
        );
        it(std:("should handle fallback exit code for unknown signals"), [=]() mutable
        {
            auto unknownSignal = std:("SIGUSR1");
            if (!shutdownState["tryInitiateShutdown"]()) {
                mockLogger["debug"](std:("Ignoring ") + unknownSignal + std:(" - shutdown already in progress"));
                return std::shared_ptr<Promise<void>>();
            }
            mockLogger["info"](std:("Received ") + unknownSignal + std:(", shutting down gracefully..."));
            try
            {
                auto serverWasStopped = std::async([=]() { mockStopServer(); });
                if (serverWasStopped) {
                    mockLogger["info"](std:("Server stopped successfully"));
                }
            }
            catch (const any& error)
            {
                auto errorMessage = (is<Error>(error)) ? error->message : String(error);
                mockLogger["error"](std:("Error stopping server: ") + errorMessage + string_empty);
                mockLogger["debug"](std:("Full error details:"), error);
            }
            auto exitCode = (unknownSignal == std:("SIGINT")) ? any(130) ((unknownSignal == std:("SIGTERM")) ? 143 : 0);
            expect(exitCode)->toBe(0);
            try
            {
                process->exit(exitCode);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(Error);
                expect((as<std::shared_ptr<Error>>(error))->message)->toBe(std:("Process exit called with code: 0"));
            }
            expect(mockLogger["info"])->toHaveBeenCalledWith(std:("Received SIGUSR1, shutting down gracefully..."));
            expect(mockLogger["info"])->toHaveBeenCalledWith(std:("Server stopped successfully"));
            expect(mockStopServer)->toHaveBeenCalled();
            expect(mockExit)->toHaveBeenCalledWith(0);
        }
        );
        it(std:("should atomically handle shutdown state to prevent race conditions"), [=]() mutable
        {
            auto state = object{
                object::pair{std:("isShuttingDown"), false}, 
            };
            auto firstAttempt = state["tryInitiateShutdown"]();
            expect(firstAttempt)->toBe(true);
            expect(state["isShuttingDown"])->toBe(true);
            auto secondAttempt = state["tryInitiateShutdown"]();
            expect(secondAttempt)->toBe(false);
            expect(state["isShuttingDown"])->toBe(true);
        }
        );
        it(std:("should not log server messages when no server is running"), [=]() mutable
        {
            mockStopServer["mockResolvedValue"](false);
            if (!shutdownState["tryInitiateShutdown"]()) {
                mockLogger["debug"](std:("Ignoring SIGINT - shutdown already in progress"));
                return std::shared_ptr<Promise<void>>();
            }
            mockLogger["info"](std:("Received SIGINT, shutting down gracefully..."));
            try
            {
                auto serverWasStopped = std::async([=]() { mockStopServer(); });
                if (serverWasStopped) {
                    mockLogger["info"](std:("Server stopped successfully"));
                }
            }
            catch (const any& error)
            {
                auto errorMessage = (is<Error>(error)) ? error->message : String(error);
                mockLogger["error"](std:("Error stopping server: ") + errorMessage + string_empty);
                mockLogger["debug"](std:("Full error details:"), error);
            }
            try
            {
                process->exit(130);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(Error);
                expect((as<std::shared_ptr<Error>>(error))->message)->toBe(std:("Process exit called with code: 130"));
            }
            expect(mockLogger["info"])->toHaveBeenCalledWith(std:("Received SIGINT, shutting down gracefully..."));
            expect(mockLogger["info"])->not->toHaveBeenCalledWith(std:("Server stopped successfully"));
            expect(mockStopServer)->toHaveBeenCalled();
            expect(mockExit)->toHaveBeenCalledWith(130);
        }
        );
    }
    );
    describe(std:("Signal handler registration"), [=]() mutable
    {
        shared<> originalProcessOn;
        shared<ReturnType<mock>> mockProcessOn;
        shared signalHandlers = object{};
        beforeEach([=]() mutable
        {
            originalProcessOn = process->on;
            mockProcessOn = mock([=](auto event, auto handler) mutable
            {
                signalHandlers[event] = handler;
                return process;
            }
            );
            process->on = as<any>(mockProcessOn);
        }
        );
        afterEach([=]() mutable
        {
            process->on = originalProcessOn;
            signalHandlers = object{};
            mockProcessOn["mockRestore"]();
        }
        );
        it(std:("should register SIGINT and SIGTERM signal handlers"), [=]() mutable
        {
            shared gracefulShutdown = [=](auto signal) mutable
            {
            };
            process->on(std:("SIGINT"), [=]() mutable
            {
                return gracefulShutdown(std:("SIGINT"));
            }
            );
            process->on(std:("SIGTERM"), [=]() mutable
            {
                return gracefulShutdown(std:("SIGTERM"));
            }
            );
            expect(mockProcessOn)->toHaveBeenCalledWith(std:("SIGINT"), expect->any(Function));
            expect(mockProcessOn)->toHaveBeenCalledWith(std:("SIGTERM"), expect->any(Function));
            expect(mockProcessOn)->toHaveBeenCalledTimes(2);
            expect(const_(signalHandlers)[std:("SIGINT")])->toBeDefined();
            expect(const_(signalHandlers)[std:("SIGTERM")])->toBeDefined();
            expect(type_of(const_(signalHandlers)[std:("SIGINT")]))->toBe(std:("function"));
            expect(type_of(const_(signalHandlers)[std:("SIGTERM")]))->toBe(std:("function"));
        }
        );
    }
    );
}

MAIN
