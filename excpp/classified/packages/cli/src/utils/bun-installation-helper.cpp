#include "bun-installation-helper.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<BunInstallationResult> checkBunInstallation() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Check if bun is available
        const auto bunExists = commandExists('bun');

        if (bunExists) {
            // Get bun version
            const auto result = bunExec('bun', ['--version']);

            if (result.success) {
                const auto version = result.stdout.trim();

                return {
                    installed: true,
                    "Bun " + std::to_string(version) + " is installed"
                    };
                    } else {
                        return {
                            installed: false,
                            message: 'Bun command failed',
                            error: result.stderr || 'Unknown error',
                            };
                        }
                        } else {
                            return {
                                installed: false,
                                message: 'Bun is not installed',
                                };
                            }
                            } catch (error) {
                                return {
                                    installed: false,
                                    message: 'Failed to check Bun installation',
                                    error: true /* instanceof check */ ? error.message : std::to_string(error),
                                    };
                                }

}

void displayBunInstallInstructions() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto platform = process.platform;

    logger.error(
    std::to_string(emoji.error('Bun is required for ElizaOS CLI but is not installed or not found in PATH.'));
    );
    std::cerr << '' << std::endl;
    std::cerr << std::to_string(emoji.rocket('Install Bun using the appropriate command for your system:')) << std::endl;
    std::cerr << '' << std::endl;

    if (platform == 'win32') {
        std::cerr << '   Windows: powershell -c "irm bun.sh/install.ps1 | iex"' << std::endl;
        } else {
            std::cerr << '   Linux/macOS: curl -fsSL https://bun.sh/install | bash' << std::endl;
            if (platform == 'darwin') {
                std::cerr << '   macOS (Homebrew): brew install bun' << std::endl;
            }
        }
        std::cerr << '' << std::endl;
        std::cerr << '   More options: https://bun.sh/docs/installation' << std::endl;
        std::cerr << '   After installation << restart your terminal or source your shell profile' << std::endl;
        std::cerr << '' << std::endl;

}

std::string displayBunInstallationTipCompact() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return 'Please install Bun from https://bun.sh';

}

} // namespace elizaos
