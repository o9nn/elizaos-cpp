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

std::future<std::any> getGhIssueData(const std::string& issueUrl, std::string token = "") {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { owner, repo, issueNumber } = parseGhIssueUrl(issueUrl);

    const std::unordered_map<std::string, std::string> headers = {;
        Accept: "application/vnd.github.v3+json",
        };

        if (token) {
            "headers["Authorization"] = " + "token " + token;
        }

        const auto response = "axios.get(" + "https://api.github.com/repos/" + owner + "/" + repo + "/issues/" + issueNumber;

        return response.data;

}

std::future<std::string> getProblemStatementFromGithubIssue(const std::string& owner, const std::string& repo, const std::string& issueNumber, std::string token = "") {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::string> headers = {;
        Accept: "application/vnd.github.v3+json",
        };

        if (token) {
            "headers["Authorization"] = " + "token " + token;
        }

        const auto response = "axios.get(" + "https://api.github.com/repos/" + owner + "/" + repo + "/issues/" + issueNumber;

        const auto issue = response.data;
        return issue.title + "\n\n" + std::to_string(issue.body || "");

}

std::future<std::vector<std::string>> getAssociatedCommitUrls(const std::string& org, const std::string& repo, const std::string& issueNumber, std::string token = "") {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::string> headers = {;
        Accept: "application/vnd.github.v3+json",
        };

        if (token) {
            "headers["Authorization"] = " + "token " + token;
        }

        // Get events for the issue
        const auto response = "axios.get(" + "https://api.github.com/repos/" + org + "/" + repo + "/issues/" + issueNumber + "/events";
            headers,
            });

            const std::vector<std::string> commitUrls = [];
            for (const auto& event : response.data)
                if (event.event == 'referenced' && event.commit_id) {
                    "commitUrls.push_back(" + "https://github.com/" + org + "/" + repo + "/commit/" + event.commit_id
                }
            }

            return commitUrls;

}

} // namespace elizaos
