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
 * Configuration utilities
 * Converted from sweagent/utils/config.py
 */

/**
 * Convert path relative to repository root
 */
std::variant<std:, path::ParsedPath> convertPathRelativeToRepoRoot(const std::variant<std:, path::ParsedPath>& inputPath, std::optional<std:> root);

/**
 * Check if a value could be a path
 */
bool couldBeAPath(const std:& value);

/**
 * Strip absolute paths from dictionary
 */
std: stripAbspathFromDict(const std:& value, std::optional<std:> root);

/**
 * Convert path to absolute path
 */

/**
 * Convert paths array to absolute paths
 */
std::vector<path::ParsedPath> convertPathsToAbspath(const std::variant<Array<std:, path::ParsedPath>>& paths);

/**
 * Load environment variables from .env file
 */
void loadEnvironmentVariables(std::optional<std:> envPath);

/**
 * Parse config file content
 */
std: parseConfigFile(const std:& content, const std:& format);

/**
 * Merge two config objects deeply
 */
std: mergeConfigs(const std:& baseConfig, const std:& overrideConfig);

/**
 * Validate config structure
 */
void validateConfig(const std:& config);

} // namespace elizaos
