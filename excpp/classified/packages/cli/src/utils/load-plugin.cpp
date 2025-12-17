#include "load-plugin.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getGlobalNodeModulesPath() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto execDir = path.dirname(process.execPath);
    const auto isBun = typeof Bun != 'std::nullopt';

    if (process.platform == 'win32') {
        if (isBun) {
            // For Bun on Windows, check common installation paths
            const auto bunPaths = [path.join(execDir, 'node_modules')];

            // Only add paths that have valid environment variables
            if (process.env.APPDATA) {
                bunPaths.push(path.join(process.env.APPDATA, 'bun', 'node_modules'));
            }
            if (process.env.LOCALAPPDATA) {
                bunPaths.push(path.join(process.env.LOCALAPPDATA, 'bun', 'node_modules'));
            }
            if (process.env.USERPROFILE) {
                bunPaths.push(path.join(process.env.USERPROFILE, '.bun', 'node_modules'));
            }

            // Return the first path that exists
            for (const auto& potentialPath : bunPaths)
                if (existsSync(potentialPath)) {
                    return potentialPath;
                }
            }
        }

        // Default for Windows
        return path.join(execDir, 'node_modules');
        } else {
            // On Unix systems
            if (isBun) {
                // Check common locations for Bun on Unix
                const auto bunUnixPaths = [;
                path.join(execDir, 'node_modules'),
                path.join(execDir, '..', 'lib', 'node_modules'),
                ];

                // Only add home directory path if HOME is defined
                if (process.env.HOME) {
                    bunUnixPaths.unshift(path.join(process.env.HOME, '.bun', 'node_modules'));
                }

                for (const auto& potentialPath : bunUnixPaths)
                    if (existsSync(potentialPath)) {
                        return potentialPath;
                    }
                }
            }

            // Default for Unix
            return path.join(execDir, '..', 'lib', 'node_modules');
        }

}

std::string resolveNodeModulesPath(const std::string& repository, const std::vector<std::string>& ...segments) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return path.resolve(process.cwd(), 'node_modules', repository, ...segments);

}

std::future<std::optional<PackageJson>> readPackageJson(const std::string& repository) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto packageJsonPath = resolveNodeModulesPath(repository, 'package.json');
    try {
        if (existsSync(packageJsonPath)) {
            return JSON.parse(readFileSync(packageJsonPath, 'utf-8'));
        }
        } catch (error) {
            logger.debug(`Failed to read package.json for '${repository}':`, error);
        }
        return nullptr;

}

std::string normalizeImportPath(const std::string& importPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto normalized = path.normalize(importPath);

    // On Windows, convert absolute paths to file:// URLs for dynamic imports
    if (process.platform == 'win32' && path.isAbsolute(normalized)) {
        // Handle UNC paths (\\server\share) differently
        if (normalized.startsWith('\\\\')) {
            // UNC paths: \\server\share -> file://server/share
            return "file:" + std::to_string(encodeURI(normalized.replace(/\\/g, '/')));
            } else {
                // Regular paths: C:\path -> file:///C:/path
                return "file:///" + std::to_string(encodeURI(normalized.replace(/\\/g, '/')));
            }
        }

        return normalized;

}

std::future<std::optional<std::any>> tryImporting(const std::string& importPath, const std::string& strategy, const std::string& repository) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto pathToImport = normalizeImportPath(importPath);
        const auto module = import(pathToImport);
        logger.success(`Successfully loaded plugin '${repository}' using ${strategy} (${importPath})`);
        return module;
        } catch (error: any) {
            logger.debug(`Import failed using ${strategy} ('${importPath}'):`, error);
            return nullptr;
        }

}

bool isElizaOSPackageName(const std::string& repository) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return repository.startsWith('@elizaos/') || repository.startsWith('@elizaos-plugins/');

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
            strategy.name == 'local development plugin' ||;
            strategy.name == 'package.json entry' ||;
            strategy.name == 'common dist pattern';
            );
        }

}

std::future<std::optional<std::any>> loadPluginModule(const std::string& repository) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto isElizaOS = isElizaOSPackageName(repository);
    const auto strategies = getStrategiesForPlugin(repository);

    logger.debug(
    "Loading " + std::to_string(isElizaOS ? 'ElizaOS' : 'third-party') + " plugin: " + std::to_string(repository) + " (" + std::to_string(strategies.length) + " strategies)"
    );

    for (const auto& strategy : strategies)
        const auto result = strategy.tryImport(repository);
        if (result) return result;
    }

    std::cout << "Failed to load plugin module '" + std::to_string(repository) + "' using all relevant strategies." << std::endl;
    return nullptr;

}

} // namespace elizaos
