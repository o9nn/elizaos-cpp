#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "load-plugin.hpp"
#include "package-manager.hpp"
#include "plugin-context.hpp"
#include "plugin-discovery.hpp"
#include "registry.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Get the CLI's installation directory when running globally
 * @returns {string|null} - The path to the CLI's directory or null if not found
 */
std::optional<std::string> getCliDirectory();

/**
 * Verifies if a plugin can be imported
 * @param {string} repository - The plugin repository/package name to import
 * @param {string} context - Description of the installation context for logging
 */
std::future<bool> verifyPluginImport(const std::string& repository, const std::string& context);

/**
 * Attempts to install a plugin in a specific directory
 * @param {string} repository - The plugin repository to install
 * @param {string} versionString - Version string for installation
 * @param {string} directory - Directory to install in
 * @param {string} context - Description of the installation context for logging
 */
std::future<bool> attemptInstallation(const std::string& packageName, const std::string& versionString, const std::string& directory, const std::string& context, auto skipVerification = false);

/**
 * Asynchronously installs a plugin to a specified directory.
 *
 * @param {string} packageName - The repository URL of the plugin to install.
 * @param {string} cwd - The current working directory where the plugin will be installed.
 * @param {string} versionSpecifier - The specific version of the plugin to install.
 * @returns {Promise<boolean>} - A Promise that resolves to true if the plugin is successfully installed, or false otherwise.
 */
std::future<bool> installPlugin(const std::string& packageName, const std::string& cwd, std::optional<std::string> versionSpecifier, auto skipVerification = false);

} // namespace elizaos
