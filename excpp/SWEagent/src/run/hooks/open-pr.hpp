#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..environment/swe-env.hpp"
#include "..types.hpp"
#include "..utils/github.hpp"
#include "..utils/log.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Hook for opening pull requests on GitHub
 */

/**
 * Configuration for opening PRs
 */
struct OpenPRConfig {
    std::optional<bool> skipIfCommitsReferenceIssue;
};

/**
 * Format trajectory for markdown PR description
 */
struct TrajectoryStep {
    std::optional<std::string> response;
    std::optional<std::string> observation;
};

std::string formatTrajectoryMarkdown(const std::vector<TrajectoryStep>& trajectory, std::optional<double> charLimit);

/**
 * Create PR to repository
 */
struct GitHubIssue {
    double number;
    std::string title;
    std::string state;
    std::optional<unknown> assignee;
    bool locked;
};

std::future<void> openPR(std::optional<std::any> params);

/**
 * This hook opens a PR if the issue is solved and the user has enabled the option
 */
struct ProblemStatementWithGithubUrl {
    std::string githubUrl;
};

class OpenPRHook {
public:
    OpenPRHook(OpenPRConfig config = { skipIfCommitsReferenceIssue: true });
    void onInit(const std::any& run);
    std::future<void> onInstanceCompleted(const std::any& params);
    std::future<bool> shouldOpenPR(AgentRunResult result);


} // namespace elizaos
