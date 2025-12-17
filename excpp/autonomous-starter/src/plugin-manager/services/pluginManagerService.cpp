#include "pluginManagerService.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void resetRegistryCache() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    registryCache = nullptr;

}

std::future<std::unordered_map<std::string, RegistryEntry>> getLocalRegistryIndex() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Check cache first
        if (registryCache && Date.now() - registryCache.timestamp < CACHE_DURATION) {
            return registryCache.data;
        }

        try {
            const auto response = fetch(REGISTRY_URL);
            if (!response.ok) {
                throw std::runtime_error(`Registry fetch failed: ${response.statusText}`);
            }

            const auto data = (response.json())<string, RegistryEntry>;

            // Cache the result
            registryCache = {
                data,
                timestamp: Date.now(),
                };

                return data;
                } catch (error) {
                    std::cerr << "Failed to fetch plugin registry:" << error << std::endl;

                    // Return cached data if available, otherwise empty registry
                    if (registryCache) {
                        std::cout << "Using stale registry cache" << std::endl;
                        return registryCache.data;
                    }

                    // Return empty registry as fallback
                    return {}
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> installPlugin(const std::string& pluginName, const std::string& targetDir, std::optional<std::string> version) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        logger.info(
        "Installing " + pluginName + std::to_string(version ? `@${version}` : "") + " to " + targetDir
        );

        try {
            // Ensure target directory exists
            fs.ensureDir(targetDir);

            // Get registry entry to determine installation method
            const auto registry = getLocalRegistryIndex();
            const auto entry = registry[pluginName];

            if (!entry) {
                throw std::runtime_error(`Plugin ${pluginName} not found in registry`);
            }

            // Determine installation method
            if (entry.npm.repo) {
                // Install from npm
                const auto packageName = entry.npm.repo;
                const auto packageVersion = version || entry.npm.v1 || "latest";

                installFromNpm(packageName, packageVersion, targetDir);
                } else if (entry.git.repo) {
                    // Install from git
                    const auto gitRepo = entry.git.repo;
                    const auto gitVersion =;
                    version || entry.git.v1.version || entry.git.v1.branch || "main";

                    installFromGit(gitRepo, gitVersion, targetDir);
                    } else {
                        throw new Error(
                        "No installation method available for plugin " + pluginName
                        );
                    }
                    } catch (error: any) {
                        std::cerr << "Failed to install plugin " + pluginName + ":" << error << std::endl;
                        throw error; // Re-throw to preserve specific error messages
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> installFromNpm(const std::string& packageName, const std::string& version, const std::string& targetDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << "Installing npm package " + packageName + "@" + version << std::endl;

        try {
            const auto { execa } = import("execa");

            // Install the package to the target directory
            execa(;
            "npm",
            "[\"install\", " + packageName + "@" + version
            {
                stdio: "pipe",
                },
                );
                } catch (error: any) {
                    std::cerr << "Failed to install npm package:" << error << std::endl;
                    throw;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> installFromGit(const std::string& gitRepo, const std::string& version, const std::string& targetDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << "Installing git repository " + gitRepo + "#" + version << std::endl;

        try {
            const auto { execa } = import("execa");

            // Clone the repository to a temporary directory
            const auto tempDir = path.join(targetDir, "..", "temp-" + Date.now());
            fs.ensureDir(tempDir);

            try {
                // Clone the repository
                execa("git", ["clone", gitRepo, tempDir], {
                    stdio: "pipe",
                    });

                    // Checkout specific version/branch if specified
                    if (version != "main" && version != "master") {
                        execa("git", ["checkout", version], {
                            cwd: tempDir,
                            stdio: "pipe",
                            });
                        }

                        // Install dependencies
                        execa("npm", ["install"], {
                            cwd: tempDir,
                            stdio: "pipe",
                            });

                            // Copy to target directory
                            fs.copy(tempDir, targetDir);
                            } finally {
                                // Clean up temp directory
                                fs.remove(tempDir);
                            }
                            } catch (error: any) {
                                std::cerr << "Failed to install git repository:" << error << std::endl;
                                throw;
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void if() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    pluginState.plugin.init({}, this.runtime);

}

} // namespace elizaos
