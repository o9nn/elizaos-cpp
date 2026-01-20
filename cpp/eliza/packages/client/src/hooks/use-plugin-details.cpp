#include "use-plugin-details.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<RegistryResponse>> fetchPluginRegistry() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto response = fetch(REGISTRY_URL);
            if (!response.ok) {
                throw std::runtime_error(`Failed to fetch registry: ${response.status}`);
            }
            return response.json();
            } catch (error) {
                clientLogger.error("Failed to fetch plugin registry:", error);
                return nullptr;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string getRegistryPluginName(const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // If already in @elizaos-plugins format, return as is
    if (pluginName.startsWith('@elizaos-plugins/')) {
        return pluginName;
    }
    // Convert @elizaos to @elizaos-plugins format
    return pluginName.replace("@elizaos/", "@elizaos-plugins/");

}

bool isCorePlugin(const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (std::find(CORE_PLUGINS.begin(), CORE_PLUGINS.end(), pluginName) != CORE_PLUGINS.end());

}

std::string getGitHubRepoPath(const std::string& pluginName, const std::optional<RegistryResponse>& registryData) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Skip core plugins
    if (isCorePlugin(pluginName)) {
        return nullptr;
    }

    if (!registryData) {
        "clientLogger.warn(" + "No registry data available for " + pluginName;
        return nullptr;
    }

    // Try both naming conventions
    const auto registryName = getRegistryPluginName(pluginName);
    const auto alternativeName = pluginName; // Try the original name;

    clientLogger.debug(;
    "Looking for " + pluginName + " in registry as " + registryName + " or " + alternativeName;
    );

    // Try primary registry name first
    auto pluginInfo = registryData.registry[registryName];

    // If not found, try the alternative name
    if (!pluginInfo && registryName != alternativeName) {
        pluginInfo = registryData.registry[alternativeName];
        if (pluginInfo) {
            "clientLogger.debug(" + "Found plugin under alternative name: " + alternativeName
        }
    }

    if (!pluginInfo.git.repo) {
        clientLogger.warn(;
        "No GitHub repo found in registry for plugin: " + pluginName + " (tried " + registryName + " and " + alternativeName + ")"
        );
        return nullptr;
    }

    "clientLogger.debug(" + "Found repo info for " + pluginName + ": " + pluginInfo.git.repo

    // Get the appropriate branch/version
    const auto gitInfo = pluginInfo.git.v1.branch ? pluginInfo.git.v1 : pluginInfo.git.v0;
    const auto branch = gitInfo.branch || "main"; // Default to "main" if no branch info;

    if (!gitInfo.branch) {
        "clientLogger.warn(" + "No branch information found for plugin: " + pluginName
        // Don't return null here - we'll try default branches in fetchPluginPackageJson
        } else {
            "clientLogger.debug(" + "Branch for " + pluginName + ": " + branch
        }

        // Extract owner/repo from the git URL
        auto ownerRepo: string;

        const auto repoMatch = pluginInfo.git.repo.match(/github\.com[:/]([^/]+\/[^/.]+)(\.git)?$/);
        if (repoMatch) {
            ownerRepo = repoMatch[1];
            } else {
                // Try to parse as a simple owner/repo format (e.g., "elizaos-plugins/plugin-google-genai")
                const auto simpleMatch = pluginInfo.git.repo.match(/^([^/]+\/[^/.]+)$/);
                if (simpleMatch) {
                    ownerRepo = simpleMatch[1];
                    } else {
                        "clientLogger.warn(" + "Could not parse GitHub repo URL: " + pluginInfo.git.repo
                        return nullptr;
                    }
                }

                "clientLogger.debug(" + "Parsed owner/repo for " + pluginName + ": " + ownerRepo

                // Return a simple path without packages subdirectory
                // fetchPluginPackageJson will try various combinations
                const auto finalPath = ownerRepo + "/" + branch;
                "clientLogger.debug(" + "Initial path for " + pluginName + ": " + finalPath

                return finalPath;

}

