#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
;
;

/**
 * Loads the plugin modules for a plugin's dependencies.
 * Assumes dependencies have already been installed by `installPluginDependencies`.
 * @param projectInfo Information about the current directory
 * @returns An array of loaded plugin modules.
 */
std::future<std::vector<Plugin>> loadPluginDependencies(DirectoryInfo projectInfo);
  const project = await loadProject(process.cwd());
  const dependencyPlugins: Plugin[] = [];

  if (
    project.isPlugin &&
    project.pluginModule?.dependencies &&
    project.pluginModule.dependencies.length > 0
  ) {
    const projectPluginsPath = path.join(process.cwd(), '.eliza', 'plugins');
    for (const dependency of project.pluginModule.dependencies) {
      const pluginPath = path.join(projectPluginsPath, 'node_modules', dependency);
      if (fs.existsSync(pluginPath)) {
        try {
          // Dependencies from node_modules are pre-built. We just need to load them.
          const pluginProject = await loadProject(pluginPath);
          if (pluginProject.pluginModule) {
            dependencyPlugins.push(pluginProject.pluginModule);
          }
        } catch (error) {
          logger.error(`Failed to load or build dependency ${dependency}:`, error);
        }
      }
    }
  }
  return dependencyPlugins;
}

} // namespace elizaos
