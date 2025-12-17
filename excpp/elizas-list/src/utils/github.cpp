#include "github.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> fetchGitHubData(const std::string& repo) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [owner, repoName] = repo.split('/').slice(-2);
    const auto response = fetch(;
    "https://api.github.com/repos/" + std::to_string(owner) + "/" + std::to_string(repoName)
    {
        headers: {
            "token " + std::to_string(process.env.GITHUB_TOKEN)
            },
        }
        );
        return response.json();

}

} // namespace elizaos
