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
std::variant<std::string, path::ParsedPath> convertPathRelativeToRepoRoot(const std::variant<std::string, path::ParsedPath>& inputPath, std::optional<std::string> root);

/**
 * Check if a value could be a path
 */
bool couldBeAPath(const std::any& value);

/**
 * Strip absolute paths from dictionary
 */
std::any stripAbspathFromDict(const std::any& value, std::optional<std::string> root);

/**
 * Convert path to absolute path
 */

/**
 * Convert paths array to absolute paths
 */
std::vector<path::ParsedPath> convertPathsToAbspath(const std::variant<Array<string, path::ParsedPath>>& paths);

/**
 * Load environment variables from .env file
 */
void loadEnvironmentVariables(std::optional<std::string> envPath);

/**
 * Parse config file content
 */
std::any parseConfigFile(const std::string& content, const std::string& format);

/**
 * Merge two config objects deeply
 */
std::any mergeConfigs(const std::any& baseConfig, const std::any& overrideConfig);

/**
 * Validate config structure
 */
void validateConfig(const std::any& config);

} // namespace elizaos
