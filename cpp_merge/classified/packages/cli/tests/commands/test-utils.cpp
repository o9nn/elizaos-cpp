#include "test-utils.h"
#include <string>

std::shared_ptr<Promise<string>> execShellCommand(string command, object options)
{
    auto shell = (process->platform == std::string("win32")) ? array<string>{ std::string("cmd"), std::string("/c") } : array<string>{ std::string("sh"), std::string("-c") };
    shared proc = Bun->spawn(array<string>{ shell, command }, object{
        object::pair{std::string("stdout"), (options["stdio"] == std::string("ignore")) ? std::string("ignore") : std::string("pipe")}, 
        object::pair{std::string("stderr"), (options["stdio"] == std::string("ignore")) ? std::string("ignore") : std::string("pipe")}
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
            if (options["stdio"] == std::string("ignore")) {
                std::async([=]() { proc->exited; });
                return string_empty;
            }
            auto [stdout, stderr] = std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<string>>, std::shared_ptr<Promise<string>>>{ (proc->stdout) ? ((std::make_shared<Response>(proc->stdout)))->text() : Promise->resolve(string_empty), (proc->stderr) ? ((std::make_shared<Response>(proc->stderr)))->text() : Promise->resolve(string_empty) }); });
            std::async([=]() { proc->exited; });
            if (AND((proc->exitCode != 0), (!options["stdio"]))) {
                auto error = as<any>(std::make_shared<Error>(std::string("Command failed: ") + command + std::string("\
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
    auto testTmpDir = std::async([=]() { mkdtemp(join(tmpdir(), std::string("eliza-test-"))); });
    process->chdir(testTmpDir);
    return object{
        object::pair{std::string("testTmpDir"), std::string("testTmpDir")}, 
        object::pair{std::string("originalCwd"), std::string("originalCwd")}
    };
};


std::shared_ptr<Promise<void>> cleanupTestEnvironment(std::shared_ptr<TestContext> context)
{
    safeChangeDirectory(context->originalCwd);
    if (AND((context->testTmpDir), (context->testTmpDir->includes(std::string("eliza-test-"))))) {
        try
        {
            std::async([=]() { rm(context->testTmpDir, object{
                object::pair{std::string("recursive"), true}
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
        object::pair{std::string("stdio"), std::string("pipe")}, 
        object::pair{std::string("timeout"), TEST_TIMEOUTS["PROJECT_CREATION"]}
    });
    try
    {
        auto result = std::async([=]() { bunExecSimple(std::string("elizaos create ") + projectName + std::string(" --yes")); });
        process->chdir(projectName);
    }
    catch (const any& error)
    {
        console->error(std::string("[Create Test Project Error] Failed to create ") + projectName + std::string(":"), object{
            object::pair{std::string("platform"), process->platform}, 
            object::pair{std::string("stdout"), OR((error["stdout"]["toString"]()), (string_empty))}, 
            object::pair{std::string("stderr"), OR((error["stderr"]["toString"]()), (string_empty))}
        });
        throw any(error);
    }
    return std::shared_ptr<Promise<void>>();
};


void expectHelpOutput(string output, string command, array<string> expectedOptions)
{
    if (!output->includes(std::string("Usage: elizaos ") + command + string_empty)) {
        throw any(std::make_shared<Error>(std::string("Expected help output to contain 'Usage: elizaos ") + command + std::string("', got: ") + output + string_empty));
    }
    for (auto& option : expectedOptions)
    {
        if (!output->includes(option)) {
            throw any(std::make_shared<Error>(std::string("Expected help output to contain '") + option + std::string("', got: ") + output + string_empty));
        }
    }
};


std::shared_ptr<Promise<void>> createTestPluginStructure(string pluginName)
{
    auto pluginDir = std::string("plugin-") + pluginName + string_empty;
    std::async([=]() { mkdir(pluginDir, object{
        object::pair{std::string("recursive"), true}
    }); });
    std::async([=]() { mkdir(join(pluginDir, std::string("src")), object{
        object::pair{std::string("recursive"), true}
    }); });
    auto packageJson = object{
        object::pair{std::string("name"), std::string("@elizaos/plugin-") + pluginName + string_empty}, 
        object::pair{std::string("version"), std::string("1.0.0")}, 
        object::pair{std::string("type"), std::string("module")}, 
        object::pair{std::string("main"), std::string("dist/index.js")}, 
        object::pair{std::string("elizaPlugin"), true}
    };
    std::async([=]() { writeFile(join(pluginDir, std::string("package.json")), JSON->stringify(packageJson, nullptr, 2)); });
    std::async([=]() { writeFile(join(pluginDir, std::string("src/index.ts")), std::string("{};")); });
    process->chdir(pluginDir);
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> createTestAgent(string agentName)
{
    auto agentData = object{
        object::pair{std::string("name"), agentName}, 
        object::pair{std::string("system"), std::string("You are a helpful assistant.")}, 
        object::pair{std::string("bio"), array<string>{ std::string("I am a test agent") }}, 
        object::pair{std::string("messageExamples"), array<array<object>>{ array<object>{ object{
            object::pair{std::string("user"), std::string("user")}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("Hello")}
            }}
        } }, array<object>{ object{
            object::pair{std::string("user"), std::string("assistant")}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("Hi there!")}
            }}
        } } }}, 
        object::pair{std::string("style"), object{
            object::pair{std::string("all"), array<string>{ std::string("helpful"), std::string("friendly") }}
        }}
    };
    std::async([=]() { writeFile(string_empty + agentName + std::string(".json"), JSON->stringify(agentData, nullptr, 2)); });
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> waitForServerReady(double port, double maxWaitTime, string endpoint)
{
    auto startTime = Date->now();
    auto isCI = OR((process->env->CI == std::string("true")), (process->env->GITHUB_ACTIONS == std::string("true")));
    auto isMacOS = process->platform == std::string("darwin");
    shared pollInterval = (AND((isMacOS), (isCI))) ? any(3000) ((isMacOS) ? 2000 : 1000);
    shared requestTimeout = (AND((isMacOS), (isCI))) ? any(6000) ((isMacOS) ? 4000 : 2000);
    console->log(std::string("[DEBUG] Waiting for server on port ") + port + std::string(", max wait: ") + maxWaitTime + std::string("ms, poll interval: ") + pollInterval + std::string("ms"));
    console->log(std::string("[DEBUG] Environment: CI=") + isCI + std::string(", macOS=") + isMacOS + string_empty);
    auto connectionAttempts = 0;
    auto maxConnectionAttempts = Math->max(5, Math->floor(maxWaitTime / (pollInterval * 2)));
    while (Date->now() - startTime < maxWaitTime)
    {
        try
        {
            shared net = require(std::string("net"));
            auto canConnect = std::async([=]() { std::make_shared<Promise<boolean>>([=](auto resolve) mutable
            {
                shared socket = std::make_shared<net["Socket"]>();
                shared connectTimeout = setTimeout([=]() mutable
                {
                    socket["destroy"]();
                    resolve(false);
                }
                , requestTimeout / 2);
                socket["connect"](port, std::string("127.0.0.1"), [=]() mutable
                {
                    clearTimeout(connectTimeout);
                    socket["destroy"]();
                    resolve(true);
                }
                );
                socket["on"](std::string("error"), [=]() mutable
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
                console->log(std::string("[DEBUG] Connection attempt ") + connectionAttempts + std::string("/") + maxConnectionAttempts + std::string(" failed - no process listening on port ") + port + std::string(", ") + Math->round(timeRemaining / 1000) + std::string("s remaining"));
                if (connectionAttempts >= maxConnectionAttempts) {
                    auto timeRemaining = maxWaitTime - (Date->now() - startTime);
                    if (timeRemaining < maxWaitTime * 0.3) {
                        console->log(std::string("[DEBUG] Giving up on connection test, trying HTTP anyway..."));
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
            auto response = std::async([=]() { fetch(std::string("http://127.0.0.1:") + port + string_empty + endpoint + string_empty, object{
                object::pair{std::string("signal"), controller->signal}, 
                object::pair{std::string("headers"), object{
                    object::pair{std::string("User-Agent"), std::string("ElizaOS-Test-Client/1.0")}, 
                    object::pair{std::string("Accept"), std::string("application/json")}, 
                    object::pair{std::string("Connection"), std::string("keep-alive")}, 
                    object::pair{std::string("Cache-Control"), std::string("no-cache")}
                }}
            }); });
            clearTimeout(timeoutId);
            if (response->ok) {
                console->log(std::string("[DEBUG] Server responded with status ") + response->status + string_empty);
                shared stabilizationTime = (AND((isMacOS), (isCI))) ? any(3000) ((isMacOS) ? 2000 : 1000);
                console->log(std::string("[DEBUG] Stabilizing for ") + stabilizationTime + std::string("ms..."));
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, stabilizationTime);
                }
                ); });
                return std::shared_ptr<Promise<void>>();
            } else {
                console->log(std::string("[DEBUG] Server responded with status ") + response->status + std::string(", continuing to wait..."));
            }
        }
        catch (const any& error)
        {
            auto timeRemaining = maxWaitTime - (Date->now() - startTime);
            auto errorMsg = (is<Error>(error)) ? any(error->message) (std::string("unknown error"));
            console->log(std::string("[DEBUG] Server not ready yet (") + errorMsg + std::string("), ") + Math->round(timeRemaining / 1000) + std::string("s remaining"));
            if (OR((errorMsg->includes(std::string("fetch"))), (errorMsg->includes(std::string("AbortError"))))) {
                connectionAttempts = 0;
            }
        }
        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
        {
            return setTimeout(resolve, pollInterval);
        }
        ); });
    }
    throw any(std::make_shared<Error>(std::string("Server failed to become ready on port ") + port + std::string(" within ") + maxWaitTime + std::string("ms")));
};


std::shared_ptr<Promise<void>> killProcessOnPort(double port)
{
    try
    {
        if (process->platform == std::string("win32")) {
            auto netstatResult = std::async([=]() { execShellCommand(std::string("netstat -ano | findstr :") + port + string_empty, object{
                object::pair{std::string("encoding"), std::string("utf8")}, 
                object::pair{std::string("stdio"), std::string("pipe")}
            }); });
            auto lines = netstatResult->split(std::string("\
"))->filter([=](auto line) mutable
            {
                return line->includes(std::string(":") + port + string_empty);
            }
            );
            auto pids = lines->map([=](auto line) mutable
            {
                auto parts = line->trim()->split((new RegExp(std::string("\s"))));
                return const_(parts)[parts->get_length() - 1];
            }
            )->filter([=](auto pid) mutable
            {
                return AND((pid), (pid != std::string("0")));
            }
            );
            for (auto& pid : pids)
            {
                try
                {
                    std::async([=]() { execShellCommand(std::string("taskkill /F /PID ") + pid + string_empty, object{
                        object::pair{std::string("stdio"), std::string("ignore")}
                    }); });
                }
                catch (const any& e)
                {
                }
            }
        } else if (process->platform == std::string("darwin")) {
            try
            {
                auto lsofResult = std::async([=]() { execShellCommand(std::string("lsof -ti:") + port + string_empty, object{
                    object::pair{std::string("encoding"), std::string("utf8")}, 
                    object::pair{std::string("stdio"), std::string("pipe")}, 
                    object::pair{std::string("timeout"), 10000}
                }); });
                auto pids = lsofResult->trim()->split(std::string("\
"))->filter([=](auto pid) mutable
                {
                    return AND((pid), ((new RegExp(std::string("^\d+")))->test(pid)));
                }
                );
                console->log(std::string("[DEBUG] Found ") + pids->get_length() + std::string(" processes on port ") + port + std::string(": ") + pids->join(std::string(", ")) + string_empty);
                for (auto& pid : pids)
                {
                    try
                    {
                        std::async([=]() { execShellCommand(std::string("ps -p ") + pid + string_empty, object{
                            object::pair{std::string("stdio"), std::string("ignore")}, 
                            object::pair{std::string("timeout"), 2000}
                        }); });
                        console->log(std::string("[DEBUG] Sending SIGTERM to PID ") + pid + string_empty);
                        std::async([=]() { execShellCommand(std::string("kill -TERM ") + pid + string_empty, object{
                            object::pair{std::string("stdio"), std::string("ignore")}, 
                            object::pair{std::string("timeout"), 3000}
                        }); });
                        shared waitTime = (process->env->CI == std::string("true")) ? 3000 : 1000;
                        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                        {
                            return setTimeout(resolve, waitTime);
                        }
                        ); });
                        try
                        {
                            std::async([=]() { execShellCommand(std::string("kill -0 ") + pid + string_empty, object{
                                object::pair{std::string("stdio"), std::string("ignore")}, 
                                object::pair{std::string("timeout"), 2000}
                            }); });
                            console->log(std::string("[DEBUG] Process ") + pid + std::string(" still running, sending SIGKILL"));
                            std::async([=]() { execShellCommand(std::string("kill -9 ") + pid + string_empty, object{
                                object::pair{std::string("stdio"), std::string("ignore")}, 
                                object::pair{std::string("timeout"), 3000}
                            }); });
                            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                            {
                                return setTimeout(resolve, 500);
                            }
                            ); });
                        }
                        catch (const any& e)
                        {
                            console->log(std::string("[DEBUG] Process ") + pid + std::string(" terminated gracefully"));
                        }
                    }
                    catch (const any& e)
                    {
                        console->log(std::string("[DEBUG] Process ") + pid + std::string(" not found or already terminated"));
                    }
                }
            }
            catch (const any& e)
            {
                console->log(std::string("[DEBUG] No processes found on port ") + port + std::string(" (expected if port is free)"));
            }
        } else {
            std::async([=]() { execShellCommand(std::string("lsof -ti:") + port + std::string(" | xargs kill -9 2>/dev/null || true"), object{
                object::pair{std::string("stdio"), std::string("ignore")}, 
                object::pair{std::string("timeout"), 5000}
            }); });
        }
        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
        {
            return setTimeout(resolve, (process->platform == std::string("darwin")) ? 2000 : 1000);
        }
        ); });
    }
    catch (const any& e)
    {
        console->log(std::string("[DEBUG] Port cleanup for ") + port + std::string(" encountered error:"), (is<Error>(e)) ? any(e->message) (std::string("unknown")));
    }
    return std::shared_ptr<Promise<void>>();
};


