#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Serialization utilities
 * Converted from sweagent/utils/serialization.py
 */

/**
 * Convert strings to YAML literal strings for better readability
 */
unknown convertToYamlLiteralString(unknown data);

/**
 * Serialize data to YAML with line breaks preserved
 */
std::string yamlSerializationWithLinebreaks(unknown data);

/**
 * Merge nested dictionaries/objects
 */
unknown mergeNestedDicts(unknown d1, unknown d2);

} // namespace elizaos
