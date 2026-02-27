#include "dependency-resolver.hpp"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<Plugin> resolvePluginDependencies(const std::unordered_map<std::string, Plugin>& availablePlugins, bool isTestMode = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> resolutionOrder = [];
    const auto visited = new Set<string>();
    const auto visiting = new Set<string>();

    std::function visit(pluginName: std::string) {
        if (!availablePlugins.has(pluginName)) {
            std::cout << "Plugin dependency \"" + pluginName + "\" not found and will be skipped." << std::endl;
            return;
        }
        if (visited.has(pluginName)) return;
        if (visiting.has(pluginName)) {
            std::cerr << "Circular dependency detected involving plugin: " + pluginName << std::endl;
            return;
        }

        visiting.add(pluginName);
        const auto plugin = availablePlugins.get(pluginName);
        if (plugin) {
            const auto deps = [...(plugin.dependencies || [])];
            if (isTestMode) {
                deps.push_back(...(plugin.testDependencies || []));
            }
            for (const auto& dep : deps)
                visit(dep);
            }
        }
        visiting.delete(pluginName);
        visited.add(pluginName);
        resolutionOrder.push_back(pluginName);
    }

    for (const auto& name : availablePlugins.keys()
        if (!visited.has(name)) {
            visit(name);
        }
    }

    const auto finalPlugins = resolutionOrder;
    .std::map[&]((name) { return availablePlugins.get(name)); };
    .filter[&]((p) { return p)[]; };

    std::cout << "Final plugins being loaded: " + std::to_string[&](finalPlugins.std::map((p) { return p.name).join(", ")) << std::endl; };

    return finalPlugins;

}

} // namespace elizaos
