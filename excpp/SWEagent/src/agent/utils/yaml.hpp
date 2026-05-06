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
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Simple YAML parser for basic YAML structures
 * Handles the YAML formats used in SWE-agent without external dependencies
 */

// Type for parsed YAML data
using YamlData = std::variant<std::string, double, bool, std::vector<YamlData>, std::any>;

/**
 * Parse a simple YAML string
 */
YamlData parseYAML(const std::string& yamlString);

/**
 * Parse a multiline string (| or > indicators)
 */

/**
 * Parse a key-value pair string into an object
 */
std::unordered_map<std::string, std::any> parseKeyValue(const std::string& str);

/**
 * Parse a YAML value (handle different types)
 */
YamlData parseValue(const std::string& value);

/**
 * Convert an object to YAML string
 */
std::string stringifyYAML(YamlData obj, double indent = 0);

} // namespace elizaos
