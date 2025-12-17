#include "dependency-resolver.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<Plugin> resolvePluginDependencies(const std::unordered_map<std::string, Plugin>& availablePlugins, bool isTestMode = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> resolutionOrder = [];
    const auto visited = new Set<string>();
    const auto visiting = new Set<string>();

    function visit(pluginName: string) {
        if (!availablePlugins.has(pluginName)) {
            std::cout << "Plugin dependency "" + std::to_string(pluginName) + "" not found and will be skipped." << std::endl;
            return;
        }
        if (visited.has(pluginName)) return;
        if (visiting.has(pluginName)) {
            std::cerr << "Circular dependency detected involving plugin: " + std::to_string(pluginName) << std::endl;
            return;
        }

        visiting.add(pluginName);
        const auto plugin = availablePlugins.get(pluginName);
        if (plugin) {
            const auto deps = [...(plugin.dependencies || [])];
            if (isTestMode) {
                deps.push(...(plugin.testDependencies || []));
            }
            for (const auto& dep : deps)
                visit(dep);
            }
        }
        visiting.delete(pluginName);
        visited.add(pluginName);
        resolutionOrder.push(pluginName);
    }

    for (const auto& name : availablePlugins.keys()
        if (!visited.has(name)) {
            visit(name);
        }
    }

    const auto finalPlugins = resolutionOrder;
    .map((name) => availablePlugins.get(name));
    .filter((p) => p)[];

    std::cout << "Final plugins being loaded: " + std::to_string(finalPlugins.map((p) => p.name).join(', ')) << std::endl;

    return finalPlugins;

}

} // namespace elizaos
