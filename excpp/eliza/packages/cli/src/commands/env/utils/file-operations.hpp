#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Get the path to the project's .env file.
 * @returns The path to the .env file
 */
std::future<std::string> getGlobalEnvPath();

/**
 * Get the path to the local .env file in the current directory
 * @returns The path to the local .env file or null if not found
 */
std::future<std::string> getLocalEnvPath();

/**
 * Parse an .env file and return the key-value pairs
 * @param filePath Path to the .env file
 * @returns Object containing the key-value pairs
 */
std::future<EnvVars> parseEnvFile(const std::string& filePath);

/**
 * Write key-value pairs to an .env file
 * @param filePath Path to the .env file
 * @param envVars Object containing the key-value pairs
 */
std::future<void> writeEnvFile(const std::string& filePath, EnvVars envVars);

/**
 * Helper function to reset an environment file by keeping keys but clearing values
 * @param filePath Path to the environment file
 * @returns A boolean indicating success/failure
 */
std::future<bool> resetEnvFile(const std::string& filePath);

} // namespace elizaos
