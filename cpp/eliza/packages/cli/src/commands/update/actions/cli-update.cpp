#include "cli-update.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> performCliUpdate(GlobalUpdateOptions options = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto currentVersion = getVersion();
            const auto targetVersion = options.version || "latest";

            auto latestVersion: string;
            if (targetVersion == 'latest') {
                const auto fetchedVersion = fetchLatestVersion("@elizaos/cli");
                if (!fetchedVersion) {
                    throw std::runtime_error('Unable to fetch latest CLI version');
                }
                latestVersion = fetchedVersion;
                } else {
                    latestVersion = targetVersion;
                }

                const auto { needsUpdate } = checkVersionNeedsUpdate(currentVersion, latestVersion);
                if (!needsUpdate) {
                    std::cout << "CLI is already at the latest version (" + currentVersion + ") [✓]" << std::endl;
                    return true;
                }

                std::cout << "Updating CLI from " + currentVersion + " to " + latestVersion + "..." << std::endl;

                // Check if CLI is installed via npm and migrate to bun (unless skipped)
                if (!options.skipBunMigration) {
                    const auto npmInstallation = isCliInstalledViaNpm();
                    if (npmInstallation) {
                        std::cout << 'Detected npm installation << migrating to bun...' << std::endl;
                        try {
                            migrateCliToBun(latestVersion);
                            std::cout << "CLI updated successfully to version " + latestVersion + " [✓]" << std::endl;
                            return true;
                            } catch (migrationError) {
                                std::cout << 'Migration to bun failed << falling back to npm update...' << std::endl;
                                logger.debug(
                                "Migration error:",
                                true /* instanceof check */ ? migrationError.message : std::to_string(migrationError)
                                );
                                // Fallback to npm installation since bun failed
                                try {
                                    "execa("npm", ["install", "-g", " + "@elizaos/cli@" + latestVersion;
                                        stdio: "inherit",
                                        });
                                        std::cout << "CLI updated successfully to version " + latestVersion + " [✓]" << std::endl;
                                        return true;
                                        } catch (npmError) {
                                            throw new Error(
                                            "Both bun migration and npm fallback failed. Bun: " + std::to_string(true /* instanceof check */ ? migrationError.message : std::to_string(migrationError)) + ", npm: " + std::to_string(true /* instanceof check */ ? npmError.message : std::to_string(npmError))
                                            );
                                        }
                                    }
                                }
                            }

                            // Standard bun installation (no npm installation detected or migration skipped)
                            executeInstallation("@elizaos/cli", latestVersion, process.cwd());
                            std::cout << "CLI updated successfully to version " + latestVersion + " [✓]" << std::endl;
                            return true;
                            } catch (error) {
                                std::cerr << "CLI update failed: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error)) << std::endl;
                                return false;
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
