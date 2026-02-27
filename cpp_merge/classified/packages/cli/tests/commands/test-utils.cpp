#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/commands/test-utils.h"

std::shared_ptr<Promise<string>> execShellCommand(string command, object options)
{
    auto shell = (process->platform == std:("win32")) ? array<string>{ std:("cmd"), std:("/c") } : array<string>{ std:("sh"), std:("-c") };
    shared proc = Bun->spawn(array<string>{ shell, command }, object{
        object::pair{std:("stdout"), (options["stdio"] == std:("ignore")) ? std:("ignore") : std:("pipe")}, 
        object::pair{std:("stderr"), (options["stdio"] == std:("ignore")) ? std:("ignore") : std:("pipe")}
    });
    auto timeoutId = nullptr;
    if (options["timeout"]) {
        timeoutId = setTimeout([=]() mutable
        {
            proc->kill();
        }
        , options["timeout"]);
    }
    {
        utils::finally __finally1769_1833([&]() mutable
        {
            if (timeoutId) {
                clearTimeout(timeoutId);
            }
        });
        try
        {
            if (options["stdio"] == std:("ignore")) {
                std::async([=]() { proc->exited; });
                return string_empty;
            }
            auto [stdout, stderr] = std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<string>>, std::shared_ptr<Promise<string>>>{ (proc->stdout) ? ((std::make_shared<Response>(proc->stdout)))->text() : Promise->resolve(string_empty), (proc->stderr) ? ((std::make_shared<Response>(proc->stderr)))->text() : Promise->resolve(string_empty) }); });
            std::async([=]() { proc->exited; });
            if (AND((proc->exitCode != 0), (!options["stdio"]))) {
                auto error = as<any>(std::make_shared<Error>(std:("Command failed: ") + command + std:("\
stderr: ") + stderr + string_empty));
                error->status = proc->exitCode;
                error->stdout = stdout;
                error->stderr = stderr;
                throw any(error);
            }
            return stdout;
        }
        catch (...)
        {
            throw;
        }
    }
};


std::shared_ptr<Promise<std::shared_ptr<TestContext>>> setupTestEnvironment()
{
    auto originalCwd = process->cwd();
    auto testTmpDir = std::async([=]() { mkdtemp(join(tmpdir(), std:("eliza-test-"))); });
    process->chdir(testTmpDir);
    return object{
        object::pair{std:("testTmpDir"), std:("testTmpDir")}, 
        object::pair{std:("originalCwd"), std:("originalCwd")}
    };
};


std::shared_ptr<Promise<void>> cleanupTestEnvironment(std::shared_ptr<TestContext> context)
{
    safeChangeDirectory(context->originalCwd);
    if (AND((context->testTmpDir), (context->testTmpDir->includes(std:("eliza-test-"))))) {
        try
        {
            std::async([=]() { rm(context->testTmpDir, object{
                object::pair{std:("recursive"), true}
            }); });
        }
        catch (const any& e)
        {
        }
    }
    return std::shared_ptr<Promise<void>>();
};


void safeChangeDirectory(string targetDir)
{
    if (existsSync(targetDir)) {
        try
        {
            process->chdir(targetDir);
        }
        catch (const any& e)
        {
            try
            {
                process->chdir(tmpdir());
            }
            catch (const any& e2)
            {
            }
        }
    } else {
        try
        {
            process->chdir(tmpdir());
        }
        catch (const any& e)
        {
        }
    }
};


