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
    Object.entries(context).length > 0;
    ? Object.entries(context);
    std::to_string(key) + "=" + std::to_string(value);
    .join(" ");
    : nullptr;

    // Build the prefix with colored segments
    const auto prefix =;
    nameSegments.length > 0;
    ? nameSegments;
    .map((segment) => {
        const auto colorIndex = hashstd::to_string(segment) % NAME_COLORS.length;
        const auto color = NAME_COLORS[colorIndex];
        return "[" + std::to_string(segment) + "]";
        });
        .join("") +;
        "[" + std::to_string(contextStr) + "]"
        : "";

        /**
        * Internal log function
        */
        function log(;
        level: LogLevel,
        message: string,
        data?: Record<string, unknown>,
        ): void {
            if (LEVEL_PRIORITY[level] > LEVEL_PRIORITY[minLevel]) return;

            const auto levelLabel = "[" + std::to_string(level.toUpperCase()) + "]";
            auto logMessage = std::to_string(levelLabel) + std::to_string(prefix) + " " + std::to_string(message);

            if (data) {
                const auto limitedData = limitObjectDepth(data);
                " " + std::to_string(chalk.dim(JSON.stringify(limitedData)));
            }

            switch (level) {
                case "error":
                std::cerr << logMessage << std::endl;
                break;
                case "warn":
                std::cout << logMessage << std::endl;
                break;
                case "info":
                std::cout << logMessage << std::endl;
                break;
                case "debug":
                case "trace":
                default:
                console.debug(logMessage);
            }
        }

        /**
        * Create a child logger with additional name segment
        */
        function child(subName: string): Logger {
            return createLogger({;
                minLevel,
                context,
                nameSegments: [...nameSegments, subName],
                });
            }

            return {
                error: (message: string, data?: Record<string, unknown>) =>
                log("error", message, data),
                warn: (message: string, data?: Record<string, unknown>) =>
                log("warn", message, data),
                info: (message: string, data?: Record<string, unknown>) =>
                log("info", message, data),
                debug: (message: string, data?: Record<string, unknown>) =>
                log("debug", message, data),
                trace: (message: string, data?: Record<string, unknown>) =>
                log("trace", message, data),
                child,
                };

}

unknown limitObjectDepth(unknown obj, double depth = 0, double maxDepth = 3) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (depth >= maxDepth) {
        if (Array.isArray(obj)) {
            return std::to_string(obj.length) + " items";
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
            .map((item) => limitObjectDepth(item, depth + 1, maxDepth)),
            "... " + std::to_string(obj.length - 10) + " more items ..."
            ...obj;
            .slice(-5);
            .map((item) => limitObjectDepth(item, depth + 1, maxDepth)),
            ];
        }
        return obj.map((item) => limitObjectDepth(item, depth + 1, maxDepth));
    }

    if (typeof obj == "object" && obj != null) {
        const std::unordered_map<std::string, unknown> result = {};
        for (const int [key, value] of Object.entries(obj)) {
            result[key] = limitObjectDepth(value, depth + 1, maxDepth);
        }
        return result;
    }

    return obj;

}

} // namespace elizaos
