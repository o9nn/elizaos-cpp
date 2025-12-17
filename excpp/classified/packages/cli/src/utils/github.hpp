#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "bun-exec.js.hpp"
#include "elizaos/core.hpp"
#include "user-environment.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct GitHubUserResponse {
    std::string login;
};

struct GitHubRepoResponse {
    std::string full_name;
};

struct GitHubBranchResponse {
    std::string sha;
};

struct GitHubFileResponse {
    std::string content;
    std::string sha;
};

struct GitHubPullRequestResponse {
    std::string html_url;
};

/**
 * Validate a GitHub token with the API
 */
std::future<bool> validateGitHubToken(const std::string& token);

/**
 * Check if a fork exists for a given repository
 */
std::future<bool> forkExists(const std::string& token, const std::string& repo, const std::string& username);

/**
 * Fork a repository
 */
std::future<std::string> forkRepository(const std::string& token, const std::string& owner, const std::string& repo);

/**
 * Check if a branch exists in a repository
 */
std::future<bool> branchExists(const std::string& token, const std::string& owner, const std::string& repo, const std::string& branch);

/**
 * Create a new branch in a repository
 */
std::future<bool> createBranch(const std::string& token, const std::string& owner, const std::string& repo, const std::string& branch, auto baseBranch);

/**
 * Get content of a file from a repository
 */
std::future<std::string> getFileContent(const std::string& token, const std::string& owner, const std::string& repo, const std::string& path, auto branch);

/**
 * Create or update a file in a repository
 */
std::future<bool> updateFile(const std::string& token, const std::string& owner, const std::string& repo, const std::string& path, const std::string& content, const std::string& message, auto branch);

/**
 * Create a pull request
 */
std::future<std::string> createPullRequest(const std::string& token, const std::string& owner, const std::string& repo, const std::string& title, const std::string& body, const std::string& head, auto base);

/**
 * Get authenticated user information
 */

/**
 * Retrieves GitHub credentials from the environment, registry, or user prompt.
 *
 * Attempts to obtain a valid GitHub username and personal access token by first checking environment variables, then a stored registry, and finally prompting the user if necessary. Validates the token before returning credentials.
 *
 * @returns An object containing the GitHub username and token if valid credentials are found or provided, otherwise `null`.
 */

  // First check environment variables

    // Validate the token

  // If we have a token, validate it and try to get username if missing
      // If we don't have a username, try to get it from GitHub
        // Save the username to env

  // No valid credentials found, prompt the user

  // Then prompt for the username with a simple message

  // Validate token

  // Save credentials

/**
 * Saves the provided GitHub username and token to the `.env` file in the user's `.eliza` directory.
 *
 * Updates or adds the `GITHUB_USERNAME` and `GITHUB_TOKEN` entries in the file and sets them in the current process environment.
 */
std::future<void> saveGitHubCredentials(const std::string& username, const std::string& token);

/**
 * Ensure a directory exists in the repository
 */
std::future<bool> ensureDirectory(const std::string& token, const std::string& repo, const std::string& path, const std::string& branch);

/**
 * Create a new GitHub repository
 */
    // Get the authenticated user to check repository existence

    // First check if the repository already exists

    // If repo exists, return success

    // If status is not 404, there might be another issue

    // If we get here, repo doesn't exist on GitHub (404), so create it

      // Wait a moment for GitHub to initialize the repository

      // Add topics to the repository

/**
 * Push local code to GitHub repository
 */
std::future<bool> pushToGitHub(const std::string& cwd, const std::string& repoUrl, auto branch);

} // namespace elizaos
