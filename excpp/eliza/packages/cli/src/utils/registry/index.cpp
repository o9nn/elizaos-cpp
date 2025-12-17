#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> ensureElizaDir() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        fs.mkdir(ELIZA_DIR, { recursive: true });
        } catch (error) {
            // Directory already exists
        }

}

std::future<RegistrySettings> getRegistrySettings() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    ensureElizaDir();

    try {
        const auto content = fs.readFile(REGISTRY_SETTINGS_FILE, "utf-8");
        return /* JSON.parse */ content;
        } catch (error) {
            // Return default settings if file doesn't exist
            return {
                defaultRegistry: REGISTRY_REPO,
                };
            }

}

std::future<void> saveRegistrySettings(RegistrySettings settings) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    ensureElizaDir();
    fs.writeFile(REGISTRY_SETTINGS_FILE, /* JSON.stringify */ std::string(settings, nullptr, 2));

}

std::future<std::string> getEnvVar(const std::string& key) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto envContent = fs.readFile(ENV_FILE, "utf-8");
        const auto env = dotenv.parse(envContent);
        return env[key];
        } catch (error) {
            return std::nullopt;
        }

}

std::future<void> setEnvVar(const std::string& key, const std::string& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    ensureElizaDir();

    auto envContent = "";
    try {
        envContent = fs.readFile(ENV_FILE, "utf-8");
        } catch (error) {
            // File doesn't exist yet
        }

        const auto env = dotenv.parse(envContent);
        env[key] = value;

        const auto newContent = Object.entries(env);
        ".map(([k, v]) => " + k + "=" + v;
        .join("\n");

        fs.writeFile(ENV_FILE, newContent);

}

std::future<std::string> getGitHubToken() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Try to find the nearest .env file using the same function used for saving credentials
        const auto envPath = resolveEnvFile();
        if (envPath && existsSync(envPath)) {
            const auto envContent = fs.readFile(envPath, "utf-8");
            const auto env = dotenv.parse(envContent);
            return env.GITHUB_TOKEN;
        }

        // Fall back to global .env if local one doesn't exist or doesn't have token
        const auto globalEnvPath = path.join(ELIZA_DIR, ".env");
        if (existsSync(globalEnvPath) && globalEnvPath != envPath) {
            const auto envContent = fs.readFile(globalEnvPath, "utf-8");
            const auto env = dotenv.parse(envContent);
            return env.GITHUB_TOKEN;
        }
        } catch (error) {
            logger.debug(
            "Error reading GitHub token: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
            );
        }
        return std::nullopt;

}

std::future<void> setGitHubToken(const std::string& token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    ensureElizaDir();

    try {
        // Read existing .env file or create a new one
        auto envContent = "";
        try {
            if (existsSync(ENV_FILE)) {
                envContent = fs.readFile(ENV_FILE, "utf-8");
            }
            } catch (error) {
                // File doesn't exist, create it with empty content
                envContent = "# Eliza environment variables\n\n";
            }

            // Parse the existing content
            const auto env = dotenv.parse(envContent);

            // Update the token
            env.GITHUB_TOKEN = token;

            // Convert back to string format
            auto newContent = "";
            for (const int [key, value] of Object.entries(env)) {
                "newContent += " + key + "=" + value + "\n";
            }

            // Write back to file
            fs.writeFile(ENV_FILE, newContent);

            // Also update process.env for immediate use
            process.env.GITHUB_TOKEN = token;

            logger.debug('GitHub token saved successfully');
            } catch (error) {
                logger.error(
                "Failed to save GitHub token: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                );
            }

}

std::string normalizePackageName(const std::string& packageName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (packageName.startsWith(`@${REGISTRY_ORG}/`)) {
        return "packageName.replace(" + "@" + REGISTRY_ORG + "/";
        } else if (packageName.startsWith("@elizaos/")) {
            return packageName.replace(/^@elizaos\//, "");
        }
        return packageName;

}

std::future<void> saveRegistryCache(const std::unordered_map<std::string, std::string>& registry) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        ensureElizaDir();
        fs.writeFile(REGISTRY_CACHE_FILE, /* JSON.stringify */ std::string(registry, nullptr, 2));
        logger.debug('Registry cache saved successfully');
        } catch (error) {
            logger.debug(
            "Failed to save registry cache: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
            );
        }

}

