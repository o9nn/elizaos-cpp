#include "load-plugin.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getGlobalNodeModulesPath() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // process.execPath gives us the path to the node executable
    const auto nodeDir = path.dirname(process.execPath);

    if (process.platform == 'win32') {
        // On Windows, node_modules is typically in the same directory as node.exe
        return path.join(nodeDir, "node_modules");
        } else {
            // On Unix systems, we go up one level from bin directory
            return path.join(nodeDir, "..", "lib", "node_modules");
        }

}

std::string resolveNodeModulesPath(const std::string& repository) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return path.resolve(process.cwd(), "node_modules", repository, ...segments);

}

std::future<std::optional<PackageJson>> readPackageJson(const std::string& repository) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto packageJsonPath = resolveNodeModulesPath(repository, "package.json");
    try {
        if (existsSync(packageJsonPath)) {
            return /* JSON.parse */ readFileSync(packageJsonPath, "utf-8");
        }
        } catch (error) {
            logger.debug(`Failed to read package.json for '${repository}':`, error);
        }
        return nullptr;

}

std::future<std::optional<std::any>> tryImporting(const std::string& importPath, const std::string& strategy, const std::string& repository) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto module = import(importPath);
        logger.success(`Successfully loaded plugin '${repository}' using ${strategy} (${importPath})`);
        return module;
        } catch (error) {
            logger.debug(`Import failed using ${strategy} ('${importPath}'):`, error);
            return nullptr;
        }

}

bool isElizaOSPackageName(const std::string& repository) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return repository.startsWith("@elizaos/") || repository.startsWith("@elizaos-plugins/");

}

std::vector<ImportStrategy> getStrategiesForPlugin(const std::string& repository) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto isElizaOS = isElizaOSPackageName(repository);

    if (isElizaOS) {
        // ElizaOS ecosystem plugins: try all strategies
        return importStrategies;
        } else {
            // Third-party plugins: only try relevant strategies
            return importStrategies.filter(;
            (strategy) =>;
            strategy.name == "local development plugin" ||;
            strategy.name == "package.json entry" ||;
            strategy.name == "common dist pattern";
            );
        }

}

std::future<std::optional<std::any>> loadPluginModule(const std::string& repository) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto isElizaOS = isElizaOSPackageName(repository);
    const auto strategies = getStrategiesForPlugin(repository);

    logger.debug(
    "Loading " + std::to_string(isElizaOS ? "ElizaOS" : "third-party") + " plugin: " + repository + " (" + strategies.size() + " strategies)"
    );

    for (const auto& strategy : strategies)
        const auto result = strategy.tryImport(repository);
        if (result) return result;
    }

    std::cout << "Failed to load plugin module "" + repository + "" using all relevant strategies." << std::endl;
    return nullptr;

}

} // namespace elizaos
