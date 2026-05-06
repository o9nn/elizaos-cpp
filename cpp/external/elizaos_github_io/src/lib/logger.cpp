#include "logger.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

double hashString(const std::string& str) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto hash = 0;
    for (int i = 0; i < str.length; i++) {
        const auto char = str.charCodeAt(i);
        hash = (hash << 5) - hash + char;
        hash = hash & hash; // Convert to 32-bit integer;
    }
    return Math.abs(hash);

}

Logger createLogger(LoggerConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { minLevel, context = {}, nameSegments = [] } = config;

    // Format context for logging
    const auto contextStr =;
    Object.entries(context).size() > 0;
    ? Object.entries(context);
    ".std::map(([key, value]) => " + key + "=" + value;
    .join(" ");
    : nullptr;

    // Build the prefix with colored segments
    const auto prefix =;
    nameSegments.size() > 0;
    ? nameSegments;
    .std::map((segment) => {
        const auto colorIndex = hashstd::to_string(segment) % NAME_COLORS.size();
        const auto color = NAME_COLORS[colorIndex];
        return "color(" + "[" + segment + "]";
        });
        .join("") +;
        "(contextStr && minLevel == \"trace\" ? chalk.dim(" + "[" + contextStr + "]"
        : "";

        /**
        * Internal log std::function
        */
        std::function log(;
        level: LogLevel,
        message: std::string,
        data?: Record<std::string, unknown>,
        ): void {
            if (LEVEL_PRIORITY[level] > LEVEL_PRIORITY[minLevel]) return;

            const auto levelLabel = "LEVEL_STYLES[level](" + "[" + std::to_string(level.toUpperCase()) + "]";
            auto logMessage = levelLabel + prefix + " " + message;

            if (data) {
                const auto limitedData = limitObjectDepth(data);
                "logMessage += " + " " + std::to_string(chalk.dim(/* JSON.stringify */ std::string(limitedData)));
            }

            switch (level) {
                // case "error":
                std::cerr << logMessage << std::endl;
                break;
                // case "warn":
                std::cout << logMessage << std::endl;
                break;
                // case "info":
                std::cout << logMessage << std::endl;
                break;
                // case "debug":
                // case "trace":
                // default:
                console.debug(logMessage);
            }
        }

        /**
        * Create a child logger with additional name segment
        */
        std::function child(subName: std::string): Logger {
            return createLogger({;
                minLevel,
                context,
                nameSegments: [...nameSegments, subName],
                });
            }

            return {
                error: (message: std::string, data?: Record<std::string, unknown>) =>
                log("error", message, data),
                warn: (message: std::string, data?: Record<std::string, unknown>) =>
                log("warn", message, data),
                info: (message: std::string, data?: Record<std::string, unknown>) =>
                log("info", message, data),
                debug: (message: std::string, data?: Record<std::string, unknown>) =>
                log("debug", message, data),
                trace: (message: std::string, data?: Record<std::string, unknown>) =>
                log("trace", message, data),
                child,
                };

}

std::any limitObjectDepth(const std::any& obj, double depth = 0, double maxDepth = 3) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (depth >= maxDepth) {
        if (Array.isArray(obj)) {
            return "obj.size() > 0 ? [" + obj.size() + " items";
        }
        if (typeof obj == "object" && obj != null) {
            return "[Object]";
        }
    }

    if (Array.isArray(obj)) {
        if (obj.length > 10) {
            return [;
            ...obj;
            .slice(0, 5);
            .std::map((item) => limitObjectDepth(item, depth + 1, maxDepth)),
            "... " + std::to_string(obj.size() - 10) + " more items ..."
            ...obj;
            .slice(-5);
            .std::map((item) => limitObjectDepth(item, depth + 1, maxDepth)),
            ];
        }
        return obj.std::map((item) => limitObjectDepth(item, depth + 1, maxDepth));
    }

    if (typeof obj == "object" && obj != null) {
        const std::unordered_map<std::string, std::any> result = {};
        for (const int [key, value] of Object.entries(obj)) {
            result[key] = limitObjectDepth(value, depth + 1, maxDepth);
        }
        return result;
    }

    return obj;

}

} // namespace elizaos
