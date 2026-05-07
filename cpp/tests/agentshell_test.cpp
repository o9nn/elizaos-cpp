// agentshell_test.cpp - E2E tests for elizaos::AgentShell.
#include <gtest/gtest.h>
#include "elizaos/agentshell.hpp"

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
        // args[0] == command name, subsequent entries are positional args.
        std::string out = "hi";
        if (args.size() > 1) out += ", " + args[1];
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

TEST_F(AgentShellTest, SetPromptDoesNotThrow) {
    EXPECT_NO_THROW(shell.setPrompt("> "));
}

TEST_F(AgentShellTest, IsRunningInitiallyFalse) {
    EXPECT_FALSE(shell.isRunning());
}

TEST(AgentShellGlobals, GlobalShellExists) {
    ASSERT_NE(globalShell, nullptr);
    EXPECT_TRUE(executeShellCommandWithResult("help").success);
    EXPECT_FALSE(getAvailableShellCommands().empty());
}