std::shared_ptr<Promise<void>> createTestProject(string projectName)
{
    auto platformOptions = getPlatformOptions(object{
        object::pair{std:("stdio"), std:("pipe")}, 
        object::pair{std:("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
    });
    try
    {
        auto result = std::async([=]() { bunExecSimple(std:("elizaos create ") + projectName + std:(" --yes")); });
        process->chdir(projectName);
    }
    catch (const any& error)
    {
        console->error(std:("[Create Test Project Error] Failed to create ") + projectName + std:(":"), object{
            object::pair{std:("platform"), process->platform}, 
            object::pair{std:("stdout"), OR((error["stdout"]["toString"]()), (string_empty))}, 
            object::pair{std:("stderr"), OR((error["stderr"]["toString"]()), (string_empty))}
        });
        throw any(error);
    }
    return std::shared_ptr<Promise<void>>();
};


void expectHelpOutput(string output, string command, array<string> expectedOptions)
{
    if (!output->includes(std:("Usage: elizaos ") + command + string_empty)) {
        throw any(std::make_shared<Error>(std:("Expected help output to contain 'Usage: elizaos ") + command + std:("', got: ") + output + string_empty));
    }
    for (auto& option : expectedOptions)
    {
        if (!output->includes(option)) {
            throw any(std::make_shared<Error>(std:("Expected help output to contain '") + option + std:("', got: ") + output + string_empty));
        }
    }
};


std::shared_ptr<Promise<void>> createTestPluginStructure(string pluginName)
{
    auto pluginDir = std:("plugin-") + pluginName + string_empty;
    std::async([=]() { mkdir(pluginDir, object{
        object::pair{std:("recursive"), true}
    }); });
    std::async([=]() { mkdir(join(pluginDir, std:("src")), object{
        object::pair{std:("recursive"), true}
    }); });
    auto packageJson = object{
        object::pair{std:("name"), std:("@elizaos/plugin-") + pluginName + string_empty}, 
        object::pair{std:("version"), std:("1.0.0")}, 
        object::pair{std:("type"), std:("module")}, 
        object::pair{std:("main"), std:("dist/index.js")}, 
        object::pair{std:("elizaPlugin"), true}
    };
    std::async([=]() { writeFile(join(pluginDir, std:("package.json")), JSON->stringify(packageJson, nullptr, 2)); });
    std::async([=]() { writeFile(join(pluginDir, std:("src/index.ts")), std:("{};")); });
    process->chdir(pluginDir);
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> createTestAgent(string agentName)
{
    auto agentData = object{
        object::pair{std:("name"), agentName}, 
        object::pair{std:("system"), std:("You are a helpful assistant.")}, 
        object::pair{std:("bio"), array<string>{ std:("I am a test agent") }}, 
        object::pair{std:("messageExamples"), array<array<object>>{ array<object>{ object{
            object::pair{std:("user"), std:("user")}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("Hello")}
            }}
        } }, array<object>{ object{
            object::pair{std:("user"), std:("assistant")}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("Hi there!")}
            }}
        } } }}, 
        object::pair{std:("style"), object{
            object::pair{std:("all"), array<string>{ std:("helpful"), std:("friendly") }}
        }}
    };
    std::async([=]() { writeFile(string_empty + agentName + std:(".json"), JSON->stringify(agentData, nullptr, 2)); });
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> waitForServerReady(double port, double maxWaitTime, string endpoint)
{
    auto startTime = Date->now();
    auto isCI = OR((process->env->CI == std:("true")), (process->env->GITHUB_ACTIONS == std:("true")));
    auto isMacOS = process->platform == std:("darwin");
    shared pollInterval = (AND((isMacOS), (isCI))) ? any(3000) ((isMacOS) ? 2000 : 1000);
    shared requestTimeout = (AND((isMacOS), (isCI))) ? any(6000) ((isMacOS) ? 4000 : 2000);
    console->log(std:("[DEBUG] Waiting for server on port ") + port + std:(", max wait: ") + maxWaitTime + std:("ms, poll interval: ") + pollInterval + std:("ms"));
    console->log(std:("[DEBUG] Environment: CI=") + isCI + std:(", macOS=") + isMacOS + string_empty);
    auto connectionAttempts = 0;
    auto maxConnectionAttempts = Math->max(5, Math->floor(maxWaitTime / (pollInterval * 2)));
    while (Date->now() - startTime < maxWaitTime)
    {
        try
        {
            shared net = require(std:("net"));
            auto canConnect = std::async([=]() { std::make_shared<Promise<boolean>>([=](auto resolve) mutable
            {
                shared socket = std::make_shared<net["Socket"]>();
                shared connectTimeout = setTimeout([=]() mutable
                {
                    socket["destroy"]();
                    resolve(false);
                }
                , requestTimeout / 2);
                socket["connect"](port, std:("127.0.0.1"), [=]() mutable
                {
                    clearTimeout(connectTimeout);
                    socket["destroy"]();
                    resolve(true);
                }
                );
                socket["on"](std:("error"), [=]() mutable
                {
                    clearTimeout(connectTimeout);
                    socket["destroy"]();
                    resolve(false);
                }
                );
            }
            ); });
            if (!canConnect) {
                connectionAttempts++;
                auto timeRemaining = maxWaitTime - (Date->now() - startTime);
                console->log(std:("[DEBUG] Connection attempt ") + connectionAttempts + std:("/") + maxConnectionAttempts + std:(" failed - no process listening on port ") + port + std:(", ") + Math->round(timeRemaining / 1000) + std:("s remaining"));
                if (connectionAttempts >= maxConnectionAttempts) {
                    auto timeRemaining = maxWaitTime - (Date->now() - startTime);
                    if (timeRemaining < maxWaitTime * 0.3) {
                        console->log(std:("[DEBUG] Giving up on connection test, trying HTTP anyway..."));
                    } else {
                        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                        {
                            return setTimeout(resolve, pollInterval);
                        }
                        ); });
                        continue;
                    }
                } else {
                    std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                    {
                        return setTimeout(resolve, pollInterval / 2);
                    }
                    ); });
                    continue;
                }
            }
            shared controller = std::make_shared<AbortController>();
            auto timeoutId = setTimeout([=]() mutable
            {
                return controller->abort();
            }
            , requestTimeout);
            auto response = std::async([=]() { fetch(std:("http://127.0.0.1:") + port + string_empty + endpoint + string_empty, object{
                object::pair{std:("signal"), controller->signal}, 
                object::pair{std:("headers"), object{
                    object::pair{std:("User-Agent"), std:("ElizaOS-Test-Client/1.0")}, 
                    object::pair{std:("Accept"), std:("application/json")}, 
                    object::pair{std:("Connection"), std:("keep-alive")}, 
                    object::pair{std:("Cache-Control"), std:("no-cache")}
                }}
            }); });
            clearTimeout(timeoutId);
            if (response->ok) {
                console->log(std:("[DEBUG] Server responded with status ") + response->status + string_empty);
                shared stabilizationTime = (AND((isMacOS), (isCI))) ? any(3000) ((isMacOS) ? 2000 : 1000);
                console->log(std:("[DEBUG] Stabilizing for ") + stabilizationTime + std:("ms..."));
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, stabilizationTime);
                }
                ); });
                return std::shared_ptr<Promise<void>>();
            } else {
                console->log(std:("[DEBUG] Server responded with status ") + response->status + std:(", continuing to wait..."));
            }
        }
        catch (const any& error)
        {
            auto timeRemaining = maxWaitTime - (Date->now() - startTime);
            auto errorMsg = (is<Error>(error)) ? any(error->message) (std:("unknown error"));
            console->log(std:("[DEBUG] Server not ready yet (") + errorMsg + std:("), ") + Math->round(timeRemaining / 1000) + std:("s remaining"));
            if (OR((errorMsg->includes(std:("fetch"))), (errorMsg->includes(std:("AbortError"))))) {
                connectionAttempts = 0;
            }
        }
        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
        {
            return setTimeout(resolve, pollInterval);
        }
        ); });
    }
    throw any(std::make_shared<Error>(std:("Server failed to become ready on port ") + port + std:(" within ") + maxWaitTime + std:("ms")));
};


