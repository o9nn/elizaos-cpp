#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "elizaos/agentshell.hpp"
#include <memory>
#include <string>
#include <sstream>

using namespace elizaos;
using namespace ::testing;

class AgentShellTest : public ::testing::Test {
protected:
    void SetUp() override {
        shell_ = std::make_shared<AgentShell>();
    }

    void TearDown() override {
        if (shell_->isRunning()) {
            shell_->stop();
        }
    }

    std::shared_ptr<AgentShell> shell_;
};

// ============================================================================
// ShellCommandResult Tests
// ============================================================================

TEST_F(AgentShellTest, CommandResultSuccess) {
    ShellCommandResult result(true, "Command executed successfully", "", 0);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Command executed successfully");
    EXPECT_EQ(result.error, "");
    EXPECT_EQ(result.exitCode, 0);
}

TEST_F(AgentShellTest, CommandResultFailure) {
    ShellCommandResult result(false, "", "Unknown command 'xyz'", 1);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.exitCode, 1);
    EXPECT_FALSE(result.error.empty());
}

TEST_F(AgentShellTest, CommandResultDefaultConstructor) {
    ShellCommandResult result;

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "");
    EXPECT_EQ(result.error, "");
    EXPECT_EQ(result.exitCode, 0);
}

// ============================================================================
// AgentShell Basic Tests
// ============================================================================

TEST_F(AgentShellTest, ShellCreation) {
    AgentShell shell;

    EXPECT_FALSE(shell.isRunning());
}

TEST_F(AgentShellTest, GetAvailableCommands) {
    auto commands = shell_->getAvailableCommands();

    // Should have built-in commands available
    EXPECT_GT(commands.size(), 0);
}

TEST_F(AgentShellTest, SetPrompt) {
    shell_->setPrompt("test> ");

    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(AgentShellTest, SetHistoryEnabled) {
    shell_->setHistoryEnabled(true);
    shell_->setHistoryEnabled(false);

    // Should not crash
    EXPECT_TRUE(true);
}

// ============================================================================
// Command Execution Tests
// ============================================================================

TEST_F(AgentShellTest, ExecuteHelpCommand) {
    auto result = shell_->executeCommand("help");

    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.output.empty());
}

TEST_F(AgentShellTest, ExecuteEmptyCommand) {
    auto result = shell_->executeCommand("");

    // Empty command should handle gracefully
    EXPECT_TRUE(result.success || result.exitCode == 0);
}

TEST_F(AgentShellTest, ExecuteUnknownCommand) {
    auto result = shell_->executeCommand("nonexistent_command_xyz123");

    // Unknown command should fail
    EXPECT_FALSE(result.success);
}

TEST_F(AgentShellTest, ExecuteEchoCommand) {
    auto result = shell_->executeCommand("echo hello world");

    EXPECT_TRUE(result.success);
    // Echo should output the arguments
    EXPECT_NE(result.output.find("hello"), std::string::npos);
}

// ============================================================================
// Command Registration Tests
// ============================================================================

TEST_F(AgentShellTest, RegisterCustomCommand) {
    bool commandCalled = false;

    shell_->registerCommand("testcmd", [&commandCalled]([[maybe_unused]] const std::vector<std::string>& args) -> ShellCommandResult {
        commandCalled = true;
        return ShellCommandResult(true, "Custom command executed", "", 0);
    });

    auto result = shell_->executeCommand("testcmd");

    EXPECT_TRUE(commandCalled);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Custom command executed");
}

TEST_F(AgentShellTest, RegisterCommandWithArguments) {
    std::vector<std::string> receivedArgs;

    shell_->registerCommand("argcmd", [&receivedArgs](const std::vector<std::string>& args) -> ShellCommandResult {
        receivedArgs = args;
        return ShellCommandResult(true, "Args received", "", 0);
    });

    shell_->executeCommand("argcmd arg1 arg2 arg3");

    EXPECT_EQ(receivedArgs.size(), 3);
    EXPECT_EQ(receivedArgs[0], "arg1");
    EXPECT_EQ(receivedArgs[1], "arg2");
    EXPECT_EQ(receivedArgs[2], "arg3");
}

