// agentlogger_test.cpp - E2E tests for elizaos::AgentLogger.
#include <gtest/gtest.h>
#include "elizaos/agentlogger.hpp"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

using namespace elizaos;

namespace {
std::string tmpFile(const std::string& suffix) {
    static int counter = 0;
    return "/tmp/agentlogger_test_" + std::to_string(++counter) + "_" + suffix;
}

std::string readFile(const std::string& path) {
    std::ifstream f(path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}
}

class AgentLoggerTest : public ::testing::Test {
protected:
    AgentLogger logger;

    void SetUp() override {
        logger.setConsoleEnabled(false);
        logger.setFileEnabled(true);
    }
};

TEST_F(AgentLoggerTest, BasicLogDoesNotThrow) {
    EXPECT_NO_THROW(logger.log("hello"));
    EXPECT_NO_THROW(logger.log("with source", "TestModule"));
    EXPECT_NO_THROW(logger.log("with title", "src", "title"));
}

TEST_F(AgentLoggerTest, AllLogLevelsAccepted) {
    for (auto lv : {LogLevel::INFO, LogLevel::WARNING, LogLevel::ERROR,
                    LogLevel::SUCCESS, LogLevel::SYSTEM, LogLevel::REASONING,
                    LogLevel::ACTION, LogLevel::PROMPT, LogLevel::EPOCH,
                    LogLevel::SUMMARY, LogLevel::START, LogLevel::STOP,
                    LogLevel::PAUSE, LogLevel::UNKNOWN}) {
        EXPECT_NO_THROW(logger.log("level test", "src", "title", lv));
    }
}

TEST_F(AgentLoggerTest, AllColorsAccepted) {
    for (auto c : {LogColor::WHITE, LogColor::MAGENTA, LogColor::BLUE,
                   LogColor::YELLOW, LogColor::GREEN, LogColor::RED,
                   LogColor::CYAN}) {
        EXPECT_NO_THROW(logger.log("color test", "src", "title",
                                   LogLevel::INFO, c));
    }
}

TEST_F(AgentLoggerTest, WriteToFileCreatesFile) {
    auto path = tmpFile("write.log");
    std::remove(path.c_str());
    logger.writeToFile("file payload", "src", LogLevel::INFO, path);
    auto contents = readFile(path);
    EXPECT_NE(contents.find("file payload"), std::string::npos);
    std::remove(path.c_str());
}

TEST_F(AgentLoggerTest, WriteToFileAppendsMultipleLines) {
    auto path = tmpFile("append.log");
    std::remove(path.c_str());
    logger.writeToFile("first", "src", LogLevel::INFO, path);
    logger.writeToFile("second", "src", LogLevel::INFO, path);
    auto contents = readFile(path);
    EXPECT_NE(contents.find("first"), std::string::npos);
    EXPECT_NE(contents.find("second"), std::string::npos);
    std::remove(path.c_str());
}

TEST_F(AgentLoggerTest, SetTypeColorOverrides) {
    EXPECT_NO_THROW(logger.setTypeColor(LogLevel::INFO, LogColor::RED));
    EXPECT_NO_THROW(logger.log("with overridden color"));
}

TEST_F(AgentLoggerTest, ConsoleEnabledToggle) {
    EXPECT_NO_THROW(logger.setConsoleEnabled(true));
    EXPECT_NO_THROW(logger.setConsoleEnabled(false));
    EXPECT_NO_THROW(logger.log("toggle"));
}

TEST_F(AgentLoggerTest, PrintHeaderDoesNotThrow) {
    EXPECT_NO_THROW(logger.printHeader("Hello"));
    EXPECT_NO_THROW(logger.printHeader("Banner", LogColor::CYAN));
}

TEST(AgentLoggerGlobals, GlobalLoggerExists) {
    ASSERT_NE(globalLogger, nullptr);
    EXPECT_NO_THROW(globalLogger->setConsoleEnabled(false));
    EXPECT_NO_THROW(globalLogger->log("via global"));
}

TEST(AgentLoggerGlobals, ConvenienceFunctions) {
    if (globalLogger) globalLogger->setConsoleEnabled(false);
    EXPECT_NO_THROW(logInfo("info msg", "src"));
    EXPECT_NO_THROW(logWarning("warn msg", "src"));
    EXPECT_NO_THROW(logError("err msg", "src"));
    EXPECT_NO_THROW(logSuccess("ok msg", "src"));
    EXPECT_NO_THROW(logSystem("sys msg", "src"));
}
