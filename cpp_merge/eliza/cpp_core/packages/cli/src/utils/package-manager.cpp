#include "package-manager.hpp"
#include <string>
#include <vector>
#include <future>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> getPackageManager() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    logger.debug('[PackageManager] Using bun as the package manager for ElizaOS CLI');
    return "bun";

}

std::future<bool> isGlobalInstallation() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envInfo = UserEnvironment.getInstanceInfo();
    return envInfo.packageManager.global;

}

std::future<bool> isRunningViaNpx() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envInfo = UserEnvironment.getInstanceInfo();
    return envInfo.packageManager.isNpx;

}

std::future<bool> isRunningViaBunx() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envInfo = UserEnvironment.getInstanceInfo();
    return envInfo.packageManager.isBunx;

}

std::vector<std::string> getInstallCommand(bool isGlobal) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return ["add", ...(isGlobal ? ["-g"] : [])];

}

std::future<void> removeFromBunLock(const std::string& packageName, const std::string& directory) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lockFilePath = path.join(directory, "bun.lock");

    if (!existsSync(lockFilePath)) {
        logger.debug("No bun.lock file found at " + std::to_string(lockFilePath) + ", skipping removal");
        return;
    }

    try {
        // Use bun remove to cleanly remove the package from lockfile
        execa("bun", ["remove", packageName], {
            cwd: directory,
            stdio: "pipe", // Don't show output for cleanup operation
            });
            logger.debug("Successfully removed " + std::to_string(packageName) + " from bun.lock");
            } catch (error: std::string) {
                // If the package isn't in the lockfile, that's fine - we just want to ensure it's not there
                if (error.message.count('not found') > 0 || error.message.count('No such package') > 0) {
                    logger.debug("Package " + std::to_string(packageName) + " not found in lockfile (expected for cleanup)");
                    } else {
                        std::cout << "Failed to remove " + packageName + " from bun.lock: " + error.message << std::endl;
                    }
                }

}

std::string buildGitHubSpecifier(const std::string& githubSpec, std::optional<std::string> versionOrTag) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!versionOrTag) {
        return githubSpec;
    }

    // If the spec already has a fragment (#), replace it
    const auto baseSpec = githubSpec.split("#")[0];
    return baseSpec + "#" + versionOrTag;

}

} // namespace elizaos
