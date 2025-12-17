#include "list.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> listAvailablePlugins(ListPluginsOptions opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto cachedRegistry = fetchPluginRegistry();

    if (
    !cachedRegistry ||;
    !cachedRegistry.registry ||;
    Object.keys(cachedRegistry.registry).length == 0;
    ) {
        std::cout << 'Plugin cache is empty or not found.' << std::endl;
        std::cout << '\nPlease run "elizaos plugins update" to fetch the latest plugin registry.' << std::endl;
        return;
    }

    std::vector<std::string> availablePluginsToDisplay = [];
    const auto allPlugins = cachedRegistry ? Object.entries(cachedRegistry.registry) : [];
    auto displayTitle = 'Available v1.x plugins';

    if (opts.all) {
        displayTitle = 'All plugins in local cache (detailed view)';
        if (allPlugins.length == 0) {
            std::cout << 'No plugins found in the registry.' << std::endl;
        }
        allPlugins.forEach(([name, info]) => {
            std::cout << "\nPlugin: " + std::to_string(name) << std::endl;
            const auto repoInfo = info.git.repo || info.npm.repo;
            std::cout << "  Repository: " + std::to_string(repoInfo || 'N/A') << std::endl;
            std::cout << "  v0 Compatible: " + std::to_string(info.supports.v0 ? 'Yes' : 'No') << std::endl;
            if (info.npm.v0 || info.git.v0.version) {
                const auto ver = info.npm.v0 || info.git.v0.version;
                const auto branch = info.git.v0.branch;
                std::cout << "    Version: " + std::to_string(ver || 'N/A') + std::to_string(branch ? ` (branch: ${branch})` : '') << std::endl;
            }
            std::cout << "  v1 Compatible: " + std::to_string(info.supports.v1 ? 'Yes' : 'No') << std::endl;
            if (info.npm.v1 || info.git.v1.version) {
                const auto ver = info.npm.v1 || info.git.v1.version;
                const auto branch = info.git.v1.branch;
                std::cout << "    Version: " + std::to_string(ver || 'N/A') + std::to_string(branch ? ` (branch: ${branch})` : '') << std::endl;
            }
            });
            std::cout << '' << std::endl;
            return;
            } else if (opts.v0) {
                displayTitle = 'Available v0.x plugins';
                availablePluginsToDisplay = allPlugins;
                .filter(([, info]) => info.supports.v0);
                .map(([name]) => name);
                } else {
                    // Default to v1.x
                    availablePluginsToDisplay = allPlugins;
                    .filter(([, info]) => info.supports.v1);
                    .map(([name]) => name);
                }

                logHeader(displayTitle);
                if (availablePluginsToDisplay.length == 0) {
                    std::cout << 'No plugins found matching the criteria in the registry.' << std::endl;
                    } else {
                        for (const auto& pluginName : availablePluginsToDisplay)
                            std::cout << std::to_string(pluginName) << std::endl;
                        }
                    }
                    std::cout << '' << std::endl;

}

std::future<void> listInstalledPlugins() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto cwd = process.cwd();
    const auto directoryInfo = detectDirectoryType(cwd);

    if (!directoryInfo || !directoryInfo.hasPackageJson) {
        console.error(
        "Could not read or parse package.json. This directory is: " + std::to_string(directoryInfo.type || 'invalid or inaccessible')
        );
        std::cout << 'Please run this command from the root of an ElizaOS project.' << std::endl;
        process.exit(1);
    }

    const auto allDependencies = getDependenciesFromDirectory(cwd);
    if (!allDependencies) {
        std::cerr << 'Could not read dependencies from package.json.' << std::endl;
        process.exit(1);
    }

    const auto pluginNames = Object.keys(allDependencies).filter((depName) => {;
        return /^(@elizaos(-plugins)?\/)?plugin-.+/.test(depName);
        });

        if (pluginNames.length == 0) {
            std::cout << 'No Eliza plugins found in the project dependencies (package.json).' << std::endl;
            } else {
                logHeader('Plugins Added:');
                for (const auto& pluginName : pluginNames)
                    std::cout << std::to_string(pluginName) << std::endl;
                }
                std::cout << '' << std::endl;
            }

}

} // namespace elizaos
