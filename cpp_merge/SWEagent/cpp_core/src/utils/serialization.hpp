#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
std: convertToYamlLiteralString(const std:& data);

/**
 * Serialize data to YAML with line breaks preserved
 */
std: yamlSerializationWithLinebreaks(const std:& data);

/**
 * Merge nested dictionaries/objects
 */
std: mergeNestedDicts(const std:& d1, const std:& d2);

} // namespace elizaos
