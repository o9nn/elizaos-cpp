#include "elizaos/core.hpp"
#include "load-plugin.hpp"
#include "package-manager.hpp"
#include "plugin-context.hpp"
#include "plugin-discovery.hpp"
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
;
;

/**
 * Get the CLI's installation directory when running globally
 * @returns {string|null} - The path to the CLI's directory or null if not found
 */

    }

    return null;
  } catch (error) {
    logger.error('Failed to determine CLI directory:', error);
    return null;
  }
}

/**
 * Verifies if a plugin can be imported
 * @param {string} repository - The plugin repository/package name to import
 * @param {string} context - Description of the installation context for logging
 * @returns {boolean} - Whether the import was successful
 */
std::future<bool> verifyPluginImport(const std::string& repository, const std::string& context); ${context} after installation.`);
    return true;
  } else {
    // The loadPluginModule  installed ${context} but could not be loaded/verified.`);
    return false;
  }
}

/**
 * Attempts to install a plugin in a specific directory
 * @param {string} repository - The plugin repository to install
 * @param {string} versionString - Version string for installation
 * @param {string} directory - Directory to install in
 * @param {string} context - Description of the installation context for logging
 * @param {boolean} skipVerification - Whether to skip import verification
 * @returns {boolean} - Whether the installation and import verification was successful
 */
std::future<bool> attemptInstallation(const std::string& packageName, const std::string& versionString, const std::string& directory, const std::string& context, auto skipVerification = false);...`);

  try {
    // Use centralized installation `);
      return false;
    }

    // If installed via direct GitHub specifier, skip import verification
    if (packageName.startsWith('github:')) {
      return true;
    }
    if (skipVerification || process.env.ELIZA_SKIP_PLUGIN_VERIFY) {
      logger.info(
        `Installation successful for ${installResult.installedIdentifier}, skipping verification`
      );
      return true;
    }
    logger.debug(
      `Installation successful for ${installResult.installedIdentifier}, verifying import...`
    );
    return await verifyPluginImport(installResult.installedIdentifier, context);
  } catch (installError) {
    // Catch any unexpected errors during the process
    logger.warn(
      `Error during installation attempt ${context}: ${installError instanceof Error ? installError.message : String(installError)}`
    );
    return false;
  }
}

/**
 * Asynchronously installs a plugin to a specified directory.
 *
 * @param {string} packageName - The repository URL of the plugin to install.
 * @param {string} cwd - The current working directory where the plugin will be installed.
 * @param {string} versionSpecifier - The specific version of the plugin to install.
 * @param {boolean} skipVerification - Whether to skip import verification.
 * @returns {Promise<boolean>} - A Promise that resolves to true if the plugin is successfully installed, or false otherwise.
 */
std::future<bool> installPlugin(const std::string& packageName, const std::string& cwd, std::optional<std::string> versionSpecifier, auto skipVerification = false);`);

  // Check if we're trying to install a plugin into its own directory
  const context = detectPluginContext(packageName);
  if (context.isLocalDevelopment) {
    logger.warn(`Prevented self-installation of plugin ${packageName}`);
    logger.info(
      `You're developing this plugin locally. Use 'bun run build' to build it instead of installing.`
    );
    return false;
  }

  const cliDir = getCliDirectory();

  // Direct GitHub installation
  if (packageName.startsWith('github:')) {
    return await attemptInstallation(packageName, '', cwd, '', skipVerification);
  }

  // Handle full GitHub URLs as well
  const httpsGitHubUrlRegex =
    /^https?:\/\/github\.com\/([a-zA-Z0-9_-]+)\/([a-zA-Z0-9_.-]+?)(?:\.git)?(?:#([a-zA-Z0-9_.-]+))?\/?$/;
  const httpsMatch = packageName.match(httpsGitHubUrlRegex);
  if (httpsMatch) {
    const [, owner, repo, ref] = httpsMatch;
    const spec = `github:${owner}/${repo}${ref ? `#${ref}` : ''}`;
    return await attemptInstallation(spec, '', cwd, '', skipVerification);
  }

  const cache = await fetchPluginRegistry();
  const possible = normalizePluginName(packageName);

  let key: string | null = null;
  for (const name of possible) {
    if (cache?.registry[name]) {
      key = name;
      break;
    }
  }

  if (!key && cache && cache.registry) {
    // Fuzzy search by stripped base name
    let base = packageName;
    if (base.includes('/')) {
      const parts = base.split('/');
      base = parts[parts.length - 1];
    }
    base = base.replace(/^@/, '').replace(/^(plugin|client)-/, '');
    const lower = base.toLowerCase();

    const matches = Object.keys(cache.registry).filter(
      (cand) => cand.toLowerCase().includes(lower) && !cand.includes('client-')
    );

    if (matches.length > 0) {
      const pluginMatch = matches.find((c) => c.includes('plugin-'));
      key = pluginMatch || matches[0];
    }
  }

  if (!key) {
    logger.warn(
      `Plugin ${packageName} not found in registry cache, attempting direct installation`
    );
    return await attemptInstallation(
      packageName,
      versionSpecifier || '',
      cwd,
      '',
      skipVerification
    );
  }

  const info = cache!.registry[key];

  // Extract GitHub fallback information if available
  const githubFallback = info.git?.repo;
  const githubVersion = info.git?.v1?.branch || info.git?.v1?.version || '';

  // Prefer npm installation with GitHub fallback if repository is available
  if (info.npm?.repo) {
    const ver = versionSpecifier || info.npm.v1 || '';
    const result = await executeInstallationWithFallback(info.npm.repo, ver, cwd, githubFallback);

    if (result.success) {
      // Verify import if not a GitHub install
      if (
        !info.npm.repo.startsWith('github:') &&
        !skipVerification &&
        !process.env.ELIZA_SKIP_PLUGIN_VERIFY
      ) {
        const importSuccess = await verifyPluginImport(
          result.installedIdentifier || info.npm.repo,
          'from npm with potential GitHub fallback'
        );
        return importSuccess;
      }
      return true;
    }
  } else if (info.npm?.v1) {
    const result = await executeInstallationWithFallback(key, info.npm.v1, cwd, githubFallback);

    if (result.success) {
      // Verify import if not a GitHub install
      if (!skipVerification && !process.env.ELIZA_SKIP_PLUGIN_VERIFY) {
        const importSuccess = await verifyPluginImport(
          result.installedIdentifier || key,
          'from npm registry with potential GitHub fallback'
        );
        return importSuccess;
      }
      return true;
    }
  }

  // If both npm approaches failed, try direct GitHub installation as final fallback
  if (info.git?.repo && cliDir) {
    const spec = `github:${info.git.repo}${githubVersion ? `#${githubVersion}` : ''}`;
    return await attemptInstallation(spec, '', cliDir, 'in CLI directory', skipVerification);
  }

  logger.error(`Failed to install plugin ${packageName}`);
  return false;
}

} // namespace elizaos
