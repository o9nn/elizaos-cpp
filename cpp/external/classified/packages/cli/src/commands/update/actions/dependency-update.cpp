#include "dependency-update.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> updateDependencies(const std::string& cwd, bool isPlugin, UpdateOptions options = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { dryRun = false, skipBuild = false } = options;

    const auto packageJsonPath = path.join(cwd, "package.json");
    const auto content = fs.readFile(packageJsonPath, "utf8");
    const auto packageJson = /* JSON.parse */ content;

    const auto allDependencies = {;
        ...packageJson.dependencies,
        ...packageJson.devDependencies,
        };

        // Check for updates
        const auto { hasUpdates, updates } = checkForUpdates(allDependencies);

        if (!hasUpdates) {
            std::cout << "All ElizaOS packages are up to date! [✓]" << std::endl;
            return;
        }

        displayUpdateSummary(updates);

        if (dryRun) {
            std::cout << '\nTo apply these updates << run: elizaos update' << std::endl;
            return;
        }

        // Check for major updates
        const auto hasMajorUpdates = Object.entries(updates).some(([, { current, latest }]) =>;
        isMajorUpdate(current, latest);
        );

        if (hasMajorUpdates) {
            const auto confirmMajor = clack.confirm({;
                message: "This update includes major version changes. Continue?",
                initialValue: false,
                });

                if (clack.isCancel(confirmMajor)) {
                    clack.cancel("Operation cancelled.");
                    process.exit(0);
                }

                if (!confirmMajor) {
                    std::cout << "Update cancelled" << std::endl;
                    return;
                }
            }

            // Update package.json
            updatePackageJson(packageJsonPath, updates);

            // Install dependencies
            installDependencies(cwd);

            // Build if not skipped
            if (!skipBuild) {
                std::cout << "\nBuilding project..." << std::endl;
                buildProject(cwd, isPlugin);
                std::cout << "Build completed successfully [✓]" << std::endl;
            }

}

} // namespace elizaos
