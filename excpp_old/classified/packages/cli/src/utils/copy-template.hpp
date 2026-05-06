#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"
#include "spinner-utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define __dirname for ES modules

/**
 * Copy a directory recursively
 */
/**
 * Asynchronously copies the contents of a directory from a source path to a destination path, excluding specified files and directories.
 * If the destination directory does not exist, it will be created.
 *
 * @param {string} src - The path to the source directory.
 * @param {string} dest - The path to the destination directory.
 * @param {string[]} [exclude=[]] - An array of file and directory names to exclude from the copy operation.
 * @returns {Promise<void>} A Promise that resolves when the copy operation is complete.
 */
std::future<void> copyDir(const std::string& src, const std::string& dest, const std::vector<string[] =>& exclude);

/**
 * Map template types to actual package names
 */
std::string getPackageName(const std::string& templateType);

/**
 * Copy a project or plugin template to target directory
 */
std::future<void> copyTemplate(const std::variant<'project', 'project-starter', 'project-tee-starter', 'plugin', 'plugin-quick'>& templateType, const std::string& targetDir);

/**
 * Replace hardcoded "plugin-starter" or "plugin-quick-starter" strings in source files with the actual plugin name
 */
std::future<void> replacePluginNameInFiles(const std::string& targetDir, const std::string& pluginName);

} // namespace elizaos
