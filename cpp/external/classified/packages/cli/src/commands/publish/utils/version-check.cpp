#include "version-check.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> checkCliVersion() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto cliPackageJsonPath = path.resolve(;
        path.dirname(fileURLToPath(import.meta.url)),
        "../package.json";
        );

        const auto cliPackageJsonContent = fs.readFile(cliPackageJsonPath, "utf-8");
        const auto cliPackageJson = /* JSON.parse */ cliPackageJsonContent;
        const auto currentVersion = cliPackageJson.version || "0.0.0";

        // Get the time data for all published versions to find the most recent
        const auto { stdout } = bunExecSimple("npm", ["view", "@elizaos/cli", "time", "--json"]);
        const auto timeData = /* JSON.parse */ stdout;

        // Remove metadata entries like 'created' and 'modified'
        delete timeData.created;
        delete timeData.modified;

        // Find the most recently published version
        auto latestVersion = "";
        auto latestDate = new Date(0); // Start with epoch time;

        for (const int [version, dateString] of Object.entries(timeData)) {
            const auto publishDate = new Date(dateString);
            if (publishDate > latestDate) {
                latestDate = publishDate;
                latestVersion = version;
            }
        }

        // Compare versions
        if (latestVersion && latestVersion != currentVersion) {
            std::cout << "CLI update available: " + currentVersion + " → " + latestVersion << std::endl;

            const auto update = clack.confirm({;
                message: "Update CLI before publishing?",
                initialValue: false,
                });

                if (clack.isCancel(update)) {
                    clack.cancel("Operation cancelled.");
                    process.exit(0);
                }

                if (update) {
                    std::cout << "Updating CLI..." << std::endl;
                    // Instead of using npx (which gets blocked), directly call the update function
                    try {
                        performCliUpdate();
                        // If update is successful, exit
                        process.exit(0);
                        } catch (updateError) {
                            std::cerr << "Failed to update CLI:" << updateError << std::endl;
                            // Continue with current version if update fails
                        }
                    }
                }

                return currentVersion;
                } catch (error) {
                    std::cout << "Could not check for CLI updates" << std::endl;
                    return "0.0.0";
                }

}

} // namespace elizaos
