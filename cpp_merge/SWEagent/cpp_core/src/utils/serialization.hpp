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
std::string convertToYamlLiteralString(const std::string& data);

/**
 * Serialize data to YAML with line breaks preserved
 */
std::string yamlSerializationWithLinebreaks(const std::string& data);

/**
 * Merge nested dictionaries/objects
 */
std::string mergeNestedDicts(const std::string& d1, const std::string& d2);

} // namespace elizaos