std::shared_ptr<Promise<void>> killProcessOnPort(double port)
{
    try
    {
        if (process->platform == std:("win32")) {
            auto netstatResult = std::async([=]() { execShellCommand(std:("netstat -ano | findstr :") + port + string_empty, object{
                object::pair{std:("encoding"), std:("utf8")}, 
                object::pair{std:("stdio"), std:("pipe")}
            }); });
            auto lines = netstatResult->split(std:("\
"))->filter([=](auto line) mutable
            {
                return line->includes(std:(":") + port + string_empty);
            }
            );
            auto pids = lines->map([=](auto line) mutable
            {
                auto parts = line->trim()->split((new RegExp(std:("\s"))));
                return const_(parts)[parts->get_length() - 1];
            }
            )->filter([=](auto pid) mutable
            {
                return AND((pid), (pid != std:("0")));
            }
            );
            for (auto& pid : pids)
            {
                try
                {
                    std::async([=]() { execShellCommand(std:("taskkill /F /PID ") + pid + string_empty, object{
                        object::pair{std:("stdio"), std:("ignore")}
                    }); });
                }
                catch (const any& e)
                {
                }
            }
        } else if (process->platform == std:("darwin")) {
            try
            {
                auto lsofResult = std::async([=]() { execShellCommand(std:("lsof -ti:") + port + string_empty, object{
                    object::pair{std:("encoding"), std:("utf8")}, 
                    object::pair{std:("stdio"), std:("pipe")}, 
                    object::pair{std:("timeout"), 10000}
                }); });
                auto pids = lsofResult->trim()->split(std:("\
"))->filter([=](auto pid) mutable
                {
                    return AND((pid), ((new RegExp(std:("^\d+")))->test(pid)));
                }
                );
                console->log(std:("[DEBUG] Found ") + pids->get_length() + std:(" processes on port ") + port + std:(": ") + pids->join(std:(", ")) + string_empty);
                for (auto& pid : pids)
                {
                    try
                    {
                        std::async([=]() { execShellCommand(std:("ps -p ") + pid + string_empty, object{
                            object::pair{std:("stdio"), std:("ignore")}, 
                            object::pair{std:("timeout"), 2000}
                        }); });
                        console->log(std:("[DEBUG] Sending SIGTERM to PID ") + pid + string_empty);
                        std::async([=]() { execShellCommand(std:("kill -TERM ") + pid + string_empty, object{
                            object::pair{std:("stdio"), std:("ignore")}, 
                            object::pair{std:("timeout"), 3000}
                        }); });
                        shared waitTime = (process->env->CI == std:("true")) ? 3000 : 1000;
                        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                        {
                            return setTimeout(resolve, waitTime);
                        }
                        ); });
                        try
                        {
                            std::async([=]() { execShellCommand(std:("kill -0 ") + pid + string_empty, object{
                                object::pair{std:("stdio"), std:("ignore")}, 
                                object::pair{std:("timeout"), 2000}
                            }); });
                            console->log(std:("[DEBUG] Process ") + pid + std:(" still running, sending SIGKILL"));
                            std::async([=]() { execShellCommand(std:("kill -9 ") + pid + string_empty, object{
                                object::pair{std:("stdio"), std:("ignore")}, 
                                object::pair{std:("timeout"), 3000}
                            }); });
                            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                            {
                                return setTimeout(resolve, 500);
                            }
                            ); });
                        }
                        catch (const any& e)
                        {
                            console->log(std:("[DEBUG] Process ") + pid + std:(" terminated gracefully"));
                        }
                    }
                    catch (const any& e)
                    {
                        console->log(std:("[DEBUG] Process ") + pid + std:(" not found or already terminated"));
                    }
                }
            }
            catch (const any& e)
            {
                console->log(std:("[DEBUG] No processes found on port ") + port + std:(" (expected if port is free)"));
            }
        } else {
            std::async([=]() { execShellCommand(std:("lsof -ti:") + port + std:(" | xargs kill -9 2>/dev/null || true"), object{
                object::pair{std:("stdio"), std:("ignore")}, 
                object::pair{std:("timeout"), 5000}
            }); });
        }
        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
        {
            return setTimeout(resolve, (process->platform == std:("darwin")) ? 2000 : 1000);
        }
        ); });
    }
    catch (const any& e)
    {
        console->log(std:("[DEBUG] Port cleanup for ") + port + std:(" encountered error:"), (is<Error>(e)) ? any(e->message) (std:("unknown")));
    }
    return std::shared_ptr<Promise<void>>();
};


