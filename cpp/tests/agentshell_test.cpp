// agentshell_test.cpp - E2E tests for elizaos::AgentShell.
#include <gtest/gtest.h>
#include "elizaos/agentshell.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

using namespace elizaos;

class AgentShellTest : public ::testing::Test {
protected:
    AgentShell shell;
};

TEST_F(AgentShellTest, BuiltinCommandsRegistered) {
    auto cmds = shell.getAvailableCommands();
    EXPECT_FALSE(cmds.empty());
}

TEST_F(AgentShellTest, ExecuteHelp) {
    auto r = shell.executeCommand("help");
    EXPECT_TRUE(r.success);
}

TEST_F(AgentShellTest, ExecuteEcho) {
    auto r = shell.executeCommand("echo hello world");
    EXPECT_TRUE(r.success);
    EXPECT_NE(r.output.find("hello"), std::string::npos);
}

TEST_F(AgentShellTest, ExecuteVersion) {
    auto r = shell.executeCommand("version");
    EXPECT_TRUE(r.success);
}

TEST_F(AgentShellTest, ExecuteStatus) {
    auto r = shell.executeCommand("status");
    EXPECT_TRUE(r.success);
}

TEST_F(AgentShellTest, ExecuteUnknownCommandFails) {
    auto r = shell.executeCommand("__no_such_command__");
    EXPECT_FALSE(r.success);
}

TEST_F(AgentShellTest, RegisterCustomCommand) {
    shell.registerCommand("greet", [](const std::vector<std::string>& args) {
        // Custom handlers receive positional arguments without the command name.
        std::string out = "hi";
        if (!args.empty()) out += ", " + args[0];
        return ShellCommandResult{true, out, "", 0};
    });
    auto r = shell.executeCommand("greet Dan");
    EXPECT_TRUE(r.success);
    EXPECT_NE(r.output.find("Dan"), std::string::npos);
}

TEST_F(AgentShellTest, UnregisterCustomCommand) {
    shell.registerCommand("ping", [](const std::vector<std::string>&) {
        return ShellCommandResult{true, "pong", "", 0};
    });
    EXPECT_TRUE(shell.executeCommand("ping").success);
    shell.unregisterCommand("ping");
    EXPECT_FALSE(shell.executeCommand("ping").success);
}

TEST_F(AgentShellTest, HistoryToggles) {
    shell.setHistoryEnabled(true);
    shell.executeCommand("help");
    shell.executeCommand("version");
    EXPECT_GE(shell.getHistory().size(), 2u);

    shell.clearHistory();
    EXPECT_TRUE(shell.getHistory().empty());

    shell.setHistoryEnabled(false);
    shell.executeCommand("status");
    EXPECT_TRUE(shell.getHistory().empty());
}

TEST_F(AgentShellTest, HistorySnapshotRemainsStableAfterFurtherCommands) {
    shell.clearHistory();
    shell.executeCommand("help");
    const auto snapshot = shell.getHistory();
    ASSERT_EQ(snapshot.size(), 1u);

    shell.executeCommand("version");
    EXPECT_EQ(snapshot.size(), 1u);
    EXPECT_EQ(snapshot.front(), "help");
    EXPECT_EQ(shell.getHistory().size(), 2u);
}

TEST_F(AgentShellTest, ConcurrentExecutionProducesCompleteHistorySnapshots) {
    shell.clearHistory();
    constexpr std::size_t kThreads = 8;
    constexpr std::size_t kCommandsPerThread = 40;
    std::vector<std::thread> workers;
    workers.reserve(kThreads);
    for (std::size_t worker = 0; worker < kThreads; ++worker) {
        // MSVC C3493: constexpr locals used in a lambda body must be
        // explicitly captured when no default capture mode is specified.
        workers.emplace_back([this, worker, kCommandsPerThread] {
            for (std::size_t command = 0; command < kCommandsPerThread; ++command) {
                const auto result = shell.executeCommand(
                    "echo worker-" + std::to_string(worker) + "-" + std::to_string(command));
                EXPECT_TRUE(result.success);
            }
        });
    }
    for (auto& worker : workers) worker.join();

    const auto history = shell.getHistory();
    EXPECT_EQ(history.size(), kThreads * kCommandsPerThread);
    const auto commands = shell.getAvailableCommands();
    EXPECT_TRUE(std::is_sorted(commands.begin(), commands.end()));
    EXPECT_NE(std::find(commands.begin(), commands.end(), "echo"), commands.end());
}

TEST_F(AgentShellTest, SetPromptDoesNotThrow) {
    EXPECT_NO_THROW(shell.setPrompt("> "));
}

TEST_F(AgentShellTest, IsRunningInitiallyFalse) {
    EXPECT_FALSE(shell.isRunning());
}

#ifndef HAVE_READLINE
TEST(AgentShellInteractive, RestartAfterNaturalExitJoinsPriorThreadSafely) {
    AgentShell interactiveShell;
    std::ostringstream capturedOutput;
    auto* originalCout = std::cout.rdbuf(capturedOutput.rdbuf());

    std::istringstream firstInput("exit\n");
    auto* originalCin = std::cin.rdbuf(firstInput.rdbuf());
    interactiveShell.start("first> ");
    for (int attempt = 0; attempt < 100 && interactiveShell.isRunning(); ++attempt) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    ASSERT_FALSE(interactiveShell.isRunning());

    std::istringstream secondInput("exit\n");
    std::cin.rdbuf(secondInput.rdbuf());
    interactiveShell.start("second> ");
    for (int attempt = 0; attempt < 100 && interactiveShell.isRunning(); ++attempt) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    interactiveShell.stop();

    std::cout.rdbuf(originalCout);
    std::cin.rdbuf(originalCin);
    const auto history = interactiveShell.getHistory();
    EXPECT_EQ(2u, static_cast<unsigned>(std::count(history.begin(), history.end(), "exit")));
}

TEST(AgentShellInteractive, StartConsumesFiniteInputJoinsCleanlyAndRecordsHistoryOnce) {
    AgentShell interactiveShell;
    interactiveShell.setHistoryEnabled(true);

    std::istringstream scriptedInput("echo interactive lifecycle\nexit\n");
    std::ostringstream capturedOutput;
    auto* originalCin = std::cin.rdbuf(scriptedInput.rdbuf());
    auto* originalCout = std::cout.rdbuf(capturedOutput.rdbuf());

    interactiveShell.start("test> ");
    for (int attempt = 0; attempt < 100 && interactiveShell.isRunning(); ++attempt) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    interactiveShell.stop();

    std::cout.rdbuf(originalCout);
    std::cin.rdbuf(originalCin);

    EXPECT_FALSE(interactiveShell.isRunning());
    const auto history = interactiveShell.getHistory();
    EXPECT_EQ(1u, static_cast<unsigned>(std::count(history.begin(), history.end(), "echo interactive lifecycle")));
    EXPECT_EQ(1u, static_cast<unsigned>(std::count(history.begin(), history.end(), "exit")));
    EXPECT_NE(capturedOutput.str().find("interactive lifecycle"), std::string::npos);
}
#endif

TEST(AgentShellGlobals, GlobalShellExists) {
    ASSERT_NE(globalShell, nullptr);
    EXPECT_TRUE(executeShellCommandWithResult("help").success);
    EXPECT_FALSE(getAvailableShellCommands().empty());
}
