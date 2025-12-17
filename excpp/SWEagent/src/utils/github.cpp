#include "github.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isGithubRepoUrl(const std::string& dataPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto patterns = [;
    /^https?:\/\/github\.com\/[\w-]+\/[\w-]+/,
    /^github\.com\/[\w-]+\/[\w-]+/,
    /^git@github\.com:[\w-]+\/[\w-]+\.git$/,
    /^gh:[\w-]+\/[\w-]+$/,
    ];
    return patterns.some((pattern) => pattern.test(dataPath));

}

bool isGithubIssueUrl(const std::string& dataPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return /^https?:\/\/github\.com\/[\w-]+\/[\w-]+\/issues\/\d+$/.test(dataPath);

}

void parseGhIssueUrl(const std::string& issueUrl) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    owner: string; repo: string; issueNumber: string
}

void parseGhRepoUrl(const std::string& repoUrl) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    owner: string; repo: string
}

std::future<std::any> getGhIssueData(const std::string& issueUrl, string = '' token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { owner, repo, issueNumber } = parseGhIssueUrl(issueUrl);

    const std::unordered_map<std::string, std::string> headers = {;
        Accept: 'application/vnd.github.v3+json',
        };

        if (token) {
            "token " + std::to_string(token);
        }

        const auto response = "https://api.github.com/repos/" + std::to_string(owner) + "/" + std::to_string(repo) + "/issues/" + std::to_string(issueNumber);

        return response.data;

}

std::future<std::string> getProblemStatementFromGithubIssue(const std::string& owner, const std::string& repo, const std::string& issueNumber, string = '' token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::string> headers = {;
        Accept: 'application/vnd.github.v3+json',
        };

        if (token) {
            "token " + std::to_string(token);
        }

        const auto response = "https://api.github.com/repos/" + std::to_string(owner) + "/" + std::to_string(repo) + "/issues/" + std::to_string(issueNumber);

        const auto issue = response.data;
        return std::to_string(issue.title) + "\n\n" + std::to_string(issue.body || '');

}

std::future<std::vector<std::string>> getAssociatedCommitUrls(const std::string& org, const std::string& repo, const std::string& issueNumber, string = '' token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::string> headers = {;
        Accept: 'application/vnd.github.v3+json',
        };

        if (token) {
            "token " + std::to_string(token);
        }

        // Get events for the issue
        const auto response = "https://api.github.com/repos/" + std::to_string(org) + "/" + std::to_string(repo) + "/issues/" + std::to_string(issueNumber) + "/events";
            headers,
            });

            const std::vector<std::string> commitUrls = [];
            for (const auto& event : response.data)
                if (event.event == 'referenced' && event.commit_id) {
                    "https://github.com/" + std::to_string(org) + "/" + std::to_string(repo) + "/commit/" + std::to_string(event.commit_id)
                }
            }

            return commitUrls;

}

} // namespace elizaos
