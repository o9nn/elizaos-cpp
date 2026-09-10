// agentshell_test.cpp - focused behavioral and concurrency tests for AgentShell.
#include <gtest/gtest.h>
#include "elizaos/agentshell.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <future>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

using namespace elizaos;
using namespace std::chrono_literals;

namespace {

struct BlockingInput {
    std::mutex mutex;
    std::condition_variable cv;
    bool entered = false;
    bool cancelled = false;
    std::vector<std::string> commands;
    std::size_t next = 0;

    bool read(const std::string&, std::string& command) {
        std::unique_lock<std::mutex> lock(mutex);
        entered = true;
        cv.notify_all();
        cv.wait(lock, [this] { return cancelled || next < commands.size(); });
        if (cancelled) {
            return false;
        }
        command = commands[next++];
        return true;
    }

    void cancel() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            cancelled = true;
        }
        cv.notify_all();
    }

    bool waitUntilEntered() {
        std::unique_lock<std::mutex> lock(mutex);
        return cv.wait_for(lock, 2s, [this] { return entered; });
    }
};

void configureBlockingInput(AgentShell& shell, const std::shared_ptr<BlockingInput>& input) {
    shell.setInputHandler(
        [input](const std::string& prompt, std::string& command) {
            return input->read(prompt, command);
        },
        [input] { input->cancel(); });
}

} // namespace

class AgentShellTest : public ::testing::Test {
protected:
    AgentShell shell;
};

TEST_F(AgentShellTest, CommandResultConstructorsPreserveValues) {
    const ShellCommandResult defaultResult;
    EXPECT_TRUE(defaultResult.success);
    EXPECT_TRUE(defaultResult.output.empty());
    EXPECT_TRUE(defaultResult.error.empty());
    EXPECT_EQ(defaultResult.exitCode, 0);

    const ShellCommandResult failure(false, "partial", "failed", 7);
    EXPECT_FALSE(failure.success);
    EXPECT_EQ(failure.output, "partial");
    EXPECT_EQ(failure.error, "failed");
    EXPECT_EQ(failure.exitCode, 7);
}

TEST_F(AgentShellTest, BuiltinsAreDeterministicSortedAndComplete) {
    const std::vector<std::string> expected{
        "clear", "echo", "exit", "help", "history", "info", "quit", "status", "version"};
    EXPECT_EQ(shell.getAvailableCommands(), expected);

    const auto help = shell.executeCommand("help");
    ASSERT_TRUE(help.success) << help.error;
    std::size_t prior = 0;
    for (const auto& command : expected) {
        const auto position = help.output.find("  " + command);
        ASSERT_NE(position, std::string::npos) << command;
        EXPECT_GE(position, prior);
        prior = position;
    }
}

TEST_F(AgentShellTest, BuiltinsHaveStableOutputsAndStrictArity) {
    const auto echo = shell.executeCommand("echo one two");
    EXPECT_TRUE(echo.success);
    EXPECT_EQ(echo.output, "one two");

    const auto clear = shell.executeCommand("clear");
    EXPECT_TRUE(clear.success);
    EXPECT_EQ(clear.output, "\033[2J\033[H");

    const auto status = shell.executeCommand("status");
    EXPECT_TRUE(status.success);
    EXPECT_NE(status.output.find("Shell: Stopped"), std::string::npos);
    EXPECT_NE(status.output.find("Commands registered: 9"), std::string::npos);

    const auto version = shell.executeCommand("version");
    EXPECT_TRUE(version.success);
    EXPECT_NE(version.output.find("ElizaOS C++ Framework Version 1.0.0"), std::string::npos);

    const auto info = shell.executeCommand("info");
    EXPECT_TRUE(info.success);
    EXPECT_NE(info.output.find("Active Commands: 9"), std::string::npos);
    EXPECT_NE(info.output.find("Shell Status: Stopped"), std::string::npos);

    for (const auto& command : {"help arg", "status arg", "version arg", "info arg",
                                "history arg", "clear arg", "exit arg", "quit arg"}) {
        const auto result = shell.executeCommand(command);
        EXPECT_FALSE(result.success) << command;
        EXPECT_EQ(result.exitCode, 2) << command;
    }
}

