// agentlogger_test.cpp - E2E tests for elizaos::AgentLogger.
#include <gtest/gtest.h>
#include "elizaos/agentlogger.hpp"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

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
    EXPECT_NO_THROW(logDebug("debug msg", "src"));
    EXPECT_NO_THROW(logTrace("trace msg", "src"));
}

// ============================================================================
// Task 1.4.1: JSON Logging Tests
// ============================================================================

TEST(AgentLoggerJSON, LogEntryToJson) {
    LogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = LogLevel::INFO;
    entry.source = "test_source";
    entry.title = "test_title";
    entry.content = "test message";
    entry.agentId = "agent-123";
    entry.sessionId = "session-456";
    entry.metadata["key1"] = "value1";
    
    auto json = entry.toJson();
    EXPECT_NE(json.find("\"level\":\"info\""), std::string::npos);
    EXPECT_NE(json.find("\"source\":\"test_source\""), std::string::npos);
    EXPECT_NE(json.find("\"content\":\"test message\""), std::string::npos);
    EXPECT_NE(json.find("\"agentId\":\"agent-123\""), std::string::npos);
    EXPECT_NE(json.find("\"metadata\""), std::string::npos);
}

TEST(AgentLoggerJSON, LogEntryEscapesSpecialChars) {
    LogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = LogLevel::INFO;
    entry.content = "message with \"quotes\" and\nnewlines";
    
    auto json = entry.toJson();
    EXPECT_NE(json.find("\\\"quotes\\\""), std::string::npos);
    EXPECT_NE(json.find("\\n"), std::string::npos);
}

TEST(AgentLoggerJSON, JsonLoggingWorks) {
    AgentLogger logger;
    logger.setConsoleEnabled(false);
    logger.setFileEnabled(false);
    
    std::unordered_map<std::string, std::string> meta = {{"req_id", "123"}};
    EXPECT_NO_THROW(logger.logJson("test message", "test_source", LogLevel::INFO, meta));
}

TEST(AgentLoggerJSON, JsonConvenienceFunctions) {
    if (globalLogger) globalLogger->setConsoleEnabled(false);
    std::unordered_map<std::string, std::string> meta = {{"test", "value"}};
    EXPECT_NO_THROW(logJsonInfo("json info", meta));
    EXPECT_NO_THROW(logJsonError("json error", meta));
}

TEST(AgentLoggerJSON, WriteJsonToFile) {
    auto path = tmpFile("json.log");
    std::remove(path.c_str());
    
    AgentLogger logger;
    logger.setConsoleEnabled(false);
    
    LogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = LogLevel::INFO;
    entry.content = "json file test";
    
    logger.writeJsonToFile(entry, path);
    
    auto contents = readFile(path);
    EXPECT_NE(contents.find("\"content\":\"json file test\""), std::string::npos);
    std::remove(path.c_str());
}

// ============================================================================
// Task 1.4.3: Log Rotation Tests
// ============================================================================

TEST(AgentLoggerRotation, RotationConfigurable) {
    AgentLogger logger;
    LogRotationConfig config;
    config.maxFileSize = 1024;
    config.maxFiles = 3;
    EXPECT_NO_THROW(logger.setRotationConfig(config));
}

TEST(AgentLoggerRotation, RotateLogFile) {
    auto path = tmpFile("rotate.log");
    std::remove(path.c_str());
    
    // Create a log file
    std::ofstream f(path);
    f << "test content" << std::endl;
    f.close();
    
    AgentLogger logger;
    EXPECT_NO_THROW(logger.rotateLogFile(path));
    
    // Original file should be gone (renamed)
    // Note: We can't easily verify the rotated filename here
}

