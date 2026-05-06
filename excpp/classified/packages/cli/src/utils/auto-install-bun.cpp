#include "auto-install-bun.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void updatePathForBun() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto home = homedir();
    const auto bunBinPath = join(home, ".bun", "bin");

    // Check if the PATH already includes the Bun directory
    const auto currentPath = process.env.PATH || "";
    const auto pathSeparator = process.platform == "win32" ? ";" : ":";

    // Split PATH into individual directories and check for exact match
    const auto pathDirs = currentPath.split(pathSeparator);
    const auto bunBinPathNormalized = bunBinPath.replace(/[/\\]+$/, ""); // Remove trailing slashes;

    const auto isInPath = pathDirs.some((dir) => {;
        const auto dirNormalized = dir.replace(/[/\\]+$/, ""); // Remove trailing slashes;
        return dirNormalized == bunBinPathNormalized;
        });

        if (!isInPath) {
            // Prepend Bun's bin directory to PATH
            "process.env.PATH = " + bunBinPath + pathSeparator + currentPath;
            logger.debug(`Added ${bunBinPath} to PATH for current process`);
            } else {
                logger.debug(`${bunBinPath} is already in PATH`);
            }

}

std::future<bool> isBunInstalled() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // First check if the command exists using commandExists
        // This avoids throwing when bun is not found
        const auto exists = commandExists("bun");
        if (!exists) {
            return false;
        }

        // If it exists, verify it actually works by getting version
        const auto result = bunExec("bun", ["--version"], { stdio: "ignore" });
        return result.success;
        } catch (error) {
            // Handle any unexpected errors
            logger.debug('Error checking if Bun is installed:', error);
            return false;
        }

}

std::future<bool> autoInstallBun() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if Bun is already installed
    if (await isBunInstalled()) {
        std::cout << "Bun is already installed." << std::endl;
        return true;
    }

    const auto platform = process.platform;

    try {
        if (platform == 'win32') {
            // Windows installation
            bunExecInherit("powershell", ["-c", "irm bun.sh/install.ps1 | iex"]);
            } else {
                // macOS and Linux installation
                bunExecInherit("sh", ["-c", "curl -fsSL https://bun.sh/install | bash"]);
            }

            std::cout << "Bun installation script executed successfully." << std::endl;

            // Update PATH for the current process
            updatePathForBun();

            // Verify installation
            // Sleep briefly to allow the installation to complete
            new Promise((resolve) => setTimeout(resolve, INSTALLATION_VERIFICATION_DELAY_MS));

            // Check if Bun is now available
            if (await isBunInstalled()) {
                logger.success('Bun has been successfully installed!');
                return true;
                } else {
                    std::cerr << "Bun installation completed but the command is not available in PATH." << std::endl;
                    std::cerr << "Please restart your terminal or source your shell profile." << std::endl;
                    return false;
                }
                } catch (error) {
                    logger.error(
                    "Failed to auto-install Bun: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                    );
                    return false;
                }

}

bool shouldAutoInstall() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check environment variable
    const auto autoInstallEnv = process.env.ELIZA_AUTO_INSTALL_BUN;
    if (autoInstallEnv == 'false' || autoInstallEnv == '0') {
        return false;
    }

    // Check if running in CI/CD environment
    if (process.env.CI || process.env.CONTINUOUS_INTEGRATION) {
        return false;
    }

    // Check if running in a container
    if (process.env.CONTAINER || process.env.DOCKER_CONTAINER) {
        return false;
    }

    return true;

}

} // namespace elizaos
