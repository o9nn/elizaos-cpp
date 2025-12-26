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
        // Setup test environment
    }

    void TearDown() override {
        // Cleanup
    }
};

// ============================================================================
// ShellConfig Tests
// ============================================================================

TEST_F(AgentShellTest, DefaultShellConfigValues) {
    ShellConfig config;

    EXPECT_TRUE(config.enableColors);
    EXPECT_TRUE(config.enableHistory);
    EXPECT_TRUE(config.enableAutoComplete);
    EXPECT_EQ(config.prompt, "elizaos> ");
    EXPECT_EQ(config.historyFile, ".elizaos_history");
    EXPECT_EQ(config.maxHistorySize, 1000);
}

TEST_F(AgentShellTest, CustomShellConfigValues) {
    ShellConfig config;
    config.enableColors = false;
    config.enableHistory = false;
    config.prompt = "agent$ ";
    config.maxHistorySize = 500;

    EXPECT_FALSE(config.enableColors);
    EXPECT_FALSE(config.enableHistory);
    EXPECT_EQ(config.prompt, "agent$ ");
    EXPECT_EQ(config.maxHistorySize, 500);
}

// ============================================================================
// ShellCommand Tests
// ============================================================================

TEST_F(AgentShellTest, ShellCommandCreation) {
    ShellCommand cmd;
    cmd.name = "help";
    cmd.description = "Display help information";
    cmd.usage = "help [command]";
    cmd.aliases = {"h", "?"};

    EXPECT_EQ(cmd.name, "help");
    EXPECT_EQ(cmd.description, "Display help information");
    EXPECT_EQ(cmd.usage, "help [command]");
    EXPECT_EQ(cmd.aliases.size(), 2);
    EXPECT_EQ(cmd.aliases[0], "h");
    EXPECT_EQ(cmd.aliases[1], "?");
}

TEST_F(AgentShellTest, ShellCommandWithHandler) {
    bool handlerCalled = false;

    ShellCommand cmd;
    cmd.name = "test";
    cmd.handler = [&handlerCalled](const std::vector<std::string>&) -> std::string {
        handlerCalled = true;
        return "Test executed";
    };

    EXPECT_FALSE(handlerCalled);
    if (cmd.handler) {
        std::vector<std::string> args;
        std::string result = cmd.handler(args);
        EXPECT_TRUE(handlerCalled);
        EXPECT_EQ(result, "Test executed");
    }
}

// ============================================================================
// CommandResult Tests
// ============================================================================

TEST_F(AgentShellTest, CommandResultSuccess) {
    CommandResult result;
    result.success = true;
    result.output = "Command executed successfully";
    result.exitCode = 0;

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Command executed successfully");
    EXPECT_EQ(result.exitCode, 0);
}

TEST_F(AgentShellTest, CommandResultFailure) {
    CommandResult result;
    result.success = false;
    result.output = "Error: Command not found";
    result.error = "Unknown command 'xyz'";
    result.exitCode = 1;

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.exitCode, 1);
    EXPECT_FALSE(result.error.empty());
}

// ============================================================================
// ShellHistory Tests
// ============================================================================

TEST_F(AgentShellTest, ShellHistoryAddAndRetrieve) {
    ShellHistory history;

    history.add("command1");
    history.add("command2");
    history.add("command3");

    EXPECT_EQ(history.size(), 3);
    EXPECT_EQ(history.get(0), "command1");
    EXPECT_EQ(history.get(1), "command2");
    EXPECT_EQ(history.get(2), "command3");
}

TEST_F(AgentShellTest, ShellHistoryMaxSize) {
    ShellHistory history(3);

    history.add("cmd1");
    history.add("cmd2");
    history.add("cmd3");
    history.add("cmd4");

    EXPECT_EQ(history.size(), 3);
    // Oldest command should be removed
    EXPECT_EQ(history.get(0), "cmd2");
    EXPECT_EQ(history.get(2), "cmd4");
}

TEST_F(AgentShellTest, ShellHistoryClear) {
    ShellHistory history;

    history.add("command1");
    history.add("command2");
    EXPECT_EQ(history.size(), 2);

    history.clear();
    EXPECT_EQ(history.size(), 0);
}

TEST_F(AgentShellTest, ShellHistorySearch) {
    ShellHistory history;

    history.add("ls -la");
    history.add("cd /home");
    history.add("ls /tmp");
    history.add("cat file.txt");

    std::vector<std::string> matches = history.search("ls");
    EXPECT_EQ(matches.size(), 2);
}

// ============================================================================
// Shell Input Parsing Tests
// ============================================================================

TEST_F(AgentShellTest, ParseSimpleCommand) {
    std::string input = "help";
    std::vector<std::string> tokens = ShellParser::tokenize(input);

    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0], "help");
}

TEST_F(AgentShellTest, ParseCommandWithArguments) {
    std::string input = "echo hello world";
    std::vector<std::string> tokens = ShellParser::tokenize(input);

    EXPECT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0], "echo");
    EXPECT_EQ(tokens[1], "hello");
    EXPECT_EQ(tokens[2], "world");
}

TEST_F(AgentShellTest, ParseCommandWithQuotedString) {
    std::string input = "echo \"hello world\"";
    std::vector<std::string> tokens = ShellParser::tokenize(input);

    EXPECT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0], "echo");
    EXPECT_EQ(tokens[1], "hello world");
}

TEST_F(AgentShellTest, ParseEmptyInput) {
    std::string input = "";
    std::vector<std::string> tokens = ShellParser::tokenize(input);

    EXPECT_EQ(tokens.size(), 0);
}

TEST_F(AgentShellTest, ParseWhitespaceOnlyInput) {
    std::string input = "   \t   ";
    std::vector<std::string> tokens = ShellParser::tokenize(input);

    EXPECT_EQ(tokens.size(), 0);
}

// ============================================================================
// Shell Color Output Tests
// ============================================================================

TEST_F(AgentShellTest, ColorCodeGeneration) {
    EXPECT_EQ(ShellColors::red("error"), "\033[31merror\033[0m");
    EXPECT_EQ(ShellColors::green("success"), "\033[32msuccess\033[0m");
    EXPECT_EQ(ShellColors::yellow("warning"), "\033[33mwarning\033[0m");
    EXPECT_EQ(ShellColors::blue("info"), "\033[34minfo\033[0m");
    EXPECT_EQ(ShellColors::bold("important"), "\033[1mimportant\033[0m");
}

TEST_F(AgentShellTest, ColorCodeStripping) {
    std::string colored = "\033[31merror\033[0m";
    std::string stripped = ShellColors::strip(colored);

    EXPECT_EQ(stripped, "error");
}
