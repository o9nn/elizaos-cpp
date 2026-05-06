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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Interface for defining options when creating a pull request.
 *
 * @typedef {Object} CreatePullRequestOptions
 * @property {string} title - The title of the pull request.
 * @property {string} body - The body of the pull request.
 * @property {string} head - The branch where changes are coming from.
 * @property {string} base - The branch where changes will be merged into.
 * @property {string[]} [labels] - Optional array of labels to assign to the pull request.
 * @property {string[]} [reviewers] - Optional array of GitHub usernames for reviewers of the pull request.
 */
struct CreatePullRequestOptions {
    std::string title;
    std::string body;
    std::string head;
    std::string base;
    std::optional<std::vector<std::string>> labels;
    std::optional<std::vector<std::string>> reviewers;
};

/**
 * Manages operations related to interacting with a Git repository using the GitHub API.
 */
/**
 * Class representing a GitManager.
 */

/**
 * Constructor for a class that initializes the Octokit instance with the provided Repository and checks if the GITHUB_ACCESS_TOKEN is set in the environment.
 * @param {Repository} repository - The repository instance to use
 * @throws {Error} Throws an error if the GITHUB_ACCESS_TOKEN is not set
 */

/**
 * Retrieve files in a specific pull request.
 * @param {number} pullNumber - The number of the pull request to get files from.
 * @returns {Promise<PrModeFileChange[]>} - Array of objects representing file changes in the pull request.
 */

/**
 * Creates a new branch in the GitHub repository using the given branch name and base branch.
 * @param {string} branchName - The name of the new branch to be created.
 * @param {string} baseBranch - The name of the branch to base the new branch off of.
 * @returns {Promise<void>} - A Promise that resolves when the branch is successfully created.
 */

/**
 * Asynchronously commits a file to a repository using the GitHub API.
 * @param {string} branchName - The name of the branch to commit the file to.
 * @param {string} filePath - The path of the file to commit.
 * @param {string} content - The content of the file to commit.
 * @param {string} message - The commit message.
 * @returns {Promise<void>} A promise that resolves when the file is successfully committed.
 */

/**
 * Create a pull request using the provided options.
 * @param {CreatePullRequestOptions} options - The options for creating the pull request.
 * @returns {Promise<void>} A Promise that resolves once the pull request is successfully created.
 */
class GitManager {
public:
    GitManager();
    std::future<std::vector<PrModeFileChange>> getFilesInPullRequest(double pullNumber);
    std::future<void> createBranch(const std::string& branchName, const std::string& baseBranch);
    std::future<void> commitFile(const std::string& branchName, const std::string& filePath, const std::string& content, const std::string& message);
    std::future<void> createPullRequest(CreatePullRequestOptions options);

private:
    Octokit octokit_;
};


} // namespace elizaos
