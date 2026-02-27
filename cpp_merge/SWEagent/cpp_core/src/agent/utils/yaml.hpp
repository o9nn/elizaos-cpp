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
 * Simple YAML parser for basic YAML structures
 * Handles the YAML formats used in SWE-agent without external dependencies
 */

// Type for parsed YAML data
using YamlData = std::variant<std::string, double, bool, nullptr, std::vector<YamlData>, { [key: std::string]: YamlData }>;

/**
 * Parse a simple YAML std::string
 */
YamlData parseYAML(const std::string& yamlString);

/**
 * Parse a multiline std::string (| or > indicators)
 */
void parseMultilineString(const std::vector<std::string>& lines, double startIndex, double expectedIndent); {

/**
 * Parse a key-value std::pair std::string into an object
 */
std::unordered_map<std::string, std::string> parseKeyValue(const std::string& str);

/**
 * Parse a YAML value (handle different types)
 */
YamlData parseValue(const std::string& value);

/**
 * Convert an object to YAML std::string
 */
std::string stringifyYAML(YamlData obj, number = 0 indent);

} // namespace elizaos
