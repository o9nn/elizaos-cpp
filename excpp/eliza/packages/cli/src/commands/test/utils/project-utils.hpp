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
;

/**
 * Determines the project type using comprehensive directory detection
 */


// Note: findMonorepoRoot() has been removed - use UserEnvironment.getInstance().findMonorepoRoot() instead
// This centralized version looks for packages/core directory instead of lerna.json for better reliability

/**
 * Process filter name to remove extensions consistently
 *
 * Note: Test filtering works in two ways:
 * 1. Matching test suite names (the string in describe() blocks)
 * 2. Matching file names (without extension)
 *
 * For best results, use the specific test suite name you want to run.
 * The filter is applied case-insensitively for better user experience.
 */
 else if (baseName.endsWith('.test') || baseName.endsWith('.spec')) {
    baseName = baseName.slice(0, -5); // Remove '.test' / '.spec'
  }

  return baseName;
}

/**
 * Install plugin dependencies for testing
 */
std::future<void> installPluginDependencies(DirectoryInfo projectInfo);

  const project = await loadProject(process.cwd());
  if (
    project.isPlugin &&
    project.pluginModule?.dependencies &&
    project.pluginModule.dependencies.length > 0
  ) {
    const pluginsDir = path.join(process.cwd(), '.eliza', 'plugins');
    if (!fs.existsSync(pluginsDir)) {
      await fs.promises.mkdir(pluginsDir, { recursive: true });
    }

    const packageJsonPath = path.join(pluginsDir, 'package.json');
    if (!fs.existsSync(packageJsonPath)) {
      const packageJsonContent = {
        name: 'test-plugin-dependencies',
        version: '1.0.0',
        description: 'A temporary package for installing test plugin dependencies',
        dependencies: {},
      };
      await fs.promises.writeFile(packageJsonPath, JSON.stringify(packageJsonContent, null, 2));
    }

    const { installPlugin } = await import('@/src/utils');

    for (const dependency of project.pluginModule.dependencies) {
      await installPlugin(dependency, pluginsDir);
      const dependencyPath = path.join(pluginsDir, 'node_modules', dependency);
      if (fs.existsSync(dependencyPath)) {
        try {
          await runBunCommand(['install'], dependencyPath);
        } catch (error) {
          logger.warn(
            `[Test Command] Failed to install devDependencies for ${dependency}: ${error}`
          );
        }
      }
    }
  }
}

} // namespace elizaos
