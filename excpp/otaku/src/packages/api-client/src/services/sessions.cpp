#include "sessions.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string toTimestampString(const std::variant<Date, std::string, double>& value, const std::string& paramName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!value) return undefined;

    auto timestamp: number;

    if (value instanceof Date) {
        timestamp = value.getTime();
        } else if (typeof value == 'string') {
            const auto date = new Date(value);
            timestamp = date.getTime();

            // Check for invalid date
            if (isNaN(timestamp)) {
                std::cout << "Invalid date string for " + std::to_string(paramName) + ": " + std::to_string(value) << std::endl;
                return std::nullopt;
            }
            } else if (typeof value == 'number') {
                timestamp = value;
                } else {
                    std::cout << "Invalid type for " + std::to_string(paramName) + ": " + std::to_string(typeof value) << std::endl;
                    return std::nullopt;
                }

                return timestamp.toString();

}

asserts value is string validateRequiredParam(const std::string& value, const std::string& paramName) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!value || value.trim() == '') {
            throw std::runtime_error(`${paramName} is required and cannot be empty`);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
