#include "build-project.hpp"
#include "directory-detection.hpp"
#include "elizaos/core.hpp"
#include "registry.hpp"
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

struct PackageInfo {
    std::string name;
    std::optional<std::string> main;
    std::optional<std::unordered_map<std::string, std::string>> scripts;
};


struct PluginContext {
    bool isLocalDevelopment;
    std::optional<std::string> localPath;
    std::optional<PackageInfo> packageInfo;
    std::optional<bool> needsBuild;
};


/**
 * Normalizes plugin names for comparison by removing common prefixes and scopes
 */


/**
 * Detects if the current directory is the same plugin being requested
 * and provides context about local development status
 */
;
  }

  // Get package info from directory detection result
  const packageJsonPath = path.join(cwd, 'package.json');
  let packageInfo: PackageInfo;
  try {
    packageInfo = JSON.parse(readFileSync(packageJsonPath, 'utf-8'));
  } catch (error) {
    logger.debug(`Failed to parse package.json: ${error}`);
    return { isLocalDevelopment: false };
  }

  // Check if the requested plugin matches the current package
  const normalizedRequestedPlugin = normalizeForComparison(pluginName);
  const normalizedCurrentPackage = normalizeForComparison(packageInfo.name);

  // Also check directory name as fallback
  const dirName = path.basename(cwd);
  const normalizedDirName = normalizeForComparison(dirName);

  const isCurrentPlugin =
    normalizedRequestedPlugin === normalizedCurrentPackage ||
    normalizedRequestedPlugin === normalizedDirName;

  if (isCurrentPlugin) {
    const mainEntry = packageInfo.main || 'dist/index.js';
    const localPath = path.resolve(cwd, mainEntry);
    const needsBuild = !existsSync(localPath);

    logger.debug(`Detected local plugin development: ${pluginName}`);
    logger.debug(`Expected output: ${localPath}`);
    logger.debug(`Needs build: ${needsBuild}`);

    return {
      isLocalDevelopment: true,
      localPath,
      packageInfo,
      needsBuild,
    };
  }

  return { isLocalDevelopment: false };
}

/**
 * Ensures a local plugin is built before attempting to load it
 */
std::future<bool> ensurePluginBuilt(PluginContext context);

  const { packageInfo, localPath } = context;

  // Check if build script exists
  if (packageInfo.scripts?.build) {
    logger.info('Plugin not built, attempting to build...');
    try {
      await buildProject(process.cwd(), true);

      // Verify the build created the expected output
      if (localPath && existsSync(localPath)) {
        logger.success('Plugin built successfully');
        return true;
      } else {
        logger.error(`Build completed but expected output not found: ${localPath}`);
        return false;
      }
    } catch (error) {
      logger.error(`Build failed: ${error}`);
      return false;
    }
  }

  logger.error(`Plugin not built and no build script found in package.json`);
  logger.info(`Add a "build" script to package.json or run 'bun run build' manually`);
  return false;
}

/**
 * Provides helpful guidance when local plugin loading fails
 */


  logger.info(`\nLocal plugin development detected for: ${pluginName}`);

  if (context.needsBuild) {
    logger.info('To fix this issue:');
    logger.info('1. Build the plugin: bun run build');
    logger.info('2. Verify the output exists at: ' + context.localPath);
    logger.info('3. Re-run the test command');
  } else {
    logger.info('Plugin appears to be built but failed to load.');
    logger.info('Try rebuilding: bun run build');
  }

  logger.info('\nFor more information, see the plugin development guide.');
}

} // namespace elizaos