TEST_F(AgentShellTest, StrictParserSupportsQuotesEscapesAndEmptyArguments) {
    std::vector<std::string> received;
    shell.registerCommand("args", [&received](const std::vector<std::string>& args) {
        received = args;
        return ShellCommandResult(true, "ok");
    });

    const auto result = shell.executeCommand("args \"hello world\" 'two words' escaped\\ value \"\"");
    ASSERT_TRUE(result.success) << result.error;
    EXPECT_EQ(received, (std::vector<std::string>{"hello world", "two words", "escaped value", ""}));
}

TEST_F(AgentShellTest, StrictParserRejectsMalformedAndBoundedInputsWithoutHistory) {
    shell.clearHistory();
    const std::vector<std::string> malformed{
        "echo \"unterminated",
        "echo trailing\\",
        std::string(AgentShell::MaxCommandLength + 1, 'x'),
        "echo " + std::string(AgentShell::MaxTokenLength + 1, 'x')};

    for (const auto& command : malformed) {
        const auto result = shell.executeCommand(command);
        EXPECT_FALSE(result.success);
        EXPECT_EQ(result.exitCode, 2);
    }

    std::string tooMany = "echo";
    for (std::size_t index = 1; index < AgentShell::MaxCommandTokens; ++index) {
        tooMany += " x";
    }
    tooMany += " overflow";
    const auto tooManyResult = shell.executeCommand(tooMany);
    EXPECT_FALSE(tooManyResult.success);
    EXPECT_EQ(tooManyResult.exitCode, 2);
    EXPECT_TRUE(shell.getHistory().empty());
}

TEST_F(AgentShellTest, UnknownCommandsFailAndAreRecorded) {
    shell.clearHistory();
    const auto result = shell.executeCommand("missing arg");
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.exitCode, 1);
    EXPECT_NE(result.error.find("Unknown command: missing"), std::string::npos);
    EXPECT_EQ(shell.getHistory(), (std::vector<std::string>{"missing arg"}));
}

TEST_F(AgentShellTest, RegistrationValidationReplacementAndUnregistrationWork) {
    EXPECT_THROW(shell.registerCommand("", [](const auto&) { return ShellCommandResult{}; }),
                 std::invalid_argument);
    EXPECT_THROW(shell.registerCommand("has space", [](const auto&) { return ShellCommandResult{}; }),
                 std::invalid_argument);
    EXPECT_THROW(shell.registerCommand(std::string(AgentShell::MaxCommandNameLength + 1, 'x'),
                                       [](const auto&) { return ShellCommandResult{}; }),
                 std::invalid_argument);
    EXPECT_THROW(shell.registerCommand("empty", {}), std::invalid_argument);

    shell.registerCommand("replace", [](const auto&) { return ShellCommandResult(true, "first"); });
    EXPECT_EQ(shell.executeCommand("replace").output, "first");
    shell.registerCommand("replace", [](const auto&) { return ShellCommandResult(true, "second"); });
    EXPECT_EQ(shell.executeCommand("replace").output, "second");
    shell.unregisterCommand("replace");
    EXPECT_FALSE(shell.executeCommand("replace").success);
    EXPECT_NO_THROW(shell.unregisterCommand("invalid name"));
}

TEST_F(AgentShellTest, HandlersRunOutsideRegistryLockAndMayReenter) {
    shell.registerCommand("reenter", [this](const std::vector<std::string>&) {
        shell.registerCommand("nested", [](const auto&) { return ShellCommandResult(true, "nested"); });
        shell.unregisterCommand("reenter");
        const auto commands = shell.getAvailableCommands();
        return ShellCommandResult(std::find(commands.begin(), commands.end(), "nested") != commands.end(),
                                  "reentered");
    });

    const auto result = shell.executeCommand("reenter");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(shell.executeCommand("nested").output, "nested");
    EXPECT_FALSE(shell.executeCommand("reenter").success);
}

