// agentlogger_test.cpp
// End-to-end unit tests for elizaos::AgentLogger covering log levels, file
// output, color override, console disable, custom type colors, panel
// formatting, and concurrent logging from multiple threads.

#include "elizaos/agentlogger.hpp"

#include <gtest/gtest.h>
#include <atomic>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>

using namespace elizaos;

class AgentLoggerFixture : public ::testing::Test {
protected:
    void SetUp() override {
        logger_ = std::make_unique<AgentLogger>();
        std::remove("events.log");
    }
    void TearDown() override {
        std::remove("events.log");
    }

    static std::string readEventsLog() {
        std::ifstream f("events.log");
        std::stringstream ss; ss << f.rdbuf();
        return ss.str();
    }

    std::unique_ptr<AgentLogger> logger_;
};

TEST_F(AgentLoggerFixture, ConsoleLogDoesNotThrowAcrossLevels) {
    logger_->setConsoleEnabled(false);  // suppress noise in test runner
    for (LogLevel lvl : {LogLevel::INFO, LogLevel::WARNING, LogLevel::ERROR,
                         LogLevel::SUCCESS, LogLevel::SYSTEM, LogLevel::ACTION,
                         LogLevel::REASONING, LogLevel::PROMPT,
                         LogLevel::EPOCH, LogLevel::SUMMARY}) {
        logger_->log("test message", "agentlogger_test", "title", lvl);
    }
    SUCCEED();  // no crash, no throw
}

TEST_F(AgentLoggerFixture, FileOutputWritesContent) {
    logger_->setConsoleEnabled(false);
    logger_->setFileEnabled(true);
    logger_->writeToFile("hello-from-test", "src", LogLevel::INFO,
                         "events.log");
    auto contents = readEventsLog();
    EXPECT_NE(contents.find("hello-from-test"), std::string::npos);
}

TEST_F(AgentLoggerFixture, FileEnableFlagToggles) {
    logger_->setConsoleEnabled(false);
    logger_->setFileEnabled(true);
    logger_->writeToFile("first-line", "src", LogLevel::INFO, "events.log");
    EXPECT_NE(readEventsLog().find("first-line"), std::string::npos);
    // Toggling the flag should still leave the writer functional and not
    // throw when re-enabled.
    EXPECT_NO_THROW(logger_->setFileEnabled(false));
    EXPECT_NO_THROW(logger_->setFileEnabled(true));
    logger_->writeToFile("second-line", "src", LogLevel::INFO, "events.log");
    EXPECT_NE(readEventsLog().find("second-line"), std::string::npos);
}

TEST_F(AgentLoggerFixture, CustomTypeColorOverride) {
    logger_->setConsoleEnabled(false);
    EXPECT_NO_THROW(logger_->setTypeColor(LogLevel::INFO, LogColor::MAGENTA));
    logger_->log("colored", "", "title", LogLevel::INFO);
    SUCCEED();
}

TEST_F(AgentLoggerFixture, PrintHeaderRendersASCII) {
    logger_->setConsoleEnabled(false);
    EXPECT_NO_THROW(logger_->printHeader("ELIZA", LogColor::CYAN));
}

TEST_F(AgentLoggerFixture, ConvenienceFunctionsAreSafe) {
    EXPECT_NO_THROW({
        logInfo("info-from-convenience", "src");
        logWarning("warn-from-convenience", "src");
        logError("err-from-convenience", "src");
        logSuccess("ok-from-convenience", "src");
        logSystem("sys-from-convenience", "src");
    });
}

TEST_F(AgentLoggerFixture, ConcurrentLoggingIsThreadSafe) {
    logger_->setConsoleEnabled(false);
    logger_->setFileEnabled(true);
    constexpr int kThreads = 8;
    constexpr int kPerThread = 50;
    std::atomic<int> done{0};
    std::vector<std::thread> ts;
    for (int t = 0; t < kThreads; ++t) {
        ts.emplace_back([this, t, &done]() {
            for (int i = 0; i < kPerThread; ++i) {
                logger_->writeToFile("t" + std::to_string(t) + "-" +
                                         std::to_string(i),
                                     "thread", LogLevel::INFO, "events.log");
                done.fetch_add(1);
            }
        });
    }
    for (auto& t : ts) t.join();
    EXPECT_EQ(done.load(), kThreads * kPerThread);
    auto contents = readEventsLog();
    // At least one entry from every thread should be present
    for (int t = 0; t < kThreads; ++t) {
        EXPECT_NE(contents.find("t" + std::to_string(t) + "-0"),
                  std::string::npos);
    }
}

TEST_F(AgentLoggerFixture, GlobalLoggerIsAccessible) {
    ASSERT_NE(globalLogger, nullptr);
    globalLogger->setConsoleEnabled(false);
    EXPECT_NO_THROW(globalLogger->log("global", "src", "title",
                                      LogLevel::INFO));
}
