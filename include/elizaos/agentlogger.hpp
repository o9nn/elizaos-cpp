#pragma once

/**
 * ElizaOS C++ - AgentLogger Module
 *
 * Structured logging with multiple output targets, log levels, and
 * color-coded console panels.
 *
 * Features:
 * - Console logging with ANSI color support
 * - File logging with rotation (size-based, retention, optional gzip)
 * - Structured JSON logging for aggregation systems
 * - Cognitive introspection trace export
 * - Audit trail support
 *
 * NOTE: This is the feature-complete logger. The basic console/file logger
 * that previously lived here is a strict subset of this API, so existing
 * call sites (log/printHeader/writeToFile/setConsoleEnabled/setFileEnabled
 * and the logInfo/logWarning/... convenience functions) remain source
 * compatible.
 */

#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <deque>

namespace elizaos {

// ============================================================================
// LogLevel / LogColor / LogFormat
// ============================================================================

enum class LogLevel {
    UNKNOWN,
    SYSTEM,
    INFO,
    WARNING,
    SUCCESS,
    ERROR,
    START,
    STOP,
    PAUSE,
    EPOCH,
    SUMMARY,
    REASONING,
    ACTION,
    PROMPT,
    DEBUG,
    TRACE,
    AUDIT
};

enum class LogColor {
    WHITE,
    MAGENTA,
    BLUE,
    YELLOW,
    GREEN,
    RED,
    CYAN
};

// Task 1.4.1: Structured logging format
enum class LogFormat {
    TEXT,    // Human-readable text format
    JSON,    // Structured JSON format for aggregation
    COMPACT  // Compact single-line format
};

// ============================================================================
// LogEntry - Structured log entry
// ============================================================================

struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string source;
    std::string title;
    std::string content;
    std::string agentId;
    std::string sessionId;
    std::unordered_map<std::string, std::string> metadata;

    // Convert to JSON string
    std::string toJson() const;

    // Convert to text string
    std::string toText() const;
};

// ============================================================================
// LogRotationConfig - Task 1.4.3: Log rotation configuration
// ============================================================================

struct LogRotationConfig {
    size_t maxFileSize = size_t{10} * 1024 * 1024;  // 10 MB default
    size_t maxFiles = 5;                      // Keep 5 rotated files
    bool compressRotated = false;             // Compress rotated files
    std::string rotationPattern = ".%Y%m%d-%H%M%S";  // Timestamp pattern
};

// ============================================================================
// CognitiveTrace - Task 1.4.4: Cognitive introspection trace
// ============================================================================

struct CognitiveTrace {
    std::string traceId;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::string agentId;
    std::string operationType;  // reasoning, memory, action, etc.
    std::vector<LogEntry> events;
    std::unordered_map<std::string, std::string> context;

    double durationMs() const;
    std::string toJson() const;
};

// ============================================================================
// AgentLogger
// ============================================================================

class AgentLogger {
public:
    static constexpr int SEPARATOR_WIDTH = 80;

    AgentLogger();
    ~AgentLogger();

    // Primary log method (default expand/panel preserved from the original
    // o9nn logger for console-output backward compatibility).
    void log(const std::string& content,
             const std::string& source    = "",
             const std::string& title     = "agentlogger",
             LogLevel           level     = LogLevel::INFO,
             LogColor           color     = LogColor::BLUE,
             bool               expand    = true,
             bool               panel     = true,
             bool               shouldLog = true);

    // Task 1.4.1: Structured logging with metadata
    void logStructured(const LogEntry& entry);
    void logJson(const std::string& content,
                 const std::string& source = "",
                 LogLevel level = LogLevel::INFO,
                 const std::unordered_map<std::string, std::string>& metadata = {});

    void printHeader(const std::string& text = "agentlogger", LogColor color = LogColor::YELLOW);

    // File logging
    void writeToFile(const std::string& content,
                     const std::string& source  = "",
                     LogLevel           level    = LogLevel::INFO,
                     const std::string& filename = "events.log");

    // Task 1.4.1: Write JSON to file
    void writeJsonToFile(const LogEntry& entry, const std::string& filename = "elizaos.json.log");

    // Configuration
    void setTypeColor(LogLevel level, LogColor color);
    void setConsoleEnabled(bool enabled);
    void setFileEnabled(bool enabled);
    void setLogFormat(LogFormat format);
    void setAgentId(const std::string& agentId);
    void setSessionId(const std::string& sessionId);

    // Task 1.4.3: Log rotation
    void setRotationConfig(const LogRotationConfig& config);
    void rotateLogFile(const std::string& filename);

    // Task 1.4.4: Cognitive introspection
    std::string startTrace(const std::string& operationType);
    void addTraceEvent(const std::string& traceId, const LogEntry& event);
    CognitiveTrace endTrace(const std::string& traceId);
    void exportTraces(const std::string& filename);

    // Task 1.4.5: Audit trail
    void logAudit(const std::string& action,
                  const std::string& subject,
                  const std::string& outcome,
                  const std::unordered_map<std::string, std::string>& details = {});
    void exportAuditTrail(const std::string& filename);

    bool isConsoleEnabled() const { return consoleEnabled_; }
    bool isFileEnabled()    const { return fileEnabled_; }
    LogFormat getLogFormat() const { return logFormat_; }

private:
    std::string getColorCode(LogColor color) const;
    LogColor    getDefaultColor(LogLevel level) const;
    std::string levelToString(LogLevel level) const;
    std::string createPanel(const std::string& content,
                            const std::string& title,
                            LogColor color,
                            int width = SEPARATOR_WIDTH) const;
    std::string getTimestamp() const;
    std::string escapeJson(const std::string& str) const;
    void checkAndRotate(const std::string& filename);
    size_t getFileSize(const std::string& filename) const;
    // Prune rotated log artifacts for the given base log so that at most
    // rotationConfig_.maxFiles remain (oldest removed first). No-op when
    // maxFiles == 0 (unlimited retention).
    void enforceRotationRetention(const std::string& baseFilename);

    bool consoleEnabled_ = true;
    bool fileEnabled_    = true;
    LogFormat logFormat_ = LogFormat::TEXT;
    std::string agentId_;
    std::string sessionId_;
    LogRotationConfig rotationConfig_;

    std::unordered_map<LogLevel, LogColor> typeColors_;
    mutable std::mutex logMutex_;

    // Active traces for cognitive introspection
    std::unordered_map<std::string, CognitiveTrace> activeTraces_;
    std::deque<CognitiveTrace> completedTraces_;
    static constexpr size_t MAX_COMPLETED_TRACES = 1000;

    // Audit trail
    std::deque<LogEntry> auditTrail_;
    static constexpr size_t MAX_AUDIT_ENTRIES = 10000;
};

// ============================================================================
// Global logger instance and convenience functions
// ============================================================================

extern std::shared_ptr<AgentLogger> globalLogger;

void logInfo(const std::string& content, const std::string& source = "");
void logWarning(const std::string& content, const std::string& source = "");
void logError(const std::string& content, const std::string& source = "");
void logSuccess(const std::string& content, const std::string& source = "");
void logSystem(const std::string& content, const std::string& source = "");
void logDebug(const std::string& content, const std::string& source = "");
void logTrace(const std::string& content, const std::string& source = "");

// Task 1.4.1: JSON logging convenience functions
void logJsonInfo(const std::string& content,
                 const std::unordered_map<std::string, std::string>& metadata = {});
void logJsonError(const std::string& content,
                  const std::unordered_map<std::string, std::string>& metadata = {});

} // namespace elizaos
