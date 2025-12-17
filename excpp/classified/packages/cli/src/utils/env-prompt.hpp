#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Retrieves the absolute path to the `.env` environment file.
 *
 * @returns A promise that resolves to the full path of the environment file.
 */
std::future<std::string> getEnvFilePath();

/**
 * Asynchronously reads environment variables from the `.env` file and returns them as key-value pairs.
 *
 * Ignores comments and empty lines. If the file does not exist or cannot be read, returns an empty object.
 *
 * @returns A record containing environment variable names and their corresponding values.
 */

/**
 * Asynchronously writes the provided environment variables to the `.env` file, creating the directory if it does not exist.
 *
 * @param envVars - A record of environment variable key-value pairs to write.
 */
std::future<void> writeEnvFile(const std::unordered_map<std::string, std::string>& envVars);

} // namespace elizaos
