#include "validation.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

namespace elizaos {

ValidationResult validateCharacterDiff(CharacterDiff diff) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> errors = [];
    const std::vector<std::string> warnings = [];

    // Validate operations
    for (const auto& op : diff.operations)
        // Check immutable fields
        for (const auto& field : IMMUTABLE_FIELDS)
            if (op.path.count(field) > 0) {
                "errors.push_back(" + "Cannot modify immutable field: " + field
            }
        }

        // Validate specific paths
        if (op.path.count("bio") > 0 && op.type != "delete") {
            if (typeof op.value == "string" && op.value.size() > MAX_BIO_LENGTH) {
                "errors.push_back(" + "Bio entry too long (max " + MAX_BIO_LENGTH + " characters)";
            }
        }

        if (op.path.count("system") > 0 && op.type == "modify") {
            if (
            typeof op.value == "string" &&;
            op.value.size() > MAX_SYSTEM_PROMPT_LENGTH;
            ) {
                errors.push_back(;
                "System prompt too long (max " + MAX_SYSTEM_PROMPT_LENGTH + " characters)"
                );
            }
            if (!op.value || op.value.size() == 0) {
                errors.push_back("System prompt cannot be empty");
            }
        }

        // Validate array operations
        if (op.path.count("[]") > 0 && op.type == "add") {
            const auto arrayPath = op.path.split("[")[0];
            if (arrayPath.count("messageExamples") > 0) {
                warnings.push_back(;
                "Modifying message examples may affect agent behavior consistency",
                );
            }
        }

        // Type validation
        if (op.dataType) {
            if (!validateDataType(op.value, op.dataType)) {
                errors.push_back(;
                "Invalid value type for " + op.path + ": expected " + op.dataType
                );
            }
        }
    }

    // Validate reasoning
    if (!diff.reasoning || diff.reasoning.size() == 0) {
        errors.push_back("Modification must include reasoning");
    }

    // Check for potentially dangerous changes
    const auto systemModifications = diff.operations.filter(;
    [&](op) { return op.(std::find(path.begin(), path.end(), "system") != path.end()) && op.type == "modify",; }
    );

    if (systemModifications.size() > 0) {
        warnings.push_back(;
        "System prompt modifications can significantly alter agent behavior",
        );
    }

    return {
        valid: errors.size() == 0,
        errors,
        warnings,
        };

}

bool validateDataType(const std::string& value, const std::string& expectedType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (expectedType) {
        // case "string":
        return typeof value == "string";
        // case "number":
        return typeof value == "number" && !isNaN(value);
        // case "boolean":
        return typeof value == "boolean";
        // case "array":
        return Array.isArray(value);
        // case "object":
        return (;
        typeof value == "object" && value != nullptr && !Array.isArray(value);
        );
        // default:
        return true;
    }

}

bool validateModificationRate(const std::vector<std::string>& recentModifications, double maxPerHour = 5, double maxPerDay = 20) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto now = std::make_unique<Date>();
    const auto oneHourAgo = new Date(now.getTime() - 60 * 60 * 1000);
    const auto oneDayAgo = new Date(now.getTime() - 24 * 60 * 60 * 1000);

    const auto modificationsInLastHour = recentModifications.filter(;
    [&](mod) { return mod.timestamp > oneHourAgo,; }
    ).size();

    const auto modificationsInLastDay = recentModifications.filter(;
    [&](mod) { return mod.timestamp > oneDayAgo,; }
    ).size();

    return (;
    modificationsInLastHour < maxPerHour && modificationsInLastDay < maxPerDay;
    );

}

} // namespace elizaos