any getPlatformOptions(any baseOptions)
{
    auto platformOptions = utils::assign(object{
    }, baseOptions);
    platformOptions["env"] = utils::assign(object{
    }, process->env, baseOptions["env"]);
    if (process->platform == std::string("win32")) {
        if (platformOptions["timeout"] != std::nullopt) {
            platformOptions["timeout"] = platformOptions["timeout"] * 1.5;
        }
        platformOptions["killSignal"] = as<NodeJS::Signals>(std::string("SIGKILL"));
        platformOptions["windowsHide"] = true;
    } else if (process->platform == std::string("darwin")) {
        if (platformOptions["timeout"] != std::nullopt) {
            platformOptions["timeout"] = platformOptions["timeout"] * 1.25;
        }
        platformOptions["killSignal"] = as<NodeJS::Signals>(std::string("SIGTERM"));
        platformOptions["env"] = utils::assign(object{
            , 
            object::pair{std::string("LANG"), std::string("en_US.UTF-8")}, 
            object::pair{std::string("LC_ALL"), std::string("en_US.UTF-8")}
        }, platformOptions["env"]);
    }
    return platformOptions;
};


any TestProcessManager::spawn(string command, array<string> args, any options)
{
    auto processOptions = object{
        object::pair{std::string("cwd"), OR((options["cwd"]), (process->cwd()))}, 
        object::pair{std::string("env"), OR((options["env"]), (process->env))}, 
        object::pair{std::string("stdout"), (options["allowOutput"]) ? std::string("inherit") : std::string("ignore")}, 
        object::pair{std::string("stderr"), (options["allowOutput"]) ? std::string("inherit") : std::string("ignore")}, 
        object::pair{std::string("stdin"), std::string("ignore")}
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
            if (process["platform"] == std::string("win32")) {
                process["kill"](std::string("SIGTERM"));
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
                        process["kill"](std::string("SIGKILL"));
                    }
                    catch (const any& e)
                    {
                    }
                }
            } else {
                process["kill"](std::string("SIGTERM"));
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
    object::pair{std::string("removeDir"), [=](auto path) mutable
    {
        try
        {
            if (process->platform == std::string("win32")) {
                std::async([=]() { execShellCommand(std::string("if exist "") + path + std::string("" rmdir /s /q "") + path + std::string("""), object{
                    object::pair{std::string("stdio"), std::string("ignore")}
                }); });
            } else {
                std::async([=]() { execShellCommand(std::string("rm -rf "") + path + std::string("""), object{
                    object::pair{std::string("stdio"), std::string("ignore")}
                }); });
            }
        }
        catch (const any& e)
        {
        }
    }
    }, 
    object::pair{std::string("removeFile"), [=](auto path) mutable
    {
        try
        {
            if (process->platform == std::string("win32")) {
                std::async([=]() { execShellCommand(std::string("if exist "") + path + std::string("" del /q "") + path + std::string("""), object{
                    object::pair{std::string("stdio"), std::string("ignore")}
                }); });
            } else {
                std::async([=]() { execShellCommand(std::string("rm -f "") + path + std::string("""), object{
                    object::pair{std::string("stdio"), std::string("ignore")}
                }); });
            }
        }
        catch (const any& e)
        {
        }
    }
    }, 
    object::pair{std::string("killProcessOnPort"), killProcessOnPort}
};

void Main(void)
{
}

MAIN