TEST(AgentLoggerRotation, RotationRetentionPrunesOlderFiles) {
    namespace fs = std::filesystem;

    const fs::path dir = tmpFile("rotation_dir");
    fs::remove_all(dir);
    fs::create_directories(dir);

    const fs::path active = dir / "retain.log";
    {
        std::ofstream f(active);
        f << "active log" << std::endl;
    }

    const auto baseTime = fs::file_time_type::clock::now() - std::chrono::hours(1);
    const std::vector<fs::path> existing = {
        dir / "retain.log.1",
        dir / "retain.log.2",
        dir / "retain.log.3"
    };

    for (size_t i = 0; i < existing.size(); ++i) {
        std::ofstream f(existing[i]);
        f << "rotated " << i << std::endl;
        f.close();
        fs::last_write_time(existing[i], baseTime + std::chrono::seconds(i));
    }

    AgentLogger logger;
    LogRotationConfig config;
    config.maxFiles = 2;
    config.rotationPattern = ".latest";
    logger.setRotationConfig(config);

    EXPECT_NO_THROW(logger.rotateLogFile(active.string()));
    EXPECT_TRUE(fs::exists(dir / "retain.log.latest"));
    EXPECT_NE(readFile((dir / "retain.log.latest").string()).find("active log"), std::string::npos);
    EXPECT_FALSE(fs::exists(active));

    std::set<std::string> rotatedNames;
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const auto leaf = entry.path().filename().string();
        if (leaf.rfind("retain.log", 0) == 0) {
            rotatedNames.insert(leaf);
        }
    }

    EXPECT_EQ(rotatedNames, (std::set<std::string>{"retain.log.3", "retain.log.latest"}));
    fs::remove_all(dir);
}

// ============================================================================
// Task 1.4.4: Cognitive Introspection Trace Tests
// ============================================================================

TEST(AgentLoggerTrace, StartAndEndTrace) {
    AgentLogger logger;
    logger.setAgentId("test-agent");
    
    auto traceId = logger.startTrace("reasoning");
    EXPECT_FALSE(traceId.empty());
    EXPECT_EQ(traceId.length(), 16);
    
    auto trace = logger.endTrace(traceId);
    EXPECT_EQ(trace.traceId, traceId);
    EXPECT_EQ(trace.operationType, "reasoning");
    EXPECT_GE(trace.durationMs(), 0.0);
}

TEST(AgentLoggerTrace, AddTraceEvents) {
    AgentLogger logger;
    
    auto traceId = logger.startTrace("action");
    
    LogEntry event;
    event.timestamp = std::chrono::system_clock::now();
    event.level = LogLevel::INFO;
    event.content = "trace event";
    
    EXPECT_NO_THROW(logger.addTraceEvent(traceId, event));
    
    auto trace = logger.endTrace(traceId);
    EXPECT_EQ(trace.events.size(), 1);
}

TEST(AgentLoggerTrace, ExportTraces) {
    auto path = tmpFile("traces.json");
    std::remove(path.c_str());
    
    AgentLogger logger;
    auto traceId = logger.startTrace("test");
    logger.endTrace(traceId);
    
    EXPECT_NO_THROW(logger.exportTraces(path));
    
    auto contents = readFile(path);
    EXPECT_NE(contents.find("traceId"), std::string::npos);
    std::remove(path.c_str());
}

TEST(AgentLoggerTrace, CognitiveTraceToJson) {
    CognitiveTrace trace;
    trace.traceId = "test-trace-id";
    trace.startTime = std::chrono::system_clock::now();
    trace.endTime = std::chrono::system_clock::now();
    trace.agentId = "agent-1";
    trace.operationType = "reasoning";
    trace.context["key"] = "value";
    
    auto json = trace.toJson();
    EXPECT_NE(json.find("\"traceId\":\"test-trace-id\""), std::string::npos);
    EXPECT_NE(json.find("\"operationType\":\"reasoning\""), std::string::npos);
    EXPECT_NE(json.find("\"durationMs\""), std::string::npos);
}

// ============================================================================
// Task 1.4.5: Audit Trail Tests
// ============================================================================

TEST(AgentLoggerAudit, LogAuditEntry) {
    AgentLogger logger;
    logger.setConsoleEnabled(false);
    logger.setAgentId("test-agent");
    
    std::unordered_map<std::string, std::string> details = {{"ip", "127.0.0.1"}};
    EXPECT_NO_THROW(logger.logAudit("login", "user@example.com", "success", details));
}

TEST(AgentLoggerAudit, ExportAuditTrail) {
    auto path = tmpFile("audit.json");
    std::remove(path.c_str());
    
    AgentLogger logger;
    logger.setConsoleEnabled(false);
    logger.logAudit("test_action", "test_subject", "success");
    
    EXPECT_NO_THROW(logger.exportAuditTrail(path));
    
    auto contents = readFile(path);
    EXPECT_NE(contents.find("test_action"), std::string::npos);
    std::remove(path.c_str());
}
