#include "..environment/swe-env.hpp"
#include "..types.hpp"
#include "..utils/github.hpp"
#include "..utils/log.hpp"
#include "types.hpp"
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

/**
 * This hook opens a PR if the issue is solved and the user has enabled the option
 */
struct ProblemStatementWithGithubUrl {
    std::string githubUrl;
};

class OpenPRHook extends AbstractRunHook {
  private config: OpenPRConfig;
  private env?: SWEEnv;
  private token: string = '';
  private problemStatement?: ProblemStatementWithGithubUrl;

  constructor(config: OpenPRConfig = { skipIfCommitsReferenceIssue: true }) {
    super();
    this.config = config;
  }

        // Map trajectory to local TrajectoryStep type
      // Don't propagate the error - handle it gracefully

      // Continue anyway - we can't check for existing commits but that shouldn't stop PR creation


} // namespace elizaos