TEST_F(AgentShellTest, HandlerExceptionsAreIsolated) {
    shell.registerCommand("standard_error", [](const auto&) -> ShellCommandResult {
        throw std::runtime_error("boom");
    });
    shell.registerCommand("unknown_error", [](const auto&) -> ShellCommandResult { throw 7; });

    const auto standard = shell.executeCommand("standard_error");
    EXPECT_FALSE(standard.success);
    EXPECT_EQ(standard.error, "Command error: boom");

    const auto unknown = shell.executeCommand("unknown_error");
    EXPECT_FALSE(unknown.success);
    EXPECT_EQ(unknown.error, "Command error: unknown exception");
    EXPECT_TRUE(shell.executeCommand("help").success);
}

TEST_F(AgentShellTest, HistoryConfigurationSnapshotsAndBoundsAreSafe) {
    shell.clearHistory();
    shell.setHistoryEnabled(true);
    shell.executeCommand("echo first");
    const auto snapshot = shell.getHistory();
    shell.executeCommand("echo second");
    EXPECT_EQ(snapshot, (std::vector<std::string>{"echo first"}));
    ASSERT_EQ(shell.getHistory().size(), 2U);

    shell.clearHistory();
    shell.setHistoryEnabled(false);
    shell.executeCommand("echo disabled");
    EXPECT_TRUE(shell.getHistory().empty());

    shell.setHistoryEnabled(true);
    for (std::size_t index = 0; index < AgentShell::MaxHistoryEntries + 25; ++index) {
        shell.executeCommand("echo " + std::to_string(index));
    }
    const auto bounded = shell.getHistory();
    ASSERT_EQ(bounded.size(), AgentShell::MaxHistoryEntries);
    EXPECT_EQ(bounded.front(), "echo 25");
    EXPECT_EQ(bounded.back(), "echo " + std::to_string(AgentShell::MaxHistoryEntries + 24));
}

TEST_F(AgentShellTest, ConcurrentHistoryConfigAndRegistrySnapshotsRemainValid) {
    shell.clearHistory();
    std::atomic<bool> go{false};
    std::vector<std::thread> workers;
    workers.reserve(8);

    for (std::size_t worker = 0; worker < 4; ++worker) {
        workers.emplace_back([this, &go, worker] {
            while (!go.load(std::memory_order_acquire)) std::this_thread::yield();
            for (std::size_t command = 0; command < 250; ++command) {
                const auto result = shell.executeCommand(
                    "echo worker-" + std::to_string(worker) + '-' + std::to_string(command));
                EXPECT_TRUE(result.success);
            }
        });
    }
    workers.emplace_back([this, &go] {
        while (!go.load(std::memory_order_acquire)) std::this_thread::yield();
        for (std::size_t index = 0; index < 500; ++index) {
            shell.setHistoryEnabled((index % 3U) != 0U);
            shell.setPrompt("prompt-" + std::to_string(index % 20U) + "> ");
        }
    });
    workers.emplace_back([this, &go] {
        while (!go.load(std::memory_order_acquire)) std::this_thread::yield();
        for (std::size_t index = 0; index < 500; ++index) {
            shell.registerCommand("dynamic", [](const auto&) { return ShellCommandResult(true, "ok"); });
            if ((index % 2U) == 0U) shell.unregisterCommand("dynamic");
        }
    });
    for (std::size_t reader = 0; reader < 2; ++reader) {
        workers.emplace_back([this, &go] {
            while (!go.load(std::memory_order_acquire)) std::this_thread::yield();
            for (std::size_t index = 0; index < 500; ++index) {
                const auto history = shell.getHistory();
                EXPECT_LE(history.size(), AgentShell::MaxHistoryEntries);
                const auto commands = shell.getAvailableCommands();
                EXPECT_TRUE(std::is_sorted(commands.begin(), commands.end()));
            }
        });
    }

    go.store(true, std::memory_order_release);
    for (auto& worker : workers) worker.join();
    EXPECT_LE(shell.getHistory().size(), AgentShell::MaxHistoryEntries);
}

TEST_F(AgentShellTest, PromptAndInputHandlerValidationIsStrict) {
    EXPECT_THROW(shell.setPrompt(std::string(AgentShell::MaxPromptLength + 1, '>')),
                 std::invalid_argument);
    EXPECT_THROW(shell.start(std::string(AgentShell::MaxPromptLength + 1, '>')),
                 std::invalid_argument);
    EXPECT_THROW(shell.setInputHandler({}), std::invalid_argument);

    auto input = std::make_shared<BlockingInput>();
    configureBlockingInput(shell, input);
    shell.start();
    ASSERT_TRUE(input->waitUntilEntered());
    EXPECT_THROW(configureBlockingInput(shell, input), std::logic_error);
    EXPECT_THROW(shell.resetInputHandler(), std::logic_error);
    shell.stop();
    EXPECT_NO_THROW(shell.resetInputHandler());
}

