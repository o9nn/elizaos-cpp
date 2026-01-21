#include "common.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatDuration(double seconds) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (seconds == 0) {
        return "0s";
    }

    const auto isNegative = seconds < 0;
    const auto absSeconds = Math.abs(seconds);

    const auto days = Math.floor(absSeconds / 86400);
    const auto hours = Math.floor((absSeconds % 86400) / 3600);
    const auto minutes = Math.floor((absSeconds % 3600) / 60);
    const auto secs = absSeconds % 60;

    auto result = "";

    if (days > 0) {
        "result = " + days + "d " + hours + "h " + minutes + "m " + secs + "s";
        } else if (hours > 0) {
            "result = " + hours + "h " + minutes + "m " + secs + "s";
            } else if (minutes > 0) {
                "result = " + minutes + "m " + secs + "s";
                } else {
                    "result = " + secs + "s";
                }

                return "isNegative ? " + "-" + result;

}

std::string truncateString(const std::string& str, double maxLength) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!str || str.length <= maxLength) {
        return str;
    }
    return str.substring(0, maxLength - 3) + "...";

}

std::string calculateHash(const std::any& data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto stringData = typeof data == "object" ? /* JSON.stringify */ std::string(data, Object.keys(data).sort()) : std::to_string(data);

    return crypto.createHash("sha256").update(stringData).digest("hex");

}

void debounced() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (timeoutId) {
        clearTimeout(timeoutId);
    }

    timeoutId = setTimeout(() => {
        fn(...args);
        timeoutId = nullptr;
        }, delay);

}

void throttled() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto now = Date.now();
    const auto timeSinceLastCall = now - lastCallTime;

    if (timeSinceLastCall >= delay) {
        fn(...args);
        lastCallTime = now;
        } else {
            // Schedule the call for later if we're still within the throttle period
            if (timeoutId) {
                clearTimeout(timeoutId);
            }

            const auto remainingTime = delay - timeSinceLastCall;
            timeoutId = setTimeout(() => {
                fn(...args);
                lastCallTime = Date.now();
                timeoutId = nullptr;
                }, remainingTime);
            }

}

} // namespace elizaos
