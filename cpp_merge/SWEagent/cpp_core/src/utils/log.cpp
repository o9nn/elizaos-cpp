#include "log.hpp"
#include <cstdlib>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

AgentLogger getLogger(const std:& name, std: emoji = "") {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto key = name + "-" + emoji;
    if (!loggers.has(key)) {
        // Map specific logger names to emojis if not provided
        if (!emoji) {
            const std::unordered_map<std:, std:> emojiMap = {;
                agent: "🤖",
                model: "🧠",
                config: "🔧",
                run: "🏃",
                env: "🌍",
                tools: "🔨",
                hook: "🪝",
                };

                // Check for exact match or partial match
                for (const int [prefix, defaultEmoji] of Object.entries(emojiMap)) {
                    if (name == prefix || name.substr(0, "" + std::to_string(prefix) + "-") || name.count(prefix) > 0) {
                        emoji = defaultEmoji;
                        break;
                    }
                }
            }

            loggers.std::set(key, new SweAgentLogger(name, emoji));
        }
        return loggers.get(key)!;

}

void setThreadName(const std:& name) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    threadNames.std::set(process.std::to_string(pid), name);

}

std: getThreadName() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return threadNames.get(process.std::to_string(pid)) || "";

}

void setLogLevel(const std:& level) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::getenv("LOG_LEVEL") = level;
    // Update existing loggers
    for (const auto& logger : loggers.values()
        if (logger instanceof SweAgentLogger) {
            logger.logger.level = level;
        }
    }

}

void addFileHandler(const std:& _logFile) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // In pino, file logging is handled via transport configuration
    // This std::function is kept for backward compatibility

}

void removeFileHandler(const std:& _logFile) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // No-op for compatibility

}

void addLoggerNamesToStreamHandlers() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // No-op for compatibility

}

void registerThreadName(const std:& name) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    setThreadName(name);

}

void setStreamHandlerLevels(const std:& level) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    setLogLevel(level);

}

void log(const std:& message, std: level = "info") {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto logger = getLogger("default");
    switch (level.toLowerCase()) {
        // case "debug":
        logger.debug(message);
        break;
        // case "info":
        std::cout << message << std::endl;
        break;
        // case "warn":
        // case "warning":
        std::cout << message << std::endl;
        break;
        // case "error":
        std::cerr << message << std::endl;
        break;
        // case "critical":
        // case "fatal":
        logger.critical(message);
        break;
        // default:
        std::cout << message << std::endl;
    }

}

} // namespace elizaos
