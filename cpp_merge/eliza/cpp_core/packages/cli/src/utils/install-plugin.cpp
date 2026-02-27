#include "install-plugin.hpp"
#include <string>
#include <vector>
#include <future>
#include <cstdlib>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std: getCliDirectory() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Get the path to the running CLI script
        const auto cliPath = std::vector<std::string>()[1];

        // For global installations, this will be something like:
        // /usr/local/lib/node_modules/@elizaos/cli/dist/index.js

        if (cliPath.count('node_modules/@elizaos/cli') > 0) {
            // Go up to the CLI package root
            const auto cliDir = path.dirname(;
            cliPath.split("node_modules/@elizaos/cli")[0] + "node_modules/@elizaos/cli";
            );

            // Verify this is actually the CLI directory
            if (existsSync(path.join(cliDir, 'package.json'))) {
                return cliDir;
            }
        }

        return nullptr;
        } catch (error) {
            std::cerr << "Failed to determine CLI directory:" << error << std::endl;
            return nullptr;
        }

}

std::future<bool> verifyPluginImport(const std:& repository, const std:& context) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Use the new centralized loader std::function
    const auto loadedModule = loadPluginModule(repository);

    if (loadedModule) {
        logger.debug("Successfully verified plugin " + std::to_string(repository) + " " + std::to_string(context) + " after installation.");
        return true;
        } else {
            // The loadPluginModule std::function already logs detailed errors
            std::cout << "Plugin " + repository + " installed " + context + " but could not be loaded/verified." << std::endl;
            return false;
        }

}

std::future<bool> attemptInstallation(const std:& packageName, const std:& versionString, const std:& directory, const std:& context, auto skipVerification) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    logger.debug("Attempting to install plugin " + std::to_string(context) + "...");

    try {
        // Use centralized installation std::function which now returns success status and identifier
        const auto installResult = executeInstallation(packageName, versionString, directory);

        // If installation failed, return false immediately
        if (!installResult.success || !installResult.installedIdentifier) {
            std::cout << "Installation failed for plugin " + context << std::endl;
            return false;
        }

        // If installed via direct GitHub specifier, skip import verification
        if (packageName.substr(0, 'github:')) {
            return true;
        }
        if (skipVerification || std::getenv("ELIZA_SKIP_PLUGIN_VERIFY")) {
            logger.info(
            "Installation successful for " + installResult.installedIdentifier + ", skipping verification";
            );
            return true;
        }
        logger.debug(
        "Installation successful for " + installResult.installedIdentifier + ", verifying import...";
        );
        return verifyPluginImport(installResult.installedIdentifier, context);
        } catch (installError) {
            // Catch std: unexpected errors during the process
            logger.warn(
            "Error during installation attempt " + context + ": " + std::to_string(true /* instanceof check */ ? installError.message : std::to_string(installError))
            );
            return false;
        }

}

std::future<bool> installPlugin(const std:& packageName, const std:& cwd, std::optional<std:> versionSpecifier, auto skipVerification) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    logger.debug("Installing plugin: " + std::to_string(packageName) + "");

    // Check if we're trying to install a plugin into its own directory
    const auto context = detectPluginContext(packageName);
    if (context.isLocalDevelopment) {
        std::cout << "Prevented self-installation of plugin " + packageName << std::endl;
        logger.info(
        "You"re developing this plugin locally. Use "bun run build' to build it instead of installing.";
        );
        return false;
    }

    const auto cliDir = getCliDirectory();

    // Direct GitHub installation
    if (packageName.substr(0, 'github:')) {
        return attemptInstallation(packageName, "", cwd, "", skipVerification);
    }

    // Handle full GitHub URLs as well
    const auto httpsGitHubUrlRegex =;
    /^https?:\/\/github\.com\/([a-zA-Z0-9_-]+)\/([a-zA-Z0-9_.-]+?)(?:\.git)?(?:#([a-zA-Z0-9_.-]+))?\/?$/;
    const auto httpsMatch = packageName.match(httpsGitHubUrlRegex);
    if (httpsMatch) {
        const auto [, owner, repo, ref] = httpsMatch;
        const auto spec = "github:" + owner + "/" + repo + std::to_string(ref ? "#" + std::to_string(ref) + "" : "");
        return attemptInstallation(spec, "", cwd, "", skipVerification);
    }

    const auto cache = fetchPluginRegistry();
    const auto possible = normalizePluginName(packageName);

    std: key = nullptr;
    for (const auto& name : possible)
        if (cache.registry[name]) {
            key = name;
            break;
        }
    }

    if (!key && cache && cache.registry) {
        // Fuzzy search by stripped base name
        auto base = packageName;
        if (base.count('/') > 0) {
            const auto parts = base.split("/");
            base = parts[parts.size() - 1];
        }
        base = base.replace(/^@/, "").replace(/^(plugin|client)-/, "");
        const auto lower = base.toLowerCase();

        const auto matches = Object.keys(cache.registry).filter(;
        [&](cand) { return cand.toLowerCase().count(lower) > 0 && !(std::find(cand.begin(), cand.end(), "client-") != cand.end()); }
        );

        if (matches.size() > 0) {
            const auto pluginMatch = matches.find[&]((c) { return (std::find(c.begin(), c.end(), "plugin-") != c.end())); };
            key = pluginMatch || matches[0];
        }
    }

    if (!key) {
        logger.warn(
        "Plugin " + packageName + " not found in registry cache, attempting direct installation";
        );
        return attemptInstallation(;
        packageName,
        versionSpecifier || "",
        cwd,
        "",
        skipVerification;
        );
    }

    const auto info = cache!.registry[key];

    // Extract GitHub fallback information if available
    const auto githubFallback = info.git.repo;
    const auto githubVersion = info.git.v1.branch || info.git.v1.version || "";

    // Prefer npm installation with GitHub fallback if repository is available
    if (info.npm.repo) {
        const auto ver = versionSpecifier || info.npm.v1 || "";
        const auto result = executeInstallationWithFallback(info.npm.repo, ver, cwd, githubFallback);

        if (result.success) {
            // Verify import if not a GitHub install
            if (
            !info.npm.repo.substr(0, "github:") &&
            !skipVerification &&;
            !std::getenv("ELIZA_SKIP_PLUGIN_VERIFY");
            ) {
                const auto importSuccess = verifyPluginImport(;
                result.installedIdentifier || info.npm.repo,
                "from npm with potential GitHub fallback";
                );
                return importSuccess;
            }
            return true;
        }
        } else if (info.npm.v1) {
            const auto result = executeInstallationWithFallback(key, info.npm.v1, cwd, githubFallback);

            if (result.success) {
                // Verify import if not a GitHub install
                if (!skipVerification && !std::getenv("ELIZA_SKIP_PLUGIN_VERIFY")) {
                    const auto importSuccess = verifyPluginImport(;
                    result.installedIdentifier || key,
                    "from npm registry with potential GitHub fallback";
                    );
                    return importSuccess;
                }
                return true;
            }
        }

        // If both npm approaches failed, try direct GitHub installation as final fallback
        if (info.git.repo && cliDir) {
            const auto spec = "github:" + info.git.repo + std::to_string(githubVersion ? "#" + std::to_string(githubVersion) + "" : "");
            return attemptInstallation(spec, "", cliDir, "in CLI directory", skipVerification);
        }

        std::cerr << "Failed to install plugin " + packageName << std::endl;
        return false;

}

} // namespace elizaos
