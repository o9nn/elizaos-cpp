#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "sentry/instrument.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Local utility function to avoid circular dependency
bool parseBooleanFromText(const std::string& value);

/**
 * Interface representing a log entry.
 */
struct LogEntry {
    std::optional<double> time;
    std::optional<std::variant<std::string, double>> level;
    std::optional<std::string> msg;
    std::optional<std::string> message;
    std::optional<std::string> agentName;
    std::optional<std::string> agentId;
};

// Custom in-memory logger class that maintains recent logs
class InMemoryLogger {
public:
    void addLog(LogEntry entry);
    std::vector<LogEntry> recentLogs();
    void clear();
};

// Create global in-memory logger instance

// Custom levels mapping

// Map adze levels to numeric values for compatibility

// Check if user wants timestamps in logs (default: true)

// Basic adze setup (no configuration needed for this version)

// Helper function to capture logs to in-memory storage
void captureLogEntry(const std::string& level, const std::vector<std::any>& args);

// Create the main logger instance with custom methods
class ElizaLogger {
public:
    ElizaLogger(std::optional<std::any> config);
    void log();
    void progress();
    void success();
    void fatal();
    void error();
    void warn();
    void info();
    void debug();
    void trace();
    void verbose();
    std::vector<std::any> formatArgs(const std::vector<std::any>& args);
    bool isLevelEnabled(const std::string& level);
    ElizaLogger child(const std::any& bindings);
    void clear();
    void level() const;
    void levels() const;
    std::future<void> flush();
    void flushSync();

private:
    std::any context_;
};

// Add symbol for accessing in-memory logger (for compatibility with logging router)

// Create logger factory function

  // Add in-memory logger access

// Create default logger instance

// Add in-memory logger access to default logger

// for backward compatibility


} // namespace elizaos
