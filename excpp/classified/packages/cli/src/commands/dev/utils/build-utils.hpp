#include ".types.hpp"
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
 * Build a single package
 */
std::future<BuildResult> buildPackage(const std::string& packagePath, bool isPlugin);;
  } catch (error) {
    const duration = Date.now() - startTime;
    return { success: false, duration, error: error as Error };
  }
}

/**
 * Build core packages in a monorepo
 */
std::future<std::vector<BuildResult>> buildCorePackages(const std::string& monorepoRoot);,
    {
      name: 'client',
      path: path.join(monorepoRoot, 'packages', 'client'),
      isPlugin: false,
    },
    {
      name: 'plugin-bootstrap',
      path: path.join(monorepoRoot, 'packages', 'plugin-bootstrap'),
      isPlugin: true,
    },
  ];

  const results: BuildResult[] = [];

  for (const pkg of corePackages) {
    console.info(`Building ${pkg.name}...`);
    const result = await buildPackage(pkg.path, pkg.isPlugin);

    if (!result.success) {
      console.error(`Error building ${pkg.name}: ${result.error?.message}`);
    } else {
      console.info(`✓ Built ${pkg.name} (${result.duration}ms)`);
    }

    results.push(result);
  }

  return results;
}

/**
 * Perform a full rebuild based on the development context
 *
 * Handles building in different contexts: monorepo, project, or plugin.
 */
std::future<void> performRebuild(DevContext context); = context;
  const isPlugin = directoryType.type === 'elizaos-plugin';
  const isMonorepo = directoryType.type === 'elizaos-monorepo';

  if (isMonorepo || directoryType.monorepoRoot) {
    const { monorepoRoot } = await UserEnvironment.getInstance().getPathInfo();
    if (monorepoRoot) {
      await buildCorePackages(monorepoRoot);
    } else {
      console.warn('Monorepo context detected, but failed to find monorepo root.');
    }
  }

  // Build the current project/plugin
  const result = await buildPackage(directory, isPlugin);

  if (result.success) {
    console.log(`✓ Rebuild successful (${result.duration}ms)`);
  } else {
    console.error(`Build failed: ${result.error?.message}`);
    throw result.error;
  }
}

/**
 * Perform initial build setup
 */
std::future<void> performInitialBuild(DevContext context); = context;
  const isPlugin = directoryType.type === 'elizaos-plugin';
  const isMonorepo = directoryType.type === 'elizaos-monorepo';

  if (process.env.ELIZA_TEST_MODE) {
    console.info('Skipping initial build in test mode');
    return;
  }

  // Ensure initial build is performed (skip for monorepo as it may have multiple projects)
  if (!isMonorepo) {
    console.info('Building project...');
    try {
      await buildProject(directory, isPlugin);
      console.info('✓ Initial build completed');
    } catch (error) {
      console.error(
        `Initial build failed: ${error instanceof Error ? error.message : String(error)}`
      );
      console.info('Continuing with dev mode anyway...');
    }
  } else {
    console.info(
      'Monorepo detected - skipping automatic build. Use specific package build commands as needed.'
    );
  }
}

/**
 * Create development context from current working directory
 */


  const srcDir = path.join(cwd, 'src');

  return {
    directory: cwd,
    directoryType,
    watchDirectory: existsSync(srcDir) ? srcDir : cwd,
    buildRequired: directoryType.type !== 'elizaos-monorepo',
  };
}

} // namespace elizaos