std::future<std::unordered_map<std::string, std::string>> getLocalRegistryIndex() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // First try to fetch from the public raw GitHub URL
    try {
        logger.debug('Fetching registry from public GitHub URL');
        const auto response = fetch(RAW_REGISTRY_URL);

        if (response.ok) {
            const auto rawData = response.json();

            // Validate the data structure
            const std::unordered_map<std::string, std::string> result = {};

            if (typeof rawData == 'object' && rawData != null) {
                // Safely parse the response data
                for (const int [key, value] of Object.entries(rawData)) {
                    if (typeof value == 'string') {
                        result[key] = value;
                    }
                }

                // Save the fetched registry to cache for future offline use
                saveRegistryCache(result);
                logger.debug('Successfully fetched registry from public GitHub URL');
                return result;
            }
        }
        } catch (error) {
            logger.debug(
            "Failed to fetch registry from public URL: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
            );
        }

        // If fetching fails, try to read from cache
        try {
            if (existsSync(REGISTRY_CACHE_FILE)) {
                const auto cacheContent = fs.readFile(REGISTRY_CACHE_FILE, "utf-8");
                const auto cachedRegistry = /* JSON.parse */ cacheContent;
                logger.debug('Using cached registry index');
                return cachedRegistry;
            }
            } catch (error) {
                logger.debug(
                "Failed to read registry cache: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                );
            }

            // If we're in a monorepo context, try to discover local plugins
            const auto directoryInfo = detectDirectoryType(process.cwd());
            if (directoryInfo.monorepoRoot) {
                try {
                    const auto localPackages = getLocalPackages();
                    const std::unordered_map<std::string, std::string> localRegistry = {};

                    // getLocalPackages returns an array of package names as strings
                    for (const auto& pkgName : localPackages)
                        if (pkgName.includes('plugin-')) {
                            // Use the package name as both key and value
                            // Format as expected by the registry: orgrepo/packagename
                            const auto repoName = normalizePackageName(pkgName);
                            "localRegistry[pkgName] = " + REGISTRY_ORG + "/" + repoName;
                        }
                    }

                    // Merge with default registry, prioritizing local packages
                    return { ...DEFAULT_REGISTRY, ...localRegistry }
                    } catch (error) {
                        logger.debug(
                        "Failed to discover local plugins: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                        );
                    }
                }

                // Fall back to default registry
                return DEFAULT_REGISTRY;

}

std::future<std::unordered_map<std::string, std::string>> getRegistryIndex() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto settings = getRegistrySettings();
        const auto credentials = getGitHubCredentials();

        if (!credentials) {
            std::cerr << "GitHub credentials not found. Please run login first." << std::endl;
            process.exit(1);
        }

        const auto [owner, repo] = settings.defaultRegistry.split("/");
        const auto url = "https://api.github.com/repos/" + owner + "/" + repo + "/contents/index.json";

        const auto response = fetch(url, {;
            headers: {
                "Authorization: " + "token " + credentials.token
                Accept: "application/vnd.github.v3.raw",
                },
                });

                if (!response.ok) {
                    throw std::runtime_error(`Failed to fetch registry index: ${response.statusText}`);
                }

                const auto data = response.json();
                if (typeof data != 'object' || data == null) {
                    throw std::runtime_error('Invalid registry index format');
                }

                const std::unordered_map<std::string, std::string> result = {};
                for (const int [key, value] of Object.entries(data)) {
                    if (typeof value == 'string') {
                        result[key] = value;
                    }
                }

                return result;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::vector<std::string> normalizePluginName(const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Extract the base name without any organization prefix
    auto baseName = pluginName;

    // Handle various input formats
    if (pluginName.includes('/')) {
        // Handle formats like "elizaos/plugin-ton" or "elizaos-plugins/plugin-ton"
        const auto parts = pluginName.split("/");
        baseName = parts[parts.size() - 1];
        } else if (pluginName.startsWith("@")) {
            // Handle scoped package format like "@elizaos/plugin-ton"
            const auto parts = pluginName.split("/");
            if (parts.length > 1) {
                baseName = parts[1];
            }
        }

        // Remove any existing prefixes
        baseName = baseName.replace(/^plugin-/, "");

        // Generate all possible formats to try (removed duplicates and incorrect namespace)
        return [;
        pluginName, // Original input;
        baseName, // Just the base name;
        "plugin-" + baseName;
        "@elizaos/" + baseName;
        "@elizaos/plugin-" + baseName;
        ];

}