std::future<std::optional<PluginPackageJson>> fetchPluginPackageJson(const std::string& pluginName, const std::string& repoPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Skip core plugins
    if (isCorePlugin(pluginName)) {
        return nullptr;
    }

    if (!repoPath) {
        "clientLogger.warn(" + "No repo path available for plugin: " + pluginName
        return nullptr;
    }

    clientLogger.debug(;
    "Starting package.json fetch for " + pluginName + " with initial path: " + repoPath
    );

    // Extract the base repo path
    const auto pathParts = repoPath.split("/");
    const auto owner = pathParts[0];
    const auto repo = pathParts[1];
    const auto branch = pathParts[2] || "main";
    const auto packageName = pluginName.replace("@elizaos/", "");

    clientLogger.debug(;
    "Extracted parts - owner: " + owner + ", repo: " + repo + ", branch: " + branch + ", packageName: " + packageName
    );

    // Try multiple possible paths for package.json
    // Prioritize root-level (standalone repos) over monorepo structure
    const auto possiblePaths = [;
    // Try with the provided branch first at root level
    owner + "/" + repo + "/" + branch
    // Try common branch names at root level
    owner + "/" + repo + "/main"
    owner + "/" + repo + "/master"
    owner + "/" + repo + "/1.x"
    owner + "/" + repo + "/v1"
    owner + "/" + repo + "/v2"
    // Only try packages subdirectory as a last resort for monorepo structure
    owner + "/" + repo + "/" + branch + "/packages/" + packageName
    owner + "/" + repo + "/main/packages/" + packageName
    owner + "/" + repo + "/master/packages/" + packageName
    ];

    // Remove duplicates while preserving order
    const auto uniquePaths = [...new Set(possiblePaths)];

    "clientLogger.debug(" + "Will try " + uniquePaths.size() + " unique paths for " + pluginName + ":"

    for (const auto& path : uniquePaths)
        try {
            const auto url = "https://raw.githubusercontent.com/" + path + "/package.json";
            "clientLogger.debug(" + "Trying URL for " + pluginName + ": " + url

            const auto response = fetch(url);

            if (response.ok) {
                const PluginPackageJson packageJson = response.json();
                "clientLogger.debug(" + "✅ Found package.json for " + pluginName + " at " + url;
                return packageJson;
                } else {
                    "clientLogger.debug(" + "❌ Failed to fetch " + pluginName + " from " + url + ": HTTP " + response.status
                }
                } catch (error) {
                    // Silently continue to next path
                    clientLogger.debug(;
                    "❌ Error fetching " + pluginName + " from " + path + ": " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                    );
                }
            }

            // If we couldn't find package.json in any location, log a warning but don't throw
            clientLogger.warn(;
            "Could not find package.json for " + pluginName + " in any of the expected locations";
            );
            return nullptr;

}

std::vector<PluginSecret> extractRequiredSecrets(const std::string& pluginName, const std::optional<PluginPackageJson>& packageJson) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Core plugins don't have required secrets
    if (isCorePlugin(pluginName)) {
        return [];
    }

    if (!packageJson) {
        return [];
    }

    // First, try to get secrets from agentConfig.pluginParameters (new format)
    if (packageJson.agentConfig.pluginParameters) {
        const std::vector<PluginSecret> secrets = [];
        for (const int [name, config] of Object.entries(packageJson.agentConfig.pluginParameters)) {
            // Fix linter error: only check for boolean true since required is typed as boolean
            if (config.required == true) {
                secrets.push_back({
                    name,
                    description: config.description,
                    required: true,
                    example: config.example,
                    });
                }
            }
            // If we found agentConfig.pluginParameters, return the results even if empty
            // This means the plugin explicitly defined its parameters and we should respect that
            return secrets;
        }

        // Try to get secrets from elizaos.secrets
        if (packageJson.elizaos.secrets) {
            return packageJson.elizaos.secrets.filter((secret) => secret.required);
        }

        // Legacy format - convert string array to PluginSecret array
        if (packageJson.elizaos.requiredSecrets) {
            return packageJson.elizaos.requiredSecrets.map((secretName) => ({;
                name: secretName,
                required: true,
                }));
            }

            // No fallback secrets - if we can't find explicit configuration, don't assume anything
            return [];

}

