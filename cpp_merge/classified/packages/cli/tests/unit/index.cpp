#include "index.test.h"
#include <string>
#include <vector>

void Main(void)
{
    describe(std::string("CLI argument parsing logic"), [=]() mutable
    {
        it(std::string("should detect --no-emoji flag in std::vector<std::string>()"), [=]() mutable
        {
            auto testArgv = array<string>{ std::string("node"), std::string("elizaos"), std::string("--no-emoji") };
            auto hasNoEmojiFlag = testArgv->includes(std::string("--no-emoji"));
            expect(hasNoEmojiFlag)->toBe(true);
        }
        );
        it(std::string("should detect --no-auto-install flag in std::vector<std::string>()"), [=]() mutable
        {
            auto testArgv = array<string>{ std::string("node"), std::string("elizaos"), std::string("--no-auto-install") };
            auto hasNoAutoInstallFlag = testArgv->includes(std::string("--no-auto-install"));
            expect(hasNoAutoInstallFlag)->toBe(true);
        }
        );
        it(std::string("should detect when no arguments are provided"), [=]() mutable
        {
            auto testArgv = array<string>{ std::string("node"), std::string("elizaos") };
            auto hasNoArgs = testArgv->get_length() == 2;
            expect(hasNoArgs)->toBe(true);
        }
        );
        it(std::string("should detect update command"), [=]() mutable
        {
            auto testArgv = array<string>{ std::string("node"), std::string("elizaos"), std::string("update") };
            auto args = testArgv->slice(2);
            auto isUpdateCommand = args->includes(std::string("update"));
            expect(isUpdateCommand)->toBe(true);
        }
        );
        it(std::string("should detect when banner should be shown"), [=]() mutable
        {
            auto testArgv = array<string>{ std::string("node"), std::string("elizaos") };
            auto args = testArgv->slice(2);
            auto willShowBanner = args->get_length() == 0;
            expect(willShowBanner)->toBe(true);
        }
        );
        it(std::string("should not show banner when command is provided"), [=]() mutable
        {
            auto testArgv = array<string>{ std::string("node"), std::string("elizaos"), std::string("start") };
            auto args = testArgv->slice(2);
            auto willShowBanner = args->get_length() == 0;
            expect(willShowBanner)->toBe(false);
        }
        );
    }
    );
    describe(std::string("Signal handling"), [=]() mutable
    {
        shared<> originalExit;
        shared<ReturnType<mock>> mockExit;
        shared<any> mockLogger;
        shared<ReturnType<mock>> mockStopServer;
        shared<object> shutdownState;
        beforeEach([=]() mutable
        {
            shutdownState = object{
                object::pair{std::string("isShuttingDown"), false}, 
            };
            originalExit = process->exit;
            mockExit = mock([=](auto code = std::nullopt) mutable
            {
                throw any(std::make_shared<Error>(std::string("Process exit called with code: ") + code + string_empty));
            }
            );
            process->exit = as<any>(mockExit);
            mockLogger = object{
                object::pair{std::string("info"), mock()}, 
                object::pair{std::string("error"), mock()}, 
                object::pair{std::string("debug"), mock()}
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
                mockLogger["debug"](std::string("Ignoring ") + signal + std::string(" - shutdown already in progress"));
                return std::shared_ptr<Promise<void>>();
            }
            mockLogger["info"](std::string("Received ") + signal + std::string(", shutting down gracefully..."));
            try
            {
                auto serverWasStopped = std::async([=]() { mockStopServer(); });
                if (serverWasStopped) {
                    mockLogger["info"](std::string("Server stopped successfully"));
                }
            }
            catch (const any& error)
            {
                auto errorMessage = (is<Error>(error)) ? error->message : String(error);
                mockLogger["error"](std::string("Error stopping server: ") + errorMessage + string_empty);
                mockLogger["debug"](std::string("Full error details:"), error);
            }
            auto exitCode = (signal == std::string("SIGINT")) ? any(130) ((signal == std::string("SIGTERM")) ? 143 : 0);
            expect(exitCode)->toBe(expectedExitCode);
            try
            {
                process->exit(exitCode);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(Error);
                expect((as<std::shared_ptr<Error>>(error))->message)->toBe(std::string("Process exit called with code: ") + exitCode + string_empty);
            }
        };

        it(std::string("should handle SIGINT gracefully and exit with code 130"), [=]() mutable
        {
            std::async([=]() { testGracefulShutdown(std::string("SIGINT"), 130); });
            expect(mockLogger["info"])->toHaveBeenCalledWith(std::string("Received SIGINT, shutting down gracefully..."));
            expect(mockLogger["info"])->toHaveBeenCalledWith(std::string("Server stopped successfully"));
            expect(mockStopServer)->toHaveBeenCalled();
            expect(mockExit)->toHaveBeenCalledWith(130);
        }
        );
        it(std::string("should handle SIGTERM gracefully and exit with code 143"), [=]() mutable
        {
            std::async([=]() { testGracefulShutdown(std::string("SIGTERM"), 143); });
            expect(mockLogger["info"])->toHaveBeenCalledWith(std::string("Received SIGTERM, shutting down gracefully..."));
            expect(mockLogger["info"])->toHaveBeenCalledWith(std::string("Server stopped successfully"));
            expect(mockStopServer)->toHaveBeenCalled();
            expect(mockExit)->toHaveBeenCalledWith(143);
        }
        );
        it(std::string("should handle server stop errors gracefully"), [=]() mutable
        {
            auto testError = std::make_shared<Error>(std::string("Server stop failed"));
            mockStopServer["mockRejectedValue"](testError);
            if (!shutdownState["tryInitiateShutdown"]()) {
                mockLogger["debug"](std::string("Ignoring SIGINT - shutdown already in progress"));
                return std::shared_ptr<Promise<void>>();
            }
            mockLogger["info"](std::string("Received SIGINT, shutting down gracefully..."));
            try
            {
                auto serverWasStopped = std::async([=]() { mockStopServer(); });
                if (serverWasStopped) {
                    mockLogger["info"](std::string("Server stopped successfully"));
                }
            }
            catch (const any& error)
            {
                auto errorMessage = (is<Error>(error)) ? error->message : String(error);
                mockLogger["error"](std::string("Error stopping server: ") + errorMessage + string_empty);
                mockLogger["debug"](std::string("Full error details:"), error);
            }
            try
            {
                process->exit(130);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(Error);
                expect((as<std::shared_ptr<Error>>(error))->message)->toBe(std::string("Process exit called with code: 130"));
            }
            expect(mockLogger["error"])->toHaveBeenCalledWith(std::string("Error stopping server: Server stop failed"));
            expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Full error details:"), testError);
            expect(mockExit)->toHaveBeenCalledWith(130);
        }
        );
        it(std::string("should handle non-Error objects in catch block"), [=]() mutable
        {
            auto testErrorObject = object{
                object::pair{std::string("message"), std::string("Non-error object")}
            };
            mockStopServer["mockRejectedValue"](testErrorObject);
            if (!shutdownState["tryInitiateShutdown"]()) {
                mockLogger["debug"](std::string("Ignoring SIGINT - shutdown already in progress"));
                return std::shared_ptr<Promise<void>>();
            }
            mockLogger["info"](std::string("Received SIGINT, shutting down gracefully..."));
            try
            {
                auto serverWasStopped = std::async([=]() { mockStopServer(); });
                if (serverWasStopped) {
                    mockLogger["info"](std::string("Server stopped successfully"));
                }
            }
            catch (const any& error)
            {
                auto errorMessage = (is<Error>(error)) ? error->message : String(error);
                mockLogger["error"](std::string("Error stopping server: ") + errorMessage + string_empty);
                mockLogger["debug"](std::string("Full error details:"), error);
            }
            try
            {
                process->exit(130);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(Error);
                expect((as<std::shared_ptr<Error>>(error))->message)->toBe(std::string("Process exit called with code: 130"));
            }
            expect(mockLogger["error"])->toHaveBeenCalledWith(std::string("Error stopping server: [object Object]"));
            expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Full error details:"), testErrorObject);
        }
        );
        it(std::string("should prevent multiple concurrent shutdown attempts"), [=]() mutable
        {
            auto firstShutdown = [=]() mutable
            {
                if (!shutdownState["tryInitiateShutdown"]()) {
                    mockLogger["debug"](std::string("Ignoring SIGINT - shutdown already in progress"));
                    return std::shared_ptr<Promise<void>>();
                }
                mockLogger["info"](std::string("Received SIGINT, shutting down gracefully..."));
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 100);
                }
                ); });
                std::async([=]() { mockStopServer(); });
                mockLogger["info"](std::string("Server stopped successfully"));
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
                    mockLogger["debug"](std::string("Ignoring SIGTERM - shutdown already in progress"));
                    return std::shared_ptr<Promise<void>>();
                }
                mockLogger["info"](std::string("Received SIGTERM, shutting down gracefully..."));
                std::async([=]() { mockStopServer(); });
                mockLogger["info"](std::string("Server stopped successfully"));
                try
                {
                    process->exit(143);
                }
                catch (const any& error)
                {
                }
            };
            std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<void>>, std::shared_ptr<Promise<void>>>{ firstShutdown(), secondShutdown() }); });
            expect(mockLogger["info"])->toHaveBeenCalledWith(std::string("Received SIGINT, shutting down gracefully..."));
            expect(mockLogger["info"])->toHaveBeenCalledWith(std::string("Server stopped successfully"));
            expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Ignoring SIGTERM - shutdown already in progress"));
            expect(mockStopServer)->toHaveBeenCalledTimes(1);
        }
        );
        it(std::string("should handle fallback exit code for unknown signals"), [=]() mutable
        {
            auto unknownSignal = std::string("SIGUSR1");
            if (!shutdownState["tryInitiateShutdown"]()) {
                mockLogger["debug"](std::string("Ignoring ") + unknownSignal + std::string(" - shutdown already in progress"));
                return std::shared_ptr<Promise<void>>();
            }
            mockLogger["info"](std::string("Received ") + unknownSignal + std::string(", shutting down gracefully..."));
            try
            {
                auto serverWasStopped = std::async([=]() { mockStopServer(); });
                if (serverWasStopped) {
                    mockLogger["info"](std::string("Server stopped successfully"));
                }
            }
            catch (const any& error)
            {
                auto errorMessage = (is<Error>(error)) ? error->message : String(error);
                mockLogger["error"](std::string("Error stopping server: ") + errorMessage + string_empty);
                mockLogger["debug"](std::string("Full error details:"), error);
            }
            auto exitCode = (unknownSignal == std::string("SIGINT")) ? any(130) ((unknownSignal == std::string("SIGTERM")) ? 143 : 0);
            expect(exitCode)->toBe(0);
            try
            {
                process->exit(exitCode);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(Error);
                expect((as<std::shared_ptr<Error>>(error))->message)->toBe(std::string("Process exit called with code: 0"));
            }
            expect(mockLogger["info"])->toHaveBeenCalledWith(std::string("Received SIGUSR1, shutting down gracefully..."));
            expect(mockLogger["info"])->toHaveBeenCalledWith(std::string("Server stopped successfully"));
            expect(mockStopServer)->toHaveBeenCalled();
            expect(mockExit)->toHaveBeenCalledWith(0);
        }
        );
        it(std::string("should atomically handle shutdown state to prevent race conditions"), [=]() mutable
        {
            auto state = object{
                object::pair{std::string("isShuttingDown"), false}, 
            };
            auto firstAttempt = state["tryInitiateShutdown"]();
            expect(firstAttempt)->toBe(true);
            expect(state["isShuttingDown"])->toBe(true);
            auto secondAttempt = state["tryInitiateShutdown"]();
            expect(secondAttempt)->toBe(false);
            expect(state["isShuttingDown"])->toBe(true);
        }
        );
        it(std::string("should not log server messages when no server is running"), [=]() mutable
        {
            mockStopServer["mockResolvedValue"](false);
            if (!shutdownState["tryInitiateShutdown"]()) {
                mockLogger["debug"](std::string("Ignoring SIGINT - shutdown already in progress"));
                return std::shared_ptr<Promise<void>>();
            }
            mockLogger["info"](std::string("Received SIGINT, shutting down gracefully..."));
            try
            {
                auto serverWasStopped = std::async([=]() { mockStopServer(); });
                if (serverWasStopped) {
                    mockLogger["info"](std::string("Server stopped successfully"));
                }
            }
            catch (const any& error)
            {
                auto errorMessage = (is<Error>(error)) ? error->message : String(error);
                mockLogger["error"](std::string("Error stopping server: ") + errorMessage + string_empty);
                mockLogger["debug"](std::string("Full error details:"), error);
            }
            try
            {
                process->exit(130);
            }
            catch (const any& error)
            {
                expect(error)->toBeInstanceOf(Error);
                expect((as<std::shared_ptr<Error>>(error))->message)->toBe(std::string("Process exit called with code: 130"));
            }
            expect(mockLogger["info"])->toHaveBeenCalledWith(std::string("Received SIGINT, shutting down gracefully..."));
            expect(mockLogger["info"])->not->toHaveBeenCalledWith(std::string("Server stopped successfully"));
            expect(mockStopServer)->toHaveBeenCalled();
            expect(mockExit)->toHaveBeenCalledWith(130);
        }
        );
    }
    );
    describe(std::string("Signal handler registration"), [=]() mutable
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
        it(std::string("should register SIGINT and SIGTERM signal handlers"), [=]() mutable
        {
            shared gracefulShutdown = [=](auto signal) mutable
            {
            };
            process->on(std::string("SIGINT"), [=]() mutable
            {
                return gracefulShutdown(std::string("SIGINT"));
            }
            );
            process->on(std::string("SIGTERM"), [=]() mutable
            {
                return gracefulShutdown(std::string("SIGTERM"));
            }
            );
            expect(mockProcessOn)->toHaveBeenCalledWith(std::string("SIGINT"), expect->any(Function));
            expect(mockProcessOn)->toHaveBeenCalledWith(std::string("SIGTERM"), expect->any(Function));
            expect(mockProcessOn)->toHaveBeenCalledTimes(2);
            expect(const_(signalHandlers)[std::string("SIGINT")])->toBeDefined();
            expect(const_(signalHandlers)[std::string("SIGTERM")])->toBeDefined();
            expect(type_of(const_(signalHandlers)[std::string("SIGINT")]))->toBe(std::string("function"));
            expect(type_of(const_(signalHandlers)[std::string("SIGTERM")]))->toBe(std::string("function"));
        }
        );
    }
    );
}

MAIN
