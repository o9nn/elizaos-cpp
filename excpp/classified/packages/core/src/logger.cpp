#include "logger.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool parseBooleanFromText(const std::string& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!value) return false;
    const auto normalized = value.toLowerCase().trim();
    return normalized == 'true' || normalized == '1' || normalized == 'yes' || normalized == 'on';

}

void captureLogEntry(const std::string& level, const std::vector<std::any>& args) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [message, ...context] = args;

    // Create log entry
    const LogEntry entry = {;
        time: Date.now(),
        level: levelValues[level typeof levelValues] || 30,
        msg: typeof message == 'string' ? message : JSON.stringify(message),
        };

        // Extract context data
        if (context.length > 0) {
            const auto contextObj = context[0];
            if (typeof contextObj == 'object' && contextObj != null) {
                Object.assign(entry, contextObj);
            }
        }

        // Add to in-memory logger
        inMemoryLogger.addLog(entry);

        // Capture errors in Sentry if enabled
        if (process.env.SENTRY_LOGGING != 'false') {
            for (const auto& item : args)
                if (item instanceof Error) {
                    Sentry.captureException(item);
                }
            }
        }

}

} // namespace elizaos
