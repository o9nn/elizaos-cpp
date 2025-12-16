#include ".types.hpp"
#include ".utils/directory.hpp"
#include ".utils/env-vars.hpp"
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
;
// Character updater imports removed - reverting to project-scoped plugins

/**
 * Install a plugin from GitHub repository
 */
std::future<void> installPluginFromGitHub(const std::string& plugin, const std::string& cwd, AddPluginOptions opts);

  const [, owner, repo, ref] = githubMatch;
  const githubSpecifier = `github:${owner}/${repo}${ref ? `#${ref}` : ''}`;
  const pluginNameForPostInstall = repo;

  const success = await installPlugin(githubSpecifier, cwd, undefined, opts.skipVerification);

  if (success) {
    logger.info(`Successfully installed ${pluginNameForPostInstall} from ${githubSpecifier}.`);

    // Prompt for environment variables if not skipped
    if (!opts.skipEnvPrompt) {
      // Brief pause to ensure installation logs are complete
      await new Promise((resolve) => setTimeout(resolve, 50));
      const packageName = extractPackageName(plugin);
      console.log(`\n🔧 Checking environment variables for ${packageName}...`);
      try {
        await promptForPluginEnvVars(packageName, cwd);
      } catch (error) {
        logger.warn(
          `Warning: Could not prompt for environment variables: ${error instanceof Error ? error.message : String(error)}`
        );
        // Don't fail the installation if env prompting fails
      }
    } else {
      console.log(`\n⏭️  Skipping environment variable prompts due to --skip-env-prompt flag`);
    }

    process.exit(0);
  } else {
    logger.error(`Failed to install plugin from ${githubSpecifier}.`);
    process.exit(1);
  }
}

/**
 * Install a plugin from registry
 */
std::future<void> installPluginFromRegistry(const std::string& plugin, const std::string& cwd, AddPluginOptions opts);

  const possibleNames = normalizePluginName(plugin);
  const pluginKey = possibleNames.find((name) => cachedRegistry.registry[name]);

  const targetName = pluginKey || plugin;

  const registryInstallResult = await installPlugin(
    targetName,
    cwd,
    opts.tag,
    opts.skipVerification
  );

  if (registryInstallResult) {
    console.log(`Successfully installed ${targetName}`);

    // Prompt for environment variables if not skipped
    if (!opts.skipEnvPrompt) {
      // Brief pause to ensure installation logs are complete
      await new Promise((resolve) => setTimeout(resolve, 50));
      // Refresh dependencies after installation to find the actual installed package name
      const updatedDependencies = getDependenciesFromDirectory(cwd);
      const actualPackageName =
        findPluginPackageName(targetName, updatedDependencies || {}) || targetName;

      console.log(`\n🔧 Checking environment variables for ${actualPackageName}...`);
      try {
        await promptForPluginEnvVars(actualPackageName, cwd);
      } catch (error) {
        logger.warn(
          `Warning: Could not prompt for environment variables: ${error instanceof Error ? error.message : String(error)}`
        );
        // Don't fail the installation if env prompting fails
      }
    } else {
      console.log(`\n⏭️  Skipping environment variable prompts due to --skip-env-prompt flag`);
    }

    process.exit(0);
  }

  console.error(`Failed to install ${targetName} from registry.`);
  process.exit(1);
}

/**
 * Main plugin installation function
 */
std::future<void> addPlugin(const std::string& pluginArg, AddPluginOptions opts);

  const cwd = process.cwd();
  const directoryInfo = detectDirectoryType(cwd);

  if (!directoryInfo || !directoryInfo.hasPackageJson) {
    logger.error(
      `Command must be run inside an ElizaOS project directory. This directory is: ${directoryInfo?.type || 'invalid or inaccessible'}`
    );
    process.exit(1);
  }

  const allDependencies = getDependenciesFromDirectory(cwd);
  if (!allDependencies) {
    logger.error('Could not read dependencies from package.json');
    process.exit(1);
  }

  let plugin = pluginArg;

  // --- Convert full GitHub HTTPS URL to shorthand ---
  const httpsGitHubUrlRegex =
    // eslint-disable-next-line no-useless-escape
    /^https?:\/\/github\.com\/([a-zA-Z0-9_-]+)\/([a-zA-Z0-9_.-]+?)(?:\.git)?(?:(?:#|\/tree\/|\/commit\/)([a-zA-Z0-9_.-]+))?\/?$/;
  const httpsMatch = plugin.match(httpsGitHubUrlRegex);

  if (httpsMatch) {
    const [, owner, repo, ref] = httpsMatch;
    plugin = `github:${owner}/${repo}${ref ? `#${ref}` : ''}`;
  }

  const installedPluginName = findPluginPackageName(plugin, allDependencies);
  if (installedPluginName) {
    logger.info(`Plugin "${installedPluginName}" is already added to this project.`);
    process.exit(0);
  }

  // Check if it's a GitHub plugin
  const githubRegex = /^(?:github:)?([a-zA-Z0-9_-]+)\/([a-zA-Z0-9_.-]+)(?:#([a-zA-Z0-9_.-]+))?$/;
  if (githubRegex.test(plugin)) {
    await installPluginFromGitHub(plugin, cwd, opts);
  } else {
    await installPluginFromRegistry(plugin, cwd, opts);
  }
}

} // namespace elizaos
