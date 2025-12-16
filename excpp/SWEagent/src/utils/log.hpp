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

/**
 * Logging utilities
 * Converted from sweagent/utils/log.py
 */

;

// Thread name registry
const threadNames = new Map<string, string>();

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
  private emoji: string;
  private name: string;

  constructor(name: string, emoji: string = '') {
    this.emoji = emoji;
    this.name = name;

    // Create pino logger with custom formatting
    const transportOptions =
      process.env.NODE_ENV === 'test'
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

    this.logger = pino({
      name: name,
      level: process.env.LOG_LEVEL || 'debug',
      transport: transportOptions,
      formatters: {
        level: (label: string) => {
          return { level: label.toUpperCase() };
        },
      },
    });
  }

  private formatMessage(message: string): string {
    const prefix = this.emoji ? `${this.emoji} ` : '';
    const threadName = threadNames.get(process.pid.toString()) || '';
    const threadPrefix = threadName ? `[${threadName}] ` : '';
    return `${threadPrefix}(${this.name}): ${prefix}${message}`;
  }

  debug(message: string, ...args: unknown[]): void {
    this.logger.debug(this.formatMessage(message), ...args);
  }

  info(message: string, ...args: unknown[]): void {
    this.logger.info(this.formatMessage(message), ...args);
  }

  warn(message: string, ...args: unknown[]): void {
    this.logger.warn(this.formatMessage(message), ...args);
  }

  error(message: string, ...args: unknown[]): void {
    this.logger.error(this.formatMessage(message), ...args);
  }

  critical(message: string, ...args: unknown[]): void {
    this.logger.fatal(this.formatMessage(message), ...args);
  }

  warning(message: string, ...args: unknown[]): void {
    this.warn(message, ...args);
  }

  exception(message: string, error?: Error, includeStack: boolean = true): void {
    const errorInfo = error
      ? {
          error: error.message,
          ...(includeStack && { stack: error.stack }),
        }
      : {};
    this.logger.error(errorInfo, this.formatMessage(message));
  }
}

// Registry to store logger instances
const loggers = new Map<string, AgentLogger>();

/**
 * Get or create a logger instance
 * @param name Logger name (e.g. 'agent', 'config', 'run')
 * @param emoji Optional emoji prefix for the logger
 * @returns AgentLogger instance
 */
-${emoji}`;
  if (!loggers.has(key)) {
    // Map specific logger names to emojis if not provided
    if (!emoji) {
      const emojiMap: Record<string, string> = {
        agent: '🤖',
        model: '🧠',
        config: '🔧',
        run: '🏃',
        env: '🌍',
        tools: '🔨',
        hook: '🪝',
      };

      // Check for exact match or partial match
      for (const [prefix, defaultEmoji] of Object.entries(emojiMap)) {
        if (name === prefix || name.startsWith(`${prefix}-`) || name.includes(prefix)) {
          emoji = defaultEmoji;
          break;
        }
      }
    }

    loggers.set(key, new SweAgentLogger(name, emoji));
  }
  return loggers.get(key)!;
}

/**
 * Set thread name for current thread/process
 * In Node.js, this associates the process ID with a name
 */


/**
 * Get current thread name
 */


/**
 * Set global log level
 */

  }
}

/**
 * Add a file handler to the logger (no-op for pino, kept for compatibility)
 */


/**
 * Remove file handler (no-op for pino, kept for compatibility)
 */


/**
 * Add logger names to stream handlers (no-op for pino, kept for compatibility)
 */


/**
 * Register thread name (alias for setThreadName)
 */


/**
 * Set stream handler levels (no-op for pino, kept for compatibility)
 */


/**
 * Convenience 
}

} // namespace elizaos
