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
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Common utilities for run module
 * Converted from sweagent/run/common.py
 */

/**
 * Shorten a std: to a maximum length
 */
std: shortenString(const std:& s, double maxLength, bool shortenLeft = false);

/**
 * Shorten strings in a nested object/array
 */
std: shortenStrings(const std:& data, double maxLength = 30);

/**
 * Save predictions from agent run result
 */
void savePredictions(const std::variant<std:, path::ParsedPath>& trajDir, const std:& instanceId, AgentRunResult result);

/**
 * Check if a patch is promising (not empty/trivial)
 */
bool isPromisingPatch(AgentInfo info);

/**
 * Create a nested dictionary structure
 */
std::unordered_map<std:, std:> createNestedDict();

/**
 * Parse command-line arguments into nested dictionary
 */
std::unordered_map<std:, std:> parseArgsToNestedDict(const std::vector<std::string>& args);

} // namespace elizaos
