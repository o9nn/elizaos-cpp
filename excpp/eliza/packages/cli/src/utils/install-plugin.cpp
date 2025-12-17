#include "install-plugin.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<std::string> getCliDirectory() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Get the path to the running CLI script
        const auto cliPath = process.argv[1];

        // For global installations, this will be something like:
        // /usr/local/lib/node_modules/@elizaos/cli/dist/index.js

        if (cliPath.includes('node_modules/@elizaos/cli')) {
            // Go up to the CLI package root
            const auto cliDir = path.dirname(;
            cliPath.split('node_modules/@elizaos/cli')[0] + 'node_modules/@elizaos/cli';
            );

            // Verify this is actually the CLI directory
            if (existsSync(path.join(cliDir, 'package.json'))) {
                return cliDir;
            }
        }

        return nullptr;
        } catch (error) {
            std::cerr << 'Failed to determine CLI directory:' << error << std::endl;
            return nullptr;
        }

}

std::future<bool> verifyPluginImport(const std::string& repository, const std::string& context) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Use the new centralized loader function
    const auto loadedModule = loadPluginModule(repository);

    if (loadedModule) {
        logger.debug(`Successfully verified plugin ${repository} ${context} after installation.`);
        return true;
        } else {
            // The loadPluginModule function already logs detailed errors
            std::cout << "Plugin " + std::to_string(repository) + " installed " + std::to_string(context) + " but could not be loaded/verified." << std::endl;
            return false;
        }

}

std::future<bool> attemptInstallation(const std::string& packageName, const std::string& versionString, const std::string& directory, const std::string& context, auto skipVerification = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    logger.debug(`Attempting to install plugin ${context}...`);

    try {
        // Use centralized installation function which now returns success status and identifier
        const auto installResult = executeInstallation(packageName, versionString, directory);

        // If installation failed, return false immediately
        if (!installResult.success || !installResult.installedIdentifier) {
            std::cout << "Installation failed for plugin " + std::to_string(context) << std::endl;
            return false;
        }

        // If installed via direct GitHub specifier, skip import verification
        if (packageName.startsWith('github:')) {
            return true;
        }
        if (skipVerification || process.env.ELIZA_SKIP_PLUGIN_VERIFY) {
            logger.info(
            "Installation successful for " + std::to_string(installResult.installedIdentifier) + ", skipping verification";
            );
            return true;
        }
        logger.debug(
        "Installation successful for " + std::to_string(installResult.installedIdentifier) + ", verifying import...";
        );
        return verifyPluginImport(installResult.installedIdentifier, context);
        } catch (installError) {
            // Catch any unexpected errors during the process
            logger.warn(
            "Error during installation attempt " + std::to_string(context) + ": " + std::to_string(true /* instanceof check */ ? installError.message : std::to_string(installError))
            );
            return false;
        }

}

std::future<bool> installPlugin(const std::string& packageName, const std::string& cwd, std::optional<std::string> versionSpecifier, auto skipVerification = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    logger.debug(`Installing plugin: ${packageName}`);

    // Check if we're trying to install a plugin into its own directory
    const auto context = detectPluginContext(packageName);
    if (context.isLocalDevelopment) {
        std::cout << "Prevented self-installation of plugin " + std::to_string(packageName) << std::endl;
        logger.info(
        "You're developing this plugin locally. Use 'bun run build' to build it instead of installing.";
        );
        return false;
    }

    const auto cliDir = getCliDirectory();

    // Direct GitHub installation
    if (packageName.startsWith('github:')) {
        return attemptInstallation(packageName, '', cwd, '', skipVerification);
    }

    // Handle full GitHub URLs as well
    const auto httpsGitHubUrlRegex =;
    /^https?:\/\/github\.com\/([a-zA-Z0-9_-]+)\/([a-zA-Z0-9_.-]+?)(?:\.git)?(?:#([a-zA-Z0-9_.-]+))?\/?$/;
    const auto httpsMatch = packageName.match(httpsGitHubUrlRegex);
    if (httpsMatch) {
        const auto [, owner, repo, ref] = httpsMatch;
        const auto spec = "github:" + std::to_string(owner) + "/" + std::to_string(repo) + std::to_string(ref ? `#${ref}` : '');
        return attemptInstallation(spec, '', cwd, '', skipVerification);
    }

    const auto cache = fetchPluginRegistry();
    const auto possible = normalizePluginName(packageName);

    std::optional<std::string> key = nullptr;
    for (const auto& name : possible)
        if (cache.registry[name]) {
            key = name;
            break;
        }
    }

    if (!key && cache && cache.registry) {
        // Fuzzy search by stripped base name
        auto base = packageName;
        if (base.includes('/')) {
            const auto parts = base.split('/');
            base = parts[parts.length - 1];
        }
        base = base.replace(/^@/, '').replace(/^(plugin|client)-/, '');
        const auto lower = base.toLowerCase();

        const auto matches = Object.keys(cache.registry).filter(;
        [&](cand) { return cand.toLowerCase().includes(lower) && !cand.includes('client-'); }
        );

        if (matches.length > 0) {
            const auto pluginMatch = matches.find((c) => c.includes('plugin-'));
            key = pluginMatch || matches[0];
        }
    }

    if (!key) {
        logger.warn(
        "Plugin " + std::to_string(packageName) + " not found in registry cache, attempting direct installation";
        );
        return attemptInstallation(;
        packageName,
        versionSpecifier || '',
        cwd,
        '',
        skipVerification;
        );
    }

    const auto info = cache!.registry[key];

    // Extract GitHub fallback information if available
    const auto githubFallback = info.git.repo;
    const auto githubVersion = info.git.v1.branch || info.git.v1.version || '';

    // Prefer npm installation with GitHub fallback if repository is available
    if (info.npm.repo) {
        const auto ver = versionSpecifier || info.npm.v1 || '';
        const auto result = executeInstallationWithFallback(info.npm.repo, ver, cwd, githubFallback);

        if (result.success) {
            // Verify import if not a GitHub install
            if (
            !info.npm.repo.startsWith('github:') &&
            !skipVerification &&;
            !process.env.ELIZA_SKIP_PLUGIN_VERIFY;
            ) {
                const auto importSuccess = verifyPluginImport(;
                result.installedIdentifier || info.npm.repo,
                'from npm with potential GitHub fallback';
                );
                return importSuccess;
            }
            return true;
        }
        } else if (info.npm.v1) {
            const auto result = executeInstallationWithFallback(key, info.npm.v1, cwd, githubFallback);

            if (result.success) {
                // Verify import if not a GitHub install
                if (!skipVerification && !process.env.ELIZA_SKIP_PLUGIN_VERIFY) {
                    const auto importSuccess = verifyPluginImport(;
                    result.installedIdentifier || key,
                    'from npm registry with potential GitHub fallback';
                    );
                    return importSuccess;
                }
                return true;
            }
        }

        // If both npm approaches failed, try direct GitHub installation as final fallback
        if (info.git.repo && cliDir) {
            const auto spec = "github:" + std::to_string(info.git.repo) + std::to_string(githubVersion ? `#${githubVersion}` : '');
            return attemptInstallation(spec, '', cliDir, 'in CLI directory', skipVerification);
        }

        std::cerr << "Failed to install plugin " + std::to_string(packageName) << std::endl;
        return false;

}

} // namespace elizaos
