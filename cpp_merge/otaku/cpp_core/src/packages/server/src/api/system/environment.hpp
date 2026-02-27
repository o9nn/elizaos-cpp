#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using EnvVars = std::unordered_map<std:, std:>;

/**
 * Parse an .env file and return the key-value pairs
 * @param filePath Path to the .env file
 * @returns Object containing the key-value pairs
 */
std::future<EnvVars> parseEnvFile(const std:& filePath);

std: serializeEnvObject(const std::unordered_map<std:, std:>& envObj);

std: getLocalEnvPath();

/**
 * Resolves the path to the nearest ".env" file.
 *
 * If no ".env" file is found when traversing up from the starting directory,
 * a path to ".env" in the starting directory is returned.
 *
 * @param startDir - The directory to start searching from. Defaults to the
 *   current working directory.
 * @returns The resolved path to the ".env" file.
 */

/**
 * Environment configuration management
 */

} // namespace elizaos
