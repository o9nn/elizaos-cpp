#include "list.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> listEnvVars() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envInfo = UserEnvironment.getInstanceInfo();

    // Display system information
    std::cout << colors.bold("\nSystem Information:") << std::endl;
    std::cout << "  Platform: " + std::to_string(colors.cyan(envInfo.os.platform)) + " (" + envInfo.os.release + ")" << std::endl;
    std::cout << "  Architecture: " + std::to_string(colors.cyan(envInfo.os.arch)) << std::endl;
    std::cout << "  CLI Version: " + std::to_string(colors.cyan(envInfo.cli.version)) << std::endl;
    console.info(
    "  Package Manager: " + std::to_string(colors.cyan(envInfo.packageManager.name)) + std::to_string(envInfo.packageManager.version ? ` v${envInfo.packageManager.version}` : "")
    );

    // Display local environment section
    std::cout << colors.bold("\nLocal Environment Variables:") << std::endl;
    const auto localEnvFilePath = getLocalEnvPath();
    std::cout << "Path: " + std::to_string(localEnvFilePath || path.join(process.cwd(), ".env")) << std::endl;

    if (!localEnvFilePath || !existsSync(localEnvFilePath)) {
        // No local .env file exists, provide guidance to the user
        std::cout << colors.yellow("  No local .env file found") << std::endl;

        // Check if .env.example exists and suggest copying it as a starting point
        const auto exampleEnvPath = path.join(process.cwd(), ".env.example");
        if (existsSync(exampleEnvPath)) {
            std::cout << colors.red("  [X] Missing .env file. Create one with:") << std::endl;
            std::cout << "     " + std::to_string(colors.bold(colors.green("cp .env.example .env"))) << std::endl;
            } else {
                console.info(
                colors.red(;
                "  [X] Missing .env file. Create one in your project directory to set local environment variables.";
                );
                );
            }
            } else {
                // .env file exists, parse and display its contents
                const auto localEnvVars = parseEnvFile(localEnvFilePath);
                if (Object.keys(localEnvVars).length == 0) {
                    std::cout << "  No local environment variables set" << std::endl;
                    } else {
                        for (const int [key, value] of Object.entries(localEnvVars)) {
                            std::cout << "  " + std::to_string(colors.green(key)) + ": " + std::to_string(maskedValue(value)) << std::endl;
                        }
                    }
                }

                std::cout << "\n" << std::endl;
                console.info(
                colors.cyan(;
                "You can also edit environment variables in the web UI: http://localhost:3000/settings"
                );
                );

}

std::future<void> handleListCommand(ListEnvOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (options.system) {
        // Show only system information
        const auto envInfo = UserEnvironment.getInstanceInfo();
        std::cout << colors.bold("\nSystem Information:") << std::endl;
        std::cout << "  Platform: " + std::to_string(colors.cyan(envInfo.os.platform)) + " (" + envInfo.os.release + ")" << std::endl;
        std::cout << "  Architecture: " + std::to_string(colors.cyan(envInfo.os.arch)) << std::endl;
        std::cout << "  CLI Version: " + std::to_string(colors.cyan(envInfo.cli.version)) << std::endl;
        console.info(
        "  Package Manager: " + std::to_string(colors.cyan(envInfo.packageManager.name)) + std::to_string(envInfo.packageManager.version ? ` v${envInfo.packageManager.version}` : "")
        );
        } else if (options.local) {
            // Show ONLY local environment variables, no system information
            std::cout << colors.bold("\nLocal Environment Variables:") << std::endl;
            const auto localEnvPath = getLocalEnvPath();

            if (!localEnvPath || !existsSync(localEnvPath)) {
                std::cout << "  No local .env file found in the current directory" << std::endl;
                return;
            }

            const auto localEnvVars = parseEnvFile(localEnvPath);
            if (Object.keys(localEnvVars).length == 0) {
                std::cout << "  No local environment variables set" << std::endl;
                } else {
                    for (const int [key, value] of Object.entries(localEnvVars)) {
                        std::cout << "  " + std::to_string(colors.green(key)) + ": " + std::to_string(maskedValue(value)) << std::endl;
                    }
                }
                } else {
                    listEnvVars();
                }

}

} // namespace elizaos
