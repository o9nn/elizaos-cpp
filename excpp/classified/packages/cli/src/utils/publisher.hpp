#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "github.hpp"
#include "registry.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct PackageJson {
    std::string name;
    std::string version;
    std::optional<std::string> description;
    std::optional<std::string> author;
    std::optional<std::string> url;
    std::optional<std::vector<std::string>> keywords;
    std::optional<std::vector<std::string>> categories;
    std::optional<std::string> platform;
    std::optional<std::string> packageType;
    std::optional<std::string> type;
};

/**
 * Tests whether the current environment is ready to publish an npm package from the specified directory.
 *
 * Performs checks for npm login status, build success, and publish permissions in the given directory.
 *
 * @param cwd - The directory containing the npm package to test.
 * @returns `true` if all checks pass; otherwise, `false`.
 */
std::future<bool> testPublishToNpm(const std::string& cwd);

/**
 * Tests whether the current user has the necessary GitHub credentials and permissions to publish a package and update the registry.
 *
 * For projects, verifies that a valid GitHub token is available. For plugins, additionally checks the ability to fork the registry repository, create a branch, and update files within that branch.
 *
 * @param cwd - The working directory of the package.
 * @param packageJson - The parsed package.json metadata for the package.
 * @param username - The GitHub username to use for repository operations.
 * @returns `true` if all required GitHub permissions and operations succeed; otherwise, `false`.
 */
std::future<bool> testPublishToGitHub(PackageJson packageJson, const std::string& username);

std::future<bool> publishToNpm(const std::string& cwd);

/**
 * Publishes a package to GitHub and optionally updates the ElizaOS registry for plugins.
 *
 * For both plugins and projects, this function creates or verifies a GitHub repository, pushes the local code, and returns success. For plugins (unless {@link skipRegistry} is true), it also updates the ElizaOS registry by forking the registry repository, creating a branch, updating or creating the package metadata, updating the registry index, and opening a pull request.
 *
 * @param cwd - The working directory containing the package to publish.
 * @param packageJson - The parsed package.json object for the package.
 * @param username - The GitHub username of the publisher.
 * @param skipRegistry - If true, skips registry updates and only publishes to GitHub.
 * @param isTest - If true, runs in test mode without making actual changes.
 * @returns True on success, or an object with success status and pull request URL if a registry PR is created; false on failure.
 *
 * @throws {Error} If required fields are missing or if publishing steps fail.
 *
 * @remark
 * - For projects or when {@link skipRegistry} is true, registry updates are skipped and only the GitHub repository is updated.
 * - For plugins, registry updates include metadata and index updates, and a pull request to the registry repository.
 */
std::future<bool> publishToGitHub(const std::string& cwd, PackageJson packageJson, const std::string& username, auto skipRegistry, auto isTest);> {
  // Get GitHub credentials using getGitHubCredentials which will prompt if needed

  // Validate required package type

  // Validate that packageType is either plugin or project

  // First, create the repository and push code to GitHub

    // Set the appropriate topic based on package type - only one topic, no mixing

    // Create GitHub repository with only the single appropriate topic

    // Repository exists or was created successfully

    // Construct repository URL with token for git operations

    // Push code to GitHub

    // For projects or when skipRegistry is true, we're done - skip registry update

  // The following code is for plugin registry updates only
  // Skip if we're publishing a project or skipRegistry is true

  // Check for fork

    // Small delay just in case

  // Create version branch - use the package type without default

  // Fix branch naming to avoid double "plugin-" prefix
    // For plugin names starting with 'plugin-', use only one 'plugin-' prefix
    // Example: plugin-apple -> plugin-apple-0.1.0 (not plugin-plugin-apple-0.1.0)
    // For other package types or non-plugin-prefixed names, use entityType prefix

  // Update package metadata

  // Use the actual npm package name from package.json (not @elizaos/ prefix)

    // Update index.json with simple mapping: npm package name -> github repo
        // Simple mapping: npm package name -> github repo

        // Check if entry already exists by parsing the JSON

        // Find the correct alphabetical position to insert the new entry

        // Find the correct insertion point alphabetically

          // Skip empty lines and opening brace

          // If we hit the closing brace, insert before it

          // Check if this is a package entry line
            // If our package should come before this one alphabetically

        // If we didn't find a position, insert before the closing brace

        // Insert the new entry at the correct alphabetical position

        // Update index.json with minimal change - preserve original structure

    // Create pull request

    // Return success with PR URL


} // namespace elizaos
