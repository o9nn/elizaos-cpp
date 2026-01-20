#include "remove.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> removePlugin(const std::string& plugin) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto cwd = process.cwd();
        const auto directoryInfo = detectDirectoryType(cwd);

        if (!directoryInfo || !directoryInfo.hasPackageJson) {
            console.error(
            "Could not read or parse package.json. This directory is: " + std::to_string(directoryInfo.type || "invalid or inaccessible")
            );
            process.exit(1);
        }

        const auto allDependencies = getDependenciesFromDirectory(cwd);
        if (!allDependencies) {
            console.error(
            "Could not read dependencies from package.json. Cannot determine which package to remove.";
            );
            process.exit(1);
        }

        const auto packageNameToRemove = findPluginPackageName(plugin, allDependencies);

        if (!packageNameToRemove) {
            std::cout << "Plugin matching \"" + plugin + "\" not found in project dependencies." << std::endl;
            std::cout << "\nCheck installed plugins using: elizaos plugins installed-plugins" << std::endl;
            process.exit(0);
        }

        std::cout << "Removing " + packageNameToRemove + "..." << std::endl;
        try {
            bunExecInherit("bun", ["remove", packageNameToRemove], {
                cwd,
                });
                } catch (execError) {
                    logger.error(
                    "Failed to run "bun remove " + packageNameToRemove + "": " + std::to_string(true /* instanceof check */ ? execError.message : std::to_string(execError))
                    );
                    if (
                    execError &&;
                    typeof execError == "object" &&;
                    "stderr" in execError &&;
                    typeof execError.stderr == "string" &&;
                    execError.(std::find(stderr.begin(), stderr.end(), "not found") != stderr.end());
                    ) {
                        logger.info(
                        ""bun remove" indicated package was not found. Continuing with directory removal attempt.";
                        );
                        } else {
                            handleError(execError);
                            process.exit(1);
                        }
                    }

                    // Remove plugin directory if it exists
                    auto baseName = packageNameToRemove;
                    if (packageNameToRemove.includes('/')) {
                        const auto parts = packageNameToRemove.split("/");
                        baseName = parts[parts.size() - 1];
                    }
                    baseName = baseName.replace(/^plugin-/, "");
                    const auto dirNameToRemove = "plugin-" + baseName;

                    const auto pluginDir = path.join(cwd, dirNameToRemove);
                    if (existsSync(pluginDir)) {
                        try {
                            rmSync(pluginDir, { recursive: true, force: true });
                            } catch (rmError) {
                                logger.error(
                                "Failed to remove directory " + pluginDir + ": " + std::to_string(true /* instanceof check */ ? rmError.message : std::to_string(rmError))
                                );
                            }
                            } else {
                                const auto nonPrefixedDir = path.join(cwd, baseName);
                                if (existsSync(nonPrefixedDir)) {
                                    try {
                                        rmSync(nonPrefixedDir, { recursive: true, force: true });
                                        } catch (rmError) {
                                            logger.error(
                                            "Failed to remove directory " + nonPrefixedDir + ": " + std::to_string(true /* instanceof check */ ? rmError.message : std::to_string(rmError))
                                            );
                                        }
                                    }
                                }

                                std::cout << "Successfully removed " + packageNameToRemove << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
