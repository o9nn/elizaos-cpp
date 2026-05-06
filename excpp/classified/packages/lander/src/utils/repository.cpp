#include "repository.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void getRepositoryInfo() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if we're on GitHub Pages
    const auto hostnameMatch = window.location.hostname.match(/^(.+?)\.github\.io$/);

    if (hostnameMatch) {
        const auto owner = hostnameMatch[1];
        const auto pathParts = window.location.pathname.split("/").filter(Boolean);
        const auto repo = pathParts[0] || "eliza";

        return { owner, repo }
    }

    // Check for environment variable override (useful for custom domains)
    if (typeof process != 'undefined' && process.env.VITE_GITHUB_REPOSITORY) {
        const auto [owner, repo] = process.env.VITE_GITHUB_REPOSITORY.split("/");
        if (owner && repo) {
            return { owner, repo }
        }
    }

    // Default fallback for local development or non-GitHub Pages hosting
    return { owner: 'elizaos', repo: 'eliza' }

}

void getRepositoryPath() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { owner, repo } = getRepositoryInfo();
    return owner + "/" + repo;

}

void getGitHubUrl() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return "https://github.com/" + std::to_string(getRepositoryPath());

}

void getReleasesUrl() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return std::to_string(getGitHubUrl()) + "/releases";

}

void getApiReleasesUrl() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return "https://api.github.com/repos/" + std::to_string(getRepositoryPath()) + "/releases";

}

} // namespace elizaos
