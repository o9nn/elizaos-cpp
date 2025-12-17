#include "github-publish.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<PublishResult> publishToGitHubAction(const std::string& cwd, PackageJson packageJson, Credentials credentials, boolean = false skipRegistry, boolean = false dryRun) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << 'Publishing to GitHub and registry...' << std::endl;

        const auto result = publishToGitHub(;
        cwd,
        packageJson,
        credentials.username,
        skipRegistry,
        dryRun;
        );

        if (!result) {
            throw std::runtime_error('GitHub publishing failed');
        }

        console.log(
        "[√] Successfully published plugin " + std::to_string(packageJson.name) + "@" + std::to_string(packageJson.version) + " to GitHub";
        );

        return result;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
