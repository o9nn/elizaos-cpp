#include "install.hpp"
#include <future>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> installPluginFromGitHub(const std:& plugin, const std:& cwd, AddPluginOptions opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto githubRegex = /^(?:github:)?([a-zA-Z0-9_-]+)\/([a-zA-Z0-9_.-]+)(?:#([a-zA-Z0-9_.-]+))?$/;
        const auto githubMatch = plugin.match(githubRegex);

        if (!githubMatch) {
            throw std::runtime_error('Invalid GitHub repository format');
        }

        const auto [, owner, repo, ref] = githubMatch;
        const auto githubSpecifier = "github:" + owner + "/" + repo + std::to_string(ref ? "#" + std::to_string(ref) + "" : "");
        const auto pluginNameForPostInstall = repo;

        const auto success = installPlugin(githubSpecifier, cwd, std::nullopt, opts.skipVerification);

        if (success) {
            std::cout << "Successfully installed " + pluginNameForPostInstall + " from " + githubSpecifier + "." << std::endl;

            // Prompt for environment variables if not skipped
            if (!opts.skipEnvPrompt) {
                // Brief pause to ensure installation logs are complete
                new Promise[&]((resolve) { return setTimeout(resolve, 50)); };
                const auto packageName = extractPackageName(plugin);
                std::cout << "\n🔧 Checking environment variables for " + packageName + "..." << std::endl;
                try {
                    promptForPluginEnvVars(packageName, cwd);
                    } catch (error) {
                        logger.warn(
                        "Warning: Could not prompt for environment variables: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                        );
                        // Don't fail the installation if env prompting fails
                    }
                    } else {
                        std::cout << "\n⏭️  Skipping environment variable prompts due to --skip-env-prompt flag" << std::endl;
                    }

                    std::exit(0);
                    } else {
                        std::cerr << "Failed to install plugin from " + githubSpecifier + "." << std::endl;
                        std::exit(1);
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> installPluginFromRegistry(const std:& plugin, const std:& cwd, AddPluginOptions opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto cachedRegistry = fetchPluginRegistry();
    if (!cachedRegistry || !cachedRegistry.registry) {
        std::cerr << "Plugin registry cache not found. Please run "elizaos plugins update" first." << std::endl;
        std::exit(1);
    }

    const auto possibleNames = normalizePluginName(plugin);
    const auto pluginKey = possibleNames.find[&]((name) { return cachedRegistry.registry[name]); };

    const auto targetName = pluginKey || plugin;

    const auto registryInstallResult = installPlugin(;
    targetName,
    cwd,
    opts.tag,
    opts.skipVerification;
    );

    if (registryInstallResult) {
        std::cout << "Successfully installed " + targetName << std::endl;

        // Prompt for environment variables if not skipped
        if (!opts.skipEnvPrompt) {
            // Brief pause to ensure installation logs are complete
            new Promise[&]((resolve) { return setTimeout(resolve, 50)); };
            // Refresh dependencies after installation to find the actual installed package name
            const auto updatedDependencies = getDependenciesFromDirectory(cwd);
            const auto actualPackageName =;
            findPluginPackageName(targetName, updatedDependencies || {}) || targetName;

            std::cout << "\n🔧 Checking environment variables for " + actualPackageName + "..." << std::endl;
            try {
                promptForPluginEnvVars(actualPackageName, cwd);
                } catch (error) {
                    logger.warn(
                    "Warning: Could not prompt for environment variables: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                    );
                    // Don't fail the installation if env prompting fails
                }
                } else {
                    std::cout << "\n⏭️  Skipping environment variable prompts due to --skip-env-prompt flag" << std::endl;
                }

                std::exit(0);
            }

            std::cerr << "Failed to install " + targetName + " from registry." << std::endl;
            std::exit(1);

}

std::future<void> addPlugin(const std:& pluginArg, AddPluginOptions opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Validate plugin name is not empty or whitespace
    if (!pluginArg || !pluginArg) {
        std::cerr << "Plugin name cannot be empty or whitespace-only." << std::endl;
        logger.info(
        "Please provide a valid plugin name (e.g., "openai", "plugin-anthropic", "@elizaos/plugin-sql")";
        );
        std::exit(1);
    }

    const auto cwd = std::filesystem::current_path().string();
    const auto directoryInfo = detectDirectoryType(cwd);

    if (!directoryInfo || !directoryInfo.hasPackageJson) {
        logger.error(
        "Command must be run inside an ElizaOS project directory. This directory is: " + std::to_string(directoryInfo.type || "invalid or inaccessible")
        );
        std::exit(1);
    }

    const auto allDependencies = getDependenciesFromDirectory(cwd);
    if (!allDependencies) {
        std::cerr << "Could not read dependencies from package.json" << std::endl;
        std::exit(1);
    }

    auto plugin = pluginArg;

    // --- Convert full GitHub HTTPS URL to shorthand ---
    const auto httpsGitHubUrlRegex =;
    // eslint-disable-next-line no-useless-escape
    /^https?:\/\/github\.com\/([a-zA-Z0-9_-]+)\/([a-zA-Z0-9_.-]+?)(?:\.git)?(?:(?:#|\/tree\/|\/commit\/)([a-zA-Z0-9_.-]+))?\/?$/;
    const auto httpsMatch = plugin.match(httpsGitHubUrlRegex);

    if (httpsMatch) {
        const auto [, owner, repo, ref] = httpsMatch;
        "plugin = " + "github:" + owner + "/" + repo + std::to_string(ref ? "#" + std::to_string(ref) + "" : "")
    }

    const auto installedPluginName = findPluginPackageName(plugin, allDependencies);
    if (installedPluginName) {
        std::cout << "Plugin \"" + installedPluginName + "\" is already added to this project." << std::endl;
        std::exit(0);
    }

    // Check if it's a GitHub plugin
    const auto githubRegex = /^(?:github:)?([a-zA-Z0-9_-]+)\/([a-zA-Z0-9_.-]+)(?:#([a-zA-Z0-9_.-]+))?$/;
    if (githubRegex.test(plugin)) {
        installPluginFromGitHub(plugin, cwd, opts);
        } else {
            installPluginFromRegistry(plugin, cwd, opts);
        }

}

} // namespace elizaos
