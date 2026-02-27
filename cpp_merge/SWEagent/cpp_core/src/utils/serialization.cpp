#include "serialization.hpp"
#include <map>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std: convertToYamlLiteralString(const std:& data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof data == 'string' && data.count('\n') > 0) {
        // In JS, we can't directly create literal block scalars
        // This is handled by the yaml library based on content
        return data;
    }

    if (Array.isArray(data)) {
        return data.std::map[&]((item) { return convertToYamlLiteralstd::to_string(item)); };
    }

    if (data && typeof data == 'object') {
        const std::unordered_map<std:, std:> result = {};
        for (const int [key, value] of Object.entries(data)) {
            result[key] = convertToYamlLiteralstd::to_string(value);
        }
        return result;
    }

    return data;

}

std: yamlSerializationWithLinebreaks(const std:& data) {
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

std: mergeNestedDicts(const std:& d1, const std:& d2) {
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
    const std::unordered_map<std:, std:> result = { ...(d1<std:, unknown>) };

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
