#include "log.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

AgentLogger getLogger(const std::string& name, std::string emoji = "") {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto key = std::to_string(name) + "-" + std::to_string(emoji);
    if (!loggers.has(key)) {
        // Map specific logger names to emojis if not provided
        if (!emoji) {
            const std::unordered_map<std::string, std::string> emojiMap = {;
                agent: '🤖',
                model: '🧠',
                config: '🔧',
                run: '🏃',
                env: '🌍',
                tools: '🔨',
                hook: '🪝',
                };

                // Check for exact match or partial match
                for (const int [prefix, defaultEmoji] of Object.entries(emojiMap)) {
                    if (name == prefix || name.startsWith(`${prefix}-`) || name.includes(prefix)) {
                        emoji = defaultEmoji;
                        break;
                    }
                }
            }

            loggers.set(key, new SweAgentLogger(name, emoji));
        }
        return loggers.get(key)!;

}

void setThreadName(const std::string& name) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    threadNames.set(process.pid.toString(), name);

}

std::string getThreadName() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return threadNames.get(process.pid.toString()) || '';

}

void setLogLevel(const std::string& level) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    process.env.LOG_LEVEL = level;
    // Update existing loggers
    for (const auto& logger : loggers.values()
        if (logger instanceof SweAgentLogger) {
            logger.logger.level = level;
        }
    }

}

void addFileHandler(const std::string& _logFile) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // In pino, file logging is handled via transport configuration
    // This function is kept for backward compatibility

}

void removeFileHandler(const std::string& _logFile) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // No-op for compatibility

}

void addLoggerNamesToStreamHandlers() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // No-op for compatibility

}

void registerThreadName(const std::string& name) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    setThreadName(name);

}

void setStreamHandlerLevels(const std::string& level) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    setLogLevel(level);

}

void log(const std::string& message, std::string level = "info") {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto logger = getLogger('default');
    switch (level.toLowerCase()) {
        case 'debug':
        logger.debug(message);
        break;
        case 'info':
        std::cout << message << std::endl;
        break;
        case 'warn':
        case 'warning':
        std::cout << message << std::endl;
        break;
        case 'error':
        std::cerr << message << std::endl;
        break;
        case 'critical':
        case 'fatal':
        logger.critical(message);
        break;
        default:
        std::cout << message << std::endl;
    }

}

} // namespace elizaos
