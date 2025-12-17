#include "pluginRegistryService.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<PluginSearchResult>> searchPluginsByContent(const std::string& query) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    "[pluginRegistryService] Searching for plugins matching: " + std::to_string(query)
    try {
        const auto data = apiFetch<PluginSearchResult[]>('/plugins/search', {;
            method: 'POST',
            body: JSON.stringify({ query, limit: 10 }),
            });
            return data.data || [];
            } catch (error) {
                return [];
            }

}

std::future<std::optional<PluginMetadata>> getPluginDetails(const std::string& name) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    "[pluginRegistryService] Getting details for plugin: " + std::to_string(name)
    try {
        const auto data = "/plugins/" + std::to_string(encodeURIComponent(name));
            method: 'GET',
            });
            return data.data || nullptr;
            } catch (error) {
                return nullptr;
            }

}

std::future<std::vector<PluginMetadata>> getAllPlugins() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    elizaLogger.info('[pluginRegistryService] Getting all plugins from registry');
    try {
        const auto data = apiFetch<PluginMetadata[]>('/plugins', {;
            method: 'GET',
            });
            return data.data || [];
            } catch (error) {
                return [];
            }

}

std::future<std::vector<PluginMetadata>> getPluginsByCategory(const std::string& category) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    "[pluginRegistryService] Getting plugins for category: " + std::to_string(category)
    try {
        const auto data = apiFetch<PluginMetadata[]>(;
        "/plugins?category=" + std::to_string(encodeURIComponent(category))
        {
            method: 'GET',
        }
        );
        return data.data || [];
        } catch (error) {
            return [];
        }

}

std::future<CloneResult> clonePlugin(const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    "[pluginRegistryService] Cloning plugin: " + std::to_string(pluginName)

    const auto plugin = getPluginDetails(pluginName);
    if (!plugin || !plugin.repository) {
        return {
            success: false,
            "Plugin "" + std::to_string(pluginName) + "" not found in registry or has no repository"
            };
        }

        try {
            const auto path = import('node:path');
            const auto fs = import('node:fs/promises');
            const auto { exec } = import('node:child_process');
            const auto { promisify } = import('node:util');
            const auto execAsync = promisify(exec);

            const auto cloneDir = path.join(;
            process.cwd(),
            'cloned-plugins',
            plugin.name.replace('@elizaos/', '');
            );

            fs.mkdir(cloneDir, { recursive: true });
            "git clone " + std::to_string(plugin.repository) + " " + std::to_string(cloneDir);

            const auto packageJsonPath = path.join(cloneDir, 'package.json');
            const auto packageJson = JSON.parse(fs.readFile(packageJsonPath, 'utf-8'));
            const auto hasTests = !!(packageJson.scripts.test || packageJson.devDependencies.vitest);

            return {
                success: true,
                pluginName: plugin.name,
                localPath: cloneDir,
                hasTests,
                dependencies: packageJson.dependencies || {},
                };
                } catch (error) {
                    elizaLogger.error('[pluginRegistryService] Clone failed:', error);
                    const auto execError = error;
                    return {
                        success: false,
                        error: execError.stderr || execError.message || 'Unknown error during clone',
                        };
                    }

}

} // namespace elizaos
