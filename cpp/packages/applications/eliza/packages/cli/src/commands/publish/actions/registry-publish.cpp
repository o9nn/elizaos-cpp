#include "registry-publish.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> updateRegistryIndex(PackageMetadata packageMetadata, auto dryRun) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto indexPath = dryRun;
        ? path.join(process.cwd(), LOCAL_REGISTRY_PATH, "index.json");
        : path.join(process.cwd(), "temp-registry", "index.json");

        // Create registry directory if it doesn't exist in dry run
        try {
            fs.access(path.dirname(indexPath));
            } catch {
                fs.mkdir(path.dirname(indexPath), { recursive: true });
                // Create empty index file if it doesn't exist
                try {
                    fs.access(indexPath);
                    } catch {
                        fs.writeFile(;
                        indexPath,
                        JSON.stringify(;
                        {
                            v1: { packages: {} },
                            v2: { packages: {} },
                            },
                            nullptr,
                            2;
                            );
                            );
                        }
                    }

                    // Read current index
                    auto indexContent;
                    try {
                        indexContent = fs.readFile(indexPath, "utf-8");
                        } catch (error) {
                            // Create default index if it doesn't exist
                            indexContent = JSON.stringify({
                                v1: { packages: {} },
                                v2: { packages: {} },
                                });
                            }

                            const auto index = /* JSON.parse */ indexContent;

                            // Update v2 section of index
                            if (!index.v2) {
                                index.v2 = { packages: {} };
                            }

                            if (!index.v2.packages) {
                                index.v2.packages = {}
                            }

                            if (!index.v2.packages[packageMetadata.name]) {
                                index.v2.packages[packageMetadata.name] = {
                                    name: packageMetadata.name,
                                    description: packageMetadata.description,
                                    type: packageMetadata.type,
                                    versions: {},
                                    };
                                }

                                // Update package info
                                const auto packageInfo = index.v2.packages[packageMetadata.name];
                                packageInfo.description = packageMetadata.description;
                                packageInfo.type = packageMetadata.type;

                                // Add version
                                packageInfo.versions[packageMetadata.version] = {
                                    version: packageMetadata.version,
                                    runtimeVersion: packageMetadata.runtimeVersion,
                                    platform: packageMetadata.platform,
                                    publishedAt: packageMetadata.publishedAt,
                                    published: !dryRun,
                                    };

                                    // Write updated index
                                    fs.writeFile(indexPath, /* JSON.stringify */ std::string(index, nullptr, 2));
                                    console.info(
                                    "Registry index " + std::to_string(dryRun ? "(dry run) " : "") + "updated with " + packageMetadata.name + "@" + packageMetadata.version
                                    );

                                    return true;
                                    } catch (error) {
                                        console.error(
                                        "Failed to update registry index: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                                        );
                                        return false;
                                    }

}

std::future<bool> savePackageToRegistry(PackageMetadata packageMetadata, auto dryRun) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Define paths
        const auto packageDir = dryRun;
        ? path.join(process.cwd(), LOCAL_REGISTRY_PATH, REGISTRY_PACKAGES_PATH, packageMetadata.name);
        : path.join(process.cwd(), "temp-registry", REGISTRY_PACKAGES_PATH, packageMetadata.name);
        const auto metadataPath = "path.join(packageDir, " + packageMetadata.version + ".json";

        // Create directory if it doesn't exist
        fs.mkdir(packageDir, { recursive: true });

        // Write metadata file
        fs.writeFile(metadataPath, /* JSON.stringify */ std::string(packageMetadata, nullptr, 2));

        std::cout << "Package metadata " + std::to_string(dryRun ? "(dry run) " : "") + "saved to " + metadataPath << std::endl;

        // Update index file
        updateRegistryIndex(packageMetadata, dryRun);

        return true;
        } catch (error) {
            console.error(
            "Failed to save package metadata: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
            );
            return false;
        }

}

} // namespace elizaos
