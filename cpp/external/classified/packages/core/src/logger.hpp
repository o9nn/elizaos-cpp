#include "sentry/instrument.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Local utility function to avoid circular dependency
bool parseBooleanFromText(string | undefined | null value);

/**
 * Interface representing a log entry.
 */
struct LogEntry {
    std::optional<double> time;
    std::optional<string | number> level;
    std::optional<std::string> msg;
    std::optional<std::string> message;
    std::optional<std::string> agentName;
    std::optional<std::string> agentId;
};

// Custom in-memory logger class that maintains recent logs
class InMemoryLogger {
  private logs: LogEntry[] = [];
  private maxLogs = 1000;

  addLog(entry: LogEntry) {
    // Add timestamp if not present
    if (!entry.time) {
      entry.time = Date.now();
    }

    // Filter out service registration logs unless in debug mode
    const isDebugMode = (process?.env?.LOG_LEVEL || '').toLowerCase() === 'debug';

    if (!isDebugMode && entry.agentName && entry.agentId) {
      const msg = entry.msg || entry.message || '';
      if (
        typeof msg === 'string' &&
        (msg.includes('registered successfully') ||
          msg.includes('Registering') ||
          msg.includes('Success:') ||
          msg.includes('linked to') ||
          msg.includes('Started'))
      ) {
        return; // Skip service registration logs
      }
    }

    this.logs.push(entry);

    // Maintain buffer size
    if (this.logs.length > this.maxLogs) {
      this.logs.shift();
    }
  }

// Create global in-memory logger instance

// Custom levels mapping

// Map adze levels to numeric values for compatibility

// Check if user wants timestamps in logs (default: true)

// Basic adze setup (no configuration needed for this version)

// Helper function to capture logs to in-memory storage
void captureLogEntry(const std::string& level, const std::vector<std::any>& args);

// Create the main logger instance with custom methods
class ElizaLogger {
  private context: any;

  constructor(config?: any) {
    this.context = config?.context || {};

    // Bind all standard methods
    this.fatal = this.fatal.bind(this);
    this.error = this.error.bind(this);
    this.warn = this.warn.bind(this);
    this.info = this.info.bind(this);
    this.debug = this.debug.bind(this);
    this.trace = this.trace.bind(this);
    this.verbose = this.verbose.bind(this);
  }

  // Custom level methods

  // Standard adze methods

  // Helper method to format arguments

    // Handle Error objects

    // Handle object as first argument (context)
      // Combine with message

    // Add context if available

  // Check if level is enabled

  // Pino compatibility methods

  // Add clear method for compatibility

  // Pino compatibility properties

    // Note: In adze, we can't change level dynamically per instance
    // This is here for compatibility only

  // Add pino-compatible flush methods

    // No-op for compatibility

// Add symbol for accessing in-memory logger (for compatibility with logging router)

// Create logger factory function

  // Add in-memory logger access

// Create default logger instance

// Add in-memory logger access to default logger

// for backward compatibility


} // namespace elizaos
