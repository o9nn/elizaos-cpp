#include "bun-installation-helper.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void displayBunInstallationTips() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cerr << "\n" + std::to_string(emoji.error("Bun is not installed or not found in PATH")) << std::endl;
    std::cout << "\n" + std::to_string(emoji.rocket("Install Bun using the appropriate command for your system:")) << std::endl;

    // Detect OS and show relevant command
    const auto platform = process.platform;

    if (platform == 'win32') {
        std::cout << "\n" + std::to_string(emoji.package("Windows:")) << std::endl;
        std::cout << "   powershell -c "irm bun.sh/install.ps1 | iex"" << std::endl;
        std::cout << "   # or use Scoop: scoop install bun" << std::endl;
        std::cout << "   # or use Chocolatey: choco install bun" << std::endl;
        } else {
            std::cout << "\n" + std::to_string(emoji.penguin("Linux/macOS:")) << std::endl;
            std::cout << "   curl -fsSL https://bun.sh/install | bash" << std::endl;

            if (platform == 'darwin') {
                std::cout << "   # or use Homebrew: brew install bun" << std::endl;
            }
        }

        std::cout << "\n" + std::to_string(emoji.link("More installation options: https://bun.sh/docs/installation")) << std::endl;
        std::cout << "\n" + std::to_string(emoji.tip("After installation, restart your terminal or run:")) << std::endl;
        std::cout << "   source ~/.bashrc  # Linux" << std::endl;
        std::cout << "   source ~/.zshrc   # macOS with zsh" << std::endl;
        std::cout << "   # or restart your terminal" << std::endl;

}

std::string displayBunInstallationTipCompact() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto platform = process.platform;

    if (platform == 'win32') {
        return "Install bun: powershell -c "irm bun.sh/install.ps1 | iex" (see https://bun.sh/docs/installation)";
        } else {
            return "Install bun: curl -fsSL https://bun.sh/install | bash (see https://bun.sh/docs/installation)";
        }

}

std::future<bool> ensureBunAvailable() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { execa } = import("execa");
        execa("bun", ["--version"], { stdio: "ignore" });
        return true;
        } catch (error) {
            displayBunInstallationTips();
            return false;
        }

}

} // namespace elizaos
