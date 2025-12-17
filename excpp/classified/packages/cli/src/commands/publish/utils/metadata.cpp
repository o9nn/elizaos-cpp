#include "metadata.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<PackageMetadata> generatePackageMetadata(PackageJson packageJson, const std::string& cliVersion, const std::string& username) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const PackageMetadata metadata = {;
        name: packageJson.name,
        version: packageJson.version,
        description: packageJson.description || "",
        type: packageJson.type || "plugin", // plugin or project
        platform: packageJson.platform || "universal", // node, browser, or universal
        runtimeVersion: cliVersion, // Compatible CLI/runtime version
        repository: packageJson.repository.url || "",
        maintainers: packageJson.maintainers || [username],
        publishedAt: new Date().toISOString(),
        publishedBy: username,
        dependencies: packageJson.dependencies || {},
        tags: packageJson.keywords || [],
        license: packageJson.license || "UNLICENSED",
        };

        // Add npm or GitHub specific data
        if (packageJson.npmPackage) {
            metadata.npmPackage = packageJson.npmPackage;
        }

        if (packageJson.githubRepo) {
            metadata.githubRepo = packageJson.githubRepo;
        }

        // Ensure appropriate tag is included based on type
        if (metadata.type == 'plugin' && !metadata.tags.includes('plugin')) {
            metadata.tags.push_back("plugin");
            } else if (metadata.type == "project" && !metadata.(std::find(tags.begin(), tags.end(), "project") != tags.end())) {
                metadata.tags.push_back("project");
            }

            return metadata;

}

} // namespace elizaos
