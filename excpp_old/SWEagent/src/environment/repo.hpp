#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".utils/github.hpp"
#include ".utils/log.hpp"
#include "deployment.hpp"
#include "runtime.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Repository management
 * Converted from sweagent/environment/repo.py
 */

/**
 * Repository protocol interface
 */
struct Repo {
    std::string baseCommit;
    std::string repoName;
};

/**
 * Get git reset commands
 */
std::vector<std::string> getGitResetCommands(const std::string& baseCommit);

/**
 * Pre-existing repository configuration
 */

using PreExistingRepoConfig = z::infer<typeof PreExistingRepoConfigSchema>;

class PreExistingRepo {
public:
    PreExistingRepo(PreExistingRepoConfig config);
    std::future<void> copy(AbstractDeployment _deployment);
    std::vector<std::string> getResetCommands();

private:
    std::string repoName_;
    std::string baseCommit_;
    bool reset_;
};

/**
 * Local repository configuration
 */

using LocalRepoConfig = z::infer<typeof LocalRepoConfigSchema>;

class LocalRepo {
public:
    LocalRepo(LocalRepoConfig config);
    void checkValidRepo();
    std::future<void> copy(AbstractDeployment deployment);
    std::vector<std::string> getResetCommands();

private:
    std::string path_;
    std::string baseCommit_;
    std::string repoName_;
};

/**
 * GitHub repository configuration
 */

using GithubRepoConfig = z::infer<typeof GithubRepoConfigSchema>;

class GithubRepo {
public:
    GithubRepo(GithubRepoConfig config);
    std::string getUrlWithToken(const std::string& token);
    std::future<void> copy(AbstractDeployment deployment);
    std::vector<std::string> getResetCommands();

private:
    std::string githubUrl_;
    std::string baseCommit_;
    double cloneTimeout_;
    std::string repoName_;
};

/**
 * Union type for all repo configurations
 */

using RepoConfig = z::infer<typeof RepoConfigSchema>;

/**
 * Factory function to create repo from simplified input
 */
Repo repoFromSimplifiedInput(const std::string& input, string = 'HEAD' baseCommit, const std::variant<'local', 'github', 'preexisting', 'auto' = 'auto'>& type);

} // namespace elizaos
