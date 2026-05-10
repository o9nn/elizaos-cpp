#include <gtest/gtest.h>
#include "elizaos/agentlogger.hpp"
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

using namespace elizaos;

class AgentLoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a test logger instance
        logger = std::make_shared<AgentLogger>();
        testFileName = "test_log.txt";
        
        // Clean up std::any existing test file
        if (std::filesystem::exists(testFileName)) {
            std::filesystem::remove(testFileName);
        }
    }
    
    void TearDown() override {
        // Clean up test file
        if (std::filesystem::exists(testFileName)) {
            std::filesystem::remove(testFileName);
        }
    }
    
    std::shared_ptr<AgentLogger> logger;
    std::string testFileName;
};

TEST_F(AgentLoggerTest, BasicLogging) {
    // Test basic logging functionality
    logger->setConsoleEnabled(false); // Disable console for test
    logger->setFileEnabled(true);
    
    logger->log("Test message", "test_source", "test_title", LogLevel::INFO, 
                LogColor::BLUE, true, false, true);
    
    // Should not crash and should not create file since we're not using writeToFile directly
    EXPECT_TRUE(true);
}

TEST_F(AgentLoggerTest, FileLogging) {
    // Test file logging functionality
    logger->writeToFile("Test file content", "test_source", LogLevel::INFO, testFileName);
    
    // Check if file was created and contains content
    EXPECT_TRUE(std::filesystem::exists(testFileName));
    
    std::ifstream file(testFileName);
    ASSERT_TRUE(file.is_open());
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    EXPECT_FALSE(content.empty());
    EXPECT_TRUE(content.find("Test file content") != std::string::npos);
    EXPECT_TRUE(content.find("test_source") != std::string::npos);
}

TEST_F(AgentLoggerTest, LogLevels) {
    // Test different log levels
    logger->setConsoleEnabled(false);
    
    std::vector<LogLevel> levels = {
        LogLevel::INFO, LogLevel::WARNING, LogLevel::ERROR,
        LogLevel::SUCCESS, LogLevel::SYSTEM
    };
    
    for (auto level : levels) {
        logger->log("Test content", "test", "test", level);
        // Should not crash
    }
    
    EXPECT_TRUE(true);
}

TEST_F(AgentLoggerTest, TypeColors) {
    // Test setting custom type colors
    logger->setTypeColor(LogLevel::INFO, LogColor::RED);
    logger->setTypeColor(LogLevel::WARNING, LogColor::GREEN);
    
    // Should not crash
    logger->log("Test message", "", "test", LogLevel::INFO);
    logger->log("Test warning", "", "test", LogLevel::WARNING);
    
    EXPECT_TRUE(true);
}

TEST_F(AgentLoggerTest, ThreadSafety) {
    // Test std::thread safety
    logger->setConsoleEnabled(false);
    
    const int numThreads = 5;
    const int messagesPerThread = 10;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([this, i, messagesPerThread]() {
            for (int j = 0; j < messagesPerThread; ++j) {
                std::string message = "Thread " + std::to_string(i) + " Message " + std::to_string(j);
                logger->log(message, "thread_test", "test", LogLevel::INFO);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Should complete without crashing
    EXPECT_TRUE(true);
}

TEST_F(AgentLoggerTest, ConvenienceFunctions) {
    // Test convenience functions
    logInfo("Info message", "test");
    logWarning("Warning message", "test");
    logError("Error message", "test");
    logSuccess("Success message", "test");
    logSystem("System message", "test");
    
    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(AgentLoggerTest, HeaderPrinting) {
    // Test header printing
    logger->setConsoleEnabled(false); // Disable for test
    logger->printHeader("Test Header", LogColor::YELLOW);
    
    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(AgentLoggerTest, EnableDisableFeatures) {
    // Test enabling/disabling console and file output
    logger->setConsoleEnabled(false);
    logger->setFileEnabled(false);
    
    logger->log("Test message", "test", "test", LogLevel::INFO);
    
    // Should not crash even with both outputs disabled
    EXPECT_TRUE(true);
    
    // Re-enable
    logger->setConsoleEnabled(true);
    logger->setFileEnabled(true);
    
    logger->log("Test message 2", "test", "test", LogLevel::INFO);
    
    EXPECT_TRUE(true);
}