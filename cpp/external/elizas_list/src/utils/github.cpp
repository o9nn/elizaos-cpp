#include "github.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> fetchGitHubData(const std::string& repo) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [owner, repoName] = repo.split("/").slice(-2);
    const auto response = fetch(;
    "https://api.github.com/repos/" + owner + "/" + repoName
    {
        headers: {
            "Authorization: " + "token " + process.env.GITHUB_TOKEN
            },
        }
        );
        return response.json();

}

} // namespace elizaos
