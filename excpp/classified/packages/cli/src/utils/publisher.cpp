#include "publisher.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> testPublishToNpm(const std::string& cwd) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Check if logged in to npm
        bunExec('npm', ['whoami']);
        std::cout << '[✓] Logged in to npm' << std::endl;

        // Test build
        std::cout << 'Testing build...' << std::endl;
        bunExec('npm', ['run', 'build', '--dry-run'], { cwd });
        std::cout << '[✓] Build test successful' << std::endl;

        // Test publish access
        bunExec('npm', ['access', 'ls-packages'], { cwd });
        std::cout << '[✓] Have publish permissions' << std::endl;

        return true;
        } catch (error) {
            std::cerr << 'Test failed:' << error << std::endl;
            if (error instanceof Error) {
                std::cerr << "Error message: " + std::to_string(error.message) << std::endl;
                std::cerr << "Error stack: " + std::to_string(error.stack) << std::endl;
            }
            return false;
        }

}

std::future<bool> testPublishToGitHub(PackageJson packageJson, const std::string& username) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Get GitHub credentials using getGitHubCredentials which will prompt if needed
        const auto credentials = getGitHubCredentials();
        if (!credentials) {
            std::cerr << 'Failed to get GitHub credentials' << std::endl;
            return false;
        }
        const auto token = credentials.token;
        std::cout << '[✓] GitHub credentials found' << std::endl;

        // Validate token permissions
        const auto response = fetch('https://api.github.com/user', {;
            "token " + std::to_string(token)
            });
            if (!response.ok) {
                std::cerr << 'Invalid GitHub token or insufficient permissions' << std::endl;
                return false;
            }
            std::cout << '[✓] GitHub token is valid' << std::endl;

            // For projects, we only need to check GitHub token validity
            if (packageJson.packageType == 'project') {
                std::cout << '[✓] Project validation complete - GitHub token is valid' << std::endl;
                return true;
            }

            // The following tests are only for plugins that need registry updates

            // Test registry access
            const auto settings = getRegistrySettings();
            const auto [registryOwner, registryRepo] = settings.defaultRegistry.split('/');

            // Log the registry we're testing with
            std::cout << "Testing with registry: " + std::to_string(registryOwner) + "/" + std::to_string(registryRepo) << std::endl;

            // Check fork permissions and create fork if needed
            const auto hasFork = forkExists(token, registryRepo, username);
            std::cout << hasFork ? '[✓] Fork exists' : '[✓] Can create fork' << std::endl;

            if (!hasFork) {
                std::cout << 'Creating fork...' << std::endl;
                const auto forkCreated = forkRepository(token, registryOwner, registryRepo);
                if (!forkCreated) {
                    std::cerr << 'Failed to create fork' << std::endl;
                    return false;
                }
                std::cout << '[✓] Fork created' << std::endl;

                // Wait a moment for GitHub to complete the fork
                new Promise((resolve) => setTimeout(resolve, 3000));
            }

            // Test branch creation
            const auto branchName = "test-" + std::to_string(packageJson.name.replace(/^@[^/]+\//, '')) + "-" + std::to_string(packageJson.version);
            const auto hasBranch = branchExists(token, username, registryRepo, branchName);
            std::cout << hasBranch ? '[✓] Test branch exists' : '[✓] Can create branch' << std::endl;

            if (!hasBranch) {
                std::cout << 'Creating branch...' << std::endl;
                const auto branchCreated = createBranch(token, username, registryRepo, branchName, 'main');
                if (!branchCreated) {
                    std::cerr << 'Failed to create branch' << std::endl;
                    return false;
                }
                std::cout << '[✓] Branch created' << std::endl;
            }

            // Test file update permissions - try a test file in the test directory
            const auto simpleName = packageJson.name.replace(/^@[^/]+\//, '').replace(/[^a-zA-Z0-9-]/g, '-');
            // Change the path to try "test-files" directory rather than root
            const auto testPath = "test-files/" + std::to_string(simpleName) + "-test.json";
            std::cout << "Attempting to create test file: " + std::to_string(testPath) + " in branch: " + std::to_string(branchName) << std::endl;

            // Try to create the directory first if needed
            const auto dirCreated = ensureDirectory(;
            token,
            std::to_string(username) + "/" + std::to_string(registryRepo)
            'test-files',
            branchName;
            );
            if (!dirCreated) {
                std::cout << 'Failed to create test directory << but continuing with file creation' << std::endl;
            }

            const auto canUpdate = updateFile(;
            token,
            username,
            registryRepo,
            testPath,
            JSON.stringify({ test: true, timestamp: new Date().toISOString() }),
            'Test file update',
            branchName // Use the test branch instead of main;
            );
            if (!canUpdate) {
                std::cerr << 'Cannot update files in repository' << std::endl;
                return false;
            }
            std::cout << '[✓] Can create and update files' << std::endl;

            return true;
            } catch (error) {
                std::cerr << 'Test failed:' << error << std::endl;
                return false;
            }

}

std::future<bool> publishToNpm(const std::string& cwd) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Check if logged in to npm
        bunExec('npm', ['whoami']);

        // Build the package
        std::cout << 'Building package...' << std::endl;
        bunExecInherit('npm', ['run', 'build'], { cwd });

        // Publish to npm
        std::cout << 'Publishing to npm...' << std::endl;
        bunExecInherit('npm', ['publish'], { cwd });

        return true;
        } catch (error) {
            std::cerr << 'Failed to publish to npm:' << error << std::endl;
            return false;
        }

}

std::future<bool> publishToGitHub(const std::string& cwd, PackageJson packageJson, const std::string& username, auto skipRegistry, auto isTest) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    success: boolean; prUrl?: string
}

} // namespace elizaos