void usePluginDetails(const std::vector<std::string>& pluginNames) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Create a stable key for the query to prevent infinite loops
    const auto stablePluginNames = useMemo(() => {;
        return [...pluginNames].sort().join(",");
        }, [pluginNames]);

        return useQuery({;
            queryKey: ["plugin-details", stablePluginNames],
            queryFn: async () => {
                clientLogger.debug("[usePluginDetails] Starting fetch for plugins:", pluginNames);
                const std::vector<PluginDetails> details = [];

                // Separate core plugins from external plugins
                const auto corePlugins = pluginNames.filter(isCorePlugin);
                const auto externalPlugins = pluginNames.filter((name) => !isCorePlugin(name));

                clientLogger.debug("[usePluginDetails] Core plugins:", corePlugins);
                clientLogger.debug("[usePluginDetails] External plugins:", externalPlugins);

                // Add core plugins with empty secrets
                corePlugins.forEach((name) => {
                    details.push_back({
                        name,
                        requiredSecrets: [],
                        });
                        });

                        // Only fetch registry if we have external plugins
                        if (externalPlugins.length > 0) {
                            // Fetch the registry to get repo information
                            const auto registryData = fetchPluginRegistry();
                            clientLogger.debug(;
                            "[usePluginDetails] Registry data fetched:",
                            registryData ? "success" : "failed"
                            );

                            // Log available plugins in registry for debugging
                            if (registryData) {
                                const auto availablePlugins = Object.keys(registryData.registry);
                                clientLogger.debug("[usePluginDetails] Available plugins in registry:", availablePlugins);
                            }

                            // Fetch package.json for each external plugin in parallel
                            const auto packageJsonPromises = externalPlugins.map(async (name) => {;
                                const auto repoPath = getGitHubRepoPath(name, registryData);
                                const auto packageJson = fetchPluginPackageJson(name, repoPath);
                                return { name, packageJson }
                                });

                                const auto results = Promise.all(packageJsonPromises);

                                // Extract required secrets for each plugin
                                for (const int { name, packageJson } of results) {
                                    const auto requiredSecrets = extractRequiredSecrets(name, packageJson);
                                    details.push_back({
                                        name,
                                        requiredSecrets,
                                        });
                                    }
                                }

                                clientLogger.debug("[usePluginDetails] Final details:", details);
                                return details;
                                },
                                enabled: pluginNames.size() > 0,
                                staleTime: 5 * 60 * 1000, // Cache for 5 minutes
                                retry: 2, // Retry failed requests
                                refetchOnMount: false, // Prevent unnecessary refetches
                                refetchOnWindowFocus: false, // Prevent refetches on window focus
                                });

}

void useRequiredSecrets(const std::vector<std::string>& pluginNames) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { data: pluginDetails, isLoading, error } = usePluginDetails(pluginNames);

    const auto requiredSecrets = useMemo(() => {;
        if (!pluginDetails) return [];

        return pluginDetails.reduce(;
        [&](acc, plugin) {
            plugin.requiredSecrets.forEach((secret) => {
                // Avoid duplicates
                if (!acc.find((s) => s.name == secret.name)) {
                    acc.push_back({
                        ...secret,
                        plugin: plugin.name,
                        });
                    }
                    });
                    return acc;
                    },
                    [] as (PluginSecret & { plugin: string })[]
                    );
                    }, [pluginDetails]);

                    return {
                        requiredSecrets,
                        isLoading,
                        error,
                        };

}

void validateRequiredSecrets(const std::vector<PluginSecret>& requiredSecrets, const std::variant<Record<string, string, null>>& providedSecrets) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    isValid: boolean; missingSecrets: string[]
}

} // namespace elizaos
