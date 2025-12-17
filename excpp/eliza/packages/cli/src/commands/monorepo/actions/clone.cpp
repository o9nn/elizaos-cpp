#include "clone.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> cloneRepository(const std::string& repo, const std::string& branch, const std::string& destination) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto repoUrl = "https://github.com/" + std::to_string(repo);

            // Clone specific branch using execa
            execa('git', ['clone', '-b', branch, repoUrl, destination], {
                stdio: 'inherit',
                });
                } catch (error) {
                    // Special handling for likely branch errors
                    if (error instanceof Error && error.message.includes('exit code 128')) {
                        std::cerr << "\n[X] Branch '" + std::to_string(branch) + "' doesn't exist in the ElizaOS repository." << std::endl;
                        std::cerr << "Please specify a valid branch name. Common branches include:" << std::endl;
                        std::cerr << "  • main - The main branch" << std::endl;
                        std::cerr << "  • develop - The development branch (default)" << std::endl;
                        console.error(
                        "\nFor a complete list of branches, visit: https://github.com/elizaOS/eliza/branches"
                        );
                        throw std::runtime_error(`Branch '${branch}' not found`);
                    }
                    throw new Error(
                    "Failed to clone repository: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                    );
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string prepareDestination(const std::string& dir) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto destinationDir = path.resolve(process.cwd(), dir);

        // Check if destination directory already exists and is not empty
        if (existsSync(destinationDir)) {
            const auto files = readdirSync(destinationDir);
            if (files.length > 0) {
                throw std::runtime_error(`Destination directory ${destinationDir} already exists and is not empty`);
            }
            } else {
                mkdirSync(destinationDir, { recursive: true });
            }

            return destinationDir;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> cloneMonorepo(CloneInfo cloneInfo) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { repo, branch, destination } = cloneInfo;

    // Prepare the destination directory
    const auto destinationDir = prepareDestination(destination);

    // Clone the repository
    cloneRepository(repo, branch, destinationDir);

    return;

}

} // namespace elizaos
