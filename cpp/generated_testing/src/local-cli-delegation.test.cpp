#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/unit/utils/local-cli-delegation.test.h"

any mockSpawn = mock();
any mockExistsSync = mock();
object mockLogger = object{
    object::pair{std::string("info"), mock()}, 
    object::pair{std::string("debug"), mock()}, 
    object::pair{std::string("error"), mock()}
};

void Main(void)
{
    mock->module(std::string("node:child_process"), [=]() mutable
    {
        return (object{
            object::pair{std::string("spawn"), mockSpawn}
        });
    }
    );
    mock->module(std::string("node:fs"), [=]() mutable
    {
        return (object{
            object::pair{std::string("existsSync"), mockExistsSync}
        });
    }
    );
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std::string("logger"), mockLogger}
        });
    }
    );
    describe(std::string("Local CLI Delegation"), [=]() mutable
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
            jest->spyOn(process, std::string("cwd"))->mockReturnValue(std::string("/test/project"));
            mockProcess = object{
                object::pair{std::string("exit"), mock()}
            };
            jest->spyOn(process, std::string("exit"))->mockImplementation(mockProcess["exit"]);
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
        describe(std::string("Test Environment Detection"), [=]() mutable
        {
            it(std::string("should skip delegation when NODE_ENV is test"), [=]() mutable
            {
                process->env->NODE_ENV = std::string("test");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should skip delegation when ELIZA_TEST_MODE is true"), [=]() mutable
            {
                process->env->ELIZA_TEST_MODE = std::string("true");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should skip delegation when BUN_TEST is true"), [=]() mutable
            {
                process->env->BUN_TEST = std::string("true");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should skip delegation when VITEST is true"), [=]() mutable
            {
                process->env->VITEST = std::string("true");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should skip delegation when JEST_WORKER_ID is set"), [=]() mutable
            {
                process->env->JEST_WORKER_ID = std::string("1");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should skip delegation when npm_lifecycle_event is test"), [=]() mutable
            {
                process->env->npm_lifecycle_event = std::string("test");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should skip delegation when --test is in process.argv"), [=]() mutable
            {
                process->argv = array<string>{ std::string("node"), std::string("script.js"), std::string("--test") };
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should skip delegation when test is in process.argv"), [=]() mutable
            {
                process->argv = array<string>{ std::string("node"), std::string("script.js"), std::string("test") };
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should skip delegation when script path includes test"), [=]() mutable
            {
                process->argv = array<string>{ std::string("node"), std::string("/path/to/test/script.js"), std::string("start") };
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should skip delegation when ELIZA_SKIP_LOCAL_CLI_DELEGATION is true"), [=]() mutable
            {
                process->env->ELIZA_SKIP_LOCAL_CLI_DELEGATION = std::string("true");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should skip delegation when CI is true"), [=]() mutable
            {
                process->env->CI = std::string("true");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should skip delegation when GITHUB_ACTIONS is true"), [=]() mutable
            {
                process->env->GITHUB_ACTIONS = std::string("true");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should skip delegation when GITLAB_CI is true"), [=]() mutable
            {
                process->env->GITLAB_CI = std::string("true");
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Running in test or CI environment, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std::string("Update Command Detection"), [=]() mutable
        {
            it(std::string("should skip delegation when update command is used"), [=]() mutable
            {
                auto originalArgv = process->argv;
                process->argv = array<string>{ std::string("node"), std::string("script.js"), std::string("update") };
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Update command detected, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
                process->argv = originalArgv;
            }
            );
            it(std::string("should skip delegation when update command is used with flags"), [=]() mutable
            {
                auto originalArgv = process->argv;
                process->argv = array<string>{ std::string("node"), std::string("script.js"), std::string("update"), std::string("--check") };
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("Update command detected, skipping local CLI delegation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
                process->argv = originalArgv;
            }
            );
        }
        );
        describe(std::string("Local CLI Detection"), [=]() mutable
        {
            it(std::string("should detect when running from local CLI"), [=]() mutable
            {
                process->env.Delete("NODE_ENV");
                process->env.Delete("ELIZA_TEST_MODE");
                process->env.Delete("BUN_TEST");
                process->env.Delete("VITEST");
                process->env.Delete("JEST_WORKER_ID");
                process->env.Delete("npm_lifecycle_event");
                process->argv = array<string>{ std::string("node"), std::string("/test/project/node_modules/@elizaos/cli/dist/index.js"), std::string("start") };
                mockExistsSync->mockReturnValue(true);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalled();
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should continue when no local CLI is found"), [=]() mutable
            {
                process->argv = array<string>{ std::string("node"), std::string("/usr/bin/elizaos"), std::string("start") };
                mockExistsSync->mockReturnValue(false);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["debug"])->toHaveBeenCalledWith(std::string("No local CLI found, using global installation"));
                expect(mockSpawn)->not->toHaveBeenCalled();
            }
            );
            it(std::string("should delegate when local CLI is found and not running from it"), [=]() mutable
            {
                process->argv = array<string>{ std::string("node"), std::string("/usr/bin/elizaos"), std::string("start"), std::string("--port"), std::string("3000") };
                mockExistsSync->mockReturnValue(true);
                auto mockChildProcess = object{
                    object::pair{std::string("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std::string("exit")) {
                            setTimeout([=]() mutable
                            {
                                return handler(0, nullptr);
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std::string("kill"), mock()}, 
                    object::pair{std::string("killed"), false}
                };
                mockSpawn->mockReturnValue(mockChildProcess);
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(true);
                expect(mockLogger["info"])->toHaveBeenCalledWith(std::string("Using local @elizaos/cli installation"));
                expect(mockSpawn)->toHaveBeenCalledWith(process->execPath, array<string>{ std::string("/test/project/node_modules/@elizaos/cli/dist/index.js"), std::string("start"), std::string("--port"), std::string("3000") }, expect->objectContaining(object{
                    object::pair{std::string("stdio"), std::string("inherit")}, 
                    object::pair{std::string("cwd"), std::string("/test/project")}, 
                    object::pair{std::string("env"), expect->objectContaining(object{
                        object::pair{std::string("FORCE_COLOR"), std::string("1")}
                    })}
                }));
            }
            );
        }
        );
        describe(std::string("Environment Setup"), [=]() mutable
        {
            it(std::string("should set up proper environment variables for local execution"), [=]() mutable
            {
                process->argv = array<string>{ std::string("node"), std::string("/usr/bin/elizaos"), std::string("start") };
                mockExistsSync->mockReturnValue(true);
                auto mockChildProcess = object{
                    object::pair{std::string("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std::string("exit")) {
                            setTimeout([=]() mutable
                            {
                                return handler(0, nullptr);
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std::string("kill"), mock()}, 
                    object::pair{std::string("killed"), false}
                };
                mockSpawn->mockReturnValue(mockChildProcess);
                std::async([=]() { tryDelegateToLocalCli(); });
                auto spawnCall = const_(mockSpawn->mock->calls)[0];
                auto spawnOptions = const_(spawnCall)[2];
                auto env = spawnOptions->env;
                expect(env->FORCE_COLOR)->toBe(std::string("1"));
                expect(env->NODE_PATH)->toContain(std::string("/test/project/node_modules"));
                expect(env->PATH)->toContain(std::string("/test/project/node_modules/.bin"));
            }
            );
            it(std::string("should preserve existing NODE_PATH and PATH"), [=]() mutable
            {
                process->env->NODE_PATH = std::string("/existing/node/path");
                process->env->PATH = std::string("/existing/bin/path");
                process->argv = array<string>{ std::string("node"), std::string("/usr/bin/elizaos"), std::string("start") };
                mockExistsSync->mockReturnValue(true);
                auto mockChildProcess = object{
                    object::pair{std::string("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std::string("exit")) {
                            setTimeout([=]() mutable
                            {
                                return handler(0, nullptr);
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std::string("kill"), mock()}, 
                    object::pair{std::string("killed"), false}
                };
                mockSpawn->mockReturnValue(mockChildProcess);
                std::async([=]() { tryDelegateToLocalCli(); });
                auto spawnCall = const_(mockSpawn->mock->calls)[0];
                auto spawnOptions = const_(spawnCall)[2];
                auto env = spawnOptions->env;
                expect(env->NODE_PATH)->toContain(std::string("/test/project/node_modules"));
                expect(env->NODE_PATH)->toContain(std::string("/existing/node/path"));
                expect(env->PATH)->toContain(std::string("/test/project/node_modules/.bin"));
                expect(env->PATH)->toContain(std::string("/existing/bin/path"));
            }
            );
        }
        );
        describe(std::string("Error Handling"), [=]() mutable
        {
            it(std::string("should handle spawn errors gracefully"), [=]() mutable
            {
                process->argv = array<string>{ std::string("node"), std::string("/usr/bin/elizaos"), std::string("start") };
                mockExistsSync->mockReturnValue(true);
                shared testError = std::make_shared<Error>(std::string("Spawn failed"));
                mockSpawn->mockImplementation([=]() mutable
                {
                    throw any(testError);
                }
                );
                auto result = std::async([=]() { tryDelegateToLocalCli(); });
                expect(result)->toBe(false);
                expect(mockLogger["error"])->toHaveBeenCalledWith(std::string("Error during local CLI delegation:"), testError);
                expect(mockLogger["info"])->toHaveBeenCalledWith(std::string("Falling back to global CLI installation"));
            }
            );
            it(std::string("should handle process errors"), [=]() mutable
            {
                process->argv = array<string>{ std::string("node"), std::string("/usr/bin/elizaos"), std::string("start") };
                mockExistsSync->mockReturnValue(true);
                shared testError = std::make_shared<Error>(std::string("Process error"));
                auto mockChildProcess = object{
                    object::pair{std::string("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std::string("error")) {
                            setTimeout([=]() mutable
                            {
                                return handler(testError);
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std::string("kill"), mock()}, 
                    object::pair{std::string("killed"), false}
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
                expect(mockLogger["error"])->toHaveBeenCalledWith(std::string("Failed to start local CLI: Process error"));
            }
            );
        }
        );
        describe(std::string("Utility Functions"), [=]() mutable
        {
            it(std::string("hasLocalCli should return true when local CLI exists"), [=]() mutable
            {
                mockExistsSync->mockReturnValue(true);
                expect(hasLocalCli())->toBe(true);
            }
            );
            it(std::string("hasLocalCli should return false when local CLI does not exist"), [=]() mutable
            {
                mockExistsSync->mockReturnValue(false);
                expect(hasLocalCli())->toBe(false);
            }
            );
            it(std::string("getCliContext should return correct context information"), [=]() mutable
            {
                process->argv = array<string>{ std::string("node"), std::string("/test/project/node_modules/@elizaos/cli/dist/index.js"), std::string("start") };
                mockExistsSync->mockReturnValue(true);
                auto context = getCliContext();
                expect(context["isLocal"])->toBe(true);
                expect(context["hasLocal"])->toBe(true);
                expect(context["localPath"])->toBe(std::string("/test/project/node_modules/@elizaos/cli/dist/index.js"));
                expect(context["currentPath"])->toBe(std::string("/test/project/node_modules/@elizaos/cli/dist/index.js"));
            }
            );
            it(std::string("getCliContext should return correct context when not running from local CLI"), [=]() mutable
            {
                process->argv = array<string>{ std::string("node"), std::string("/usr/bin/elizaos"), std::string("start") };
                mockExistsSync->mockReturnValue(false);
                auto context = getCliContext();
                expect(context["isLocal"])->toBe(false);
                expect(context["hasLocal"])->toBe(false);
                expect(context["localPath"])->toBe(nullptr);
                expect(context["currentPath"])->toBe(std::string("/usr/bin/elizaos"));
            }
            );
        }
        );
        describe(std::string("Process Exit Handling"), [=]() mutable
        {
            it(std::string("should exit with child process exit code"), [=]() mutable
            {
                process->argv = array<string>{ std::string("node"), std::string("/usr/bin/elizaos"), std::string("start") };
                mockExistsSync->mockReturnValue(true);
                auto mockChildProcess = object{
                    object::pair{std::string("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std::string("exit")) {
                            setTimeout([=]() mutable
                            {
                                return handler(42, nullptr);
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std::string("kill"), mock()}, 
                    object::pair{std::string("killed"), false}
                };
                mockSpawn->mockReturnValue(mockChildProcess);
                std::async([=]() { tryDelegateToLocalCli(); });
                expect(mockProcess["exit"])->toHaveBeenCalledWith(42);
            }
            );
            it(std::string("should exit with appropriate code when killed by signal"), [=]() mutable
            {
                process->argv = array<string>{ std::string("node"), std::string("/usr/bin/elizaos"), std::string("start") };
                mockExistsSync->mockReturnValue(true);
                auto mockChildProcess = object{
                    object::pair{std::string("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std::string("exit")) {
                            setTimeout([=]() mutable
                            {
                                return handler(nullptr, std::string("SIGTERM"));
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std::string("kill"), mock()}, 
                    object::pair{std::string("killed"), false}
                };
                mockSpawn->mockReturnValue(mockChildProcess);
                std::async([=]() { tryDelegateToLocalCli(); });
                expect(mockProcess["exit"])->toHaveBeenCalledWith(143);
            }
            );
            it(std::string("should exit with 130 for SIGINT"), [=]() mutable
            {
                process->argv = array<string>{ std::string("node"), std::string("/usr/bin/elizaos"), std::string("start") };
                mockExistsSync->mockReturnValue(true);
                auto mockChildProcess = object{
                    object::pair{std::string("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std::string("exit")) {
                            setTimeout([=]() mutable
                            {
                                return handler(nullptr, std::string("SIGINT"));
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std::string("kill"), mock()}, 
                    object::pair{std::string("killed"), false}
                };
                mockSpawn->mockReturnValue(mockChildProcess);
                std::async([=]() { tryDelegateToLocalCli(); });
                expect(mockProcess["exit"])->toHaveBeenCalledWith(130);
            }
            );
            it(std::string("should exit with 1 for unknown signal"), [=]() mutable
            {
                process->argv = array<string>{ std::string("node"), std::string("/usr/bin/elizaos"), std::string("start") };
                mockExistsSync->mockReturnValue(true);
                auto mockChildProcess = object{
                    object::pair{std::string("on"), mock([=](auto event, auto handler) mutable
                    {
                        if (event == std::string("exit")) {
                            setTimeout([=]() mutable
                            {
                                return handler(nullptr, std::string("SIGUSR1"));
                            }
                            , 10);
                        }
                    }
                    )}, 
                    object::pair{std::string("kill"), mock()}, 
                    object::pair{std::string("killed"), false}
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
