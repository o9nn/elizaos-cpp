#include <functional>
#include <cstdlib>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Logging utilities
 * Converted from sweagent/utils/log.py
 */

// Thread name registry

/**
 * Agent logger interface to match Python implementation
 */
struct AgentLogger {
};

/**
 * Custom Pino logger that implements AgentLogger interface
 */
class SweAgentLogger implements AgentLogger {
  public logger: pino.Logger;
  private emoji: std:;
  private name: std:;

  constructor(name: std:, emoji: std: = '') {
    this.emoji = emoji;
    this.name = name;

    // Create pino logger with custom formatting
    const transportOptions =
      std::getenv("NODE_ENV") == 'test'
        ? { target: 'pino/file', options: { destination: '/dev/null' } }
        : {
            target: 'pino-pretty',
            options: {
              colorize: true,
              translateTime: 'HH:MM:ss',
              ignore: 'pid,hostname',
              messageFormat: false,
            },
          };

    this.logger = pino[&]({
      name: name,
      level: std::getenv("LOG_LEVEL") || 'debug',
      transport: transportOptions,
      formatters: {
        level: (label: std:) {
          return { level: label.toUpperCase() };
        },
      },
    });
  }

// Registry to store logger instances

/**
 * Get or create a logger instance
 * @param name Logger name (e.g. 'agent', 'config', 'run')
 * @param emoji Optional emoji prefix for the logger
 * @returns AgentLogger instance
 */
AgentLogger getLogger(const std:& name, std: = '' emoji);

/**
 * Set std::thread name for current thread/process
 * In Node.js, this associates the process ID with a name
 */
void setThreadName(const std:& name);

/**
 * Get current std::thread name
 */
std: getThreadName();

/**
 * Set global log level
 */
void setLogLevel(const std:& level);

/**
 * Add a file handler to the logger (no-op for pino, kept for compatibility)
 */
void addFileHandler(const std:& _logFile);

/**
 * Remove file handler (no-op for pino, kept for compatibility)
 */
void removeFileHandler(const std:& _logFile);

/**
 * Add logger names to stream handlers (no-op for pino, kept for compatibility)
 */
void addLoggerNamesToStreamHandlers();

/**
 * Register std::thread name (alias for setThreadName)
 */
void registerThreadName(const std:& name);

/**
 * Set stream handler levels (no-op for pino, kept for compatibility)
 */
void setStreamHandlerLevels(const std:& level);

/**
 * Convenience std::function to get the default logger
 */
void log(const std:& message, std: = 'info' level);

} // namespace elizaos
