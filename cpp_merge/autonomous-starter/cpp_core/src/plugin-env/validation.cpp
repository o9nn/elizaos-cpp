#include "validation.hpp"
#include <string>
#include <future>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<ValidationResult> validateEnvVar(const std::string& varName, const std::string& value, const std::string& type, std::optional<std::string> validationMethod) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        std::cout << "Validating environment variable " + varName + " of type " + type << std::endl;

        if (!value || value == "") {
            return { isValid: false, error: "Environment variable value is empty" }
        }

        // Determine validation strategy
        const auto [category, method] = (validationMethod || type).split(":");

        if (validationStrategies[category as keyof typeof validationStrategies]) {
            const auto categoryStrategies =;
            validationStrategies[category typeof validationStrategies];

            if (
            method &&;
            categoryStrategies[method typeof categoryStrategies];
            ) {
                const auto strategy = categoryStrategies[;
                method typeof categoryStrategies;
                ] as [&](value: std::string) { return Promise<ValidationResult>; };
                return strategy(value);
            }
        }

        // Default validation - just check if value exists
        logger.warn(
        "No specific validation strategy found for " + varName + ", using basic validation"
        );
        return {
            isValid: true,
            details: "Basic validation passed - value is present",
            };
            } catch (error) {
                std::cerr << "Error validating environment variable " + varName + ":" << error << std::endl;
                return {
                    isValid: false,
                    error: "Validation failed due to unexpected error",
                    details: true /* instanceof check */ ? error.message : "Unknown error",
                    };
                }

}

} // namespace elizaos
