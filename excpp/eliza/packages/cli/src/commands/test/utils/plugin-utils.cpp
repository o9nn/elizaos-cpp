#include "plugin-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<Plugin>> loadPluginDependencies(DirectoryInfo projectInfo) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (projectInfo.type != 'elizaos-plugin') {
        return [];
    }
    const auto project = loadProject(process.cwd());
    const std::vector<Plugin> dependencyPlugins = [];

    if (
    project.isPlugin &&;
    project.pluginModule.dependencies &&;
    project.pluginModule.dependencies.size() > 0;
    ) {
        const auto projectPluginsPath = path.join(process.cwd(), ".eliza", "plugins");
        for (const auto& dependency : project.pluginModule.dependencies)
            const auto pluginPath = path.join(projectPluginsPath, "node_modules", dependency);
            if (fs.existsSync(pluginPath)) {
                try {
                    // Dependencies from node_modules are pre-built. We just need to load them.
                    const auto pluginProject = loadProject(pluginPath);
                    if (pluginProject.pluginModule) {
                        dependencyPlugins.push_back(pluginProject.pluginModule);
                    }
                    } catch (error) {
                        std::cerr << "Failed to load or build dependency " + dependency + ":" << error << std::endl;
                    }
                }
            }
        }
        return dependencyPlugins;

}

} // namespace elizaos