any getPlatformOptions(any baseOptions)
{
    auto platformOptions = utils::assign(object{
    }, baseOptions);
    platformOptions["env"] = utils::assign(object{
    }, process->env, baseOptions["env"]);
    if (process->platform == std:("win32")) {
        if (platformOptions["timeout"] != undefined) {
            platformOptions["timeout"] = platformOptions["timeout"] * 1.5;
        }
        platformOptions["killSignal"] = as<NodeJS::Signals>(std:("SIGKILL"));
        platformOptions["windowsHide"] = true;
    } else if (process->platform == std:("darwin")) {
        if (platformOptions["timeout"] != undefined) {
            platformOptions["timeout"] = platformOptions["timeout"] * 1.25;
        }
        platformOptions["killSignal"] = as<NodeJS::Signals>(std:("SIGTERM"));
        platformOptions["env"] = utils::assign(object{
            , 
            object::pair{std:("LANG"), std:("en_US.UTF-8")}, 
            object::pair{std:("LC_ALL"), std:("en_US.UTF-8")}
        }, platformOptions["env"]);
    }
    return platformOptions;
};


any TestProcessManager::spawn(string command, array<string> args, any options)
{
    auto processOptions = object{
        object::pair{std:("cwd"), OR((options["cwd"]), (process->cwd()))}, 
        object::pair{std:("env"), OR((options["env"]), (process->env))}, 
        object::pair{std:("stdout"), (options["allowOutput"]) ? std:("inherit") : std:("ignore")}, 
        object::pair{std:("stderr"), (options["allowOutput"]) ? std:("inherit") : std:("ignore")}, 
        object::pair{std:("stdin"), std:("ignore")}
    };
    shared childProcess = Bun->spawn(array<string>{ command, args }, processOptions);
    this->processes->add(childProcess);
    childProcess->exited->then([=]() mutable
    {
        this->processes->delete(childProcess);
    }
    );
    return childProcess;
}

