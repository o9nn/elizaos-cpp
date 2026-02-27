#include "local-cli-delegation.test.h"
#include <string>
#include <vector>

any mockSpawn = mock();
any mockExistsSync = mock();
object mockLogger = object{
    object::pair{std:("info"), mock()}, 
    object::pair{std:("debug"), mock()}, 
    object::pair{std:("error"), mock()}
};

void Main(void)
{
    mock->module(std:("node:child_process"), [=]() mutable
    {
        return (object{
            object::pair{std:("spawn"), mockSpawn}
        });
    }
    );
    mock->module(std:("node:fs"), [=]() mutable
    {
        return (object{
            object::pair{std:("existsSync"), mockExistsSync}
        });
    }
    );
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std:("logger"), mockLogger}
        });
    }
    );
    describe(std:("Local CLI Delegation"), [=]() mutable
    {
        shared<std::shared_ptr<NodeJS::ProcessEnv>> originalEnv;
        shared<array<string>> originalArgv;
        shared<string> originalCwd;
        shared<any> mockProcess;
        beforeEach([=]() mutable
        {
            originalEnv = utils::assign(object{
            }, process->env);
            originalArgv = array<string>{ process->argv };
            originalCwd = process->cwd();
            mockSpawn->mockReset();
            mockExistsSync->mockReset();
            mockLogger["info"]->mockReset();
            mockLogger["debug"]->mockReset();
            mockLogger["error"]->mockReset();
            jest->spyOn(process, std:("cwd"))->mockReturnValue(std:("/test/project"));
            mockProcess = object{
                object::pair{std:("exit"), mock()}
            };
            jest->spyOn(process, std:("exit"))->mockImplementation(mockProcess["exit"]);
            process->env.Delete("NODE_ENV");
            process->env.Delete("ELIZA_TEST_MODE");
            process->env.Delete("BUN_TEST");
            process->env.Delete("VITEST");
            process->env.Delete("JEST_WORKER_ID");
            process->env.Delete("npm_lifecycle_event");
        }
        );
        afterEach([=]() mutable
        {
            process->env = originalEnv;
            process->argv = originalArgv;
            jest->restoreAllMocks();
        }
        );
        describe(std:("Test Environment Detection"), [=]() mutable
        {
            it(std:("should skip delegation when NODE_ENV is test"), [=]() mutable
            {
                process->env->NODE_ENV = std:("test");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std:("should skip delegation when ELIZA_TEST_MODE is true"), [=]() mutable
            {
                process->env->ELIZA_TEST_MODE = std:("true");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std:("should skip delegation when BUN_TEST is true"), [=]() mutable
            {
                process->env->BUN_TEST = std:("true");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std:("should skip delegation when VITEST is true"), [=]() mutable
            {
                process->env->VITEST = std:("true");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std:("should skip delegation when JEST_WORKER_ID is set"), [=]() mutable
            {
                process->env->JEST_WORKER_ID = std:("1");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std:("should skip delegation when npm_lifecycle_event is test"), [=]() mutable
            {
                process->env->npm_lifecycle_event = std:("test");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std:("should skip delegation when --test is in std::vector<std::string>()"), [=]() mutable
            {
                process->argv = array<string>{ std:("node"), std:("script.js"), std:("--test") };
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std:("should skip delegation when test is in std::vector<std::string>()"), [=]() mutable
            {
                process->argv = array<string>{ std:("node"), std:("script.js"), std:("test") };
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std:("should skip delegation when script path includes test"), [=]() mutable
            {
                process->argv = array<string>{ std:("node"), std:("/path/to/test/script.js"), std:("start") };
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std:("should skip delegation when ELIZA_SKIP_LOCAL_CLI_DELEGATION is true"), [=]() mutable
            {
                process->env->ELIZA_SKIP_LOCAL_CLI_DELEGATION = std:("true");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std:("should skip delegation when CI is true"), [=]() mutable
            {
                process->env->CI = std:("true");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std:("should skip delegation when GITHUB_ACTIONS is true"), [=]() mutable
            {
                process->env->GITHUB_ACTIONS = std:("true");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std:("should skip delegation when GITLAB_CI is true"), [=]() mutable
            {
                process->env->GITLAB_CI = std:("true");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std:("Update Command Detection"), [=]() mutable
        {
            it(std:("should skip delegation when update command is used"), [=]() mutable
            {
                auto originalArgv = process->argv;
                process->argv = array<string>{ std:("node"), std:("script.js"), std:("update") };
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Update command detected, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
                process->argv = originalArgv;
            }
            );
            it(std:("should skip delegation when update command is used with flags"), [=]() mutable
            {
                auto originalArgv = process->argv;
                process->argv = array<string>{ std:("node"), std:("script.js"), std:("update"), std:("--check") };
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("Update command detected, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
                process->argv = originalArgv;
            }
            );
        }
        );
        describe(std:("Local CLI Detection"), [=]() mutable
        {
            it(std:("should detect when running from local CLI"), [=]() mutable
            {
                process->env.Delete("NODE_ENV");
                process->env.Delete("ELIZA_TEST_MODE");
                process->env.Delete("BUN_TEST");
                process->env.Delete("VITEST");
                process->env.Delete("JEST_WORKER_ID");
                process->env.Delete("npm_lifecycle_event");
                process->argv = array<string>{ std:("node"), std:("/test/project/node_modules/@elizaos/cli/dist/index.js"), std:("start") };
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalled();
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std:("should continue when no local CLI is found"), [=]() mutable
            {
                process->argv = array<string>{ std:("node"), std:("/usr/bin/elizaos"), std:("start") };
                mockExistsSync->mockReturnValue(false);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std:("No local CLI found, using global installation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std:("should delegate when local CLI is found and not running from it"), [=]() mutable
            {
                process->argv = array<string>{ std:("node"), std:("/usr/bin/elizaos"), std:("start"), std:("--port"), std:("3000") };
                mockExistsSync->mockReturnValue(true);
                auto mockChildProcess = object{
                    object::pair{std:("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std:("exit")) {
                            setTimeout([=]() mutable
                            {
                                return handler(0, nullptr);
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std:("kill"), mock()}, 
                    object::pair{std:("killed"), false}
                };
                mockSpawn->mockReturnValue(mockChildProcess);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(true);
                expect(mockLogger["info"])->toHaveBeenCalledWith(std:("Using local @elizaos/cli installation"));
                expect(mockSpawn)->toHaveBeenCalledWith(process->execPath, array<string>{ std:("/test/project/node_modules/@elizaos/cli/dist/index.js"), std:("start"), std:("--port"), std:("3000") }, expect->objectContaining(object{
                    object::pair{std:("stdio"), std:("inherit")}, 
                    object::pair{std:("cwd"), std:("/test/project")}, 
                    object::pair{std:("env"), expect->objectContaining(object{
                        object::pair{std:("FORCE_COLOR"), std:("1")}
                    })}
                }));
            }
            );
        }
        );
        describe(std:("Environment Setup"), [=]() mutable
        {
            it(std:("should set up proper environment variables for local execution"), [=]() mutable
            {
                process->argv = array<string>{ std:("node"), std:("/usr/bin/elizaos"), std:("start") };
                mockExistsSync->mockReturnValue(true);
                auto mockChildProcess = object{
                    object::pair{std:("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std:("exit")) {
                            setTimeout([=]() mutable
                            {
                                return handler(0, nullptr);
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std:("kill"), mock()}, 
                    object::pair{std:("killed"), false}
                };
                mockSpawn->mockReturnValue(mockChildProcess);
                std::async([=]() { tryDelegateToLocalCli(); });
                auto spawnCall = const_(mockSpawn->mock->calls)[0];
                auto spawnOptions = const_(spawnCall)[2];
                auto env = spawnOptions->env;
                expect(env->FORCE_COLOR)->toBe(std:("1"));
                expect(env->NODE_PATH)->toContain(std:("/test/project/node_modules"));
                expect(env->PATH)->toContain(std:("/test/project/node_modules/.bin"));
            }
            );
            it(std:("should preserve existing NODE_PATH and PATH"), [=]() mutable
            {
                process->env->NODE_PATH = std:("/existing/node/path");
                process->env->PATH = std:("/existing/bin/path");
                process->argv = array<string>{ std:("node"), std:("/usr/bin/elizaos"), std:("start") };
                mockExistsSync->mockReturnValue(true);
                auto mockChildProcess = object{
                    object::pair{std:("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std:("exit")) {
                            setTimeout([=]() mutable
                            {
                                return handler(0, nullptr);
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std:("kill"), mock()}, 
                    object::pair{std:("killed"), false}
                };
                mockSpawn->mockReturnValue(mockChildProcess);
                std::async([=]() { tryDelegateToLocalCli(); });
                auto spawnCall = const_(mockSpawn->mock->calls)[0];
                auto spawnOptions = const_(spawnCall)[2];
                auto env = spawnOptions->env;
                expect(env->NODE_PATH)->toContain(std:("/test/project/node_modules"));
                expect(env->NODE_PATH)->toContain(std:("/existing/node/path"));
                expect(env->PATH)->toContain(std:("/test/project/node_modules/.bin"));
                expect(env->PATH)->toContain(std:("/existing/bin/path"));
            }
            );
        }
        );
        describe(std:("Error Handling"), [=]() mutable
        {
            it(std:("should handle spawn errors gracefully"), [=]() mutable
            {
                process->argv = array<string>{ std:("node"), std:("/usr/bin/elizaos"), std:("start") };
                mockExistsSync->mockReturnValue(true);
                shared testError = std::make_shared<Error>(std:("Spawn failed"));
                mockSpawn->mockImplementation([=]() mutable
                {
                    throw any(testError);
                }
                );
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["error"])->toHaveBeenCalledWith(std:("Error during local CLI delegation:"), testError);
                expect(mockLogger["info"])->toHaveBeenCalledWith(std:("Falling back to global CLI installation"));
            }
            );
            it(std:("should handle process errors"), [=]() mutable
            {
                process->argv = array<string>{ std:("node"), std:("/usr/bin/elizaos"), std:("start") };
                mockExistsSync->mockReturnValue(true);
                shared testError = std::make_shared<Error>(std:("Process error"));
                auto mockChildProcess = object{
                    object::pair{std:("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std:("error")) {
                            setTimeout([=]() mutable
                            {
                                return handler(testError);
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std:("kill"), mock()}, 
                    object::pair{std:("killed"), false}
                };
                mockSpawn->mockReturnValue(mockChildProcess);
                try
                {
                    std::async([=]() { tryDelegateToLocalCli(); });
                }
                catch (const any& error)
                {
                    expect(error)->toBe(testError);
                }
                expect(mockLogger["error"])->toHaveBeenCalledWith(std:("Failed to start local CLI: Process error"));
            }
            );
        }
        );
        describe(std:("Utility Functions"), [=]() mutable
        {
            it(std:("hasLocalCli should return true when local CLI exists"), [=]() mutable
            {
                mockExistsSync->mockReturnValue(true);
                expect(hasLocalCli())->toBe(true);
            }
            );
            it(std:("hasLocalCli should return false when local CLI does not exist"), [=]() mutable
            {
                mockExistsSync->mockReturnValue(false);
                expect(hasLocalCli())->toBe(false);
            }
            );
            it(std:("getCliContext should return correct context information"), [=]() mutable
            {
                process->argv = array<string>{ std:("node"), std:("/test/project/node_modules/@elizaos/cli/dist/index.js"), std:("start") };
                mockExistsSync->mockReturnValue(true);
                auto context = getCliContext();
                expect(context["isLocal"])->toBe(true);
                expect(context["hasLocal"])->toBe(true);
                expect(context["localPath"])->toBe(std:("/test/project/node_modules/@elizaos/cli/dist/index.js"));
                expect(context["currentPath"])->toBe(std:("/test/project/node_modules/@elizaos/cli/dist/index.js"));
            }
            );
            it(std:("getCliContext should return correct context when not running from local CLI"), [=]() mutable
            {
                process->argv = array<string>{ std:("node"), std:("/usr/bin/elizaos"), std:("start") };
                mockExistsSync->mockReturnValue(false);
                auto context = getCliContext();
                expect(context["isLocal"])->toBe(false);
                expect(context["hasLocal"])->toBe(false);
                expect(context["localPath"])->toBe(nullptr);
                expect(context["currentPath"])->toBe(std:("/usr/bin/elizaos"));
            }
            );
        }
        );
        describe(std:("Process Exit Handling"), [=]() mutable
        {
            it(std:("should exit with child process exit code"), [=]() mutable
            {
                process->argv = array<string>{ std:("node"), std:("/usr/bin/elizaos"), std:("start") };
                mockExistsSync->mockReturnValue(true);
                auto mockChildProcess = object{
                    object::pair{std:("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std:("exit")) {
                            setTimeout([=]() mutable
                            {
                                return handler(42, nullptr);
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std:("kill"), mock()}, 
                    object::pair{std:("killed"), false}
                };
                mockSpawn->mockReturnValue(mockChildProcess);
                std::async([=]() { tryDelegateToLocalCli(); });
                expect(mockProcess["exit"])->toHaveBeenCalledWith(42);
            }
            );
            it(std:("should exit with appropriate code when killed by signal"), [=]() mutable
            {
                process->argv = array<string>{ std:("node"), std:("/usr/bin/elizaos"), std:("start") };
                mockExistsSync->mockReturnValue(true);
                auto mockChildProcess = object{
                    object::pair{std:("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std:("exit")) {
                            setTimeout([=]() mutable
                            {
                                return handler(nullptr, std:("SIGTERM"));
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std:("kill"), mock()}, 
                    object::pair{std:("killed"), false}
                };
                mockSpawn->mockReturnValue(mockChildProcess);
                std::async([=]() { tryDelegateToLocalCli(); });
                expect(mockProcess["exit"])->toHaveBeenCalledWith(143);
            }
            );
            it(std:("should exit with 130 for SIGINT"), [=]() mutable
            {
                process->argv = array<string>{ std:("node"), std:("/usr/bin/elizaos"), std:("start") };
                mockExistsSync->mockReturnValue(true);
                auto mockChildProcess = object{
                    object::pair{std:("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std:("exit")) {
                            setTimeout([=]() mutable
                            {
                                return handler(nullptr, std:("SIGINT"));
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std:("kill"), mock()}, 
                    object::pair{std:("killed"), false}
                };
                mockSpawn->mockReturnValue(mockChildProcess);
                std::async([=]() { tryDelegateToLocalCli(); });
                expect(mockProcess["exit"])->toHaveBeenCalledWith(130);
            }
            );
            it(std:("should exit with 1 for unknown signal"), [=]() mutable
            {
                process->argv = array<string>{ std:("node"), std:("/usr/bin/elizaos"), std:("start") };
                mockExistsSync->mockReturnValue(true);
                auto mockChildProcess = object{
                    object::pair{std:("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std:("exit")) {
                            setTimeout([=]() mutable
                            {
                                return handler(nullptr, std:("SIGUSR1"));
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std:("kill"), mock()}, 
                    object::pair{std:("killed"), false}
                };
                mockSpawn->mockReturnValue(mockChildProcess);
                std::async([=]() { tryDelegateToLocalCli(); });
                expect(mockProcess["exit"])->toHaveBeenCalledWith(1);
            }
            );
        }
        );
    }
    );
}

MAIN
