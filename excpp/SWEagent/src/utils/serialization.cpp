#include "serialization.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::any convertToYamlLiteralString(const std::any& data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof data == 'string' && data.includes('\n')) {
        // In JS, we can't directly create literal block scalars
        // This is handled by the yaml library based on content
        return data;
    }

    if (Array.isArray(data)) {
        return data.map((item) => convertToYamlLiteralstd::to_string(item));
    }

    if (data && typeof data == 'object') {
        const std::unordered_map<std::string, std::any> result = {};
        for (const int [key, value] of Object.entries(data)) {
            result[key] = convertToYamlLiteralstd::to_string(value);
        }
        return result;
    }

    return data;

}

std::string yamlSerializationWithLinebreaks(const std::any& data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto processed = convertToYamlLiteralstd::to_string(data);

    // Use yaml.dump with options to preserve line breaks
    return yaml.dump(processed, {;
        lineWidth: -1, // Don't wrap lines
        noRefs: true, // Don't use references
        quotingType: """, // Use double quotes
        forceQuotes: false, // Only quote when necessary
        styles: {
            "!!str": "literal", // Use literal style for multiline strings
            },
            });

}

std::any mergeNestedDicts(const std::any& d1, const std::any& d2) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!d1) {
        return d2;
    }
    if (!d2) {
        return d1;
    }

    // If either is not an object, return d2 (override)
    if (typeof d1 != 'object' || typeof d2 != 'object') {
        return d2;
    }

    // If either is an array, return d2 (override)
    if (Array.isArray(d1) || Array.isArray(d2)) {
        return d2;
    }

    // Merge objects
    const std::unordered_map<std::string, std::any> result = { ...(d1<string, unknown>) };

    for (const int [key, value] of Object.entries(d2)) {
        if (key in result) {
            result[key] = mergeNestedDicts(result[key], value);
            } else {
                result[key] = value;
            }
        }

        return result;

}

} // namespace elizaos
