#include ".init.hpp"
#include "elizaos/core.hpp"
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



// Get the current file's directory

// Read and convert to Base64

/**
 * Recursively gets all files in a directory with the given extension
 *
 * @param {string} dir - Directory to search
 * @param {string[]} extensions - File extensions to look for
 * @returns {string[]} - Array of file paths
 */
std::vector<std::string> getFilesRecursively(const std::string& dir, const std::vector<std::string>& extensions);

/**
 * Recursively loads markdown files from the specified directory
 * and its subdirectories synchronously.
 *
 * @param {string} directoryPath - The path to the directory containing markdown files
 * @returns {string[]} - Array of strings containing file contents with relative paths
 */
std::vector<std::string> loadDocumentation(const std::string& directoryPath);

/**
 * Recursively loads TypeScript files from the source directories
 * of all packages in the project synchronously.
 *
 * @param {string} packagesDir - The path to the packages directory
 * @returns {string[]} - Array of strings containing file contents with relative paths
 */
std::vector<std::string> loadSourceCode(const std::string& packagesDir);

// Load knowledge synchronously before creating the character

  // Load documentation

  // Load source code
  // if it doesnt exist, try "../../"

/**
 * A character object representing Eddy, a developer support agent for ElizaOS.
 */
  // Knowledge will be set after adapter initialization

/**
 * Configuration object for onboarding settings.
 */

    // Initialize the character
    await initCharacter({ runtime, config });


} // namespace elizaos