TEST_F(AgentShellTest, StopCancelsBlockedInputAndJoinsOwnedThread) {
    auto input = std::make_shared<BlockingInput>();
    configureBlockingInput(shell, input);
    shell.start("blocked> ");
    ASSERT_TRUE(input->waitUntilEntered());
    ASSERT_TRUE(shell.isRunning());

    const auto started = std::chrono::steady_clock::now();
    shell.stop();
    EXPECT_FALSE(shell.isRunning());
    EXPECT_LT(std::chrono::steady_clock::now() - started, 2s);
}

TEST_F(AgentShellTest, InteractiveCallbackSelfStopDefersJoinWithoutDetach) {
    auto input = std::make_shared<BlockingInput>();
    input->commands.push_back("selfstop");
    configureBlockingInput(shell, input);
    shell.registerCommand("selfstop", [this](const auto&) {
        shell.stop();
        return ShellCommandResult(true, "stopped");
    });

    shell.start();
    for (std::size_t attempt = 0; attempt < 200 && shell.isRunning(); ++attempt) {
        std::this_thread::sleep_for(1ms);
    }
    EXPECT_FALSE(shell.isRunning());
    EXPECT_NO_THROW(shell.stop());
    EXPECT_EQ(shell.getHistory(), (std::vector<std::string>{"selfstop"}));
}

TEST_F(AgentShellTest, ConcurrentStartStopRestartIsSerialized) {
    auto input = std::make_shared<BlockingInput>();
    configureBlockingInput(shell, input);

    for (std::size_t cycle = 0; cycle < 100; ++cycle) {
        {
            std::lock_guard<std::mutex> lock(input->mutex);
            input->entered = false;
            input->cancelled = false;
        }
        std::vector<std::thread> starters;
        for (std::size_t index = 0; index < 4; ++index) {
            starters.emplace_back([this, cycle] { shell.start("cycle-" + std::to_string(cycle) + "> "); });
        }
        for (auto& starter : starters) starter.join();
        ASSERT_TRUE(input->waitUntilEntered()) << cycle;

        std::vector<std::thread> stoppers;
        for (std::size_t index = 0; index < 4; ++index) {
            stoppers.emplace_back([this] { shell.stop(); });
        }
        for (auto& stopper : stoppers) stopper.join();
        EXPECT_FALSE(shell.isRunning()) << cycle;
    }
}

TEST(AgentShellLifetime, DestructorCancelsInputAndBlocksUntilExecutionEnds) {
    auto input = std::make_shared<BlockingInput>();
    const auto started = std::chrono::steady_clock::now();
    {
        auto shell = std::make_unique<AgentShell>();
        configureBlockingInput(*shell, input);
        shell->start();
        ASSERT_TRUE(input->waitUntilEntered());
    }
    EXPECT_LT(std::chrono::steady_clock::now() - started, 2s);
    std::lock_guard<std::mutex> lock(input->mutex);
    EXPECT_TRUE(input->cancelled);
}

TEST(AgentShellGlobals, ConvenienceFunctionsExposeGlobalShellAPI) {
    ASSERT_NE(globalShell, nullptr);
    stopInteractiveShell();
    EXPECT_FALSE(isShellRunning());
    EXPECT_TRUE(executeShellCommand("help"));
    EXPECT_TRUE(executeShellCommandWithResult("echo global").success);

    registerShellCommand("global_test", [](const auto&) { return ShellCommandResult(true, "global"); });
    EXPECT_EQ(executeShellCommandWithResult("global_test").output, "global");
    const auto commands = getAvailableShellCommands();
    EXPECT_TRUE(std::is_sorted(commands.begin(), commands.end()));
    EXPECT_NE(std::find(commands.begin(), commands.end(), "global_test"), commands.end());
    globalShell->unregisterCommand("global_test");
}
