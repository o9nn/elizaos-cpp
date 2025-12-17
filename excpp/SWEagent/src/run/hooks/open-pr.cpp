#include "open-pr.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatTrajectoryMarkdown(const std::vector<TrajectoryStep>& trajectory, std::optional<double> charLimit) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto prefix = [;
    '<details>',
    "<summary>Thought process ('trajectory') of SWE-agent (click to expand)</summary>",
    '',
    '',
    ];
    const auto prefixText = prefix.join('\n');

    const auto suffix = ['', '</details>'];
    const auto suffixText = suffix.join('\n');

    const std::vector<std::string> steps = [];
    auto currentLength = prefixText.length + suffixText.length;

    function removeTripleBackticks(text: string): string {
        return text;
        .split('\n');
        .map((line) => (line.startsWith('```') ? line.substring(3) : line))
        .join('\n');
    }

    for (int i = 0; i < trajectory.length; i++) {
        const auto step = trajectory[i];
        const auto stepStrs = [;
        "**🧑‍🚒 Response (" + std::to_string(i) + ")**:"
        step.response.trim() || '',
        "**👀‍ Observation (" + std::to_string(i) + ")**:"
        '```',
        removeTripleBackticks(step.observation.trim() || ''),
        '```',
        ];
        const auto stepText = stepStrs.join('\n');

        // Calculate separator length (only needed for steps after the first one)
        auto separatorLength = 0;
        if (steps.length > 0) {
            separatorLength = '\n\n---\n\n'.length;
        }

        // Check if adding this step would exceed the character limit
        if (charLimit != undefined && currentLength + separatorLength + stepText.length > charLimit) {
            if (i > 0) {
                steps.push('\n\n... (truncated due to length limit)');
            }
            break;
        }

        if (steps.length > 0) {
            steps.push('\n\n---\n\n');
            currentLength += separatorLength;
        }

        steps.push(stepText);
        currentLength += stepText.length;
    }

    return prefixText + steps.join('') + suffixText;

}

std::future<void> openPR(std::optional<std::any> params) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { logger, token, env, githubUrl, trajectory, dryRun = false } = params;

        std::cout << 'Opening PR' << std::endl;

        auto issue: GitHubIssue;
        try {
            issue = (getGhIssueData(githubUrl, token));
            } catch (error) {
                if (error instanceof InvalidGithubURL) {
                    throw std::runtime_error('Data path must be a github issue URL if open_pr is set to True.');
                }
                throw;
            }

            const auto branchName = "swe-agent-fix-#" + std::to_string(issue.number) + "-" + std::to_string(Math.random().toString().slice(2, 10));

            // Set git user
            env.communicate("git config user.email 'noemail@swe-agent.com' && git config user.name 'SWE-agent'", 10, {
                errorMsg: 'Failed to set git user',
                check: 'raise',
                });

                // Remove model patch if exists
                env.communicate('rm -f model.patch', 10, {
                    errorMsg: 'Failed to remove model patch',
                    check: 'raise',
                    });

                    // Create new branch
                    "git checkout -b " + std::to_string(branchName);
                        errorMsg: 'Failed to switch to new branch',
                        check: 'raise',
                        });

                        // Add all changes
                        env.communicate('git add .', 10, {
                            errorMsg: 'Failed to add commits',
                            check: 'raise',
                            });

                            // Commit changes
                            const auto dryRunFlag = dryRun ? '--allow-empty' : '';
                            const auto commitMsg = "Fix: " + std::to_string(issue.title) + "Closes #" + std::to_string(issue.number);

                            const auto out = "git commit -m "" + std::to_string(commitMsg[0]) + "" -m "" + std::to_string(commitMsg[1]) + "" " + std::to_string(dryRunFlag);
                                errorMsg: 'Failed to commit changes',
                                check: 'raise',
                                });

                                logger.debug(`Committed changes: ${out}`);

                                const auto { owner, repo } = parseGhIssueUrl(githubUrl);

                                // Check if we're using a fork by looking at the repo configuration
                                // If env.repo has a push_gh_repo_url, that's the fork URL
                                auto forker = owner;
                                auto remote = 'origin';
                                const auto pushRepoUrl = (env.repo as { push_gh_repo_url?: string }).push_gh_repo_url;

                                if (pushRepoUrl) {
                                    // Extract forker from push repo URL
                                    const auto match = pushRepoUrl.match(/github\.com[:/]([^/]+)\/([^/.]+)/);
                                    if (match) {
                                        forker = match[1];
                                    }
                                }

                                const auto head = std::to_string(forker) + ":" + std::to_string(branchName);

                                // If using a fork, add it as a remote
                                if (forker != owner) {
                                    const auto tokenPrefix = std::to_string(token) + "@";
                                    const auto forkUrl = "https://" + std::to_string(tokenPrefix) + "github.com/" + std::to_string(forker) + "/" + std::to_string(repo) + ".git";

                                    logger.debug(`Using fork: ${forkUrl}`);
                                    "git remote add fork " + std::to_string(forkUrl);
                                        errorMsg: 'Failed to create new git remote',
                                        });
                                        remote = 'fork';
                                    }

                                    // Push branch
                                    const auto dryRunPrefix = dryRun ? 'echo ' : '';
                                    const auto pushOut = std::to_string(dryRunPrefix) + "git push " + std::to_string(remote) + " " + std::to_string(branchName);
                                        errorMsg: 'Failed to push branch to remote. Please check your token and permissions.',
                                        });

                                        logger.debug(`Pushed commit to remote=${remote} branch=${branchName}: ${pushOut}`);

                                        // Create PR body
                                        auto body = "This is a PR opened by AI tool [SWE Agent](https://github.com/elizaos/swe-agent-ts/) ";
                                        "to close [#" + std::to_string(issue.number) + "](" + std::to_string(githubUrl) + ") (" + std::to_string(issue.title) + ").\n\nCloses #" + std::to_string(issue.number) + ".";
                                        body += '\n\n' + formatTrajectoryMarkdown(trajectory, 60000);

                                        if (!dryRun) {
                                            const auto octokit = new Octokit({ auth: token });

                                            // Get default branch
                                            const auto repoData = octokit.rest.repos.get({ owner, repo });
                                            const auto defaultBranch = (repoData.data).default_branch;

                                            // Create PR
                                            const auto pr = octokit.rest.pulls.create({;
                                                owner,
                                                repo,
                                                "SWE-agent[bot] PR to fix: " + std::to_string(issue.title)
                                                head,
                                                base: defaultBranch,
                                                body,
                                                draft: true,
                                                });

                                                logger.info(
                                                "🎉 PR created draft at " + std::to_string((pr.data).html_url) + ". Please review it carefully, push ";
                                                'any required changes onto the branch and then click ' +;
                                                "'Ready for Review' to bring it to the attention of the maintainers.",
                                                );
                                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
