#include ".utils/directory.hpp"
#include ".utils/naming.hpp"
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
;
;
// Character updater imports removed - reverting to project-scoped plugins

/**
 * Remove a plugin from the project
 */
std::future<void> removePlugin(const std::string& plugin);`
    );
    process.exit(1);
  }

  const allDependencies = getDependenciesFromDirectory(cwd);
  if (!allDependencies) {
    console.error(
      'Could not read dependencies from package.json. Cannot determine which package to remove.'
    );
    process.exit(1);
  }

  const packageNameToRemove = findPluginPackageName(plugin, allDependencies);

  if (!packageNameToRemove) {
    logger.warn(`Plugin matching "${plugin}" not found in project dependencies.`);
    console.info('\nCheck installed plugins using: elizaos plugins installed-plugins');
    process.exit(0);
  }

  console.info(`Removing ${packageNameToRemove}...`);
  try {
    await execa('bun', ['remove', packageNameToRemove], {
      cwd,
      stdio: 'inherit',
    });
  } catch (execError) {
    logger.error(
      `Failed to run 'bun remove ${packageNameToRemove}': ${execError instanceof Error ? execError.message : String(execError)}`
    );
    if (
      execError &&
      typeof execError === 'object' &&
      'stderr' in execError &&
      typeof execError.stderr === 'string' &&
      execError.stderr.includes('not found')
    ) {
      logger.info(
        `'bun remove' indicated package was not found. Continuing with directory removal attempt.`
      );
    } else {
      handleError(execError);
      process.exit(1);
    }
  }

  // Remove plugin directory if it exists
  let baseName = packageNameToRemove;
  if (packageNameToRemove.includes('/')) {
    const parts = packageNameToRemove.split('/');
    baseName = parts[parts.length - 1];
  }
  baseName = baseName.replace(/^plugin-/, '');
  const dirNameToRemove = `plugin-${baseName}`;

  const pluginDir = path.join(cwd, dirNameToRemove);
  if (existsSync(pluginDir)) {
    try {
      rmSync(pluginDir, { recursive: true, force: true });
    } catch (rmError) {
      logger.error(
        `Failed to remove directory ${pluginDir}: ${rmError instanceof Error ? rmError.message : String(rmError)}`
      );
    }
  } else {
    const nonPrefixedDir = path.join(cwd, baseName);
    if (existsSync(nonPrefixedDir)) {
      try {
        rmSync(nonPrefixedDir, { recursive: true, force: true });
      } catch (rmError) {
        logger.error(
          `Failed to remove directory ${nonPrefixedDir}: ${rmError instanceof Error ? rmError.message : String(rmError)}`
        );
      }
    }
  }

  console.log(`Successfully removed ${packageNameToRemove}`);
}

} // namespace elizaos