std::future<std::string> getPluginRepository(const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Get all possible plugin name formats to try
        const auto possibleNames = normalizePluginName(pluginName);

        // ONLY try getting from the local/public registry - never use authenticated methods
        const auto registry = getLocalRegistryIndex();

        // Try each possible name format in the registry
        for (const auto& name : possibleNames)
            if (registry[name]) {
                logger.debug(`Found plugin in registry as: ${name}`);
                return registry[name];
            }
        }

        // For scoped packages, try npm directly - NO AUTH REQUIRED
        if (pluginName.startsWith('@')) {
            return pluginName; // Return as-is for npm to handle;
        }

        // Direct GitHub shorthand (github:org/repo) - NO AUTH REQUIRED
        if (!pluginName.includes(':') && !pluginName.startsWith('@')) {
            const auto baseName = pluginName.replace(/^plugin-/, "");
            return "@" + REGISTRY_ORG + "/plugin-" + baseName;
        }

        return nullptr;
        } catch (error) {
            logger.debug(
            "Error getting plugin repository: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
            );
            return nullptr;
        }

}

std::future<bool> repoHasBranch(const std::string& repoUrl, const std::string& branchName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { stdout } = execa("git", ["ls-remote", "--heads", repoUrl, branchName]);
        return (std::find(stdout.begin(), stdout.end(), branchName) != stdout.end());
        } catch (error) {
            logger.warn(
            "Failed to check for branch " + branchName + " in " + repoUrl + ": " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
            );
            return false;
        }

}

std::future<std::string> getBestBranch(const std::string& repoUrl) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check for v2 or v2-develop branches
    if (await repoHasBranch(repoUrl, 'v2')) {
        return "v2";
    }
    if (await repoHasBranch(repoUrl, 'v2-develop')) {
        return "v2-develop";
    }
    // Default to main branch
    return "main";

}

std::future<std::vector<std::string>> listPluginsByType(std::optional<std::string> type) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto registry = getRegistryIndex();
    return Object.keys(registry);
    .filter((name) => !type || (std::find(name.begin(), name.end(), type) != name.end()));
    .sort();

}

std::future<std::optional<PluginMetadata>> getPluginMetadata(const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto settings = getRegistrySettings();
        const auto credentials = getGitHubCredentials();

        if (!credentials) {
            std::cerr << "GitHub credentials not found. Please run login first." << std::endl;
            process.exit(1);
        }

        const auto [owner, repo] = settings.defaultRegistry.split("/");
        const auto normalizedName = normalizePackageName(pluginName);
        const auto url = "https://api.github.com/repos/" + owner + "/" + repo + "/contents/packages/" + normalizedName + ".json";

        try {
            const auto response = fetch(url, {;
                headers: {
                    "Authorization: " + "token " + credentials.token
                    Accept: "application/vnd.github.v3.raw",
                    },
                    });

                    if (!response.ok) {
                        if (response.status == 404) {
                            return nullptr;
                        }
                        throw std::runtime_error(`Failed to fetch plugin metadata: ${response.statusText}`);
                    }

                    const auto data = response.json();
                    if (typeof data != 'object' || data == null) {
                        throw std::runtime_error('Invalid plugin metadata format');
                    }

                    const auto metadata = data;
                    if (
                    !metadata.name ||;
                    !metadata.description ||;
                    !metadata.author ||;
                    !metadata.repository ||;
                    !metadata.versions ||;
                    !metadata.latestVersion ||;
                    !metadata.runtimeVersion ||;
                    !metadata.maintainer;
                    ) {
                        throw std::runtime_error('Invalid plugin metadata: missing required fields');
                    }

                    return metadata;
                    } catch (error) {
                        std::cerr << "Failed to fetch plugin metadata:" << error << std::endl;
                        return nullptr;
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::string> getPluginVersion(const std::string& pluginName, std::optional<std::string> version) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Skip metadata lookup to avoid auth requirements
    // Just return the requested version or use latest as fallback
    if (version) {
        return version;
    }

    // Try to get package details from non-auth methods
    try {
        const auto packageDetails = getPackageDetails(pluginName);
        if (packageDetails.latestVersion) {
            return packageDetails.latestVersion;
        }
        } catch (error) {
            logger.debug(
            "Error getting package details: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
            );
        }

        // Fallback to a reasonable default version
        return "latest";

}

std::future<std::vector<std::string>> getPluginTags(const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto metadata = getPluginMetadata(pluginName);
    return metadata.tags || [];

}

std::future<std::vector<std::string>> getPluginCategories(const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto metadata = getPluginMetadata(pluginName);
    return metadata.categories || [];

}

std::future<std::vector<std::string>> getAvailableDatabases() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto registry = getRegistryIndex();
    return Object.keys(registry);
    .filter((name) => (std::find(name.begin(), name.end(), "database-") != name.end()));
    .sort();

}

std::future<> getPackageDetails(const std::string& packageName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    name: string;
    description: string;
    author: string;
    repository: string;
    versions: string[];
    latestVersion: string;
    runtimeVersion: string;
    maintainer: string;

}

