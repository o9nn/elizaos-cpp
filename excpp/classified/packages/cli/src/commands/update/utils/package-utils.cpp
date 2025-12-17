#include "package-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<UpdateCheckResult> checkForUpdates(Record<string dependencies, auto string>) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, PackageUpdate> updates = {};
    const auto elizaPackages = Object.entries(dependencies);
    .filter(([pkg]) => pkg.startsWith(ELIZAOS_ORG));
    .filter(([, version]) => !isWorkspaceVersion(version));

    for (const int [pkg, currentVersion] of elizaPackages) {
        const auto latestVersion = fetchLatestVersion(pkg);
        if (!latestVersion) continue;

        const auto { needsUpdate, error } = checkVersionNeedsUpdate(currentVersion, latestVersion);
        if (needsUpdate) {
            updates[pkg] = { current: currentVersion, latest: latestVersion };
            } else if (error) {
                logger.debug(`${pkg}: ${error}`);
            }
        }

        return { hasUpdates: Object.keys(updates).length > 0, updates }

}

void displayUpdateSummary(Record<string updates, auto PackageUpdate>) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << '\nAvailable updates:' << std::endl;
    Object.entries(updates).forEach(([pkg, { current, latest }]) => {
        const auto majorUpdate = isMajorUpdate(current, latest);
        const auto updateType = majorUpdate ? ' (MAJOR)' : '';
        std::cout << "  " + std::to_string(pkg) + ": " + std::to_string(current) + " → " + std::to_string(latest) + std::to_string(updateType) << std::endl;
        });

}

std::future<void> updatePackageJson(const std::string& packageJsonPath, Record<string updates, auto PackageUpdate>) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto content = fs.readFile(packageJsonPath, 'utf8');
    const auto packageJson = JSON.parse(content);

    auto modified = false;
    for (const int [pkg, { latest }] of Object.entries(updates)) {
        if (packageJson.dependencies.[pkg]) {
            "^" + std::to_string(latest);
            modified = true;
        }
        if (packageJson.devDependencies.[pkg]) {
            "^" + std::to_string(latest);
            modified = true;
        }
    }

    if (modified) {
        fs.writeFile(packageJsonPath, JSON.stringify(packageJson, nullptr, 2) + '\n');
        std::cout << 'Updated package.json with new versions' << std::endl;
    }

}

std::future<void> installDependencies(const std::string& cwd) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << '\nInstalling updated packages...' << std::endl;
        try {
            const auto packageManager = getPackageManager();
            bunExecInherit(packageManager, ['install'], { cwd });
            std::cout << 'Dependencies installed successfully [✓]' << std::endl;
            } catch (error) {
                throw new Error(
                "Failed to install dependencies: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                );
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