std::shared_ptr<Promise<void>> TestProcessManager::terminateProcess(any process)
{
    if (OR((!process), (process["exitCode"] != nullptr))) {
        return std::shared_ptr<Promise<void>>();
    }
    {
        utils::finally __finally19077_19123([&]() mutable
        {
            this->processes->delete(process);
        });
        try
        {
            auto exitPromise = process["exited"];
            if (process["platform"] == std:("win32")) {
                process["kill"](std:("SIGTERM"));
                auto gracefulTimeout = std::make_shared<Promise<boolean>>([=](auto resolve) mutable
                {
                    setTimeout([=]() mutable
                    {
                        return resolve(false);
                    }
                    , 1000);
                }
                );
                auto wasGraceful = std::async([=]() { Promise->race(array<any>{ exitPromise["then"]([=]() mutable
                {
                    return true;
                }
                ), gracefulTimeout }); });
                if (AND((!wasGraceful), (process["exitCode"] == nullptr))) {
                    try
                    {
                        process["kill"](std:("SIGKILL"));
                    }
                    catch (const any& e)
                    {
                    }
                }
            } else {
                process["kill"](std:("SIGTERM"));
            }
            auto timeoutPromise = std::make_shared<Promise<void>>([=](auto resolve) mutable
            {
                setTimeout(resolve, TEST_TIMEOUTS["PROCESS_CLEANUP"]);
            }
            );
            std::async([=]() { Promise->race(array<any>{ exitPromise, timeoutPromise }); });
        }
        catch (const any& error)
        {
        }
    }
}

std::shared_ptr<Promise<void>> TestProcessManager::cleanup()
{
    auto cleanupPromises = Array->from(this->processes)->map([=](auto proc) mutable
    {
        return this->terminateProcess(proc);
    }
    );
    std::async([=]() { Promise->allSettled(cleanupPromises); });
    this->processes->clear();
    return std::shared_ptr<Promise<void>>();
}

double TestProcessManager::getActiveCount()
{
    return this->processes->size;
}

object assertions = object{
    , 
    , 
};
object crossPlatform = object{
    object::pair{std:("removeDir"), [=](auto path) mutable
    {
        try
        {
            if (process->platform == std:("win32")) {
                std::async([=]() { execShellCommand(std:("if exist "") + path + std:("" rmdir /s /q "") + path + std:("""), object{
                    object::pair{std:("stdio"), std:("ignore")}
                }); });
            } else {
                std::async([=]() { execShellCommand(std:("rm -rf "") + path + std:("""), object{
                    object::pair{std:("stdio"), std:("ignore")}
                }); });
            }
        }
        catch (const any& e)
        {
        }
    }
    }, 
    object::pair{std:("removeFile"), [=](auto path) mutable
    {
        try
        {
            if (process->platform == std:("win32")) {
                std::async([=]() { execShellCommand(std:("if exist "") + path + std:("" del /q "") + path + std:("""), object{
                    object::pair{std:("stdio"), std:("ignore")}
                }); });
            } else {
                std::async([=]() { execShellCommand(std:("rm -f "") + path + std:("""), object{
                    object::pair{std:("stdio"), std:("ignore")}
                }); });
            }
        }
        catch (const any& e)
        {
        }
    }
    }, 
    object::pair{std:("killProcessOnPort"), killProcessOnPort}
};

void Main(void)
{
}

MAIN
