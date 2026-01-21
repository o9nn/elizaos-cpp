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
std::any convertToYamlLiteralString(const std::any& data);

/**
 * Serialize data to YAML with line breaks preserved
 */
std::string yamlSerializationWithLinebreaks(const std::any& data);

/**
 * Merge nested dictionaries/objects
 */
std::any mergeNestedDicts(const std::any& d1, const std::any& d2);

} // namespace elizaos
