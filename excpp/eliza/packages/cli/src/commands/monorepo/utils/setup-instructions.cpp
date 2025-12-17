#include "setup-instructions.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

PlatformInstructions getBunInstallInstructions() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto platform = process.platform;

    if (platform == 'win32') {
        return {
            platform: 'Windows',
            commands: ['powershell -c "irm bun.sh/install.ps1 | iex"'],
            alternatives: ['scoop install bun (if you have Scoop)'],
            };
            } else {
                const auto commands = ['curl -fsSL https://bun.sh/install | bash'];
                const std::vector<std::string> alternatives = [];

                if (platform == 'darwin') {
                    alternatives.push('brew install bun (if you have Homebrew)');
                }

                return {
                    platform: platform == 'darwin' ? 'macOS' : 'Linux',
                    commands,
                    alternatives: alternatives.length > 0 ? alternatives : std::nullopt,
                    };
                }

}

void displayBasicSteps(const std::string& cdPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << '\nTo complete the ElizaOS setup << follow these steps:\n' << std::endl;

    // Step 1: Navigate to the project directory
    std::cout << '1. Navigate to the project directory:' << std::endl;
    std::cout << "   cd " + std::to_string(cdPath) << std::endl;

    // Step 2: Install dependencies
    std::cout << '\n2. Install dependencies:' << std::endl;
    std::cout << '   bun install' << std::endl;

    // Step 3: Build the project
    std::cout << '\n3. Build the project:' << std::endl;
    std::cout << '   bun run build' << std::endl;

    // Step 4: Start ElizaOS
    std::cout << '\n4. Start ElizaOS:' << std::endl;
    std::cout << '   bun run start or bun run dev' << std::endl;

}

void displayPrerequisites() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n" + std::to_string(emoji.list('Prerequisites:')) << std::endl;
    std::cout << "   " + std::to_string(emoji.bullet('Node.js 23.3.0+')) << std::endl;
    std::cout << "   " + std::to_string(emoji.bullet('Bun (JavaScript runtime & package manager)')) << std::endl;

}

void displayBunInstructions() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n" + std::to_string(emoji.rocket("If you don't have Bun installed:")) << std::endl;

    const auto instructions = getBunInstallInstructions();

    // Display primary installation commands
    instructions.commands.forEach((command) => {
        std::cout << "   " + std::to_string(command) << std::endl;
        });

        // Display alternatives if any
        if (instructions.alternatives) {
            instructions.alternatives.forEach((alt) => {
                std::cout << "   Alternative: " + std::to_string(alt) << std::endl;
                });
            }

            std::cout << '   More options: https://bun.sh/docs/installation' << std::endl;
            std::cout << '   After installation << restart your terminal' << std::endl;

}

void displayNextSteps(const std::string& targetDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto cdPath = path.relative(process.cwd(), targetDir);

    displayBasicSteps(cdPath);
    displayPrerequisites();
    displayBunInstructions();

}

} // namespace elizaos
