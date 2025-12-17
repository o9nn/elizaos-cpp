#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Common utilities for run module
 * Converted from sweagent/run/common.py
 */

/**
 * Shorten a string to a maximum length
 */
std::string shortenString(const std::string& s, double maxLength, boolean = false shortenLeft);

/**
 * Shorten strings in a nested object/array
 */
unknown shortenStrings(unknown data, number = 30 maxLength);

/**
 * Save predictions from agent run result
 */
void savePredictions(const std::variant<std::string, path::ParsedPath>& trajDir, const std::string& instanceId, AgentRunResult result);

/**
 * Check if a patch is promising (not empty/trivial)
 */
bool isPromisingPatch(AgentInfo info);

/**
 * Create a nested dictionary structure
 */
std::unordered_map<std::string, unknown> createNestedDict();

/**
 * Parse command-line arguments into nested dictionary
 */
std::unordered_map<std::string, unknown> parseArgsToNestedDict(const std::vector<std::string>& args);

} // namespace elizaos
