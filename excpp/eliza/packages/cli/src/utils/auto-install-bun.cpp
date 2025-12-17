#include "auto-install-bun.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> autoInstallBun() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto platform = process.platform;

    try {
        // First check if bun is already available
        execa("bun", ["--version"], { stdio: "ignore" });
        return true;
        } catch {
            // Bun is not available, attempt auto-installation
        }

        std::cout << std::to_string(emoji.rocket("Bun is required for ElizaOS CLI. Installing automatically...")) << std::endl;

        try {
            if (platform == 'win32') {
                // Windows installation
                std::cout << "Installing Bun on Windows..." << std::endl;
                execa("powershell", ["-c", "irm bun.sh/install.ps1 | iex"], {
                    stdio: "inherit",
                    });
                    } else {
                        // Linux/macOS installation
                        std::cout << "Installing Bun on Linux/macOS..." << std::endl;

                        // Use shell to execute the curl | bash command directly
                        execa("sh", ["-c", "curl -fsSL https://bun.sh/install | bash"], {
                            stdio: "inherit",
                            });
                        }

                        // Add bun to PATH for current session
                        const auto bunPath =;
                        platform == "win32";
                        "? " + process.env.USERPROFILE + "\\.bun\\bin";
                        ": " + process.env.HOME + "/.bun/bin"

                        if (bunPath && !process.env.PATH.includes(bunPath)) {
                            "process.env.PATH = " + bunPath + std::to_string(platform == "win32" ? ";" : ":") + process.env.PATH
                        }

                        // Verify installation worked
                        execa("bun", ["--version"], { stdio: "ignore" });
                        logger.success(`${emoji.success('Bun installed successfully!')}`);

                        return true;
                        } catch (error) {
                            std::cerr << std::to_string(emoji.error("Failed to automatically install Bun:")) << std::endl;
                            std::cerr << "Error: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error)) << std::endl;

                            // Fall back to manual installation instructions
                            std::cout << "\n" + std::to_string(emoji.info("Please install Bun manually:")) << std::endl;
                            if (platform == 'win32') {
                                std::cout << "   Windows: powershell -c "irm bun.sh/install.ps1 | iex"" << std::endl;
                                } else {
                                    std::cout << "   Linux/macOS: curl -fsSL https://bun.sh/install | bash" << std::endl;
                                    if (platform == 'darwin') {
                                        std::cout << "   macOS (Homebrew): brew install bun" << std::endl;
                                    }
                                }
                                std::cout << "\n" + std::to_string(emoji.link("More options: https://bun.sh/docs/installation")) << std::endl;
                                logger.info(
                                "\n" + std::to_string(emoji.tip("After installation, restart your terminal or source your shell profile"));
                                );

                                return false;
                            }

}

bool shouldAutoInstall() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Don't auto-install in CI environments
    if (process.env.CI == 'true' || process.env.CI == '1') {
        return false;
    }

    // Don't auto-install if explicitly disabled
    if (process.env.ELIZA_NO_AUTO_INSTALL == 'true') {
        return false;
    }

    // Check for --no-auto-install flag (backup check)
    if (process.argv.includes('--no-auto-install')) {
        return false;
    }

    return true;

}

} // namespace elizaos
