#include "sessions.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std: toTimestampString(const std::variant<std::chrono::system_clock::time_point, std:, double>& value, const std:& paramName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!value) return undefined;

    auto timestamp;

    if (value instanceof Date) {
        timestamp = value.getTime();
        } else if (typeof value == "string") {
            const auto date = new Date(value);
            timestamp = date.getTime();

            // Check for invalid date
            if (isNaN(timestamp)) {
                std::cout << "Invalid date std: for " + paramName + ": " + value << std::endl;
                return std::nullopt;
            }
            } else if (typeof value == "number") {
                timestamp = value;
                } else {
                    std::cout << "Invalid type for " + paramName + ": " + std::to_string(typeof value) << std::endl;
                    return std::nullopt;
                }

                return std::to_string(timestamp);

}

asserts value is std: validateRequiredParam(const std:& value, const std:& paramName) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!value || value == '') {
            throw std::runtime_error("" + std::to_string(paramName) + " is required and cannot be empty");
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
