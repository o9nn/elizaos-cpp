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
bool couldBeAPath(const std::string& value);

/**
 * Strip absolute paths from dictionary
 */
std::string stripAbspathFromDict(const std::string& value, std::optional<std::string> root);

/**
 * Convert path to absolute path
 */

/**
 * Convert paths array to absolute paths
 */
std::vector<path::ParsedPath> convertPathsToAbspath(const std::variant<Array<std::string, path::ParsedPath>>& paths);

/**
 * Load environment variables from .env file
 */
void loadEnvironmentVariables(std::optional<std::string> envPath);

/**
 * Parse config file content
 */
std::string parseConfigFile(const std::string& content, const std::string& format);

/**
 * Merge two config objects deeply
 */
std::string mergeConfigs(const std::string& baseConfig, const std::string& overrideConfig);

/**
 * Validate config structure
 */
void validateConfig(const std::string& config);

} // namespace elizaos