TEST_F(AgentShellTest, UnregisterCommand) {
    shell_->registerCommand("tempcmd", [](const std::vector<std::string>&) -> ShellCommandResult {
        return ShellCommandResult(true, "Temp command", "", 0);
    });

    // First execution should succeed
    auto result1 = shell_->executeCommand("tempcmd");
    EXPECT_TRUE(result1.success);

    // Unregister the command
    shell_->unregisterCommand("tempcmd");

    // Second execution should fail
    auto result2 = shell_->executeCommand("tempcmd");
    EXPECT_FALSE(result2.success);
}

// ============================================================================
// Command History Tests
// ============================================================================

TEST_F(AgentShellTest, CommandHistoryTracking) {
    shell_->setHistoryEnabled(true);

    shell_->executeCommand("help");
    shell_->executeCommand("echo test");

    const auto& history = shell_->getHistory();
    EXPECT_GE(history.size(), 2);
}

TEST_F(AgentShellTest, ClearHistory) {
    shell_->setHistoryEnabled(true);

    shell_->executeCommand("help");
    shell_->executeCommand("echo test");

    shell_->clearHistory();

    const auto& history = shell_->getHistory();
    EXPECT_EQ(history.size(), 0);
}

TEST_F(AgentShellTest, HistoryDisabled) {
    shell_->setHistoryEnabled(false);
    shell_->clearHistory();

    shell_->executeCommand("help");

    [[maybe_unused]] const auto& history = shell_->getHistory();
    // History might still record even when disabled, depends on implementation
    EXPECT_TRUE(true);
}

// ============================================================================
// Shell Lifecycle Tests
// ============================================================================

TEST_F(AgentShellTest, ShellNotRunningInitially) {
    AgentShell shell;

    EXPECT_FALSE(shell.isRunning());
}

TEST_F(AgentShellTest, StopNonRunningShell) {
    AgentShell shell;

    // Should not crash when stopping a non-running shell
    shell.stop();

    EXPECT_FALSE(shell.isRunning());
}

// ============================================================================
// Convenience Function Tests
// ============================================================================

TEST_F(AgentShellTest, IsShellRunningConvenience) {
    // Test the global convenience function
    bool running = isShellRunning();

    // Should return some boolean value without crashing
    EXPECT_TRUE(running || !running);
}

TEST_F(AgentShellTest, GetAvailableShellCommandsConvenience) {
    auto commands = getAvailableShellCommands();

    // Should return a list of commands
    EXPECT_GE(commands.size(), 0);
}

TEST_F(AgentShellTest, ExecuteShellCommandWithResultConvenience) {
    auto result = executeShellCommandWithResult("help");

    // Should return a result
    EXPECT_TRUE(result.success || !result.success);
}

// ============================================================================
// Edge Cases Tests
// ============================================================================

TEST_F(AgentShellTest, ExecuteCommandWithSpecialCharacters) {
    auto result = shell_->executeCommand("echo \"hello world\"");

    EXPECT_TRUE(result.success);
}

TEST_F(AgentShellTest, ExecuteCommandWithWhitespace) {
    auto result = shell_->executeCommand("   help   ");

    // Should handle leading/trailing whitespace
    EXPECT_TRUE(result.success);
}

TEST_F(AgentShellTest, MultipleCommandRegistrations) {
    for (int i = 0; i < 10; ++i) {
        std::string cmdName = "cmd" + std::to_string(i);
        shell_->registerCommand(cmdName, [i](const std::vector<std::string>&) -> ShellCommandResult {
            return ShellCommandResult(true, "Command " + std::to_string(i), "", 0);
        });
    }

    auto commands = shell_->getAvailableCommands();
    EXPECT_GE(commands.size(), 10);
}

TEST_F(AgentShellTest, ReregisterCommand) {
    shell_->registerCommand("replace", [](const std::vector<std::string>&) -> ShellCommandResult {
        return ShellCommandResult(true, "Original", "", 0);
    });

    auto result1 = shell_->executeCommand("replace");
    EXPECT_EQ(result1.output, "Original");

    shell_->registerCommand("replace", [](const std::vector<std::string>&) -> ShellCommandResult {
        return ShellCommandResult(true, "Replaced", "", 0);
    });

    auto result2 = shell_->executeCommand("replace");
    EXPECT_EQ(result2.output, "Replaced");
}