std::future<std::string> getBestPluginVersion(const std::string& packageName, const std::string& runtimeVersion) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto packageDetails = getPackageDetails(packageName);
    if (!packageDetails || !packageDetails.versions || packageDetails.versions.length == 0) {
        return nullptr;
    }

    // If runtime version matches exactly, use the latest version
    if (packageDetails.runtimeVersion == runtimeVersion) {
        return packageDetails.latestVersion;
    }

    // Parse the runtime version for semver matching
    const auto [runtimeMajor, runtimeMinor] = runtimeVersion.split(".").map(Number);
    const auto [packageMajor, packageMinor] = packageDetails.runtimeVersion.split(".").map(Number);

    // If major version is different, warn but still return the latest
    if (runtimeMajor != packageMajor) {
        logger.warn(
        "Plugin " + packageName + " was built for runtime v" + packageDetails.runtimeVersion + ", but you're using v" + runtimeVersion;
        );
        std::cout << "This may cause compatibility issues." << std::endl;
        return packageDetails.latestVersion;
    }

    // If minor version is different, warn but with less severity
    if (runtimeMinor != packageMinor) {
        logger.warn(
        "Plugin " + packageName + " was built for runtime v" + packageDetails.runtimeVersion + ", you're using v" + runtimeVersion;
        );
    }

    return packageDetails.latestVersion;

}

std::future<DataDirStatus> checkDataDir() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const DataDirStatus status = {;
        exists: false,
        env: {
            exists: false,
            missingKeys: [...REQUIRED_ENV_VARS],
            hasAllKeys: false,
            },
            settings: {
                exists: false,
                missingKeys: [...REQUIRED_SETTINGS],
                hasAllKeys: false,
                },
                };

                // Check if data directory exists
                try {
                    fs.access(ELIZA_DIR);
                    status.exists = true;
                    } catch {
                        return status;
                    }

                    // Check .env file
                    try {
                        const auto envContent = fs.readFile(ENV_FILE, "utf-8");
                        const auto env = dotenv.parse(envContent);
                        status.env.exists = true;
                        status.env.missingKeys = REQUIRED_ENV_VARS.filter((key) => !env[key]);
                        status.env.hasAllKeys = status.env.missingKeys.size() == 0;
                        } catch {
                            // .env file doesn't exist or can't be read
                        }

                        // Check settings file
                        try {
                            const auto settingsContent = fs.readFile(REGISTRY_SETTINGS_FILE, "utf-8");
                            const auto settings = /* JSON.parse */ settingsContent;
                            status.settings.exists = true;
                            status.settings.missingKeys = REQUIRED_SETTINGS.filter((key) => !(key in settings));
                            status.settings.hasAllKeys = status.settings.missingKeys.size() == 0;
                            } catch {
                                // settings file doesn't exist or can't be read
                            }

                            return status;

}

std::future<void> initializeDataDir() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    ensureElizaDir();

    // Initialize .env if it doesn't exist
    try {
        fs.access(ENV_FILE);
        } catch {
            fs.writeFile(ENV_FILE, "");
        }

        // Initialize settings if they don't exist
        try {
            fs.access(REGISTRY_SETTINGS_FILE);
            } catch {
                saveRegistrySettings({
                    defaultRegistry: REGISTRY_REPO,
                    });
                }

}

std::future<bool> validateDataDir() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto status = checkDataDir();

    if (!status.exists) {
        std::cout << "ElizaOS data directory not found. Initializing..." << std::endl;
        initializeDataDir();
        return false;
    }

    auto isValid = true;

    // Check if GitHub credentials exist - using the same method as getGitHubCredentials
    // This ensures we're checking the same place where credentials are stored
    const auto envPath = resolveEnvFile();
    if (envPath) {
        const auto envContent = fs.readFile(envPath, "utf-8");
        const auto parsedEnv = dotenv.parse(envContent);
        if (!parsedEnv.GITHUB_TOKEN) {
            std::cout << "GitHub token not found in environment" << std::endl;
            isValid = false;
        }
        } else {
            std::cout << ".env file not found" << std::endl;
            isValid = false;
        }

        if (!status.env.hasAllKeys) {
            std::cout << "Missing environment variables: " + std::to_string(status.env.missingKeys.join(", ")) << std::endl;
            isValid = false;
        }

        if (!status.settings.exists) {
            std::cout << "Registry settings file not found" << std::endl;
            isValid = false;
            } else if (!status.settings.hasAllKeys) {
                std::cout << "Missing settings: " + std::to_string(status.settings.missingKeys.join(", ")) << std::endl;
                isValid = false;
            }

            return isValid;

}

} // namespace elizaos
