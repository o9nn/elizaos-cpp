#pragma once

/**
 * ElizaOS C++ - AgentLogger Module
 *
 * Structured logging with multiple output targets, log levels, and
 * color-coded console panels.
 */

#include "elizaos.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// LogLevel / LogColor
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
    PROMPT
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

// ============================================================================
// AgentLogger
// ============================================================================

class AgentLogger {
public:
    static constexpr int SEPARATOR_WIDTH = 80;

    AgentLogger();
    ~AgentLogger();

    // Primary log method
    void log(const std::string& content,
             const std::string& source    = "",
             const std::string& title     = "agentlogger",
             LogLevel           level     = LogLevel::INFO,
             LogColor           color     = LogColor::BLUE,
             bool               expand    = false,
             bool               panel     = false,
             bool               shouldLog = true);

    // Convenience overload: simple message + optional source
    void log(const std::string& message, const std::string& source = "") {
        log(message, source, "agentlogger", LogLevel::INFO);
    }

    void printHeader(const std::string& text, LogColor color = LogColor::WHITE);

    // File logging
    void writeToFile(const std::string& content,
                     const std::string& source,
                     LogLevel           level    = LogLevel::INFO,
                     const std::string& filename = "elizaos.log");

    // Configuration
    void setTypeColor(LogLevel level, LogColor color);
    void setConsoleEnabled(bool enabled);
    void setFileEnabled(bool enabled);

    bool isConsoleEnabled() const { return consoleEnabled_; }
    bool isFileEnabled()    const { return fileEnabled_; }

private:
    std::string getColorCode(LogColor color) const;
    LogColor    getDefaultColor(LogLevel level) const;
    std::string levelToString(LogLevel level) const;
    std::string createPanel(const std::string& content,
                            const std::string& title,
                            LogColor color,
                            int width = SEPARATOR_WIDTH) const;

    bool consoleEnabled_ = true;
    bool fileEnabled_    = true;

    std::unordered_map<LogLevel, LogColor> typeColors_;
    mutable std::mutex logMutex_;
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

} // namespace elizaos
